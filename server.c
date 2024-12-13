#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#define SHM_NAME "/voting_shm"
#define MAX_VOTERS 100
#define MAX_NAME_LEN 50

typedef struct {
    int vote_count[3];                           // Vote counts for (A), (B), (C)
    int total_votes;                             // Total votes cast
    char voters[MAX_VOTERS][MAX_NAME_LEN];       // List of voter usernames to ensure no duplicates
    int voter_count;                             // Number of unique voters
} SharedData;

SharedData *data;
int shm_fd;

// Signal handler for graceful shutdown
void handle_sigint(int sig) {
    (void)sig;  

    printf("\nVoting ended. Final Results:\n");
    printf(" Canidate 1(A): %d (%.2f%%)\n", data->vote_count[0],(data->vote_count[0] * 100.0) / data->total_votes);
    printf(" Canidate 2(B): %d (%.2f%%)\n", data->vote_count[1],(data->vote_count[1] * 100.0) / data->total_votes);
    printf(" Canidate 3(C): %d (%.2f%%)\n", data->vote_count[2],(data->vote_count[2] * 100.0) / data->total_votes);

    // Write results to a file
    FILE *file = fopen("voting_results.txt", "w");
    if (file) {
        fprintf(file, "(A): %d\n(B): %d\n(C): %d\n",
                data->vote_count[0], data->vote_count[1], data->vote_count[2]);
        fclose(file);
    } else {
        perror("Error writing to file");
    }

    // Detach and remove shared memory
    if (munmap(data, sizeof(SharedData)) == -1) {
        perror("Error detaching shared memory");
    }
    if (close(shm_fd) == -1) {
        perror("Error closing shared memory file descriptor");
    }
    if (shm_unlink(SHM_NAME) == -1) {
        perror("Error removing shared memory");
    }

    printf("Server shutting down. Shared memory cleaned.\n");
    exit(0);
}

int main() {
    signal(SIGINT, handle_sigint);

   // printf("Size of SharedData: %lu bytes\n", sizeof(SharedData));  // Debug print

    // Create shared memory
    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("Error creating shared memory");
        exit(1);
    }

    if (ftruncate(shm_fd, sizeof(SharedData)) == -1) {
        perror("Error setting size of shared memory");
        close(shm_fd);
        shm_unlink(SHM_NAME);
        exit(1);
    }

    data = (SharedData *)mmap(NULL, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (data == MAP_FAILED) {
        perror("Error mapping shared memory");
        close(shm_fd);
        shm_unlink(SHM_NAME);
        exit(1);
    }

    memset(data, 0, sizeof(SharedData));  // Initialize shared memory
   // printf("Shared memory created and attached successfully!\n");
    printf("Voting server started. Press Ctrl+C to stop voting.\n");

    int last_total_votes = 0;

    while (1) {
        if (data->total_votes > last_total_votes) {
            printf("New vote received! Total votes: %d\n", data->total_votes);
            last_total_votes = data->total_votes;
        }
        sleep(1);
    }

    return 0;
}
