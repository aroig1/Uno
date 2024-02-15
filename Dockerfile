FROM gcc:latest

COPY . /Uno

WORKDIR /Uno/

RUN gcc -o Uno UnoMain.c

CMD ["./Uno"]