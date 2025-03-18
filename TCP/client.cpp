#include "client.h"
#include <iostream>
#include <cstring>
#include <winsock2.h>
#include <string>
#include <ws2tcpip.h>  

#pragma comment(lib, "ws2_32.lib")  // Dodanie biblioteki Winsock

#define PORT 50000
//#define SERVER_IP "192.168.1.100"  // Ustaw IP serwera
#define SERVER_IP "127.0.0.1"

void runClient() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET sock;
    sockaddr_in servAddr;
    char buffer[1024] = { 0 };

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Błąd tworzenia gniazda\n";
        return;
    }

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP, &servAddr.sin_addr) <= 0) {
        std::cerr << "Błąd konwersji adresu IP\n";
        return;
    }

    if (connect(sock, (struct sockaddr*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) {
        std::cerr << "Błąd połączenia\n";
        return;
    }

    std::cout << "Połączono z serwerem!\n";

    while (true) {
        std::string message;
        std::cout << "Wpisz wiadomość: ";
        std::getline(std::cin, message);
        if (message == "exit") break;

        send(sock, message.c_str(), message.size(), 0);
        memset(buffer, 0, sizeof(buffer));
        recv(sock, buffer, sizeof(buffer), 0);
        std::cout << "Serwer: " << buffer << std::endl;
    }

    closesocket(sock);
    WSACleanup();
}
