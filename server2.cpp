// server2.cpp
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include <fstream>
#include <sstream>
#include <chrono>
#include <cstdlib>
#include <mach/mach_host.h>
#include <mutex>

std::mutex server2Mutex;

// Функция для получения процента свободной физической памяти
auto getFreeMemoryPercentage()
{
    // get vmstat

    auto count = HOST_VM_INFO_COUNT;
    vm_statistics_data_t vmstat;
    auto host_port = mach_host_self();
    
    if (host_statistics(host_port, HOST_VM_INFO, (host_info_t)&vmstat, &count) != KERN_SUCCESS) {
        // TODO: handle error
    }

    // get page size

    
    vm_size_t pagesize;
    host_page_size(host_port, &pagesize);

    // calculate available RAM

    auto mem_free = vmstat.free_count * pagesize;

    return mem_free / 1024.0 / 1024.0;
}

// Функция для получения времени работы серверного процесса в пользовательском режиме
double getUserModeTime() {
    struct rusage r_usage;
    getrusage(RUSAGE_SELF, &r_usage);

    // r_usage.ru_utime содержит время в пользовательском режиме в структуре timeval
    return r_usage.ru_utime.tv_sec + r_usage.ru_utime.tv_usec / 1000000.0;
}


void handleClient(int clientSocket) {
    // Получение процента свободной памяти
    double freeMemoryPercentage = getFreeMemoryPercentage();

    // Получение времени работы серверного процесса в пользовательском режиме
    auto userModeTime = getUserModeTime();

    // Формирование ответа для клиента
    std::string response = "Free Memory: " + std::to_string(freeMemoryPercentage) + "%\n";
    response += "User Mode Time: " + std::to_string(userModeTime) + " seconds";

    {
        
        std::lock_guard<std::mutex> lock(server2Mutex);
        // Отправка ответа клиенту
        send(clientSocket, response.c_str(), response.length(), 0);
    }

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
    serverAddress.sin_port = htons(8082);  // Порт сервера 2
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // Привязка сокета к адресу и порту
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Ошибка при привязке сокета, сервер уже запущен.\n";
        return 2;
    }

    // Прослушивание порта
    if (listen(serverSocket, 10) == -1) {
        std::cerr << "Ошибка при прослушивании порта\n";
        return 3;
    }

    std::cout << "Сервер 2 запущен. Ожидание подключений...\n";

    while (true) {
        // Принятие запроса от клиента
        int clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket == -1) {
            std::cerr << "Ошибка при принятии запроса\n";
            continue;
        }

        // Обработка клиента в отдельном потоке
        std::thread(handleClient, clientSocket).detach();
    }

    // Закрытие сокета при завершении работы сервера
    close(serverSocket);

    return 0;
}
