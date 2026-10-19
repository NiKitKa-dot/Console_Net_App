#include <iostream>
#include <cstring>
#include <winsock2.h>
#include <ws2tcpip.h> // Для inet_pton, inet_ntop

// Подключаем библиотеку Winsock для линковки (для MSVC)
#pragma comment(lib, "ws2_32.lib")

// Параметры варианта 13
const char* SERVER_IP = "127.0.0.1";
const int SERVER_PORT = 9000;

int main() {
    setlocale(LC_ALL, "Russian"); // Для корректного вывода русских букв в консоли

    // ШАГ 1: Инициализация Winsock (специфика Windows)
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cerr << "Ошибка WSAStartup: " << iResult << std::endl;
        return 1;
    }


    // ШАГ 2: Создание сокета
    SOCKET sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET) {
        std::cerr << "Ошибка socket: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }
    std::cout << "Сокет создан успешно. Дескриптор (SOCKET): " << sockfd << std::endl;

    // ШАГ 3: Заполнение структуры адреса

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr)); // Обязательно обнуляем!

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT); // Преобразование порта в сетевой порядок

    // Преобразование IP-адреса из текста в бинарный формат
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        std::cerr << "Ошибка: неверный формат IP-адреса '" << SERVER_IP << "'" << std::endl;
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }
    std::cout << "Адрес заполнен: " << SERVER_IP << ":" << SERVER_PORT << std::endl;

    //ШАГ 4: Вывод адреса в текстовом виде (проверка)

    char ip_str[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, &server_addr.sin_addr, ip_str, sizeof(ip_str)) == NULL) {
        std::cerr << "Ошибка inet_ntop: " << WSAGetLastError() << std::endl;
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }
    std::cout << "Проверка адреса: " << ip_str << ":" << ntohs(server_addr.sin_port) << std::endl;

    //ШАГ 5: Привязка сокета к адресу (bind)

    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "Ошибка bind: " << WSAGetLastError() << std::endl;
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }
    std::cout << "Сокет успешно привязан к " << SERVER_IP << ":" << SERVER_PORT << std::endl;

    // ШАГ 6: Вывод информации о сокете

    std::cout << "\n--- Информация о сокете ---" << std::endl;
    std::cout << "Файловый дескриптор (SOCKET): " << sockfd << std::endl;
    std::cout << "Семейство адресов: AF_INET (" << server_addr.sin_family << ")" << std::endl;
    // порт в шестнадцатеричном формате. 9000 = 0x2328
    std::cout << "Порт (сетевой): 0x" << std::hex << ntohs(server_addr.sin_port) << std::dec << std::endl;
    std::cout << "IP-адрес: " << ip_str << std::endl;

    //ШАГ 7: Закрытие сокета и очистка

    if (closesocket(sockfd) == SOCKET_ERROR) {
        std::cerr << "Ошибка closesocket: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }
    std::cout << "\nСокет закрыт. Дескриптор " << sockfd << " освобождён." << std::endl;

    WSACleanup(); // Завершаем работу с Winsock
    return 0;
}
