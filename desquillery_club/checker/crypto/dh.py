from random import choice, randint
from typing import Optional

from crypto.secure_dh_params import gs, p


class DHext:
    P: Optional[int] = None
    G: Optional[int] = None
    a: Optional[int] = None
    A: Optional[int] = None
    B: Optional[int] = None
    Shared_Secret: Optional[int] = None

    def __init__(
        self,
        P: Optional[int] = None,
        G: Optional[int] = None,
        a: Optional[int] = None,
        Shared: Optional[int] = None,
    ):
        if not P:
            self.P = p
        else:
            self.P = P

        if not Shared:
            if not G:
                self.G = choice(gs)
            else:
                self.G = G
            if not a:
                self.a = randint(2, p - 1)
            else:
                self.a = a
            self.A = pow(self.G, self.a, self.P)
        else:
            self.Shared_Secret = Shared

    def calcShared(self, B: int):
        self.B = B
        self.Shared_Secret = pow(self.B, self.a, self.P)

    def encrypt(self, pt: int):
        if not self.Shared_Secret:
            raise Exception("no known key")
        if pt > self.P:
            raise Exception(ValueError)
        return (pt * self.Shared_Secret) % self.P

    def decrypt(self, ct: int):
        return (ct * pow(self.Shared_Secret, -1, self.P)) % self.P
