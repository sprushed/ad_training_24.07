from typing import Literal, Optional

from pydantic import BaseModel


class Registration(BaseModel):
    login: str
    password: str
    type: Literal["user", "support"]


class Login(BaseModel):
    login: str
    password: str


class ExchangeRequest(BaseModel):
    p: int
    g: int
    A: int


class SendB(BaseModel):
    p: int
    g: int
    B: int


class SymmetricKey(BaseModel):
    SK: int


class PostMessage(BaseModel):
    text: str
    timestamp: str


class GetMessage(BaseModel):
    text: str
