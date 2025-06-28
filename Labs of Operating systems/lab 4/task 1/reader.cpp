#include <windows.h>
#include <string>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <fstream>

using namespace std;

const size_t PAGE_NUMBER = 16;
const string MAP_NAME = "SharedMemoryMap";
const string STOP_EVENT = "StopEvent";
const string TURNSTILE_SEM = "Reader_turner";
const string READING_SEM = "Reading_authorization";

// Fichier journal unique par lecteur
const string readerLogDir = "logs/reader/";
const string fileJournal = readerLogDir + "journal_reader_" + to_string(GetCurrentProcessId()) + ".txt";

void journalMessage(const string& message, size_t processID) {
    ofstream file(fileJournal, ios::app);
    if (file) {
        file << timeGetTime() << " | READER " << processID << " | " << message << endl;
    }
}

int main() {
    size_t processID = GetCurrentProcessId();
    srand(static_cast<unsigned int>(time(nullptr)));

    // Obtenir la taille de page
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    const DWORD PAGE_SIZE = sysInfo.dwPageSize;

    // Ouvrir l'événement d'arrêt
    HANDLE stopEvent = OpenEventA(EVENT_ALL_ACCESS, FALSE, STOP_EVENT.c_str());
    if (!stopEvent) {
        cerr << "Erreur ouverture événement arrêt (" << GetLastError() << ")" << endl;
        return 1;
    }

    // Ouvrir la mémoire partagée
    HANDLE hMappedFile = OpenFileMapping(FILE_MAP_READ, FALSE, MAP_NAME.c_str());
    if (!hMappedFile) {
        cerr << "Erreur ouverture mémoire partagée (" << GetLastError() << ")" << endl;
        CloseHandle(stopEvent);
        return 1;
    }

    void* mappedFile = MapViewOfFile(hMappedFile, FILE_MAP_READ, 0, 0, 0);
    if (!mappedFile) {
        cerr << "Erreur mappage mémoire partagée (" << GetLastError() << ")" << endl;
        CloseHandle(hMappedFile);
        CloseHandle(stopEvent);
        return 1;
    }

    // Créer/ouvrir les sémaphores
    HANDLE hRead = CreateSemaphore(NULL, 5, 5, READING_SEM.c_str());
    HANDLE hTurn = CreateSemaphore(NULL, 1, 1, TURNSTILE_SEM.c_str());

    // Boucle principale
    while (WaitForSingleObject(stopEvent, 0) != WAIT_OBJECT_0) {
        int pageNum = rand() % PAGE_NUMBER;
        journalMessage("Waiting", processID);

        // Tourniquet (FIFO)
        WaitForSingleObject(hTurn, INFINITE);
        ReleaseSemaphore(hTurn, 1, NULL);

        // Limite de lecteurs simultanés
        WaitForSingleObject(hRead, INFINITE);
        journalMessage("Reading page " + to_string(pageNum), processID);

        // Lecture de la page
        char* pageAddress = static_cast<char*>(mappedFile) + (pageNum * PAGE_SIZE);
        string content(pageAddress, strnlen(pageAddress, PAGE_SIZE));
        Sleep((rand() % 1000) + 500); // Pause aléatoire

        ReleaseSemaphore(hRead, 1, NULL);
        journalMessage("Released page " + to_string(pageNum), processID);
    }

    // Nettoyage
    UnmapViewOfFile(mappedFile);
    CloseHandle(hMappedFile);
    CloseHandle(hTurn);
    CloseHandle(hRead);
    CloseHandle(stopEvent);

    journalMessage("Processus arrêté", processID);
    return 0;
}