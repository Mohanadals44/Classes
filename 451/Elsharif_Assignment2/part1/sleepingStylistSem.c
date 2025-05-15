#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define CHAIRS 6
#define DELAY 1000  

sem_t mutex;
sem_t stylist;
sem_t customers;
int total_customers = 75;
int waiting = 0;
int haircuts_done = 0;

void* func_stylist(void *arg);
void* func_customer(void *arg);

int main(void)
{
    // initialize semaphores
    sem_init(&mutex,     0, 1);
    sem_init(&customers, 0, 0);
    sem_init(&stylist,   0, 0);
    // create a specified number of customer threads
    pthread_t customer_thread[75];
    for (int i = 0; i < 75; i++) {
        pthread_create(&customer_thread[i], NULL, func_customer, NULL);
    }
    // and a stylist thread. Don't forget to join threads
    pthread_t stylist_thread;
    pthread_create(&stylist_thread, NULL, func_stylist, NULL);
    for (int i = 0; i < 75; i++){
        pthread_join(customer_thread[i], NULL);
    }
    // destroy semaphores
    sem_destroy(&mutex);
    sem_destroy(&customers);
    sem_destroy(&stylist);
    return 0;
}

void* func_stylist(void *arg)
{
    int j;
    while(1) {
        sem_wait(&customers);
        sem_wait(&mutex);
        waiting = waiting - 1;
        total_customers --;
        printf("%d Customers are left\n", total_customers);
        sem_post(&stylist);
        sem_post(&mutex); 
        for (j = 0; j < DELAY; j++);    // cut hair
        haircuts_done++;
        printf("Hair cut received. Number of haircuts done: %d. Number of people waiting: %d\n", haircuts_done, waiting);
    }
    
    return NULL;
}

void* func_customer(void *arg)
{
    int j;
    while(1) {
        sem_wait(&mutex);
        if (waiting < CHAIRS) {
            waiting = waiting + 1;
            sem_post(&customers);
            sem_post(&mutex);
            sem_wait(&stylist);
            break;
        }
        else {
            sem_post(&mutex);
            for (j = 0; j < DELAY; j++); // go shopping
        }
    }
    return NULL;
}
