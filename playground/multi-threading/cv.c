#include <pthread.h>
#include <stdio.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int ready = 0;

void* child(void* arg)
{
    pthread_mutex_lock(&lock);
    ready = 1;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&lock);
}

void thr_join()
{
    pthread_mutex_lock(&lock);
    while(ready == 0)
        pthread_cond_wait(&cond, &lock);
    pthread_mutex_unlock(&lock);
    printf("successfully joined\n");
}

int main()
{
    pthread_t child_thr;
    pthread_create(&child_thr, NULL, child, NULL);
    thr_join();
    return 0;
}
