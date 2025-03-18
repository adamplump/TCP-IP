#include <iostream>
#include <cstring>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")  // Dodanie biblioteki Winsock

#define PORT 50000

void runServer() {

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET serverSocket, clientSocket;
    sockaddr_in serverAddr, clientAddr;
    int addrlen = sizeof(clientAddr);
    char buffer[1024] = { 0 };

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "B��d tworzenia gniazda\n";
        return;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "B��d bindowania\n";
        return;
    }

    listen(serverSocket, 3);
    std::cout << "Serwer dzia�a na porcie " << PORT << ". Oczekiwanie na po��czenie...\n";

    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrlen);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "B��d akceptowania po��czenia\n";
        return;
    }

    std::cout << "Po��czono z klientem!\n";

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int valread = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (valread <= 0) {
            std::cout << "Po��czenie zako�czone.\n";
            break;
        }
        std::cout << "Klient: " << buffer << std::endl;
        std::string response = "Serwer otrzyma�: " + std::string(buffer);
        send(clientSocket, response.c_str(), response.size(), 0);
    }

    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();
}
