#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 100000000
#define BLOCKSIZE (10 * 331219)

typedef struct Block {
    double start;
    double end;
    double result;
} BLOCK;

void display_menu() {
    printf("\n╔══════════════════════════════╗\n");
    printf("║    THREAD COUNT SELECTION    ║\n");
    printf("╠══════════════════════════════╣\n");
    printf("║ 1. 1 thread                  ║\n");
    printf("║ 2. 2 threads                 ║\n");
    printf("║ 3. 4 threads                 ║\n");
    printf("║ 4. 8 threads                 ║\n");
    printf("║ 5. 12 threads                ║\n");
    printf("║ 6. 16 threads                ║\n");
    printf("║ 0. Exit                      ║\n");
    printf("╚══════════════════════════════╝\n");
    printf("Your choice: ");
}

int main() {
    int nblocks = (N + BLOCKSIZE - 1) / BLOCKSIZE;
    BLOCK *blocks = malloc(nblocks * sizeof(BLOCK));
    
    if (blocks == NULL) {
        printf("Memory allocation error!\n");
        return 1;
    }

    for (int i = 0; i < nblocks; i++) {
        blocks[i].start = i * BLOCKSIZE;
        blocks[i].end = ((i + 1) * BLOCKSIZE - 1 < N) ? ((i + 1) * BLOCKSIZE - 1) : (N - 1);
        blocks[i].result = 0.0;
    }

    int choice;
    int nThread = 1; 
    
    do {
        display_menu();
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input! Please enter a number.\n");
            while (getchar() != '\n'); 
            continue;
        }
        
        switch(choice) {
            case 1: nThread = 1; break;
            case 2: nThread = 2; break;
            case 3: nThread = 4; break;
            case 4: nThread = 8; break;
            case 5: nThread = 12; break;
            case 6: nThread = 16; break;
            case 0: 
                free(blocks);
                printf("Goodbye!\n");
                return 0;
            default:
                printf("\nInvalid choice! Please try again.\n");
                continue;
        }
        
        printf("\n══════════════════════════════\n");
        printf("Calculating with %d thread(s)...\n", nThread);
        
        double start_time = omp_get_wtime();
        
        omp_set_num_threads(nThread);
        #pragma omp parallel
        {
            #pragma omp for schedule(dynamic, 1)
            for (int i = 0; i < nblocks; i++) {
                double partial_sum = 0.0;
                for (int j = blocks[i].start; j <= blocks[i].end; j++) {
                    double x = (j + 0.5) * (1.0 / N);
                    partial_sum += 4.0 / (1.0 + x * x);
                }
                blocks[i].result = partial_sum;
            }
        }

        double total = 0.0;
        for (int i = 0; i < nblocks; i++) {
            total += blocks[i].result;
        }
        
        double end_time = omp_get_wtime();
        double pi_approx = total / N;

        printf("\n══════════════════════════════\n");
        printf("Results:\n");
        printf("• π approximation: %.15f\n", pi_approx);
        printf("• Execution time:  %.4f seconds\n", end_time - start_time);
        printf("══════════════════════════════\n");
        
        printf("\nPress Enter to continue...");
        while (getchar() != '\n');
        getchar();
        
    } while (choice != 0);

    free(blocks);
    return 0;
}