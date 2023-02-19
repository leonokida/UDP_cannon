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
    int sock_escuta;
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

    if ((sock_escuta = socket(hp->h_addrtype, SOCK_DGRAM, 0)) < 0) {
        std::cerr << "Não consegui abrir o socket\n";
        exit(1);
    }

    if (bind(sock_escuta, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
        std::cerr << "Não consegui fazer o bind\n";
        exit(1);
    }

    while (true) {
        socklen_t isa_len = sizeof(isa);
        if (recvfrom(sock_escuta, buf, BUFSIZ, 0, (struct sockaddr *)&isa, &isa_len) < 0) {
            std::cerr << "Não consegui receber a mensagem\n";
            exit(1);
        }
        std::cout << "Sou o servidor, recebi " << buf << "\n";
        sendto(sock_escuta, buf, BUFSIZ, 0, (struct sockaddr *)&isa, isa_len);
    }

    close(sock_escuta);
    exit(0);
}
