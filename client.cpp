/*
O cliente do canhão UDP, envia mensagens com número de sequência.
Autores: Leon Augusto Okida Goncalves / Guilherme Carbonari Boneti
Ultima atualizacao: 21/02/2023
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
    struct sockaddr_in sa;
    struct hostent *hp;
    char *host;
    unsigned int numMsgs;

    std::cout << "Iniciando execução do cliente\n";

    if (argc != 4) {
        std::cerr << "Uso correto: cliente <nome-servidor> <porta> <numero-pacotes>\n";
        exit(1);
    }

    host = argv[1];
    numMsgs = atoi(argv[3]);

    if ((hp = gethostbyname(host)) == NULL) {
        std::cerr << "Não consegui obter o endereço IP do servidor\n";
        exit(1);
    }

    std::cout << "Consegui obter o endereço IP do servidor\n";

    bcopy((char *)hp->h_addr, (char *)&sa.sin_addr, hp->h_length);
    sa.sin_family = hp->h_addrtype;

    sa.sin_port = htons(atoi(argv[2])); // faz a conversão little endian -> big endian

    if ((sockdescr = socket(hp->h_addrtype, SOCK_DGRAM, 0)) < 0) {
        std::cerr << "Não consegui abrir o socket\n";
        exit(1);
    }

    std::cout << "Consegui abrir o socket\n";
    std::cout << "Enviando mensagens\n";

    // Envia mensagens com número de sequência de 0 até o máximo suportado pelo tipo unsigned int
    for (unsigned int i = 0; i < numMsgs; i++) {
        std::string dadosStr = std::to_string(i);
        dadosStr = dadosStr + "-";
        const char * dados = dadosStr.c_str();
        std::cout << "Enviando pacote " << i << "\n";
        if (sendto(sockdescr, dados, strlen(dados), 0, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
            std::cerr << "Não consegui fazer a transmissão \n";
            exit(1);
        }
    }
    std::cout << "Foram enviadas todas as mensagens\n";
    std::cout << "Enviando fim de transmissão\n";

    // Espera 10 segundos antes de enviar fim de transmissão (para dar tempo para o servidor tratar mensagens recebidas)
    sleep(10);

    bool ack = false;
    char buf[BUFSIZ + 1];
    while (!ack) {
        // Envia fim de transmissão
        std::string dadosStr = "FIN";
        const char * dados = dadosStr.c_str();
        if (sendto(sockdescr, dados, strlen(dados), 0, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
            std::cerr << "Não consegui fazer a transmissão \n";
            exit(1);
        }
        // Recebe ACK
        socklen_t sa_len = sizeof(sa);
        if (recvfrom(sockdescr, buf, BUFSIZ, 0, (struct sockaddr *)&sa, &sa_len) < 0) {
            std::cerr << "Não consegui receber a mensagem\n";
            exit(1);
        }

        if ((strcmp(buf, "ACK")) == 0)
            ack = true;
    }

    std::cout << "Encerrando execução\n";

    close(sockdescr);

    return 0;
}
