#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void *acceptClient(void *clientsd) {
  int connect = *(int *)clientsd;
  int read_size;
  char *message, mes[sizeof(int)];
  while ((read_size = recv(connect, mes, sizeof(int), 0)) > 0) {
    write(connect, mes, strlen(mes));
    printf("Receiving message: %s\n", mes);
    memset(mes, 0, sizeof(int));
  }
  if (read_size == 0) {
    printf("Client disconnected.\n");
    fflush(stdout);
  } else if (read_size == -1) {
    perror("Receive");
  }
  return 0;
}

int main(int argc, char const *argv[]) {
  struct sockaddr_in serverAddr, client;
  int sockfd, connection, c = sizeof(struct sockaddr_in);
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    printf("Error in connection.\n");
    perror("Socket");
    exit(1);
  }
  printf("Server started.\n");
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(0);
  serverAddr.sin_addr.s_addr = INADDR_ANY;
  if (bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
    printf("Error in binding.\n");
    perror("Bind");
    exit(2);
  }
  listen(sockfd, 3);
  getsockname(sockfd, (struct sockaddr *)&serverAddr, &c);
  printf("Server port = %d\n", ntohs(serverAddr.sin_port));
  c = sizeof(struct sockaddr_in);
  pthread_t thread_id;
  while ((connection =
              accept(sockfd, (struct sockaddr *)&client, (socklen_t *)&c))) {
    printf("Connection accepted from id: %d\n", thread_id);
    if (pthread_create(&thread_id, NULL, acceptClient, (void *)&connection) <
        0) {
      perror("Thread is not created.");
      return 1;
    }
  }
  if (connection < 0) {
    printf("Accept failed.\n");
    perror("Accept");
    exit(3);
  }
  return 0;
}