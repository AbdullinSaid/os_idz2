#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <signal.h>

int main(int argc, char *argv[]) {
    int books_num = atoi(argv[1]);
    int i = atoi(argv[2]);
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
    return 0;
}
