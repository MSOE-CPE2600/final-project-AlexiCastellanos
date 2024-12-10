#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#define SHM_KEY 1234

typedef struct {
    int vote_count[3];
    int total_votes;
} SharedData;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <candidate: A/B/C>\n", argv[0]);
        return 1;
    }

    int candidate = argv[1][0] - 'A';  // Map A/B/C to 0/1/2
    if (candidate < 0 || candidate > 2) {
        printf("Invalid candidate. Choose A, B, or C.\n");
        return 1;
    }

    int shmid = shmget(SHM_KEY, sizeof(SharedData), 0666);
    SharedData *data = (SharedData *)shmat(shmid, NULL, 0);

    data->vote_count[candidate]++;
    data->total_votes++;

    printf("Vote submitted for Candidate %c.\n", argv[1][0]);

    shmdt(data);
    return 0;
}
