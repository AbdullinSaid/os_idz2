#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/wait.h>

#define MAX_BOOKS 50

sem_t *sem_books[MAX_BOOKS];

int books_num, readers_num;

void clean_all() {
    for (int i = 0; i < books_num; ++i) {
        sem_close(sem_books[i]);
    }
}

void sigint_handler(int signum) {
    clean_all();
    exit(-1);
}

int main(int argc, char *argv[]) {
    signal(SIGINT, sigint_handler);
    signal(SIGTERM, sigint_handler);

    scanf("%d %d", &books_num, &readers_num);

    for (int i = 0; i < books_num; ++i) {
        sem_books[i] = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        sem_init(sem_books[i], 1, 1);
        if (sem_books[i] == SEM_FAILED) {
            perror("sem_open");
            exit(-1);
        }
    }

    for (int i = 0; i < readers_num; ++i) {
        int pid = fork();
        if (pid < 0) {
            printf("Error creating process\n");
            return 1;
        } else if (pid == 0) {
            srand(1234 * (i + 1));
            for (int j = 0; j < 5; ++j) {
                int ind = rand() % books_num;
                printf("Reader %d trying to take book %d\n", i, ind);
                sem_wait(sem_books[ind]);
                printf("Reader %d took book %d\n", i, ind);
                usleep(100000);
                printf("Reader %d returned book %d\n", i, ind);
                sem_post(sem_books[ind]);
            }
        }
    }

    for (int i = 0; i < readers_num; ++i) {
        wait(NULL);
    }

    clean_all();
    return 0;
}
