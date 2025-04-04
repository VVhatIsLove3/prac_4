#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Использование: %s <адрес сервера> <порт>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *server_ip = argv[1];
    int port = atoi(argv[2]);

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0)
    {
        perror("Ошибка при открытии сокета");
        return 1;
    }
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    inet_pton(AF_INET, server_ip, &server_address.sin_addr);
        // Подключаемся к серверу
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror("Ошибка при подключении к серверу");
        return 1;
    }
    else
    {
        connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address));
    }

    int low = 1;
    int high = 1000;
    int chislo;
    char buffer[BUFFER_SIZE];
    int count_of_attempt = 0;

    while (low <= high)
    {
        chislo = (low + high) / 2;  // Используем бинарный поиск
        count_of_attempt++;
        printf("Попытка %d: Угадываю число %d\n", count_of_attempt, chislo);
        snprintf(buffer, sizeof(buffer), "%d", chislo);
        send(client_socket, buffer, strlen(buffer), 0);

        int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        buffer[bytes_received] = '\0';
        printf("Сервер: %s\n", buffer);
        if (strstr(buffer, "Поздравляем!") != NULL)
            break;
        else
        {
            if(strstr(buffer, "Загаданное число больше")!=NULL)
                low=chislo+1;
            if (strstr(buffer, "Загаданное число меньше")!=NULL)
                high=chislo-1;
        }

    }
    close(client_socket);
    return 0;
}
