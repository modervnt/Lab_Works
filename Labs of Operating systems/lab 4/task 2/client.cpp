#include <windows.h>
#include <stdio.h>

#define BUFFER_SIZE 1024

HANDLE hPipe = INVALID_HANDLE_VALUE;
char buffer[BUFFER_SIZE];

void ConnectToPipe() {
    hPipe = CreateFile(
        TEXT("\\\\.\\pipe\\MyPipe"),
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_OVERLAPPED,
        NULL);

    if (hPipe == INVALID_HANDLE_VALUE) {
        printf("CreateFile failed, GLE=%d.\n", GetLastError());
        return;
    }

    DWORD dwMode = PIPE_READMODE_MESSAGE;
    BOOL fSuccess = SetNamedPipeHandleState(
        hPipe,
        &dwMode,
        NULL,
        NULL);

    if (!fSuccess) {
        printf("SetNamedPipeHandleState failed, GLE=%d.\n", GetLastError());
        CloseHandle(hPipe);
        hPipe = INVALID_HANDLE_VALUE;
        return;
    }

    printf("Connected to pipe successfully.\n");
}

VOID CALLBACK ReadCompletionRoutine(
    DWORD dwErrorCode,
    DWORD dwNumberOfBytesTransfered,
    LPOVERLAPPED lpOverlapped) {
    if (dwErrorCode != 0) {
        printf("Read operation failed, error=%d.\n", dwErrorCode);
        return;
    }

    buffer[dwNumberOfBytesTransfered] = '\0';
    printf("Received message: %s\n", buffer);
}

void ReadFromPipe() {
    if (hPipe == INVALID_HANDLE_VALUE) {
        printf("Not connected to pipe yet.\n");
        return;
    }

    OVERLAPPED overlapped = {0};
    overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (overlapped.hEvent == NULL) {
        printf("CreateEvent failed, GLE=%d.\n", GetLastError());
        return;
    }

    BOOL fSuccess = ReadFileEx(
        hPipe,
        buffer,
        BUFFER_SIZE - 1,
        &overlapped,
        ReadCompletionRoutine);

    if (!fSuccess) {
        printf("ReadFileEx failed, GLE=%d.\n", GetLastError());
        CloseHandle(overlapped.hEvent);
        return;
    }

    // Wait for the asynchronous operation to complete
    SleepEx(INFINITE, TRUE);
    CloseHandle(overlapped.hEvent);
}

void ClosePipe() {
    if (hPipe != INVALID_HANDLE_VALUE) {
        CloseHandle(hPipe);
        hPipe = INVALID_HANDLE_VALUE;
    }
    printf("Pipe handle closed.\n");
}

void DisplayMenu() {
    printf("\nClient Menu:\n");
    printf("1. Connect to Pipe\n");
    printf("2. Read from Pipe\n");
    printf("3. Close Pipe\n");
    printf("0. Exit\n");
    printf("Enter your choice: ");
}

int main() {
    int choice = 0;

    do {
        DisplayMenu();
        scanf("%d", &choice);
        while (getchar() != '\n'); // Clear input buffer

        switch (choice) {
            case 1:
                ConnectToPipe();
                break;
            case 2:
                ReadFromPipe();
                break;
            case 3:
                ClosePipe();
                break;
            case 0:
                ClosePipe();
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice. Try again.\n");
        }
    } while (choice != 0);

    return 0;
}