#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#define cursup    "\033[A"
#define curshome  "\033[0;0H"

int input=0;
int first=0;
int second=0;
int third=0;
pthread_mutex_t lock_data;
pthread_mutex_t lock_input;

void PrintValues(void* ptr) {
    printf("Hello in thread\n");
    sleep(1);
    int i=0;    
    printf("==============================\n");
    printf("type the data you want to refresh\n");
    printf("First value = %d\n",first);
    printf("Second value = %d\n",second);
    printf("Third value = %d\n",third);
    while(1)
    {
        sleep(1);
        pthread_mutex_lock(&lock_data);
        switch (input)
        {
            case 1:
                printf(cursup);
                printf(cursup);
                printf(cursup);
                printf("\r");
                printf("First value = %d",first);
                printf("\n\n\n");
                input=0;
                break;
            case 2:
                printf(cursup);
                printf(cursup);
                printf("\r");
                printf("Second value = %d",second);
                printf("\n\n");
                input=0;
                break;
            case 3:
                printf(cursup);
                printf("\r");
                printf("Third value = %d",third);
                printf("\n");
                input=0;
                break;
            default:
                break;
        }
        pthread_mutex_unlock(&lock_data);
        fflush(stdout);  //comment this, to see the difference in O/P
    }
}

void Processing(void* ptr)
{
    printf("Hello in processing\n");
    sleep(1);
    while(1)
    {
        pthread_mutex_lock(&lock_data);
        first=rand()%5000;
        second=rand()%5000;
        third=rand()%5000;
        pthread_mutex_unlock(&lock_data);
        sleep(0.2);
    }
}

void Input(void* ptr)
{
    printf("Hello from Input");
    sleep(1);
    char str[100];
    while(1)
    {
        scanf("%d", &input);
        printf("\b");
        printf(cursup);
    }
}

int main(void) {
    if (pthread_mutex_init(&lock_data, NULL) != 0) {
        printf("\n mutex init has failed\n");
        return 1;
    }
    int* ptr;
    pthread_t tid_print;
    pthread_t tid_processing;
    pthread_t tid_input;
    printf("creating a new thread\n");
    pthread_create(&tid_print, NULL, (void*)PrintValues, 0);
    pthread_create(&tid_processing, NULL, (void*)Processing, 0);
    pthread_create(&tid_input, NULL, (void*)Input, 0);
    pthread_join(tid_print, (void**) &ptr);
    printf("joined..!!\n");
    return 0;
}