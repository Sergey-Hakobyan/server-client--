#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

void* receiveMessages(void* arg){
    int sock = *(int*)arg;
    char buffer[1024];

    while(1){
        memset(buffer, 0, sizeof(buffer));

        int bytes = recv(sock, buffer, sizeof(buffer), 0);

        if(bytes <= 0){
            printf("Server disconnected\n");
            break;
        }

        printf("\nServer: %s\n", buffer);

        if(strcmp(buffer, "exit") == 0)
            break;
    }

    return NULL;
}

void* sendMessages(void* arg){
    int sock = *(int*)arg;
    char buffer[1024];

    while(1){
        printf("You: ");

        fgets(buffer, sizeof(buffer), stdin);

        buffer[strcspn(buffer, "\n")] = '\0';

        send(sock, buffer, strlen(buffer) + 1, 0);

        if(strcmp(buffer, "exit") == 0)
            break;
    }

    return NULL;
}

int main(){
    int sock;
    struct sockaddr_in server_addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);

    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));

    printf("Connected to server!\n");

    pthread_t sender;
    pthread_t receiver;

    pthread_create(&sender, NULL, sendMessages, &sock);
    pthread_create(&receiver, NULL, receiveMessages, &sock);

    pthread_join(sender, NULL);
    pthread_join(receiver, NULL);

    close(sock);

    return 0;
} 