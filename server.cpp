#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    // Create a UDP socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        std::cerr << "Error creating socket" << std::endl;
        return 1;
    }

    // Bind the socket to a local address and port
    struct sockaddr_in server_address;
    std::memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(1234);
    if (bind(sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        std::cerr << "Error binding socket" << std::endl;
        return 1;
    }

    // Receive messages and print them to the console
    char buffer[1024];
    struct sockaddr_in client_address;
    socklen_t client_address_len = sizeof(client_address);
    while (true) {
        std::memset(buffer, 0, sizeof(buffer));
        int bytes_received = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_address, &client_address_len);
        if (bytes_received < 0) {
            std::cerr << "Error receiving message" << std::endl;
            continue;
        }
        std::string message(buffer, bytes_received);
        std::cout << "Received message from " << inet_ntoa(client_address.sin_addr) << ":" << ntohs(client_address.sin_port) << " - " << message << std::endl;
    }

    // Close the socket
    close(sock);

    return 0;
}
