#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Использование: %s <IP сервера> <порт>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char* server_ip = argv[1];
    int port = atoi(argv[2]);

    // Создание сокета
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Ошибка создания сокета");
        return 1;
    }

    // Настройка адреса сервера
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("Ошибка в IP адресе");
        close(client_socket);
        return 1;
    }

    // Подключение к серверу
    printf("Подключаемся к серверу %s:%d...\n", server_ip, port);
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Ошибка подключения");
        close(client_socket);
        return 1;
    }
    printf("Успешно подключено!\n\n");

    int low = 1;
    int high = 1000;
    int guess;
    char buffer[BUFFER_SIZE];
    int attempt = 0;

    printf("Начинаем угадывать число от %d до %d\n", low, high);

    while (low <= high) {
        attempt++;
        guess = (low + high) / 2;
        
        // Формируем и отправляем предположение
        printf("Попытка %d: отправляем число %d\n", attempt, guess);
        snprintf(buffer, sizeof(buffer), "%d", guess);
        
        if (send(client_socket, buffer, strlen(buffer), 0) < 0) {
            perror("Ошибка отправки");
            break;
        }

        // Получаем ответ сервера
        int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) {
            perror("Ошибка получения данных или сервер закрыл соединение");
            break;
        }
        buffer[bytes_received] = '\0';
        
        // Анализируем ответ
        printf("Ответ сервера: %s\n", buffer);
        
        if (strstr(buffer, "Поздравляем")) {
            printf("\nУспех! Число угадано за %d попыток\n", attempt);
            break;
        } else if (strstr(buffer, "Больше")) {
            printf("Загаданное число больше %d\n", guess);
            low = guess + 1;
        } else if (strstr(buffer, "Меньше")) {
            printf("Загаданное число меньше %d\n", guess);
            high = guess - 1;
        } else {
            printf("Неизвестный ответ сервера\n");
        }
        
        printf("Новый диапазон поиска: %d - %d\n\n", low, high);
    }

    close(client_socket);
    printf("Соединение закрыто\n");
    return 0;
}
