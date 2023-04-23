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

    books_num = atoi(argv[1]);

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

    usleep(100000000);

    clean_all();
    return 0;
}
