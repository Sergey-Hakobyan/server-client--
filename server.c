#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdlib.h>



int clients[100];
int clientCount = 0;
pthread_mutex_t mutex;


void* handleClient(void* arg){
    int client_fd = *(int*)arg;
    free(arg);
    char buffer[1024];

    while(1){
        memset(buffer, 0, sizeof(buffer));
        int bytes = recv(client_fd, buffer, sizeof(buffer), 0);
        
        if(bytes<=0){
            printf("Client[%d] disconnected", client_fd);
            break;
        };
        pthread_mutex_lock(&mutex);
        printf("Client[%d]: %s\n",client_fd, buffer);
        pthread_mutex_unlock(&mutex);
    }
    close(client_fd);

    return NULL;

}

int main(){
    int server_fd;
    struct sockaddr_in server_addr;
    

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_fd, (struct sockaddr*)&server_addr,sizeof(server_addr));

    listen(server_fd, 5);

    printf("Waiting for connection...\n");

    pthread_mutex_init(&mutex, NULL);
    while(1){

        int* client_ptr = calloc(1, sizeof(int));

        *client_ptr = accept(server_fd, NULL, NULL);

        if(*client_ptr < 0){
            free(client_ptr);
            continue; //back to while
        }
        clients[clientCount++] = *client_ptr;
        printf("client connected succesfully, Socket = %d\n", *client_ptr);
        pthread_t thread;

        pthread_create(&thread, NULL, handleClient, client_ptr);
        pthread_detach(thread);
    }
    close(server_fd);
    return 0;
}