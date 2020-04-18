#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv) {
  char buffer[sizeof(int)];
  int clientSocket, ret;
  struct sockaddr_in serverAddr;
  int *value = calloc(1, sizeof(int));
  int send_value;
  if (argc != 4) {
    printf("Please indicate addres, port and message.\n");
    printf(
        "Command need to be like: ./client [server addres] [server port] "
        "[message].\n");
    exit(1);
  }
  clientSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (clientSocket < 0) {
    printf("Error in connection.\n");
    exit(1);
  }
  printf("Client is started.\n");
  memset(&serverAddr, '\0', sizeof(serverAddr));
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(atoi(argv[2]));
  serverAddr.sin_addr.s_addr = inet_addr(argv[1]);
  ret =
      connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
  if (ret < 0) {
    printf("Connection to server failed !\n");
    perror("Connect");
    exit(2);
  }
  printf("Connected to server on port: %d.\n", ntohs(serverAddr.sin_port));
  for (int i = 0; i < atoi(argv[3]); i++) {
    memcpy(buffer, argv[3], sizeof(int));
    printf("Send to server: %s\n", buffer);
    send(clientSocket, buffer, sizeof(int), 0);
    sleep(atoi(argv[3]));
  }
}