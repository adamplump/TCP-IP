#include "client.h"
#include <iostream>
#include <cstring>
#include <winsock2.h>
#include <string>
#include <ws2tcpip.h>  
#include <opencv2/opencv.hpp>

#pragma comment(lib, "ws2_32.lib")  // Dodanie biblioteki Winsock

#define PORT 50000
//#define SERVER_IP "192.168.1.100"  // Ustaw IP serwera
#define SERVER_IP "127.0.0.1"

std::chrono::steady_clock::time_point startTimer;
std::chrono::steady_clock::time_point endTimer;
std::chrono::duration<double, std::milli> elapsedTimer;

void runClient() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET sock;
    sockaddr_in servAddr;

    // Wczytaj obraz i sprawdź, czy jest poprawny
    cv::String fileread = "C:\\Home\\PROJEKTY\\2024-11-07 Ikea - pomiar plyt\\3. Zdjęcia\\Camera3_dalsa.jpg";
    //cv::String fileread = "C:\\Home\\PROJEKTY\\2024-11-07 Ikea - pomiar plyt\\podpis.jpg";
    cv::Mat img = cv::imread(fileread, cv::IMREAD_GRAYSCALE);

    if (img.empty()) {
        std::cerr << "Błąd: obraz nie został poprawnie wczytany!\n";
        return;
    }

    startTimer = std::chrono::high_resolution_clock::now();
    // Kompresja obrazu do JPEG
    std::vector<uchar> buf;
    std::vector<int> params = { cv::IMWRITE_JPEG_QUALITY, 30,  // Obniżona jakość, szybka kompresja
    cv::IMWRITE_JPEG_OPTIMIZE, 0,  // Wyłączenie optymalizacji (przyspiesza)
    cv::IMWRITE_JPEG_PROGRESSIVE, 0 }; // Wyłączenie progresywnej kompresji
    cv::imencode(".jpg", img, buf, params);

    // Wczytanie i kompresja obrazu do WebP coś nie działa dla dużego obrazu
    //std::vector<uchar> buf;
    //std::vector<int> params = { cv::IMWRITE_WEBP_QUALITY, 50 };
    //cv::imencode(".webp", img, buf, params);

    endTimer = std::chrono::high_resolution_clock::now();
    elapsedTimer = endTimer - startTimer;
    std::cout << "czas kompresji obrazu: " << elapsedTimer.count() << std::endl;

    size_t imgSize = buf.size();
    int imgSizeNet = htonl(static_cast<int>(imgSize)); // Konwersja na Big Endian

    // Utworzenie gniazda
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

    startTimer = std::chrono::high_resolution_clock::now();
    // Wysyłanie rozmiaru obrazu
    if (send(sock, reinterpret_cast<const char*>(&imgSizeNet), sizeof(imgSizeNet), 0) == SOCKET_ERROR) {
        std::cerr << "Błąd wysyłania rozmiaru obrazu\n";
        closesocket(sock);
        return;
    }

    // Wysyłanie obrazu w pakietach
    int i = 0;
    size_t bytesSent = 0;
    while (bytesSent < imgSize) {
        int sent = send(sock, reinterpret_cast<const char*>(buf.data()) + bytesSent, imgSize - bytesSent, 0);
        ++i;
        if (sent == SOCKET_ERROR) {
            std::cerr << "Błąd wysyłania obrazu\n";
            closesocket(sock);
            return;
        }
        bytesSent += sent;
    }

    endTimer = std::chrono::high_resolution_clock::now();
    elapsedTimer = endTimer - startTimer;
    std::cout << "czas ładowania na server obrazu: " << elapsedTimer.count() << std::endl;

    std::cout << "Wysłano " << imgSize << " bajtów\n";
    closesocket(sock);
    WSACleanup();
}
