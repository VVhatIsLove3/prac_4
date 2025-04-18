#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define BUFFER_SIZE 1024

void handle_client(int client_socket, struct sockaddr_in client_addr) {
    char buffer[BUFFER_SIZE];
    int number_to_guess = 1 + rand() % 1000; // Случайное число от 1 до 1000
    printf("Сервер загадал число: %d\n", number_to_guess);
    int count_of_attempt = 0;

    printf("Подключен клиент: %s:%d\n", 
           inet_ntoa(client_addr.sin_addr), 
           ntohs(client_addr.sin_port));

    while (1) {
        int bytes_arrived = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_arrived <= 0)
            break;
        
        buffer[bytes_arrived] = '\0';
        int number_from_guess = atoi(buffer);
        count_of_attempt++;

        printf("Клиент предположил: %d\n", number_from_guess);

        if (number_from_guess < number_to_guess) {
            snprintf(buffer, sizeof(buffer), 
                    "Больше. Загаданное число: %d", number_to_guess);
        } else if (number_from_guess > number_to_guess) {
            snprintf(buffer, sizeof(buffer), 
                    "Меньше. Загаданное число: %d", number_to_guess);
        } else {
            snprintf(buffer, sizeof(buffer), 
                    "Поздравляем! Вы угадали число %d за %d попыток", 
                    number_to_guess, count_of_attempt);
            send(client_socket, buffer, strlen(buffer), 0);
            break;
        }
        send(client_socket, buffer, strlen(buffer), 0);
    }

    printf("Отключен клиент: %s:%d\n", 
           inet_ntoa(client_addr.sin_addr), 
           ntohs(client_addr.sin_port));
    close(client_socket);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <порт>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    srand(time(NULL));

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Ошибка создания сокета");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr))) {
        perror("Ошибка привязки сокета");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 5) < 0) {
        perror("Ошибка прослушивания");
        exit(EXIT_FAILURE);
    }

    printf("Сервер запущен на порту %d\n", port);

    while (1) {
        int client_socket = accept(server_socket, 
                                  (struct sockaddr*)&client_addr, 
                                  &client_len);
        if (client_socket < 0) {
            perror("Ошибка принятия соединения");
            continue;
        }
        handle_client(client_socket, client_addr);
    }

    close(server_socket);
    return 0;
}
