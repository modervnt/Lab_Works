#include <iostream>
#include <windows.h>
#include <omp.h>
#include <limits>
#include <iomanip>

using namespace std;

#define N 100000000
#define BLOCKSIZE (10 * 331219)

struct BLOCK {
    int start;
    int end;
    double result;
    BLOCK() : start(0), end(0), result(0.0) {}
};

int currentBlock = 0;
int nblocks = (N + BLOCKSIZE - 1) / BLOCKSIZE;
HANDLE mutex;

struct ThreadData {
    BLOCK* blocks;
    int* currentBlock;
    HANDLE mutex;
    int threadId;
};

DWORD WINAPI CalculateSum(LPVOID lpParam) {
    ThreadData* data = (ThreadData*)lpParam;
    while (true) {
        WaitForSingleObject(data->mutex, INFINITE);
        int blockIndex = *(data->currentBlock);
        if (blockIndex >= nblocks) {
            ReleaseMutex(data->mutex);
            break;
        }
        *(data->currentBlock) += 1;
        ReleaseMutex(data->mutex);
        BLOCK* block = &data->blocks[blockIndex];

        for (int j = block->start; j <= block->end; j++) {
            double x = (j + 0.5) * (1.0 / N);
            block->result += 4.0 / (1.0 + x * x);
        }
    }
    return 0;
}

void display_menu() {

    cout << "   THREAD COUNT SELECTION    \n";
    cout << "--------------------------------\n";
    cout << " 1. 1 thread                  \n";
    cout << " 2. 2 threads                 \n";
    cout << " 3. 4 threads                 \n";
    cout << " 4. 8 threads                 \n";
    cout << " 5. 12 threads                \n";
    cout << " 6. 16 threads                \n";
    cout << " 0. Exit                      \n";
    cout << "-------------------------------\n";
    cout << "Your choice: ";
}

int main() {
    mutex = CreateMutex(NULL, FALSE, NULL);
    BLOCK* blocks = new BLOCK[nblocks];

    for (int i = 0; i < nblocks; i++) {
        blocks[i].start = i * BLOCKSIZE;
        blocks[i].end = ((i + 1) * BLOCKSIZE - 1 < N) ? ((i + 1) * BLOCKSIZE - 1) : (N - 1);
        blocks[i].result = 0.0;
    }

    int choice;
    int nThread = 1;

    do {
        display_menu();
        if (!(cin >> choice)) {
            cout << "Invalid input! Please enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
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
                delete[] blocks;
                CloseHandle(mutex);
                cout << "Goodbye!\n";
                return 0;
            default:
                cout << "\nInvalid choice! Please try again.\n";
                continue;
        }

        currentBlock = 0; // Reset for each run
        HANDLE* threads = new HANDLE[nThread];
        ThreadData threadData = {blocks, &currentBlock, mutex, 0};

        cout << "\n----------------------------\n";
        cout << "Calculating with " << nThread << " thread(s)...\n";

        double start_time = omp_get_wtime();

        for(int i = 0; i < nThread; i++) {
            threads[i] = CreateThread(NULL, 0, CalculateSum, &threadData,0, NULL);
            if (threads[i] == NULL) {
                cerr << "Thread Creation Error " << i << endl;
            }
        }

        while (currentBlock < nblocks) {
            for (int i = 0; i < nThread; i++) {
                DWORD status = WaitForSingleObject(threads[i], 0);
                if (status == WAIT_OBJECT_0) {
                    WaitForSingleObject(mutex, INFINITE);
                    if (currentBlock < nblocks) {
                        ResumeThread(threads[i]);
                    }
                    ReleaseMutex(mutex);
                }
            }
        }

        WaitForMultipleObjects(nThread, threads, TRUE, INFINITE);


        double total = 0.0;
        for (int i = 0; i < nblocks; i++) {
            total += blocks[i].result;
        }

        double end_time = omp_get_wtime();
        double pi_approx = total / N;

        cout << "\n------------------------------------\n";
        cout << "Results:\n";
        int precision = 8;
        cout << fixed << setprecision(precision);
        cout << "• π approximation: " << pi_approx << "\n";
        cout << "• Execution time: " << (end_time - start_time) << " seconds\n";
        cout << "-----------------------------------\n";

        for (int i = 0; i < nThread; i++) {
            CloseHandle(threads[i]);
        }
        delete[] threads;

        cout << "\nPress Enter to continue...";
        cin.ignore();
        cin.get();

    } while (choice != 0);

    delete[] blocks;
    CloseHandle(mutex);
    return 0;
}