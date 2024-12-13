/**
* Name: Alexi Castellanos
* Filename: server.c
* CPE2600 - Lab 13 Final Project
*
* Description:This program acts as the backend for a voting system, handling shared memory
* to store votes and voter information. The server ensures that the voting
* process runs smoothly and records the results upon termination.
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

// Define shared memory name and constants
#define SHM_NAME "/voting_shm"  // Name of the shared memory segment
#define MAX_VOTERS 100          // Maximum number of voters
#define MAX_NAME_LEN 50         // Maximum length of a voter's username

// Structure to hold voting data
typedef struct {
    int vote_count[3];                 // Array to store vote counts for candidates A, B, and C
    int total_votes;                   // Total number of votes cast
    char voters[MAX_VOTERS][MAX_NAME_LEN]; // Array to store usernames of voters
    int voter_count;                   // Count of unique voters
} SharedData;

SharedData *data;  // Pointer to shared memory data
int shm_fd;        // File descriptor for shared memory

// Signal handler for graceful shutdown.
void handle_sigint(int sig) {
    (void)sig;  // Suppress unused parameter warning

    printf("\nVoting ended. Final Results:\n");
    printf(" Candidate 1 (A): %d (%.2f%%)\n", data->vote_count[0], (data->vote_count[0] * 100.0) / data->total_votes);
    printf(" Candidate 2 (B): %d (%.2f%%)\n", data->vote_count[1], (data->vote_count[1] * 100.0) / data->total_votes);
    printf(" Candidate 3 (C): %d (%.2f%%)\n", data->vote_count[2], (data->vote_count[2] * 100.0) / data->total_votes);

    // Write results to a file
    FILE *file = fopen("voting_results.txt", "w");
    if (file) {
        fprintf(file, "Candidate A: %d\nCandidate B: %d\nCandidate C: %d\n",
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
    // Register signal handler for graceful shutdown
    signal(SIGINT, handle_sigint);

    // Create shared memory
    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("Error creating shared memory");
        exit(1);
    }

    // Set size of shared memory
    if (ftruncate(shm_fd, sizeof(SharedData)) == -1) {
        perror("Error setting size of shared memory");
        close(shm_fd);
        shm_unlink(SHM_NAME);
        exit(1);
    }

    // Map shared memory to process address space
    data = (SharedData *)mmap(NULL, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (data == MAP_FAILED) {
        perror("Error mapping shared memory");
        close(shm_fd);
        shm_unlink(SHM_NAME);
        exit(1);
    }

    // Initialize shared memory data
    memset(data, 0, sizeof(SharedData));

    printf("Voting server started. Press Ctrl+C to stop voting.\n");

    // Monitor for new votes
    int last_total_votes = 0;
    while (1) {
        if (data->total_votes > last_total_votes) {
            printf("New vote received! Total votes: %d\n", data->total_votes);
            last_total_votes = data->total_votes;
        }
        sleep(1);  // Pause to reduce CPU usage
    }

    return 0;
}