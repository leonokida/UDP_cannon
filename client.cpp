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

    if (argc != 3) {
        std::cerr << "Uso correto: cliente <nome-servidor> <porta> <dados>\n";
        exit(1);
    }

    host = argv[1];

    if ((hp = gethostbyname(host)) == NULL) {
        std::cerr << "Não consegui obter o endereço IP do servidor\n";
        exit(1);
    }

    bcopy((char *)hp->h_addr, (char *)&sa.sin_addr, hp->h_length);
    sa.sin_family = hp->h_addrtype;

    sa.sin_port = htons(atoi(argv[2])); // faz a conversão little endian -> big endian

    if ((sockdescr = socket(hp->h_addrtype, SOCK_DGRAM, 0)) < 0) {
        std::cerr << "Não consegui abrir o socket\n";
        exit(1);
    }

    for (unsigned int i = 0; i <= 4294967295; i++) {
        std::string dadosStr = std::to_string(i);
        const char * dados = dadosStr.c_str();
        if (sendto(sockdescr, dados, strlen(dados), 0, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
            std::cerr << "Não consegui fazer a transmissão \n";
            exit(1);
        }
        std::cout << "Enviando pacote " << i << "\n";
    }

    close(sockdescr);

    return 0;
}
