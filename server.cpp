/*
O servidor do canhão UDP, recebe mensagens e estima erros, além de detectar mensagens fora de ordem.
Autores: Leon Augusto Okida Goncalves / Guilherme Carbonari Boneti
Ultima atualizacao: 19/02/2023
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
    // Número de sequência esperado
    unsigned int esperado = 0;
    // Pacotes perdidos
    int pacotesPerdidos = 0;
    // Pacotes que vieram fora de ordem
    int pacotesForaDeOrdem = 0;
    // Estima quantidade total de pacotes
    int pacotesTotais = 0;
    // Lista que armazena valores perdidos
    std::list<unsigned int> perdidos;

    while (true) {
        memset(buf, 0, BUFSIZ+1);
        socklen_t isa_len = sizeof(isa);

        // Recebe pacote com número de sequência
        if (recvfrom(sock_escuta, buf, BUFSIZ, 0, (struct sockaddr *)&isa, &isa_len) < 0) {
            std::cerr << "Não consegui receber a mensagem\n";
            exit(1);
        }
        // Recebeu mais um pacote
        pacotesTotais++;
        std::cout << "\n###############################\n";

        // Extrai número de sequência
        unsigned int recebido = atoi(buf);
        std::cout << "Esperado: " << esperado << ", Recebido: " << recebido << "\n";

        // Primeiro caso: pacote(s) perdido(s)
        if (recebido > esperado) {
            // O pacote que foi recebido veio fora de ordem (recebido antes dos perdidos serem recebidos)
            pacotesForaDeOrdem++;

            if (recebido - esperado > 1)
                std::cout << "Foram perdidos os pacotes de " << esperado << " a " << recebido - 1 << "\n";
            else
                std::cout << "Foi perdido o pacote " << esperado << "\n";

            for (unsigned int i = esperado; i < recebido; i++) {
                perdidos.push_back(i);
                // Adiciona à quantidade de pacotes perdidos
                pacotesPerdidos++;
                // Adiciona à quantidade total de pacotes
                pacotesTotais++;
            }
            esperado = recebido + 1;
        }
        // Segundo caso: pacote repetido ou fora de ordem
        if (recebido < esperado) {
            // Se estiver na lista de perdidos, significa que veio fora de ordem
            if (std::find(perdidos.begin(), perdidos.end(), recebido) != perdidos.end()) {
                std::cout << "O pacote " << recebido << " veio fora de ordem\n";
                perdidos.remove(recebido);
                pacotesForaDeOrdem++;
                // Pacote não está mais perdido
                pacotesPerdidos--;
            }
        }
        // Terceiro caso: recebido corretamente
        else {
            std::cout << "O pacote " << recebido << " veio corretamente\n";
            esperado++;
        }

        std::cout << "Quantidade estimada de pacotes enviados: " << pacotesTotais << ", ";
        std::cout << "Pacotes perdidos: " << pacotesPerdidos << ", ";
        std::cout << "Pacotes fora de ordem: " << pacotesForaDeOrdem << "\n";
        double taxa = (double)pacotesPerdidos / ((double)pacotesTotais) * 100;
        std::cout << "Taxa estimada de perda: " << taxa << "%\n";
        std::cout << "###############################\n";
    }

    close(sock_escuta);
    exit(0);
}
