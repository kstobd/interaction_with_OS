#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include <ctime>
#include <mutex>
#include <iomanip>
#include <arpa/inet.h>

// #include <fstream>


std::mutex server1Mutex; // Мьютекс для обеспечения безопасности работы с ресурсами

std::string formatSessionDuration(time_t currentTime) {
    // Используем структуру tm для представления времени в различных компонентах
    struct tm *localTime = localtime(&currentTime);

    // Буфер для форматированного времени
    char buffer[20]; // Максимальная длина времени в формате HH:MM:SS - 8 символов
    strftime(buffer, sizeof(buffer), "%T", localTime);

    return buffer;
}

// void log(const std::string& message) {
//     std::ofstream logFile("server_log.txt", std::ios_base::app);
//     logFile << message << "\n";
// }

void handleClient(int clientSocket) {
    // Получение продолжительности текущего сеанса
    time_t currentTime = time(nullptr);
    std::string sessionDuration = formatSessionDuration(currentTime);

    // Получение текущего часового пояса
    char timeZone[256];
    strftime(timeZone, sizeof(timeZone), "%z", localtime(&currentTime));

    // Формирование ответа для клиента
    std::string response = "Session Duration: " + sessionDuration + "\nTime Zone: " + timeZone;

    {
        std::lock_guard<std::mutex> lock(server1Mutex); // Защита доступа к ресурсам
        // Отправка ответа клиенту
        send(clientSocket, response.c_str(), response.length(), 0);
    }

    // log("Handled client connection");

    // Закрытие сокета после отправки ответа
    close(clientSocket);
}

int main() {
    // Создание сокета
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Ошибка при создании сокета\n";
        return 1;
    }

    // Установка адреса сервера
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8081);  // Порт сервера 1
    serverAddress.sin_addr.s_addr = INADDR_ANY;
   // Преобразование IP-адреса из строки в структуру in_addr
    // if (inet_pton(AF_INET, "192.168.0.1", &(serverAddress.sin_addr)) <= 0) {
    //     perror("inet_pton");
    //     return 4; // или другой код ошибки
    // }

    // Привязка сокета к адресу и порту
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Ошибка при привязке сокета\n";
        return 2;
    }

    // Прослушивание порта
    if (listen(serverSocket, 10) == -1) {
        std::cerr << "Ошибка при прослушивании порта\n";
        return 3;
    }

    std::cout << "Сервер 1 запущен. Ожидание подключений...\n";

    while (true) {
        // Принятие запроса от клиента
        int clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket == -1) {
            std::cerr << "Ошибка при принятии запроса\n";
            continue;
        }

        // Обработка клиента в отдельном потоке
        std::thread(handleClient, clientSocket).detach();

        // log("Accepted client connection");
    }

    // Закрытие сокета при завершении работы сервера
    close(serverSocket);

    return 0;
}
