CREATE TABLE IF NOT EXISTS users (
username TEXT PRIMARY KEY NOT NULL UNIQUE,
password TEXT NOT NULL,
type TEXT NOT NULL,
timestamp TEXT NOT NULL,
shared_key TEXT,
symmetric_key TEXT,
state TEXT);

CREATE TABLE IF NOT EXISTS params (
username TEXT PRIMARY KEY NOT NULL UNIQUE,
p TEXT NOT NULL,
g TEXT NOT NULL,
A TEXT NOT NULL,
B TEXT,
a_private TEXT NOT NULL,
FOREIGN KEY(username) REFERENCES users(username));

CREATE TABLE IF NOT EXISTS messages (
from_user TEXT NOT NULL,
to_user TEXT NOT NULL,
text TEXT NOT NULL,
timestamp TEXT NOT NULL,
checked INTEGER NOT NULL,
FOREIGN KEY(from_user) REFERENCES users(username),
FOREIGN KEY(to_user) REFERENCES users(username));
