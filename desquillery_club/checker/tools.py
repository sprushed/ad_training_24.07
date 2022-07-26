from datetime import datetime


def parse_timestamp(text: str):
    spl = text.split("/")
    last = spl[-1].split(":")
    spl = spl[:-1] + last
    d, m, y, h, min, sec = spl
    return datetime(
        int(y), int(m), int(d), hour=int(h), minute=int(min), second=int(sec)
    )
