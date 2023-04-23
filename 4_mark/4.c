#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <signal.h>
#define MAX_BOOKS 50

sem_t *sem_books[MAX_BOOKS];

int books_num, readers_num;

void clean_all() {
    char buf[8];
    buf[0] = 'b';
    buf[1] = 'o';
    buf[2] = 'o';
    buf[3] = 'k';
    for (int i = 0; i < books_num; ++i) {
        sem_close(sem_books[i]);
        sprintf(buf + 4, "%d", i);
        sem_unlink(buf);
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

    char buf[8];
    buf[0] = 'b';
    buf[1] = 'o';
    buf[2] = 'o';
    buf[3] = 'k';
    for (int i = 0; i < books_num; ++i) {
        sprintf(buf + 4, "%d", i);
        sem_books[i] = sem_open(buf, O_CREAT, 0666, 1);
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
                char book_sem[8];
                book_sem[0] = 'b';
                book_sem[1] = 'o';
                book_sem[2] = 'o';
                book_sem[3] = 'k';
                sprintf(book_sem + 4, "%d", ind);
                sem_t *sem = sem_open(book_sem, O_CREAT, 0666);
                printf("Reader %d trying to take book %d\n", i, ind);
                sem_wait(sem);
                printf("Reader %d took book %d\n", i, ind);
                usleep(100000);
                printf("Reader %d returned book %d\n", i, ind);
                sem_post(sem);
            }
        }
    }

    for (int i = 0; i < readers_num; ++i) {
        wait(NULL);
    }

    clean_all();
    return 0;
}
