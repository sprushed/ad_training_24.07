import os
import sqlite3
from typing import Dict, List, Optional, Tuple, Union


class AbstractDatabase:
    def user_exists(self, username: str) -> bool:
        ...

    def get_list_of_users(self, username: str) -> List[str]:
        ...

    def create_user(
        self,
        username: str,
        password: str,
        type: str,
        params: Dict[str, int],
        timestamp: str,
    ) -> bool:
        ...

    def login_user(self, username: str, password: str) -> bool:
        ...

    def check_type(self, username: str, type: str) -> Optional[str]:
        ...

    def get_params(self, username: str) -> Dict[str, int]:
        ...

    def put_shared_key(self, from_user: str, B: int, shared_key: int) -> bool:
        ...

    def get_shared_key(self, from_user: str) -> Optional[int]:
        ...

    def put_symmetric_key(self, from_user: str, symmetric_key: int) -> bool:
        ...

    def update_state(self, from_user: str, state: str):
        ...

    def get_symmetric_params(self, from_user: str) -> Tuple[int]:
        ...

    def create_message(self, from_user: str, to_user: str, text: str, timestamp: str):
        ...

    def get_messages_to(self, to_user: str) -> List[Dict[str, str]]:
        ...

    def get_messages_from(self, from_user: str) -> List[Dict[str, str]]:
        ...

    def got_messages(self, username: str) -> bool:
        ...


class SqliteDatabase(AbstractDatabase):
    def __init__(self):
        if not os.path.exists("data"):
            os.mkdir("data")

        self.db_name = "data/desq.db"
        con = sqlite3.connect("data/desq.db")

        with open("db_init.sql", "r") as f:
            init_script = f.read()

        cursor = con.cursor()
        cursor.executescript(init_script)
        con.commit()
        con.close()

    def user_exists(self, username: str) -> bool:
        con = sqlite3.connect(self.db_name)
        cursor = con.cursor()
        res = cursor.execute(
            "SELECT * FROM users WHERE username = (?) LIMIT 1", (username,)
        ).fetchone()
        con.close()
        return res is not None

    def get_list_of_users(self, username: str) -> List[str]:
        con = sqlite3.connect(self.db_name)
        cursor = con.cursor()
        res = cursor.execute(
            "SELECT type FROM users WHERE username = (?) LIMIT 1", (username,)
        ).fetchone()
        type = res[0]

        cursor = con.cursor()
        if type == "user":
            res = cursor.execute(
                "SELECT username FROM users WHERE type = (?)", (type,)
            ).fetchone()
            con.close()
            return [(x, type) for x in res]
        res = cursor.execute("SELECT username, type FROM users").fetchall()
        con.close()
        return [tuple(x) for x in res]

    def check_type(self, username: str, type: str) -> bool:
        con = sqlite3.connect(self.db_name)
        cursor = con.cursor()
        res = cursor.execute(
            "SELECT type FROM users WHERE username = (?) LIMIT 1", (username,)
        ).fetchone()
        con.close()
        if res is None:
            return False

        return type == res[0]

    def create_user(
        self,
        username: str,
        password: str,
        type: str,
        params: Dict[str, int],
        timestamp: str,
    ):
        con = sqlite3.connect(self.db_name)
        cursor = con.cursor()
        cursor.execute(
            "INSERT INTO users (username, password, type, timestamp) VALUES (?, ?, ?, ?)",
            (username, password, type, timestamp),
        )
        con.commit()

        cursor = con.cursor()
        cursor.execute(
            "INSERT INTO params (username, p, g, A, a_private) VALUES (?, ?, ?, ?, ?)",
            (
                username,
                str(params["p"]),
                str(params["g"]),
                str(params["A"]),
                str(params["a"]),
            ),
        )
        con.commit()
        con.close()

    def login_user(self, username: str, password: str) -> bool:
        con = sqlite3.connect(self.db_name)
        cursor = con.cursor()
        res = cursor.execute(
            "SELECT 1 FROM users WHERE username = (?) AND password =(?) LIMIT 1",
            (username, password),
        ).fetchone()
        con.close()
        return res is not None

    def put_shared_key(self, to_user: str, B: int, shared_key: int) -> bool:
        con = sqlite3.connect(self.db_name)
        cursor = con.cursor()
        res = cursor.execute("SELECT 1 FROM users WHERE username = (?)", (to_user,))
        if res is None:
            con.close()
            return False

        cursor = con.cursor()
        res = cursor.execute(
            "SELECT shared_key FROM users WHERE username = (?) LIMIT 1", (to_user,)
        ).fetchone()
        if res[0] is not None:
            con.close()
            return False

        cursor = con.cursor()
        # params in parenthesis and trailing comma is important, desu
        res = cursor.execute(
            "SELECT B FROM params WHERE username = (?)", (to_user,)
        ).fetchone()
        if res[0] is not None:
            con.close()
            return False

        cursor = con.cursor()
        cursor.execute(
            "UPDATE users SET shared_key = (?) WHERE username = (?)",
            (
                str(shared_key),
                to_user,
            ),
        )
        con.commit()

        cursor = con.cursor()
        cursor.execute(
            "UPDATE params SET B = (?) WHERE username = (?)",
            (
                str(B),
                to_user,
            ),
        )
        con.commit()
        con.close()
        return True

    def get_params(self, user: str) -> Dict[str, int]:
        con = sqlite3.connect(self.db_name)
        cursor = con.cursor()

        res = cursor.execute(
            "SELECT p, g, a, b, a_private FROM params WHERE username = (?)", (user,)
        ).fetchone()
        p, g, A, B, a_private = res
        con.close()
        return {"p": int(p), "g": int(g), "A": int(A), "a": int(a_private)}

    def get_shared_key(self, to_user: str) -> Optional[int]:
        con = sqlite3.connect(self.db_name)
        cursor = con.cursor()
        res = cursor.execute(
            "SELECT shared_key FROM users WHERE username = (?)", (to_user,)
        ).fetchone()
        if res[0] is None:
            con.close()
            return None
        con.close()
        return int(res[0])

    def put_symmetric_key(self, from_user: str, symmetric_key: int) -> bool:
        con = sqlite3.connect(self.db_name)
        cursor = con.cursor()
        res = cursor.execute(
            "SELECT symmetric_key FROM users WHERE username = (?)", (from_user,)
        ).fetchone()

        if res[0] is not None:
            con.close()
            return False

        cursor = con.cursor()
        cursor.execute(
            "UPDATE users SET symmetric_key = (?) WHERE username = (?)",
            (
                str(symmetric_key),
                from_user,
            ),
        )
        con.commit()
        con.close()
        return True

    def update_state(self, from_user: str, state: str):
        con = sqlite3.connect(self.db_name)
        cursor = con.cursor()

        cursor.execute(
            "UPDATE users SET state = (?) WHERE username = (?)",
            (
                state,
                from_user,
            ),
        )
        con.commit()
        con.close()

    def get_symmetric_params(self, from_user: str) -> Tuple[Optional[Union[int, str]]]:
        con = sqlite3.connect(self.db_name)
        cursor = con.cursor()
        res = cursor.execute(
            "SELECT symmetric_key FROM users WHERE username = (?)", (from_user,)
        ).fetchone()
        if res[0] is None:
            con.close()
            return (None, None)
        sym_key = int(res[0])

        res = cursor.execute(
            "SELECT state FROM users WHERE username = (?)", (from_user,)
        ).fetchone()
        state = res[0]
        con.close()
        return (sym_key, state)

    def create_message(self, from_user: str, to_user: str, text: str, timestamp: str):
        con = sqlite3.connect(self.db_name)
        cursor = con.cursor()
        cursor.execute(
            "INSERT INTO messages (from_user, to_user, text, timestamp, checked) VALUES (?, ?, ?, ?, 0)",
            (from_user, to_user, text, timestamp),
        )
        con.commit()
        con.close()

    def get_messages_to(self, to_user: str) -> List[Dict[str, str]]:
        con = sqlite3.connect(self.db_name)
        cursor = con.cursor()
        res = cursor.execute(
            "SELECT * FROM messages WHERE to_user = (?)", (to_user,)
        ).fetchall()
        con.close()
        return [
            {
                "from_user": x[0],
                "to_user": x[1],
                "text": x[2],
                "timestamp": x[3],
                "checked": x[4],
            }
            for x in res
        ]

    def get_messages_from(self, from_user: str) -> List[Dict[str, str]]:
        con = sqlite3.connect(self.db_name)
        cursor = con.cursor()
        res = cursor.execute(
            "SELECT * FROM messages WHERE from_user = (?)", (from_user,)
        ).fetchall()
        con.close()
        return [
            {
                "from_user": x[0],
                "to_user": x[1],
                "text": x[2],
                "timestamp": x[3],
                "checked": x[4],
            }
            for x in res
        ]

    def got_messages(self, username: str) -> bool:
        con = sqlite3.connect(self.db_name)
        cursor = con.cursor()
        res = [
            x[0]
            for x in cursor.execute(
                "SELECT checked FROM messages WHERE to_user = (?)", (username,)
            ).fetchall()
        ]
        con.close()
        return all(res)

    def set_recieved(self, username: str):
        con = sqlite3.connect(self.db_name)
        cursor = con.cursor()

        cursor.execute(
            "UPDATE messages SET checked = 1 WHERE to_user = (?) and checked = 0",
            (username,),
        )
        con.commit()
        con.close()
