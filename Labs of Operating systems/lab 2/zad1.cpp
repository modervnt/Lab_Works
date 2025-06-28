#include <windows.h>
#include <iostream>
#include <iomanip>
#include <vector>

using namespace std;

struct MemoryBlock {
    LPVOID address;
    SIZE_T size;
    DWORD allocationType;
    DWORD protection;
};

vector<MemoryBlock> allocatedBlocks;

// Prototypes
void displaySystemInfo();
void displayMemoryStatus();
void queryMemoryRegion();
void reserveMemory(bool manualAddress);
void reserveAndCommitMemory(bool manualAddress);
void commitReservedMemory();
void writeToMemory();
void setMemoryProtection();
void displayAllocatedBlocks();
void freeAllMemory();
void freeSpecificMemory();

int main() {
    int choice;

    do {
        system("cls");
        cout << "=== Memory Management Application ===" << endl;
        cout << "1. Display System Information" << endl;
        cout << "2. Display Memory Status" << endl;
        cout << "3. Query Memory Region" << endl;
        cout << "=== Memory Allocation ===" << endl;
        cout << "4. Reserve Memory (Auto)" << endl;
        cout << "5. Reserve Memory (Manual)" << endl;
        cout << "6. Reserve+Commit Memory (Auto)" << endl;
        cout << "7. Reserve+Commit Memory (Manual)" << endl;
        cout << "8. Commit Reserved Memory" << endl;
        cout << "=== Memory Operations ===" << endl;
        cout << "9. Write to Memory" << endl;
        cout << "10. Set Memory Protection" << endl;
        cout << "11. Display Allocated Blocks" << endl;
        cout << "12. Free Specific Memory Block" << endl;
        cout << "13. Free All Memory" << endl;
        cout << "0. Exit" << endl;
        cout << "Enter your choice: ";

        cin >> choice;

        switch(choice) {
            case 1: displaySystemInfo(); break;
            case 2: displayMemoryStatus(); break;
            case 3: queryMemoryRegion(); break;
            case 4: reserveMemory(false); break;
            case 5: reserveMemory(true); break;
            case 6: reserveAndCommitMemory(false); break;
            case 7: reserveAndCommitMemory(true); break;
            case 8: commitReservedMemory(); break;
            case 9: writeToMemory(); break;
            case 10: setMemoryProtection(); break;
            case 11: displayAllocatedBlocks(); break;
            case 12: freeSpecificMemory(); break;
            case 13: freeAllMemory(); break;
            case 0: freeAllMemory(); cout << "Exiting..." << endl; break;
            default: cout << "Invalid choice!" << endl;
        }

        if (choice != 0) {
            cout << "\nPress Enter to continue...";
            cin.ignore();
            cin.get();
        }
    } while (choice != 0);

    return 0;
}

void displaySystemInfo() {
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);

    cout << "\n=== System Information ===" << endl;
    cout << "Processor Architecture: ";
    switch (sysInfo.wProcessorArchitecture) {
        case PROCESSOR_ARCHITECTURE_AMD64: cout << "x64" << endl; break;
        case PROCESSOR_ARCHITECTURE_INTEL: cout << "x86" << endl; break;
        case PROCESSOR_ARCHITECTURE_ARM: cout << "ARM" << endl; break;
        default: cout << "Unknown" << endl; break;
    }
    cout << "Page Size: " << sysInfo.dwPageSize << " bytes" << endl;
    cout << "Allocation Granularity: " << sysInfo.dwAllocationGranularity << " bytes" << endl;
    cout << "Min App Address: 0x" << hex << sysInfo.lpMinimumApplicationAddress << endl;
    cout << "Max App Address: 0x" << hex << sysInfo.lpMaximumApplicationAddress << endl;
    cout << "Number of Processors: " << sysInfo.dwNumberOfProcessors << endl;
    cout << "Processor Type: " << sysInfo.dwProcessorType << endl;
}

void displayMemoryStatus() {
    MEMORYSTATUSEX memStatus = { sizeof(MEMORYSTATUSEX) };
    GlobalMemoryStatusEx(&memStatus);

    cout << "\n=== Memory Status ===" << endl;
    cout << "Memory Load: " << memStatus.dwMemoryLoad << "%" << endl;
    cout << "Total Phys: " << memStatus.ullTotalPhys / (1024 * 1024) << " MB" << endl;
    cout << "Avail Phys: " << memStatus.ullAvailPhys / (1024 * 1024) << " MB" << endl;
    cout << "Total Virtual: " << memStatus.ullTotalVirtual / (1024 * 1024) << " MB" << endl;
    cout << "Avail Virtual: " << memStatus.ullAvailVirtual / (1024 * 1024) << " MB" << endl;
}

void queryMemoryRegion() {
    LPVOID address;
    cout << "Enter memory address to query (hex): 0x";
    cin >> hex >> address;

    MEMORY_BASIC_INFORMATION memInfo;
    if (VirtualQuery(address, &memInfo, sizeof(memInfo))) {
        cout << "\nBase Address: 0x" << memInfo.BaseAddress << endl;
        cout << "Region Size: " << memInfo.RegionSize / 1024 << " KB" << endl;
        cout << "State: " << (memInfo.State == MEM_COMMIT ? "Committed" :
                             memInfo.State == MEM_RESERVE ? "Reserved" : "Free") << endl;
        cout << "Protection: 0x" << hex << memInfo.Protect << endl;
    } else {
        cerr << "Query failed. Error: " << GetLastError() << endl;
    }
}

void reserveMemory(bool manualAddress) {
    SIZE_T size;
    LPVOID desiredAddress = NULL;

    cout << "Enter size to reserve (bytes): ";
    cin >> dec >> size;

    if (manualAddress) {
        cout << "Enter desired address (hex): 0x";
        cin >> hex >> desiredAddress;
    }

    LPVOID address = VirtualAlloc(desiredAddress, size, MEM_RESERVE, PAGE_READWRITE);

    if (address) {
        allocatedBlocks.push_back({address, size, MEM_RESERVE, PAGE_READWRITE});
        cout << "Memory reserved at 0x" << hex << address << endl;
    } else {
        cerr << "Reservation failed. Error: " << GetLastError() << endl;
    }
}

void reserveAndCommitMemory(bool manualAddress) {
    SIZE_T size;
    LPVOID desiredAddress = NULL;

    cout << "Enter size to allocate (bytes): ";
    cin >> dec >> size;

    if (manualAddress) {
        cout << "Enter desired address (hex): 0x";
        cin >> hex >> desiredAddress;
    }

    LPVOID address = VirtualAlloc(desiredAddress, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    if (address) {
        allocatedBlocks.push_back({address, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE});
        cout << "Memory allocated at 0x" << hex << address << endl;
    } else {
        cerr << "Allocation failed. Error: " << GetLastError() << endl;
    }
}

void commitReservedMemory() {
    LPVOID address;
    SIZE_T size;

    cout << "Enter reserved address (hex): 0x";
    cin >> hex >> address;
    cout << "Enter size to commit (bytes): ";
    cin >> dec >> size;

    LPVOID result = VirtualAlloc(address, size, MEM_COMMIT, PAGE_READWRITE);

    if (result) {
        // Update existing block or add new?
        bool found = false;
        for (auto& block : allocatedBlocks) {
            if (block.address == address) {
                block.allocationType |= MEM_COMMIT;
                found = true;
                break;
            }
        }

        if (!found) {
            allocatedBlocks.push_back({address, size, MEM_COMMIT, PAGE_READWRITE});
        }

        cout << "Memory committed at 0x" << hex << address << endl;
    } else {
        cerr << "Commit failed. Error: " << GetLastError() << endl;
    }
}

void writeToMemory() {
    if (allocatedBlocks.empty()) {
        cerr << "No allocated blocks available!" << endl;
        return;
    }

    displayAllocatedBlocks();

    size_t index;
    cout << "Select block index: ";
    cin >> dec >> index;

    if (index >= allocatedBlocks.size()) {
        cerr << "Invalid index!" << endl;
        return;
    }

    MemoryBlock& block = allocatedBlocks[index];

    if (!(block.allocationType & MEM_COMMIT)) {
        cerr << "Memory not committed!" << endl;
        return;
    }

    int value;
    cout << "Enter integer value to write: ";
    cin >> value;

    // Simple write at start of block for demonstration
    *(int*)block.address = value;
    cout << "Value " << value << " written to 0x" << hex << block.address << endl;

    // Verify
    cout << "Read back value: " << dec << *(int*)block.address << endl;
}

void setMemoryProtection() {
    if (allocatedBlocks.empty()) {
        cerr << "No allocated blocks available!" << endl;
        return;
    }

    displayAllocatedBlocks();

    size_t index;
    cout << "Select block index: ";
    cin >> dec >> index;

    if (index >= allocatedBlocks.size()) {
        cerr << "Invalid index!" << endl;
        return;
    }

    MemoryBlock& block = allocatedBlocks[index];

    cout << "\nCurrent protection: 0x" << hex << block.protection << endl;
    cout << "1. PAGE_READONLY\n2. PAGE_READWRITE\n3. PAGE_EXECUTE_READ\n4. PAGE_NOACCESS\n";
    cout << "Select new protection: ";

    int choice;
    cin >> choice;

    DWORD newProtect;
    switch (choice) {
        case 1: newProtect = PAGE_READONLY; break;
        case 2: newProtect = PAGE_READWRITE; break;
        case 3: newProtect = PAGE_EXECUTE_READ; break;
        case 4: newProtect = PAGE_NOACCESS; break;
        default:
            cerr << "Invalid choice!" << endl;
            return;
    }

    DWORD oldProtect;
    if (VirtualProtect(block.address, block.size, newProtect, &oldProtect)) {
        block.protection = newProtect;
        cout << "Protection changed from 0x" << hex << oldProtect
             << " to 0x" << newProtect << endl;
    } else {
        cerr << "Failed to change protection. Error: " << GetLastError() << endl;
    }
}

void displayAllocatedBlocks() {
    if (allocatedBlocks.empty()) {
        cout << "No memory blocks allocated." << endl;
        return;
    }

    cout << "\n=== Allocated Memory Blocks ===" << endl;
    for (size_t i = 0; i < allocatedBlocks.size(); ++i) {
        const auto& block = allocatedBlocks[i];
        cout << "[" << i << "] 0x" << hex << block.address
             << " Size: " << dec << block.size << " bytes"
             << " Type: " << (block.allocationType & MEM_RESERVE ? "Reserved" : "")
             << (block.allocationType & MEM_COMMIT ? "Committed" : "")
             << " Protect: 0x" << hex << block.protection << endl;
    }
}

void freeSpecificMemory() {
    if (allocatedBlocks.empty()) {
        cerr << "No allocated blocks available!" << endl;
        return;
    }

    displayAllocatedBlocks();

    size_t index;
    cout << "Select block index to free: ";
    cin >> dec >> index;

    if (index >= allocatedBlocks.size()) {
        cerr << "Invalid index!" << endl;
        return;
    }

    if (VirtualFree(allocatedBlocks[index].address, 0, MEM_RELEASE)) {
        cout << "Memory at 0x" << hex << allocatedBlocks[index].address << " freed." << endl;
        allocatedBlocks.erase(allocatedBlocks.begin() + index);
    } else {
        cerr << "Free failed. Error: " << GetLastError() << endl;
    }
}

void freeAllMemory() {
    if (allocatedBlocks.empty()) {
        cout << "No memory to free." << endl;
        return;
    }

    for (const auto& block : allocatedBlocks) {
        if (!VirtualFree(block.address, 0, MEM_RELEASE)) {
            cerr << "Failed to free 0x" << hex << block.address
                 << " Error: " << GetLastError() << endl;
        }
    }

    allocatedBlocks.clear();
    cout << "All memory blocks freed." << endl;
}

	