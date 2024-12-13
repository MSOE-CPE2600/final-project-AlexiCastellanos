#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

#define SHM_NAME "/voting_shm"
#define MAX_NAME_LEN 50

typedef struct {
    int vote_count[3];
    int total_votes;
    char voters[100][MAX_NAME_LEN];
    int voter_count;
} SharedData;

int has_voted(SharedData *data, const char *username) {
    for (int i = 0; i < data->voter_count; i++) {
        if (strcmp(data->voters[i], username) == 0) {
            return 1;  // Username has already voted
        }
    }
    return 0;
}

void print_choices() {
    printf("Available Candidates:\n");
    printf("A: Candidate 1\n");
    printf("B: Candidate 2\n");
    printf("C: Candidate 3\n");
}

void print_usage(const char *prog_name) {
    print_choices();
    printf("Usage: %s -u <username> -c <candidate>\n", prog_name);
    printf("    -u <username>   Specify your username\n");
    printf("    -c <candidate>  Specify candidate (A, B, or C)\n");
}

int main(int argc, char *argv[]) {
    char *username = NULL;
    char *candidate_str = NULL;
    int opt;

    // Display available candidates
    

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

    if (!username || !candidate_str) {
        print_usage(argv[0]);
        return 1;
    }

    int candidate = candidate_str[0] - 'A';
    if (candidate < 0 || candidate > 2) {
        printf("Invalid candidate. Choose A, B, or C.\n");
        return 1;
    }

    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("Error accessing shared memory");
        return 1;
    }

    SharedData *data = (SharedData *)mmap(NULL, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (data == MAP_FAILED) {
        perror("Error mapping shared memory");
        close(shm_fd);
        return 1;
    }

    if (has_voted(data, username)) {
        printf("You (%s) have already voted!\n", username);
    } else {
        data->vote_count[candidate]++;
        data->total_votes++;
        strncpy(data->voters[data->voter_count++], username, MAX_NAME_LEN - 1);
        data->voters[data->voter_count - 1][MAX_NAME_LEN - 1] = '\0';  // Null-terminate

        printf("Vote for Candidate %c submitted successfully. Thank you, %s!\n", candidate_str[0], username);
    }

    munmap(data, sizeof(SharedData));
    close(shm_fd);
    return 0;
}
