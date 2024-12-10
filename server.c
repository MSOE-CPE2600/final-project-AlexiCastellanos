#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#define SHM_KEY 1234
#define MAX_VOTES 10

typedef struct {
    int vote_count[3];  // For 3 candidates
    int total_votes;
} SharedData;

int main() {
    int shmid = shmget(SHM_KEY, sizeof(SharedData), 0666 | IPC_CREAT);
    if (shmid < 0) {
        perror("shmget");
        exit(1);
    }

    SharedData *data = (SharedData *)shmat(shmid, NULL, 0);
    memset(data, 0, sizeof(SharedData));

    printf("Voting server started. Waiting for votes...\n");

    while (data->total_votes < MAX_VOTES) {
        sleep(1);  // Simulate real-time update
        printf("Votes so far: %d\n", data->total_votes);
    }

    printf("Final vote counts:\nCandidate A: %d\nCandidate B: %d\nCandidate C: %d\n",
           data->vote_count[0], data->vote_count[1], data->vote_count[2]);

    FILE *file = fopen("voting_results.txt", "w");
    fprintf(file, "Candidate A: %d\nCandidate B: %d\nCandidate C: %d\n",
            data->vote_count[0], data->vote_count[1], data->vote_count[2]);
    fclose(file);

    shmdt(data);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}
