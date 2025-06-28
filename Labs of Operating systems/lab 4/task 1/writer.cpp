#include <windows.h>
#include <string>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <fstream>

using namespace std;

const size_t PAGE_NUMBER = 16;
const string writerLogDir = "logs/writer/";
const string fileJournal = writerLogDir + "journal_writer_" + to_string(GetCurrentProcessId()) + ".txt";
const string MAP_NAME = "SharedMemoryMap";
const string STOP_EVENT = "StopEvent";
const string TURNSTILE_SEM = "Writer_turner";

void journalMessage(const string& message, size_t processID) {
    ofstream file(fileJournal, ios::app);
    if (file) {
        file << timeGetTime() << " | WRITER " << processID << " | " << message << endl;
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
        cerr << "Erreur ouverture événément arrêt (" << GetLastError() << ")" << endl;
        return 1;
    }

    // Ouvrir la mémoire partagée
    HANDLE hMappedFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, MAP_NAME.c_str());
    if (!hMappedFile) {
        cerr << "Erreur ouverture mémoire partagée (" << GetLastError() << ")" << endl;
        CloseHandle(journalMutex);
        CloseHandle(stopEvent);
        return 1;
    }
    void* mappedFile = MapViewOfFile(hMappedFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    if (!mappedFile) {
        cerr << "Erreur mappage mémoire partagée (" << GetLastError() << ")" << endl;
        CloseHandle(hMappedFile);
        CloseHandle(journalMutex);
        CloseHandle(stopEvent);
        return 1;
    }

    HANDLE hTurn = CreateSemaphore(NULL, 1, 1, TURNSTILE_SEM);

    // Ouvrir les mutex des pages
    HANDLE mutexes[PAGE_NUMBER];
    for (size_t i = 0; i < PAGE_NUMBER; ++i) {
        string mutexName = "PageMutex_" + to_string(i);
        mutexes[i] = OpenMutexA(MUTEX_ALL_ACCESS, FALSE, mutexName.c_str());
        if (!mutexes[i]) {
            cerr << "Erreur ouverture mutex page " << i << " (" << GetLastError() << ")" << endl;
            for (size_t j = 0; j < i; ++j) CloseHandle(mutexes[j]);
            UnmapViewOfFile(mappedFile);
            CloseHandle(hMappedFile);
            CloseHandle(journalMutex);
            CloseHandle(stopEvent);
            return 1;
        }
    }

    // Boucle principale
    while (WaitForSingleObject(stopEvent, 0) != WAIT_OBJECT_0) {
        int pageNum = rand() % PAGE_NUMBER;
        string message = "Data from Writer " + to_string(processID) + " - " + to_string(rand());
        journalMessage("Waiting", processID);
        WaitForSingleObject(hTurn, INFINITE);
        ReleaseSemaphore(hTurn, 1, NULL);
        WaitForSingleObject(mutexes[pageNum], INFINITE);
        //journalMessage("Writing to page " + to_string(pageNum), processID, journalMutex);
        journalMessage("Writting", processID);
        char* pageAddress = static_cast<char*>(mappedFile) + (pageNum * PAGE_SIZE);
        strncpy_s(pageAddress, PAGE_SIZE, message.c_str(), _TRUNCATE); 
        Sleep((rand() % 1000) + 500); // Pause aléatoire
        ReleaseMutex(mutexes[pageNum]);
        journalMessage("Released", processID);
        //journalMessage("Finished writing to page " + to_string(pageNum), processID, journalMutex);

        
    }

    // Nettoyage
    UnmapViewOfFile(mappedFile);
    CloseHandle(hMappedFile);
    for (size_t i = 0; i < PAGE_NUMBER; ++i) {
        CloseHandle(mutexes[i]);
    }
    CloseHandle(hTurn);
    CloseHandle(stopEvent);

    journalMessage("Processus arrêté", processID, journalMutex);
    return 0;
}