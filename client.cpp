/*
O cliente do canhão UDP, envia mensagens com número de sequência.
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

int main(int argc, char *argv[]) {
    int sockdescr;
    int numbytesrecv;
    struct sockaddr_in sa;
    struct hostent *hp;
    char buf[BUFSIZ+1];
    char *host;
    char *dados;

    if (argc != 4) {
        std::cerr << "Uso correto: cliente <nome-servidor> <porta> <dados>\n";
        exit(1);
    }

    host = argv[1];
    dados = argv[3];

    if ((hp = gethostbyname(host)) == NULL) {
        std::cerr << "Não consegui obter o endereço IP do servidor\n";
        exit(1);
    }

    bcopy((char *)hp->h_addr, (char *)&sa.sin_addr, hp->h_length);
    sa.sin_family = hp->h_addrtype;

    sa.sin_port = htons(atoi(argv[2])); // faz a conversão little endian -> big endian

    if ((sockdescr = socket(hp->h_addrtype, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Não consegui abrir o socket\n";
        exit(1);
    }

    if (connect(sockdescr, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
        std::cerr << "Não consegui conectar ao servidor\n";
        exit(1);
    }

    if (write(sockdescr, dados, strlen(dados)) < 0) {
        std::cerr << "Não consegui fazer a transmissão \n";
        exit(1);
    }

    read(sockdescr, buf, BUFSIZ);
    std::cout << "Cliente recebeu: " << buf << "\n";

    close(sockdescr);

    return 0;
}