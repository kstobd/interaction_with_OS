// client.cpp
#include <iostream>
#include <cstring>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib") // Link with the Winsock library

void fetchDataFromServer(const char* serverIP, int serverPort) {
    // Инициализация библиотеки Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Ошибка при инициализации Winsock\n";
        return;
    }

    // Создание сокета
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Ошибка при создании сокета\n";
        WSACleanup();
        return;
    }

    // Установка адреса сервера
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(serverPort);

    // Используйте inet_pton для преобразования строки в IP-адрес
    serverAddress.sin_addr.s_addr = inet_addr(serverIP);
    if (serverAddress.sin_addr.s_addr == INADDR_NONE) {
        std::cerr << "Ошибка при преобразовании IP-адреса\n";
        closesocket(clientSocket);
        WSACleanup();
        return;
    }



    // Подключение к серверу
    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cerr << "Ошибка при подключении к серверу\n";
        closesocket(clientSocket);
        WSACleanup();
        return;
    }

    // Получение данных от сервера
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead == SOCKET_ERROR) {
        std::cerr << "Ошибка при чтении данных от сервера\n";
    } else {
        std::cout << "Данные от сервера:\n" << buffer << std::endl;
    }

    // Закрытие сокета при завершении работы
    closesocket(clientSocket);
    WSACleanup();
}

int main() {
    // Установка кодировки консоли
    SetConsoleOutputCP(CP_UTF8);
    // Инициализация библиотеки Winsock

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Ошибка при инициализации Winsock\n";
        return 1;
    }

    while (true) {
        std::cout << "Выберите сервер (1 или 2) или введите 'q' для выхода: ";
        char choice;
        std::cin >> choice;

        if (choice == 'q') {
            break;
        }

        int serverPort;
        const char* serverIP;

        switch (choice) {
            case '1':
                serverPort = 8081;
                serverIP = "192.168.0.61";
                break;
            case '2':
                serverPort = 8082;
                serverIP = "192.168.0.61";
                break;
            default:
                std::cout << "Некорректный ввод\n";
                continue;
        }

        fetchDataFromServer(serverIP, serverPort);
    }

    // Завершение использования библиотеки Winsock
    WSACleanup();

    return 0;
}
