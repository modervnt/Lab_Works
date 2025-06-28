#include <windows.h>
#include <stdio.h>

#define BUFFER_SIZE 1024

HANDLE hPipe = INVALID_HANDLE_VALUE;
HANDLE hEvent = INVALID_HANDLE_VALUE;
OVERLAPPED overlapped = {0};

void CreatePipe() {
    hPipe = CreateNamedPipe(
        TEXT("\\\\.\\pipe\\MyPipe"),
        PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        1,
        BUFFER_SIZE,
        BUFFER_SIZE,
        0,
        NULL);

    if (hPipe == INVALID_HANDLE_VALUE) {
        printf("CreateNamedPipe failed, GLE=%d.\n", GetLastError());
        return;
    }

    printf("Named pipe created successfully.\n");
}

void CreateEventObject() {
    hEvent = CreateEvent(
        NULL,
        TRUE,
        FALSE,
        NULL);

    if (hEvent == NULL) {
        printf("CreateEvent failed, GLE=%d.\n", GetLastError());
        return;
    }

    overlapped.hEvent = hEvent;
    printf("Event object created successfully.\n");
}

void ConnectToClient() {
    if (hPipe == INVALID_HANDLE_VALUE) {
        printf("Pipe not created yet.\n");
        return;
    }

    BOOL fConnected = ConnectNamedPipe(hPipe, &overlapped);
    if (fConnected) {
        printf("ConnectNamedPipe failed, GLE=%d.\n", GetLastError());
        return;
    }

    DWORD dwWait = WaitForSingleObject(hEvent, INFINITE);
    if (dwWait != WAIT_OBJECT_0) {
        printf("WaitForSingleObject failed, GLE=%d.\n", GetLastError());
        return;
    }

    printf("Client connected successfully.\n");
}

void DisconnectFromClient() {
    if (hPipe == INVALID_HANDLE_VALUE) {
        printf("Pipe not created yet.\n");
        return;
    }

    if (!DisconnectNamedPipe(hPipe)) {
        printf("DisconnectNamedPipe failed, GLE=%d.\n", GetLastError());
        return;
    }

    printf("Disconnected from client.\n");
}

void WriteToPipe() {
    if (hPipe == INVALID_HANDLE_VALUE) {
        printf("Pipe not created yet.\n");
        return;
    }

    char buffer[BUFFER_SIZE];
    printf("Enter message to send: ");
    fgets(buffer, BUFFER_SIZE, stdin);
    buffer[strlen(buffer) - 1] = '\0'; // Remove newline

    ResetEvent(hEvent);
    overlapped.hEvent = hEvent;

    DWORD cbWritten;
    BOOL fSuccess = WriteFile(
        hPipe,
        buffer,
        strlen(buffer) + 1,
        &cbWritten,
        &overlapped);

    if (!fSuccess && GetLastError() != ERROR_IO_PENDING) {
        printf("WriteFile failed, GLE=%d.\n", GetLastError());
        return;
    }

    DWORD dwWait = WaitForSingleObject(hEvent, INFINITE);
    if (dwWait != WAIT_OBJECT_0) {
        printf("WaitForSingleObject failed, GLE=%d.\n", GetLastError());
        return;
    }

    printf("Message sent successfully.\n");
}

void CloseHandles() {
    if (hPipe != INVALID_HANDLE_VALUE) {
        CloseHandle(hPipe);
        hPipe = INVALID_HANDLE_VALUE;
    }
    if (hEvent != INVALID_HANDLE_VALUE) {
        CloseHandle(hEvent);
        hEvent = INVALID_HANDLE_VALUE;
    }
    printf("Handles closed.\n");
}

void DisplayMenu() {
    printf("\nServer Menu:\n");
    printf("1. Create Named Pipe\n");
    printf("2. Create Event Object\n");
    printf("3. Connect to Client\n");
    printf("4. Disconnect from Client\n");
    printf("5. Write to Pipe\n");
    printf("6. Close Handles\n");
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
                CreatePipe();
                break;
            case 2:
                CreateEventObject();
                break;
            case 3:
                ConnectToClient();
                break;
            case 4:
                DisconnectFromClient();
                break;
            case 5:
                WriteToPipe();
                break;
            case 6:
                CloseHandles();
                break;
            case 0:
                CloseHandles();
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice. Try again.\n");
        }
    } while (choice != 0);

    return 0;
}