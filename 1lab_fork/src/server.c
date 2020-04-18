#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
  int delay;
  char buffer[sizeof(int)];
  int sockfd, ret, newSocket, bytes_read;
  struct sockaddr_in serverAddr;
  struct sockaddr_in newAddr;
  socklen_t addr_size;
  pid_t childpid;
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    printf("Error in connection.\n");
    perror("Socket");
    exit(1);
  }
  printf("Server started.\n");
  memset(&serverAddr, '\0', sizeof(serverAddr));
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(0);
  serverAddr.sin_addr.s_addr = INADDR_ANY;
  ret = bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
  getsockname(sockfd, (struct sockaddr *)&serverAddr, &addr_size);
  if (ret < 0) {
    printf("Error in binding.\n");
    perror("Bind");
    exit(2);
  }
  printf("Server port = %d\n", ntohs(serverAddr.sin_port));
  if (listen(sockfd, 10) == 0) {
    printf("Listening....\n");
  } else {
    printf("Error in binding.\n");
  }
  while (1) {
    newSocket = accept(sockfd, (struct sockaddr *)&newAddr, &addr_size);
    if (newSocket < 0) {
      perror("Accept");
      exit(1);
    }
    printf("Connection accepted from %s:%d\n", inet_ntoa(newAddr.sin_addr),
           ntohs(newAddr.sin_port));
    switch (fork()) {
      case -1:
        perror("Fork");
        break;
      case 0:
        close(sockfd);
        while (1) {
          bytes_read = recv(newSocket, buffer, sizeof(int), 0);
          if (bytes_read <= 0) break;
          printf("Reciving message from %d: %s\n", ntohs(newAddr.sin_port),
                 buffer);
        }
        close(newSocket);
        printf("Connection from %d closed\n", ntohs(newAddr.sin_port));
        exit(0);
      default:
        close(newSocket);
    }
  }
  close(sockfd);
  return 0;
}