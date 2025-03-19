#include <iostream>
#include <cstring>
#include <winsock2.h>
#include <opencv2/opencv.hpp>

#pragma comment(lib, "ws2_32.lib")  // Dodanie biblioteki Winsock

#define PORT 50000

//std::chrono::steady_clock::time_point startTimer;
//std::chrono::steady_clock::time_point endTimer;
//std::chrono::duration<double, std::milli> elapsedTimer;

void runServer() {

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET serverSocket, clientSocket;
    sockaddr_in serverAddr, clientAddr;
    int addrlen = sizeof(clientAddr);
    char buffer[1024] = { 0 };

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "B³¹d tworzenia gniazda\n";
        return;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "B³¹d bindowania\n";
        return;
    }

    listen(serverSocket, 3);
    std::cout << "Serwer dzia³a na porcie " << PORT << ". Oczekiwanie na po³¹czenie...\n";

    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrlen);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "B³¹d akceptowania po³¹czenia\n";
        return;
    }

    std::cout << "Po³¹czono z klientem!\n";

    //startTimer = std::chrono::high_resolution_clock::now();

    // Odbiór rozmiaru obrazu
    int imgSizeNet;
    if (recv(clientSocket, reinterpret_cast<char*>(&imgSizeNet), sizeof(imgSizeNet), 0) <= 0) {
        std::cerr << "B³¹d odbioru rozmiaru obrazu\n";
        closesocket(clientSocket);
        return;
    }

    int imgSize = ntohl(imgSizeNet);
    std::cout << "Odebrany rozmiar obrazu: " << imgSize << " bajtów\n";

    // Odbiór obrazu
    std::vector<uchar> data(imgSize);
    int i = 0;
    size_t bytesReceived = 0;
    while (bytesReceived < imgSize) {
        int received = recv(clientSocket, reinterpret_cast<char*>(data.data()) + bytesReceived, imgSize - bytesReceived, 0);
        ++i;
        if (received <= 0) {
            std::cerr << "B³¹d odbioru obrazu\n";
            closesocket(clientSocket);
            return;
        }
        bytesReceived += received;
        std::cout << "odebrane bajty " << bytesReceived << " bajtów\n";
    }

    //endTimer = std::chrono::high_resolution_clock::now();
    //elapsedTimer = endTimer - startTimer;
    //std::cout << "czas obierania obrazu: " << elapsedTimer.count() << std::endl;

    //startTimer = std::chrono::high_resolution_clock::now();
    // Dekodowanie obrazu
    cv::Mat img = cv::imdecode(data, cv::IMREAD_COLOR);

    //endTimer = std::chrono::high_resolution_clock::now();
    //elapsedTimer = endTimer - startTimer;
    //std::cout << "czas dekodowania obrazu: " << elapsedTimer.count() << std::endl;

    if (img.empty()) {
        std::cerr << "B³¹d dekodowania obrazu!\n";
    }
    else {
        cv::namedWindow("Odebrany obraz", cv::WINDOW_NORMAL);
        cv::imshow("Odebrany obraz", img);
        cv::waitKey(0);
    }

    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();
}
