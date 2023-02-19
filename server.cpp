/*
O servidor do canhão UDP, recebe mensagens e estima erros, além de detectar mensagens fora de ordem.
Autor: Leon Augusto Okida Gonçalves / Guilherme Carbonari Boneti
Ultima atualizacao: 18/02/2023
*/

#include <iostream>
#include <string>
#include <cstring>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h> // sistema DNS

#define TAMFILA 5
#define MAXHOSTNAME 30

int main(int argc, char *argv[]) {
    int sock_escuta, sock_atende;
    int i;
    char buf[BUFSIZ + 1];
    struct sockaddr_in sa, isa;
    struct hostent *hp;
    char localhost[MAXHOSTNAME];

    if (argc != 2) {
        std::cerr << "Uso correto: servidor <porta>\n";
        exit(1);
    }

    gethostname(localhost, MAXHOSTNAME);

    if ((hp = gethostbyname(localhost)) == NULL) {
        std::cerr << "Não consegui meu próprio endereço IP\n";
        exit(1);
    }

    sa.sin_port = htons(atoi(argv[1]));

    bcopy((char *)hp->h_addr, (char *)&sa.sin_addr, hp->h_length);

    sa.sin_family = hp->h_addrtype;

    if ((sock_escuta = socket(hp->h_addrtype, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Não consegui abrir o socket\n";
        exit(1);
    }

    if (bind(sock_escuta, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
        std::cerr << "Não consegui fazer o bind\n";
        exit(1);
    }

    listen(sock_escuta, TAMFILA);

    while (1) {
        i = sizeof(sa);
        if ((sock_atende = accept(sock_escuta, (struct sockaddr *) &isa, (socklen_t *)&i)) < 0) {
            std::cerr << "Não consegui aceitar a conexão\n";
            exit(1);
        }

        read(sock_atende, buf, BUFSIZ);
        std::cout << "Sou o servidor, recebi " << buf << "\n";
        write(sock_atende, buf, BUFSIZ);

        close(sock_atende);
    }

    exit(0);
}