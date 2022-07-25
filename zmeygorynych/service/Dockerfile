from archlinux:latest

RUN useradd -u 1000 unpriv
RUN pacman --noconfirm -Syyu && pacman --noconfirm -S libpqxx base-devel gcc vim postgresql-libs fmt socat
COPY ./app /app/
WORKDIR /app
USER unpriv
