#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>
#include "monitor.c"
#include <unistd.h>

#define CHAIRS 6
#define DELAY 10000
int i = 0;
// global var
cond stylistAvailable, customerAvailable;
int customer = 0;
int stylist = 0;
sem_t mutex;
int totalHaircuts = 0;
int salonFullCount = 0;
int salonEmptyCount = 0;

void * funcCustomer(void *arg);
void * funcStylist(void *arg);
void * mon_checkCustomers(void *arg);
int mon_checkStylist(void *arg);
void mon_debugPrint();

int main(void) {
    sem_init(&mutex, 0, 1);
    cond_init(&customerAvailable);
    cond_init(&stylistAvailable);

    pthread_t customer_thread[75];
    for (int i = 0; i < 75; i++) {
        pthread_create(&customer_thread[i], NULL, funcCustomer, NULL);
    }
    pthread_t stylist_thread;
    pthread_create(&stylist_thread, NULL, funcStylist, NULL);

    for (int i = 0; i < 75; i++) {
        pthread_join(customer_thread[i], NULL);
    }

    pthread_cancel(stylist_thread);
    pthread_join(stylist_thread, NULL);

    printf("Total haircuts served: %d\n", totalHaircuts);
    printf("Salon full = %d times\n", salonFullCount);
    printf("Salon empty = %d times\n", salonEmptyCount);

    sem_destroy(&mutex);
    return 0;
}

void * funcStylist(void *arg){
    int j;
    while (1) {
        mon_debugPrint();
        mon_checkCustomers(arg);
        usleep(15);    // cut hair
        sem_wait(&mutex);
        totalHaircuts++;
        printf("Finished haircut #%d\n", totalHaircuts);
        sem_post(&mutex);
    }
    return NULL;
}

void * funcCustomer(void *arg){
    int j;
    while (1) {
        mon_debugPrint();
        if (mon_checkStylist(arg)) {
            break;
        }
        int num = (rand() % 90) + 1;  
        usleep(num * 100);
    }
    return NULL;
}

void * mon_checkCustomers(void *arg){
    stylist++;
    cond_signal(&customerAvailable);
    if (customer == 0) {
        salonEmptyCount++;
        cond_wait(&customerAvailable, &mutex);
    }
    customer--;
    return NULL;
}

int mon_checkStylist(void *arg){
    int status = 0;
    if (customer < CHAIRS) {
        customer++;
        cond_signal(&customerAvailable);
        if (stylist == 0) {
            cond_wait(&stylistAvailable, &mutex);
        }
        stylist--;
        status = 1;
    } else {
        salonFullCount++;
    }
    sem_post(&mutex);
    return status;
}

void mon_debugPrint(void)
{    
    // Lock the mutex 
    sem_wait(&mutex);
    int occupied = customer;

    for (int i = 0; i < CHAIRS; i++) {
        printf("|%d|", (i < occupied) ? 1 : 0);
    }
    printf(" => %d\n", occupied);
    printf("Given haircuts = %d\n", totalHaircuts);
    printf("Salon full = %d times\n", salonFullCount);
    printf("Salon empty = %d times\n", salonEmptyCount);

    sem_post(&mutex);
}

