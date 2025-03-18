#include "client.h"
#include "server.h"
#include <iostream>
#include <cstring>

int main() {
    std::cout << "Wybierz tryb: \n";
    std::cout << "1 - Serwer\n";
    std::cout << "2 - Klient\n";
    int choice;
    std::cin >> choice;
    std::cin.ignore();  // Usuniêcie znaku nowej linii

    if (choice == 1) {
        runServer();
    }
    else if (choice == 2) {
        runClient();
    }
    else {
        std::cout << "Niepoprawny wybór!\n";
    }

    return 0;
}
