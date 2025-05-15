#include <stdio.h>
#include <semaphore.h>

typedef struct cond
{
    int    threads_count;            
    sem_t  threads_counter_mutex;    
    sem_t  cond_mutex;               
} cond;

int  cond_count(cond *c);
void cond_init(cond *c);
void cond_wait(cond *c, sem_t *mutex);
void cond_signal(cond *c);

void cond_init(cond *c) {
    c->threads_count = 0;
    sem_init(&c->threads_counter_mutex, 0, 1);
    sem_init(&c->cond_mutex,            0, 0);
}

int cond_count(cond *c)
{
    int n;
    // create an atomic read of the thread count
    sem_wait(&c->threads_counter_mutex);
    n = c->threads_count;
    sem_post(&c->threads_counter_mutex);
    return n;
}

void cond_wait(cond *c, sem_t *mutex){
    // create an atomic update for the thread count 
    sem_wait(&c->threads_counter_mutex);
    c->threads_count++;
    sem_post(&c->threads_counter_mutex);

    sem_post(mutex);
    // wait for signal so ill block the cond mutex until other 
    sem_wait(&c->cond_mutex);
    sem_wait(mutex);
}

void cond_signal(cond *c) {
    int should_signal = 0;
    // Check if there are threads waiting and decrement the count
    sem_wait(&c->threads_counter_mutex);
    if (c->threads_count > 0) {
        c->threads_count--;
        should_signal = 1;
    }
    sem_post(&c->threads_counter_mutex);
    
    // If there are waiting threads, signal one of them
    if (should_signal) {
        sem_post(&c->cond_mutex);
    }
}
