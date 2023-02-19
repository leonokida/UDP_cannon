#Makefile que compila cliente e servidor.
#Autores: Leon Augusto Okida Gonçalves / Guilherme Carbonari Boneti
#Ultima atualizacao: 19/02/

CFLAGS = -Wall

all: server client

server: server.cpp
	g++ server.cpp -o server $(CFLAGS)

client: client.cpp $(objects)
	g++ client.cpp -o client $(CFLAGS)

clean:
	-rm -f server client