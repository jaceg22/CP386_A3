#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

#define RAND_MAX_DOUBLE ((double)RAND_MAX + 1)

long total_darts, darts_per_thread;
long points_in_circle = 0;
pthread_mutex_t lock;

// Function to generate a random double in range [0,1]
double random_double() {
    return rand() / RAND_MAX_DOUBLE;
}

// Thread function to throw darts
void* throw_darts(void* arg) {
    long count = 0;
    for (long i = 0; i < darts_per_thread; i++) {
        double x = random_double() * 2.0 - 1.0;
        double y = random_double() * 2.0 - 1.0;
        
        if ((x * x + y * y) <= 1.0) {  // Check if inside circle
            count++;
        }
    }

    // Update global count with thread safety
    pthread_mutex_lock(&lock);
    points_in_circle += count;
    pthread_mutex_unlock(&lock);
    
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s <NUMBER_OF_DARTS> <NUMBER_OF_THREADS>\n", argv[0]);
        return 1;
    }

    total_darts = atol(argv[1]);
    int num_threads = atoi(argv[2]);

    if (total_darts < 5000000 || num_threads < 2) {
        printf("The number of darts must be >= 5000000\n");
        printf("The number of threads must be >= 2\n");
        return 1;
    }

    pthread_t threads[num_threads];
    darts_per_thread = total_darts / num_threads;
    pthread_mutex_init(&lock, NULL);

    // Create threads
    for (int i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, throw_darts, NULL);
    }

    // Join threads
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&lock);

    // Estimate and print π
    double pi = 4.0 * points_in_circle / total_darts;
    printf("Pi = %f\n", pi);

    return 0;
}