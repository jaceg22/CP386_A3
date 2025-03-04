#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

#define RAND_MAX_DOUBLE ((double)RAND_MAX+1)
long totalDarts, darts_per_thread;
long circle_points=0;
pthread_mutex_t lock;
double random_double(){
    return rand()/RAND_MAX_DOUBLE;
}
void* throw_darts(void* arg){
    long count=0;
    for (long i=0; i<darts_per_thread; i++){
        double x=random_double()*2.0-1.0;
        double y=random_double()*2.0-1.0;
        if ((x*x+y*y)<=1.0){
            count++;
        }
    }
    pthread_mutex_lock(&lock);
    circle_points+=count;
    pthread_mutex_unlock(&lock);
    return NULL;
}
int main(int argc, char* argv[]){
    if (argc!=3){
        fprintf(stderr, "usage: pi <NUMBER_OF_DARTS> <NUMBER_OF_THREADS>\n");
        return 1;
    }
    totalDarts=atol(argv[1]);
    int numThreads=atoi(argv[2]);
    if (totalDarts<5000000){
        fprintf(stderr, "The number of darts must be >= 5000000\n");
        return 1;
    }
    if (numThreads<2){
        fprintf(stderr, "The number of threads must be >= 2\n");
        return 1;
    }
    pthread_t threads[numThreads];
    darts_per_thread=totalDarts/numThreads;
    pthread_mutex_init(&lock, NULL);
    for (int i=0; i<numThreads; i++){
        pthread_create(&threads[i], NULL, throw_darts, NULL);
    }
    for (int i=0; i<numThreads; i++){
        pthread_join(threads[i], NULL);
    }
    pthread_mutex_destroy(&lock);
    double pi=4.0*circle_points/totalDarts;
    printf("Pi = %f\n", pi);
    return 0;
}
