#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <atomic>
#include <thread>
#include <fstream>

using namespace std;

const int NUM_READERS = 16;
const int NUM_WRITERS = 16;
const string READER_EXE = "reader.exe";
const string WRITER_EXE = "writer.exe";
const string MAP_NAME = "SharedMemoryMap";
const string MUTEX_JOURNAL = "JournalMutex";
const string STOP_EVENT = "StopEvent";
const size_t PAGE_NUMBER = 16;

void monitorProcesses(const vector<PROCESS_INFORMATION>& processes, HANDLE stopEvent) {
    while (!WaitForSingleObject(stopEvent, 1000) == WAIT_OBJECT_0) {
        // Boucle d'attente
    }
    // Signal d'arrêt déjà envoyé, attendre la fin des processus
    for (const auto& pi : processes) {
        WaitForSingleObject(pi.hProcess, 5000); // Attendre max 5s
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
}

int main() {
    CreateDirectoryA("logs",NULL);
    CreateDirectoryA("logs/reader", NULL);
    CreateDirectoryA("logs/writer", NULL);
    // Vérifier l'existence des exécutables
    for (const auto& exe : {READER_EXE, WRITER_EXE}) {
        ifstream f(exe);
        if (!f.good()) {
            cerr << "Erreur : " << exe << " introuvable." << endl;
            return 1;
        }
    }

    // Obtenir la taille de page
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    const DWORD PAGE_SIZE = sysInfo.dwPageSize;

    // Créer la mémoire partagée
    HANDLE hMappedFile = CreateFileMapping(
        INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE,
        0, PAGE_SIZE * PAGE_NUMBER, MAP_NAME.c_str());
    if (!hMappedFile) {
        cerr << "Erreur création mémoire partagée (" << GetLastError() << ")" << endl;
        return 1;
    }

    // Créer le mutex du journal
    HANDLE journalMutex = CreateMutexA(NULL, FALSE, MUTEX_JOURNAL.c_str());
    if (!journalMutex) {
        cerr << "Erreur création mutex journal (" << GetLastError() << ")" << endl;
        CloseHandle(hMappedFile);
        return 1;
    }

    // Créer les mutex des pages
    vector<HANDLE> mutexes(PAGE_NUMBER);
    for (size_t i = 0; i < PAGE_NUMBER; ++i) {
        string mutexName = "PageMutex_" + to_string(i);
        mutexes[i] = CreateMutexA(NULL, FALSE, mutexName.c_str());
        if (!mutexes[i]) {
            cerr << "Erreur création mutex page " << i << " (" << GetLastError() << ")" << endl;
            for (size_t j = 0; j < i; ++j) CloseHandle(mutexes[j]);
            CloseHandle(journalMutex);
            CloseHandle(hMappedFile);
            return 1;
        }
    }

    // Créer l'événement d'arrêt
    HANDLE stopEvent = CreateEventA(NULL, TRUE, FALSE, STOP_EVENT.c_str());
    if (!stopEvent) {
        cerr << "Erreur création événement arrêt (" << GetLastError() << ")" << endl;
        for (auto& m : mutexes) CloseHandle(m);
        CloseHandle(journalMutex);
        CloseHandle(hMappedFile);
        return 1;
    }

    vector<PROCESS_INFORMATION> processes;

    cout << "Lancement de " << NUM_READERS << " readers et " << NUM_WRITERS << " writers..." << endl;
    cout << "Appuyez sur Entrée pour arrêter..." << endl;

    // Lancement des writers
    for (int i = 0; i < NUM_WRITERS; ++i) {
        STARTUPINFO si = { sizeof(si) };
        PROCESS_INFORMATION pi;
        if (!CreateProcess(WRITER_EXE.c_str(), NULL, NULL, NULL, FALSE,
                          CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
            cerr << "Erreur création writer " << i << " (" << GetLastError() << ")" << endl;
        } else {
            processes.push_back(pi);
        }
    }

    // Lancement des readers
    for (int i = 0; i < NUM_READERS; ++i) {
        STARTUPINFO si = { sizeof(si) };
        PROCESS_INFORMATION pi;
        if (!CreateProcess(READER_EXE.c_str(), NULL, NULL, NULL, FALSE,
                          CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
            cerr << "Erreur création reader " << i << " (" << GetLastError() << ")" << endl;
        } else {
            processes.push_back(pi);
        }
    }

    // Thread de monitoring
    thread monitor(monitorProcesses, cref(processes), stopEvent);

    // Attente arrêt
    cin.get();
    SetEvent(stopEvent); // Signal d'arrêt
    monitor.join();

    // Nettoyage
    for (auto& m : mutexes) CloseHandle(m);
    CloseHandle(journalMutex);
    CloseHandle(hMappedFile);
    CloseHandle(stopEvent);

    cout << "Tous les processus ont été arrêtés." << endl;
    return 0;
}