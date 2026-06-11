#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>


void* receiveMessages(void *arg){
    int client_fd = *(int*)arg;
    char buffer[1024];

    while(1){
        memset(buffer, 0, sizeof(buffer));
    int bytes = recv(client_fd, buffer, sizeof(buffer), 0);

    if(bytes <= 0){
        printf("Client disconnected\n");
        break;
    }

printf("Client: %s\n", buffer);
        if(strcmp(buffer, "exit") == 0){
            break; 
        }
    }
    return NULL;
}

void* sendMessages(void *arg){
    int client_fd = *(int*)arg;
    char buffer[1024];

    while(1){
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0'; //ищет первый символ \n из buffer в строке и возвращает его индекс.
        send(client_fd, buffer, strlen(buffer) + 1, 0);
    }
    return NULL;
}




int main(){
    int server_fd, client_fd;
    struct sockaddr_in server_addr;
    char buffer[1024];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_fd, (struct sockaddr*)&server_addr,sizeof(server_addr));

    listen(server_fd, 5);

    printf("Waiting for connection...\n");

    client_fd = accept(server_fd, NULL, NULL);

    printf("Client succesfully connected!\n");

    pthread_t sender;
    pthread_t receiver;

    pthread_create(&sender, NULL, sendMessages, &client_fd);
    pthread_create(&receiver, NULL, receiveMessages, &client_fd);


    pthread_join(sender, NULL);
    pthread_join(receiver, NULL);

    close(client_fd);
    close(server_fd);
    return 0;
}