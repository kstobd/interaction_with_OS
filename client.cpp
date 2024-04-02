// client.cpp
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> // Добавленный заголовочный файл

void fetchDataFromServer(const char* serverIP, int serverPort) {
    // Создание сокета
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Ошибка при создании сокета\n";
        return;
    }

    // Установка адреса сервера
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(serverPort);
    
    // Важное изменение: inet_pton теперь используется правильно
    if (inet_pton(AF_INET, serverIP, &(serverAddress.sin_addr)) <= 0) {
        std::cerr << "Ошибка при преобразовании IP-адреса\n";
        close(clientSocket);
        return;
    }

    // Подключение к серверу
    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Ошибка при подключении к серверу\n";
        close(clientSocket);
        return;
    }

    // Получение данных от сервера
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead == -1) {
        std::cerr << "Ошибка при чтении данных от сервера\n";
    } else {
        std::cout << "Данные от сервера:\n" << buffer << std::endl;
    }

    // Закрытие сокета при завершении работы
    close(clientSocket);
}

int main() {
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
                serverIP = "127.0.0.1";
                break;
            case '2':
                serverPort = 8082;
                serverIP = "127.0.0.1";
                break;
            default:
                std::cout << "Некорректный ввод\n";
                continue;
        }

        fetchDataFromServer(serverIP, serverPort);
    }

    return 0;
}
