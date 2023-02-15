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

    // Set the destination address and port
    struct sockaddr_in server_address;
    std::memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(1234);
    if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0) {
        std::cerr << "Invalid address/Address not supported" << std::endl;
        return 1;
    }

    // Send a message to the server
    std::string message = "Hello, server!";
    int bytes_sent = sendto(sock, message.c_str(), message.length(), 0, (struct sockaddr *)&server_address, sizeof(server_address));
    if (bytes_sent < 0) {
        std::cerr << "Error sending message" << std::endl;
        return 1;
    }

    // Close the socket
    close(sock);

    return 0;
}
