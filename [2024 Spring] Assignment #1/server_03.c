#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#define PORT 8090
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE (MAX_MSG_SIZE + 10)
#define QUEUE_NAME "/message_queue"
#define MAX_SERVERS 5

typedef struct {
    int id;
    mqd_t mq;
} Server;

Server servers[MAX_SERVERS];
int serverIdx = 0;

void *server_logic(void *arg) {
    Server server = *(Server *)arg;
    char buffer[MSG_BUFFER_SIZE];
    while (1) {
        ssize_t bytes_read = mq_receive(server.mq, buffer, MSG_BUFFER_SIZE, NULL);
        buffer[bytes_read] = '\0';
        printf("Server %d received: %s\n", server.id, buffer);
    }
}

void *handle_connection(void *socket_desc) {
    int sock = *(int*)socket_desc;
    long valread;
    char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain" \
                  "Content-Length: 20\n\nMy first web server!";
    char buffer[30000] = {0};
    valread = read(sock , buffer, 30000);
    printf("%s\n", buffer);

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    mqd_t mq = mq_open(QUEUE_NAME, O_CREAT | O_RDONLY, 0644, &attr);
    if (mq == -1) {
        perror("In mq_open");
        exit(EXIT_FAILURE);
    }

    if (attr.mq_curmsgs > MAX_MESSAGES * 0.8) {
        if (serverIdx < MAX_SERVERS) {
            servers[serverIdx].id = serverIdx;
            servers[serverIdx].mq = mq;
            pthread_t server_thread;
            if (pthread_create(&server_thread, NULL, server_logic, &servers[serverIdx]) < 0) {
                perror("could not create server thread");
                return NULL;
            }
            serverIdx++;
        }
    }

    sleep(5);
    write(sock, hello, strlen(hello));
    printf("-------------Hello message sent---------------\n");
    close(sock);
    mq_close(mq);
    free(socket_desc);

    return 0;
}

int main(int argc, char const *argv[]) {

    while(1) {

        if (pthread_create(&thread_id, NULL, handle_connection, (void*) new_sock) < 0) {
            perror("could not create thread");
            return 1;
        }
        
        printf("Handler assigned\n");
    }
    
    return 0;
}

