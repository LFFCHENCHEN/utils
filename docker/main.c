#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
void *thread_function(void *arg) {
    while (1)
    {
        printf("Thread is running...\n");
        sleep(10);
    }
    return NULL;
}
int main() {
    printf("Hello, Docker!\n");
    pthread_t thread;
    if (pthread_create(&thread, NULL, thread_function, NULL) != 0) {
        fprintf(stderr, "Error creating thread\n");
        return 1;
    }
    pthread_join(thread, NULL);
    return 0;
}