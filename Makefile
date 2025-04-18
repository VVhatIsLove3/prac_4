# Определяем компилятор и флаги
CC = gcc
CFLAGS = -Wall -Wextra -g

# Определяем исполняемые файлы
SERVER = server
CLIENT = client

# Определяем исходные файлы
SERVER_SRC = server.c
CLIENT_SRC = client.c

# Правила сборки
all: $(SERVER) $(CLIENT)

$(SERVER): $(SERVER_SRC)
	$(CC) $(CFLAGS) -o $(SERVER) $(SERVER_SRC)

$(CLIENT): $(CLIENT_SRC)
	$(CC) $(CFLAGS) -o $(CLIENT) $(CLIENT_SRC)

# Правило для очистки сгенерированных файлов
clean:
	rm -f $(SERVER) $(CLIENT)
