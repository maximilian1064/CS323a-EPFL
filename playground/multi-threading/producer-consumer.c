#include <pthread.h>
#include <stdio.h>
#include <assert.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
// Prevent one problem: mixing conditions
pthread_cond_t cond_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_empty = PTHREAD_COND_INITIALIZER;
int buffer;
int count = 0;

// fill buffer
void put(int val)
{
    assert(count == 0);
    count = 1;
    buffer = val;
}

// empty buffer
int get()
{
    assert(count == 1);
    count = 0;
    return buffer;
}

// use while instead of if, lest somebody sneaks in and change condition
// before waking thread get the lock and after the signaling thread send
// signals
void* producer(void* arg)
{
    int i;
    for(i=0; i<10; i++)
    {
        pthread_mutex_lock(&lock);
        while(count)
            pthread_cond_wait(&cond_empty, &lock);
        put(i);
        pthread_cond_signal(&cond_full);
        pthread_mutex_unlock(&lock);
    }
}

void* consumer(void* arg)
{
    int i;
    for(i=0; i<5; i++)
    {
        pthread_mutex_lock(&lock);
        while(!count)
            pthread_cond_wait(&cond_full, &lock);
        printf("%d\n", get());
        pthread_cond_signal(&cond_empty);
        pthread_mutex_unlock(&lock);
    }
}

int main()
{
    pthread_t producer_thr, consumer_1, consumer_2;
    pthread_create(&producer_thr, NULL, producer, NULL);
    pthread_create(&consumer_1, NULL, consumer, NULL);
    pthread_create(&consumer_2, NULL, consumer, NULL);
    pthread_join(producer_thr, NULL);
    pthread_join(consumer_1, NULL);
    pthread_join(consumer_2, NULL);
    return 0;
}
