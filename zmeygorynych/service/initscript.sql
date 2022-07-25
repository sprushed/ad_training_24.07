CREATE USER unpriv WITH PASSWORD '51mpl3p455w0rdbu7f0ru53r';
CREATE TABLE IF NOT EXISTS credVault (
	id serial PRIMARY KEY,
	username VARCHAR ( 64 ) UNIQUE NOT NULL,
	password VARCHAR ( 64 ) NOT NULL,
	money INTEGER,
	last_login TIMESTAMP
);
GRANT SELECT ON credVault TO unpriv;
CREATE TABLE IF NOT EXISTS products (
	id serial PRIMARY KEY,
	owner INTEGER,
	name VARCHAR ( 64 ) NOT NULL,
	description VARCHAR ( 1024 ),
	secret VARCHAR ( 256 ),
	cost INTEGER
);
CREATE TABLE IF NOT EXISTS shops (
	id serial PRIMARY KEY,
	owner INTEGER,
	name VARCHAR ( 64 ) NOT NULL,
	description VARCHAR ( 1024 ),
	secret VARCHAR ( 256 )
);
