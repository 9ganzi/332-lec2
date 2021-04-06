#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))  // return the smaller value of the two

void *north(void *arg);
void *south(void *arg);

static pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;  // mutex for multiple north processes
static pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;  // mutex for multiple south processes
static pthread_mutex_t mutex3 = PTHREAD_MUTEX_INITIALIZER;  // mutex for north and south functions

int nFarmers = 0, sFarmers = 0;     // keep track of number of farmers from north and south
int n, s;                           // user input of number of north farmers and south farmers
bool nWait = false, sWait = false;  // these values make sure farmer from the other side have priority over framer from the same side

int main() {
    printf("please enter the number of North Turnbridge farmer: ");
    scanf("%d", &n);
    printf("please enter the number of South Turnbridge farmer: ");
    scanf("%d", &s);

    pthread_t nFarmersT[n];  // threads of north farmers
    pthread_t sFarmersT[s];  // threads of south farmers
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    int i;
    for (i = 0; i < MIN(n, s); i++) {
        pthread_create(&nFarmersT[i], &attr, north, NULL);
        pthread_create(&sFarmersT[i], &attr, south, NULL);
    }

    if (n != s) {
        if (i == n)  // when n is smaller
            for (int j = i; i < s; i++)
                pthread_create(&sFarmersT[j], &attr, south, NULL);
        else  // when s is smaller
            for (int j = i; i < n; i++)
                pthread_create(&nFarmersT[i], &attr, north, NULL);
    }

    for (i = 0; i < MIN(n, s); i++) {
        pthread_join(nFarmersT[i], NULL);
        pthread_join(sFarmersT[i], NULL);
    }

    if (n != s) {
        if (i == n)  // when n is smaller
            for (int j = i; i < s; i++)
                pthread_join(sFarmersT[i], NULL);
        else  // when s is smaller
            for (int j = i; i < n; i++)
                pthread_join(nFarmersT[i], NULL);
    }

    return 0;
}

void *north(void *arg) {
    pthread_mutex_lock(&mutex1);  // lock other north farmer threads
    pthread_mutex_lock(&mutex3);  // lock south farmer threads
    srand(time(0));
    int random = 1 + (rand() % 3);  // get random integer [1,2]
    printf("North Tunbridge #%d farmer can cross the bridge\n", nFarmers + 1);
    printf("North Tunbridge #%d is traveling on the bridge\n", nFarmers + 1);
    sleep(random);
    printf("North Tunbridge #%d farmer has left the bridge\n\n", nFarmers + 1);
    nFarmers++;  // increment number of north farmers crossed the bridge
    nWait = true;
    sWait = false;
    pthread_mutex_unlock(&mutex3);  // unlock south farmer threads
    while (nWait)                   // wait until south farmer cross the bridge
        if (sFarmers == s)          // when all south farmers crossed the bridge
            break;
    pthread_mutex_unlock(&mutex1);  // unlock other north farmer threads
    pthread_exit(0);
}

void *south(void *arg) {
    pthread_mutex_lock(&mutex2);  // lock other south farmer threads
    pthread_mutex_lock(&mutex3);  // lock north farmer threads
    srand(time(0));
    int random = 1 + (rand() % 3);
    printf("South Tunbridge #%d farmer can cross the bridge\n", sFarmers + 1);
    printf("South Tunbridge #%d is traveling on the bridge\n", sFarmers + 1);
    sleep(random);
    printf("South Tunbridge #%d farmer has left the bridge\n\n", sFarmers + 1);
    sFarmers++;  // increment number of south farmers crossed the bridge
    sWait = true;
    nWait = false;
    pthread_mutex_unlock(&mutex3);  // unlock north farmer threads
    while (sWait)                   // wait until north farmer cross the bridge
        if (nFarmers == n)          // when all north farmers crossed the bridge
            break;
    pthread_mutex_unlock(&mutex2);  // unlock other south farmer threads
    pthread_exit(0);
}