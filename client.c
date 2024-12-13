/**
* Name: Alexi Castellanos
* Filename: client.c
* CPE2600 - Lab 13 Final Project
*
* Description: program allows users to cast votes for one of three candidates. 
* It prevents duplicate votes by tracking voter usernames and ensures 
* fairness in the voting process. The application interacts with a shared 
* memory segment managed by a server to store vote counts and voter information.
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

// Define shared memory name and constants
#define SHM_NAME "/voting_shm"  // Name of the shared memory segment
#define MAX_NAME_LEN 50         // Maximum length of a voter's username

// Structure to hold voting data
typedef struct {
    int vote_count[3];            // Array to store vote counts for candidates A, B, and C
    int total_votes;              // Total number of votes cast
    char voters[100][MAX_NAME_LEN]; // Array to store usernames of voters
    int voter_count;              // Count of unique voters
} SharedData;


// Check if a user has already voted.
int has_voted(SharedData *data, const char *username) {
    for (int i = 0; i < data->voter_count; i++) {
        if (strcmp(data->voters[i], username) == 0) {
            return 1;  // Username has already voted
        }
    }
    return 0;
}

// Display the list of available candidates.
void print_choices() {
    printf("Available Candidates:\n");
    printf("A: Candidate 1\n");
    printf("B: Candidate 2\n");
    printf("C: Candidate 3\n");
}


//Print usage instructions for the client program.
void print_usage(const char *prog_name) {
    print_choices();
    printf("Usage: %s -u <username> -c <candidate>\n", prog_name);
    printf("    -u <username>   Specify your username\n");
    printf("    -c <candidate>  Specify candidate (A, B, or C)\n");
}

int main(int argc, char *argv[]) {
    char *username = NULL;         // User's username
    char *candidate_str = NULL;    // Selected candidate
    int opt;

    // Parse command-line arguments using getopt
    while ((opt = getopt(argc, argv, "u:c:")) != -1) {
        switch (opt) {
            case 'u':
                username = optarg;
                break;
            case 'c':
                candidate_str = optarg;
                break;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }

    // Ensure both username and candidate are provided
    if (!username || !candidate_str) {
        print_usage(argv[0]);
        return 1;
    }

    // Convert candidate input to index (0 for A, 1 for B, 2 for C)
    int candidate = candidate_str[0] - 'A';
    if (candidate < 0 || candidate > 2) {
        printf("Invalid candidate. Choose A, B, or C.\n");
        return 1;
    }

    // Open shared memory segment
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("Error accessing shared memory");
        return 1;
    }

    // Map shared memory to process address space
    SharedData *data = (SharedData *)mmap(NULL, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (data == MAP_FAILED) {
        perror("Error mapping shared memory");
        close(shm_fd);
        return 1;
    }

    // Check if the user has already voted
    if (has_voted(data, username)) {
        printf("You (%s) have already voted!\n", username);
    } else {
        // Update vote count and voter information
        data->vote_count[candidate]++;
        data->total_votes++;
        strncpy(data->voters[data->voter_count++], username, MAX_NAME_LEN - 1);
        data->voters[data->voter_count - 1][MAX_NAME_LEN - 1] = '\0';  // Null-terminate

        printf("Vote for Candidate %c submitted successfully. Thank you, %s!\n", candidate_str[0], username);
    }

    // Unmap and close shared memory
    munmap(data, sizeof(SharedData));
    close(shm_fd);
    return 0;
}