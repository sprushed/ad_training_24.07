from math import floor, isqrt
from random import randrange, seed
from typing import List, Optional

from Crypto.Util.number import long_to_bytes
from gmpy2 import mpq
from pwn import xor


class Squre:
    enits: int = 10
    pos: List[int] = []
    d: int = 0
    K: int = 0

    def __init__(
        self,
        kdig: Optional[int] = None,
        ddig: Optional[int] = None,
        K: Optional[int] = None,
        init: Optional[str] = None,
    ):
        if not K:
            self.K = randrange(pow(10, kdig - 1), pow(10, kdig))
        else:
            self.K = K

        if not init and not ddig:
            ddig = (3 * len(str(self.K))) // 2

        if not init:
            self.d = ddig
            self.pos = list(range(ddig))

        elif not ddig:
            self.set_state(init)

    def frac(self) -> mpq:
        st0 = isqrt(self.K)
        a0 = self.K - st0**2
        b0 = 2 * st0
        a, b = a0, b0
        for i in range(self.enits):
            a, b = a0 * b, b0 * b + a
        return mpq(a, b)

    def get_alpha(self) -> int:
        frac = int(floor(10**self.d * self.frac()))
        frac = str(frac).zfill(self.d)
        alpha = ""
        for i in self.pos:
            alpha += frac[i]
        return alpha

    def next_round(self, alpha: int) -> None:
        seed(self.K + alpha)
        self.pos = [randrange(0, self.d) for _ in "_" * self.d]

    def set_state(self, init: str):
        self.d = len(init)
        self.next_round(int(init))

    def pad(self, mes: bytes, beta: bytes) -> bytes:
        if len(mes) < len(beta):
            mes = mes + b"\xff" * (len(beta) - len(mes))
        return mes

    def unpad(self, mes: bytes) -> bytes:
        return mes.strip(b"\xff").strip(b"\xfe")

    def encrypt(self, plaintext: bytes) -> bytes:
        alpha = int(self.get_alpha())
        beta = long_to_bytes(alpha)
        pt = [plaintext[i : i + len(beta)] for i in range(0, len(plaintext), len(beta))]
        pt[-1] = self.pad(pt[-1], beta)
        assert all(len(x) == len(beta) for x in pt)
        self.next_round(alpha)
        return b"".join([xor(beta, y) for y in pt])

    def decrypt(self, ciphertext) -> bytes:
        alpha = int(self.get_alpha())
        beta = long_to_bytes(alpha)
        if len(ciphertext) % len(beta) != 0:
            raise Exception("Invalid ciphertext or Initial State")
        ct = [
            ciphertext[i : i + len(beta)] for i in range(0, len(ciphertext), len(beta))
        ]
        self.next_round(alpha)
        pt = b"".join([xor(beta, y) for y in ct])
        return self.unpad(pt)


if __name__ == "__main__":
    k = 7524129794291415881873234780911478807841042669661578272217229956207778512260926591805685317049704219
    E = Squre(100, 100 + 20 + 2, K=k)
    print(E.K, E.pos)
    c1 = E.encrypt(b"pidoras" * 100)
    print(c1)
    c2 = E.encrypt(b"pidoras" * 100)
    print(c2)
    E = Squre(100, 100 + 20 + 2, K=k)
    m1 = E.decrypt(c1)
    m2 = E.decrypt(c2)
    print(m1, m2)
