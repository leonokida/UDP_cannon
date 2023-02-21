/*
O servidor do canhão UDP, recebe mensagens e estima erros, além de detectar mensagens fora de ordem.
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
#include <list>
#include <algorithm>
#include <iostream>
#include <fstream>

#define MAXHOSTNAME 30

int main(int argc, char *argv[]) {
    int sock_escuta;
    char buf[BUFSIZ + 1];
    struct sockaddr_in sa, isa;
    struct hostent *hp;
    char localhost[MAXHOSTNAME];

    std::cout << "Iniciando execução do servidor\n";

    if (argc != 2) {
        std::cerr << "Uso correto: servidor <porta>\n";
        exit(1);
    }

    gethostname(localhost, MAXHOSTNAME);

    if ((hp = gethostbyname(localhost)) == NULL) {
        std::cerr << "Não consegui meu próprio endereço IP\n";
        exit(1);
    }

    std::cout << "Consegui meu próprio endereço IP\n";

    sa.sin_port = htons(atoi(argv[1]));

    bcopy((char *)hp->h_addr, (char *)&sa.sin_addr, hp->h_length);

    sa.sin_family = hp->h_addrtype;

    if ((sock_escuta = socket(hp->h_addrtype, SOCK_DGRAM, 0)) < 0) {
        std::cerr << "Não consegui abrir o socket\n";
        exit(1);
    }

    std::cout << "Consegui abrir o socket\n";

    if (bind(sock_escuta, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
        std::cerr << "Não consegui fazer o bind\n";
        exit(1);
    }

    std::cout << "Consegui fazer o bind\n";
    std::cout << "Pronto para receber mensagens\n";

    // Lista que armazena números de sequência recebidos
    std::list<unsigned int> recebidos;

    bool loop = true;
    while (loop) {
        memset(buf, 0, BUFSIZ+1);
        socklen_t isa_len = sizeof(isa);

        // Recebe
        int num_bytes;
        if ((num_bytes = recvfrom(sock_escuta, buf, BUFSIZ, 0, (struct sockaddr *)&isa, &isa_len)) < 0) {
            std::cerr << "Não consegui receber a mensagem\n";
            exit(1);
        }
        
        // Recebeu fim de transmissão
        if (strcmp(buf, "FIN") == 0) {
            std::cout << "Recebi fim de transmissão\n";
            // Termina o loop
            loop = false;
            // Envia resposta
            std::string dadosStr = "ACK";
            const char * dados = dadosStr.c_str();
            if (sendto(sock_escuta, dados, strlen(dados), 0, (struct sockaddr *) &isa, sizeof(isa)) < 0) {
                std::cerr << "Não consegui fazer a transmissão \n";
                exit(1);
            }
        }
        // Recebeu pacote com número de sequência
        else {
            // Faz o split do buffer com strtok (pois pode conter vários números)
            char *token = strtok(buf, "-");

            // Processa cada token
            while (token != NULL) {
                // Transforma em número com atoi e adiciona à lista de números de sequência recebidos
                unsigned int numeroRecebido = atoi(token);
                std::cout << "Recebi o número de sequência " << numeroRecebido << "\n";
                recebidos.push_back(numeroRecebido);
                token = strtok(NULL, "-");
            }
        }
    }
    std::cout << "Terminei de receber as mensagens\n";
    close(sock_escuta);

    // Inicia processamento das mensagens recebidas
    std::cout << "Iniciando análise das mensagens recebidas\n";

    // Abre arquivo de log
    std::ofstream log("log.txt");

    // Indica a mensagem que deveria ter sido recebida
    unsigned int esperado = 0;

    // Lista que armazena mensagens perdidas
    std::list<unsigned int> perdidos;

    // Contador de mensagens fora de ordem
    unsigned int mensagensForaDeOrdem = 0;

    std::list<unsigned int>::iterator it;
    for (it = recebidos.begin(); it != recebidos.end(); ++it){
        log << "*******************************************\n";
        log << "* Esperado: " << esperado << ", Recebido: " << *it << "\n";

        // Pacote veio igual ao esperado
        if (*it == esperado) {
            log << "* Pacote " << *it << " recebido era esperado\n";
            esperado++;
        }
        // Pacote menor que o esperado (repetido ou fora de ordem)
        else if (*it < esperado) {
            if (std::find(perdidos.begin(), perdidos.end(), *it) != perdidos.end()) {
                log << "* Pacote " << *it << " veio fora de ordem\n";
                perdidos.remove(*it);
                mensagensForaDeOrdem++;
            }
        }
        // Pacote maior que o esperado (pode ter tido perdas)
        else {
            if (*it - esperado > 1)
                log << "* Foram perdidos os pacotes de " << esperado << " a " << *it - 1 << "\n";
            else
                log << "* Foi perdido o pacote " << esperado << "\n";

            for (unsigned int i = esperado; i < *it; i++)
                perdidos.push_back(i);
            
            esperado = *it + 1;
        }
        log << "*******************************************\n\n";
    }

    log << "Pacotes recebidos: " << recebidos.size() << ", Pacotes perdidos: " << perdidos.size() << ", Pacotes fora de ordem: " << mensagensForaDeOrdem << "\n";
    log << "Taxa estimada de perdas: " <<  (double)perdidos.size()/(double)recebidos.size() * 100 << "%\n";

    log.close();
    std::cout << "Resultados disponíveis em log.txt\n";

    exit(0);
}
