from base64 import b64decode, b64encode
from hashlib import md5
from os import urandom
from typing import Optional

import jwt
from fastapi import APIRouter, Header, status
from fastapi.responses import JSONResponse

import database
import models
from crypto.dh import DHext
from crypto.squresystem import Squre
from tools import datetime, parse_timestamp

secret = urandom(100)
router = APIRouter()
db = database.SqliteDatabase()


@router.post("/register")
def register(form: models.Registration):
    timestamp = datetime.now().strftime("%d/%m/%y/%H:%M:%S")

    if db.user_exists(form.login):
        return JSONResponse(
            status_code=status.HTTP_409_CONFLICT, content={"status": "user exists"}
        )
    if form.type not in ["support", "user"]:
        return JSONResponse(
            status_code=status.HTTP_406_NOT_ACCEPTABLE, content={"status": "wrong type"}
        )

    if form.type == "support" and "support_" != form.login[:8]:
        return JSONResponse(
            status_code=status.HTTP_409_CONFLICT,
            content={"status": "verification failed"},
        )

    dh = DHext()

    db.create_user(
        form.login,
        form.password,
        form.type,
        {"p": dh.P, "g": dh.G, "A": dh.A, "a": dh.a},
        timestamp,
    )
    timestamp = datetime.now().strftime("%d/%m/%y/%H:%M:%S")
    db.create_message(
        "server", form.login, str({"p": dh.P, "g": dh.G, "A": dh.A}), timestamp
    )
    db.set_recieved(form.login)

    return JSONResponse(
        status_code=status.HTTP_201_CREATED,
        content={
            "login": form.login,
            "status": "success",
            "params": {"p": dh.P, "g": dh.G, "A": dh.A},
        },
    )


@router.post("/login")
def login(form: models.Login):
    if not db.user_exists(form.login):
        return JSONResponse(
            status_code=status.HTTP_409_CONFLICT,
            content={"status": "user does not exist"},
        )

    if db.login_user(form.login, form.password):
        return JSONResponse(
            status_code=status.HTTP_200_OK,
            content={
                "login": form.login,
                "status": "success",
                "token": jwt.encode(
                    payload={"login": form.login},
                    key=secret,
                ),
            },
        )

    return JSONResponse(
        status_code=status.HTTP_401_UNAUTHORIZED,
        content={"login": form.login, "status": "login failed"},
    )


@router.post("/sendb")
def sendb(
    form: models.SendB,
    x_auth_token: Optional[str] = Header(default=None),
):
    if x_auth_token is None:
        return JSONResponse(
            status_code=status.HTTP_401_UNAUTHORIZED,
            content={"status": "token is not provided"},
        )
    try:
        from_user = jwt.decode(x_auth_token, key=secret, algorithms=["HS256"])["login"]
    except Exception as e:
        return JSONResponse(
            status_code=status.HTTP_401_UNAUTHORIZED, content={"status": str(e)}
        )

    timestamp = datetime.now().strftime("%d/%m/%y/%H:%M:%S")
    db.create_message(
        from_user, "server", str({"p": form.p, "g": form.g, "B": form.B}), timestamp
    )
    db.set_recieved("server")

    pars = db.get_params(from_user)
    if pars["p"] != form.p or pars["g"] != form.g:
        return JSONResponse(
            status_code=status.HTTP_400_BAD_REQUEST,
            content={"status": "malformed values"},
        )

    dh = DHext(pars["p"], pars["g"], pars["a"])
    dh.calcShared(form.B)
    shared_key = dh.Shared_Secret

    s = db.put_shared_key(from_user, form.B, shared_key)
    if not s:
        return JSONResponse(
            status_code=status.HTTP_409_CONFLICT, content={"status": "key exists"}
        )

    return JSONResponse(
        status_code=status.HTTP_201_CREATED, content={"status": "success"}
    )


@router.post("/sendkey")
def sendkey(
    form: models.SymmetricKey,
    x_auth_token: Optional[str] = Header(default=None),
):
    if x_auth_token is None:
        return JSONResponse(
            status_code=status.HTTP_401_UNAUTHORIZED, content={"status": "unauthorized"}
        )
    try:
        from_user = jwt.decode(x_auth_token, key=secret, algorithms=["HS256"])["login"]
    except Exception as e:
        return JSONResponse(
            status_code=status.HTTP_401_UNAUTHORIZED, content={"status": e}
        )
    shared = db.get_shared_key(from_user)
    if not shared:
        return JSONResponse(
            status_code=status.HTTP_400_BAD_REQUEST, content={"status": "no shared key"}
        )

    p = db.get_params(from_user)["p"]
    dh = DHext(P=p, Shared=shared)
    key = dh.decrypt(form.SK)
    s = db.put_symmetric_key(from_user, key)

    if not s:
        return JSONResponse(
            status_code=status.HTTP_409_CONFLICT, content={"status": "key exists"}
        )

    return JSONResponse(
        status_code=status.HTTP_201_CREATED, content={"status": "success"}
    )


@router.get("/users")
def get_all_users(
    x_auth_token: Optional[str] = Header(default=None),
):
    timestamp = datetime.now().strftime("%d/%m/%y/%H:%M:%S")

    if x_auth_token is None:
        return JSONResponse(
            status_code=status.HTTP_401_UNAUTHORIZED, content={"status": "unauthorized"}
        )
    try:
        user = jwt.decode(x_auth_token, key=secret, algorithms=["HS256"])["login"]
    except Exception as e:
        return JSONResponse(
            status_code=status.HTTP_401_UNAUTHORIZED, content={"status": e}
        )
    if db.check_type(user, "user"):
        return JSONResponse(
            status_code=status.HTTP_403_FORBIDDEN,
            content={"status": "you have no rights"},
        )
    users = db.get_list_of_users(user)
    return JSONResponse(
        status_code=status.HTTP_200_OK,
        content={"status": "success", "users": users},
    )


@router.post("/message/{username}")
def send_message(
    username: str,
    form: models.PostMessage,
    x_auth_token: Optional[str] = Header(default=None),
):
    if x_auth_token is None:
        return JSONResponse(
            status_code=status.HTTP_401_UNAUTHORIZED, content={"status": "unauthorized"}
        )
    try:
        from_user = jwt.decode(x_auth_token, key=secret, algorithms=["HS256"])["login"]
    except Exception as e:
        return JSONResponse(
            status_code=status.HTTP_401_UNAUTHORIZED, content={"status": e}
        )
    if not db.user_exists(username):
        return JSONResponse(
            status_code=status.HTTP_400_BAD_REQUEST,
            content={"status": "user does not exist"},
        )

    if not (
        (db.check_type(from_user, "user") and db.check_type(username, "support"))
        or (db.check_type(username, "user") and db.check_type(from_user, "support"))
    ):
        return JSONResponse(
            status_code=status.HTTP_400_BAD_REQUEST,
            content={"status": "wrong destination"},
        )

    if not db.got_messages(from_user):
        return JSONResponse(
            status_code=status.HTTP_403_FORBIDDEN,
            content={"status": "you still have messages to recieve, rejected"},
        )

    key, alpha = db.get_symmetric_params(from_user)
    if not key:
        return JSONResponse(
            status_code=status.HTTP_400_BAD_REQUEST,
            content={"status": f"symmetric key is not provided for {from_user}"},
        )
    Sqr = Squre(K=key, init=alpha)
    db.update_state(from_user, Sqr.get_alpha())
    message = Sqr.decrypt(b64decode(form.text.encode()))

    if not message.decode().isprintable() or len(message) == 0:
        return JSONResponse(
            status_code=status.HTTP_409_CONFLICT, content={"status": "bad message"}
        )

    key, alpha = db.get_symmetric_params(username)
    if not key:
        return JSONResponse(
            status_code=status.HTTP_400_BAD_REQUEST,
            content={"status": f"symmetric key is not provided for {username}"},
        )

    Sqr = Squre(K=key, init=alpha)
    db.update_state(username, Sqr.get_alpha())
    message = b64encode(Sqr.encrypt(message)).decode()
    db.create_message(from_user, username, message, form.timestamp)
    return JSONResponse(
        status_code=status.HTTP_201_CREATED, content={"status": "success"}
    )


@router.get("/messagesto/{username}")
def get_all_messages_to(
    username: str,
    x_auth_token: Optional[str] = Header(default=None),
):
    timestamp = datetime.now().strftime("%d/%m/%y/%H:%M:%S")

    if x_auth_token is None:
        return JSONResponse(
            status_code=status.HTTP_401_UNAUTHORIZED, content={"status": "unauthorized"}
        )
    try:
        to_user = jwt.decode(x_auth_token, key=secret, algorithms=["HS256"])["login"]
    except Exception as e:
        return JSONResponse(
            status_code=status.HTTP_401_UNAUTHORIZED, content={"status": e}
        )

    if not db.check_type(to_user, "support") and username != to_user:
        return JSONResponse(
            status_code=status.HTTP_403_FORBIDDEN,
            content={"status": "you have no rights"},
        )

    if username == to_user and db.got_messages(username):
        return JSONResponse(
            status_code=status.HTTP_204_NO_CONTENT,
            content={"status": "no new messages to {username}"},
        )

    messages = db.get_messages_to(username)
    if len(messages) == 0:
        return JSONResponse(
            status_code=status.HTTP_204_NO_CONTENT,
            content={"status": f"no messages to {username}"},
        )

    db.set_recieved(username)
    return JSONResponse(
        status_code=status.HTTP_200_OK,
        content={"status": "success", "messages": messages},
    )


@router.get("/messagesfrom/{username}")
def get_all_messages_from(
    username: str,
    x_auth_token: Optional[str] = Header(default=None),
):
    timestamp = datetime.now().strftime("%d/%m/%y/%H:%M:%S")

    if x_auth_token is None:
        return JSONResponse(
            status_code=status.HTTP_401_UNAUTHORIZED, content={"status": "unauthorized"}
        )
    try:
        to_user = jwt.decode(x_auth_token, key=secret, algorithms=["HS256"])["login"]
    except Exception as e:
        return JSONResponse(
            status_code=status.HTTP_401_UNAUTHORIZED, content={"status": e}
        )

    if not db.check_type(to_user, "support"):
        return JSONResponse(
            status_code=status.HTTP_403_FORBIDDEN,
            content={"status": "you have no rights"},
        )

    messages = db.get_messages_from(username)
    if len(messages) == 0:
        return JSONResponse(
            status_code=status.HTTP_204_NO_CONTENT,
            content={"status": f"no messages from {username}"},
        )

    return JSONResponse(
        status_code=status.HTTP_200_OK,
        content={"status": "success", "messages": messages},
    )
