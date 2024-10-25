/* count.c
 * Copyright 2024 FSO FCT Universidade Nova de lisboa
 */
#include <stdio.h>
#include <pthread.h>

int count=0;
char chartofind;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void* count_char_in_file(void *arg) {
    char *filename = (char *) arg;
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error");
        return NULL;
    }

    int local_count = 0;
    char c;
    while ((c = fgetc(file)) != EOF) {
        if (c == chartofind) {
            local_count++;
        }
    }
    fclose(file);

  
    pthread_mutex_lock(&mutex);
    count += local_count;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main(int argc, char **argv) {
    if (argc<3) {
        printf("usage: counter char file1 ...\n");
        return 1;
}

chartofind = argv[1][0];
int num_files = argc - 2;

pthread_t threads[10]; 

    
    for (int i = 0; i < num_files; i++) {
        pthread_create(&threads[i], NULL, count_char_in_file, argv[i + 2]);
    }

    
    for (int i = 0; i < num_files; i++) {
        pthread_join(threads[i], NULL);
    }

    
    printf( "Total count: %d\n", count);
    return 0;
}
