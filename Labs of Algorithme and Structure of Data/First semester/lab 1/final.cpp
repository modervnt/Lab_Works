#include <iostream>
#include <cstdlib> 
#include <ctime>
#include <chrono>
//const int SIZEARRAY = 10;
const int MAX = 26;

using namespace std;
/*for array*/
void generator(char* A, char* B, char* C, char* D, int power);
int Esolver(char* A,char* B, char* C,char* D, char* E, int power);
void printer(char* A, int sizeA);
int checker(char* A,int sizeA, char element);

/*for list*/
struct List {
    char element;
    List* next;
    List(char e, List * p = nullptr) : element(e), next(p){};
    ~List(){ delete next; }
};
void addToList(List*& A, char newElement);
void Listprinter(List* A);
//void ConvertToList(char* A, int SIZEARRAY, List** myList);
void ConvertToList(char* A, int power, List*& myList);
void ListEsolver(List** A, List** B, List** C, List**  D, List** E);
int ListChecker(List** myList, char toTest);
void ListDestruction(List** myList);

/*For masbit*/
void solverMasBitE(int* masBitA, int* masBitB, int* masBitC,  int* masBitD, int* masBitE);
void createMasBit(char* A, int* mas_bits, int power);
void printMasBit(int* mas_bits);
/*for machine world*/
void createMachineWord(char* A, int* machineWord);
void printMachineWord(int machineWord);
void solverMachineWordE(int machineWordA, int machineWordB, int machineWordC,  int machineWordD, int* machineWordE);

/*for the test*/
void myTester(char*A, char* B, char* C, char*D, int power);

int main(){
    cout << "for the array" << "\n";
    char A[MAX+1];
    char B[MAX+1];
    char C[MAX+1];
    char D[MAX+1];
    char E[MAX+1];
    char temp[30] = {'\0'};
    int power;
    cout << "input the power of set:";
    cin >> power;
    generator(A,B,C,D,power);
    int sizeE = Esolver(A,B,C,D,E,power);
    cout << "Array A : ";
    // printer(A,power);
    cout << A << endl;
    cout << "Array B : ";
    // printer(B, power);
    cout << B << endl;
    cout << "Array C : ";
    cout << C << endl;
    //printer(C, power);
    cout << "Array D : ";
    cout << D << endl;
    //printer(D, power);
    cout << "Array E : ";
    cout << E << endl;
    //printer(E,sizeE);

    cout << "Using list Array" << "\n";
    List* listA = nullptr ;
    ConvertToList(A,power, listA);
    List* listB = nullptr;
    ConvertToList(B,power, listB);
    List* listC = nullptr;
    ConvertToList(C,power, listC);
    List* listD = nullptr ;
    ConvertToList(D,power, listD);
    List* listE = nullptr;
    ListEsolver(&listA, &listB, &listC, &listD, &listE);
    cout << "List A : ";
    Listprinter(listA);
    cout << "List B : ";
    Listprinter(listB);
    cout << "List C : ";
    Listprinter(listC);
    cout << "List D : ";
    Listprinter(listD);
    cout << "List E : ";
    Listprinter(listE);
    ListDestruction(&listA);
    ListDestruction(&listB);
    ListDestruction(&listC);
    ListDestruction(&listD);
    ListDestruction(&listE);

    cout << "using masBit" << "\n";
    int masBitA[MAX] = {0};
    int masBitB[MAX] = {0};
    int masBitC[MAX] = {0};
    int masBitD[MAX] = {0};
    int masBitE[MAX] = {0};

    createMasBit(A, masBitA, power);
    createMasBit(B, masBitB, power);
    createMasBit(C, masBitC, power);
    createMasBit(D, masBitD, power);
    cout << "Massive Bit A :";
    printMasBit(masBitA);
    cout << "Massive Bit B :";
    printMasBit(masBitB);
    cout << "Massive Bit C :";
    printMasBit(masBitC);
    cout << "Massive Bit D :";
    printMasBit(masBitD);
    
    solverMasBitE(masBitA, masBitB, masBitC, masBitD, masBitE);
    cout << "Massive Bit E :";
    printMasBit(masBitE);

    /*for the machine word*/
    cout <<"for the machine word" << "\n";
    int machineWordA, machineWordB, machineWordC, machineWordD, machineWordE;
    createMachineWord(A, &machineWordA);
    createMachineWord(B, &machineWordB);
    createMachineWord(C, &machineWordC);
    createMachineWord(D, &machineWordD);
    solverMachineWordE(machineWordA,machineWordB, machineWordC, machineWordD, &machineWordE);
    cout << "Machine Word A :";
    printMachineWord(machineWordA);
    cout << "Machine Word B :";
    printMachineWord(machineWordB);
    cout << "Machine Word C :";
    printMachineWord(machineWordC);
    cout << "Machine Word D :";
    printMachineWord(machineWordD);
    cout << "Machine Word E :";
    printMachineWord(machineWordE);

    myTester(A,B,C,D, power);
}

void generator(char* A, char* B, char* C, char* D, int power){
    srand(time(0));
    char alphabet[MAX] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z' };
    int useA[MAX] = {0};
    int useB[MAX] = {0};
    int useC[MAX] = {0};
    int useD[MAX] = {0};
    int latter;
    char* arrs[] = {A,B,C,D};

    for (int j = 0; j < 4; j++){
        int useArr[MAX] = {0};
        int i;
        for (i = 0; i < power; i++) {
            latter = rand() % MAX;
            if (!useArr[latter]) {
                arrs[j][i] = alphabet[latter]; 
                useArr[latter] = 1;
            }
            else i--;
        }
        arrs[j][power] = '\0';
    }   
}

int checker(char* A,int sizeA, char element){
    int test = 0;
    for (int i = 0; i < sizeA; i++) {
        if (A[i] == element) {
            test = 1;
            break;
        }
    }
    return test;
}

int Esolver(char* A,char* B, char* C,char* D, char* E,int power){
    int position = 0, i;
    for (i = 0; i < power; i++)
    {
        if (!checker(B,power,A[i]) && !checker(C,power,A[i]) && !checker(D,power,A[i]) && !checker(E,position,A[i]))
        {
            E[position]=A[i];
            position++;
        }
        
    }
    E[position] = '\0';
    return position;
}

void printer(char* A, int sizeA) {
    for(int i=0; A[i]=!'\0'; ++i) {  //авьшмушфьмм and sizeArray
        cout << A[i] << " ";
    }
    cout << "\n";
}

void addToList(List* & A, char newElement) { A = new List(newElement, A); }

void Listprinter(List* A) {
    List* current = A;
    if (!current) {
        cout << "List is empty" << endl;
    } else {
        while (current) {
            cout << current->element << " ";
            current = current->next;
        }
        cout << endl;
    }
}
void ConvertToList(char* A, int power, List*& myList) {
    for (int i = 0; i < power; i++) {
        addToList(myList, A[i]);
    }
}

int ListChecker(List** myList, char toTest) {
    List* temp = *myList;
    while (temp) {
        if (temp->element == toTest) {
            return 1; // Found
        }
        temp = temp->next;
    }
    return 0; // Not found
}

void ListDestruction(List** myList) {
    //List* temp = *myList;
    //List* next;
    delete *myList;
    // while (temp) {
    //     next = temp->next;
    //     //free(temp);
    //     delete(temp);
    //     temp = next;
    // }
    *myList = nullptr; // Set the head to nullptr after freeing
}

void ListEsolver(List** A,List** B,List** C, List**  D, List** E){
    List* tempA = (*A);
    while (tempA)
    {
        if (!ListChecker(B, tempA->element) &&  !ListChecker(C, tempA->element) && !ListChecker(D, tempA->element) && !ListChecker(E, tempA->element))
        {
            addToList(*E, tempA->element);
        }
        tempA = tempA->next;
    }
    
}

void createMasBit(char* A, int* mas_bits,int power){
    for (int i = 0; i < power; i++)
    {
        mas_bits[A[i] - 'a'] = 1;
    }
}

void printMasBit(int* mas_bits){
    for (int i = 0; i < MAX; i++)
    {
        if (mas_bits[i] == 1) cout << static_cast<char>(i + 'a') << " ";
    }
    cout << "\n";  
}

void solverMasBitE(int* masBitA, int* masBitB, int* masBitC,  int* masBitD, int* masBitE){
    for (int i = 0; i < MAX; i++)
    {
        if (masBitA[i] && !masBitB[i] && !masBitC[i] && !masBitD[i])
        {
            masBitE[i] = 1;
        }   
    }
}

void createMachineWord(char* A, int* machineWord){
    *machineWord = 0;
    for (int i = 0; A[i]!='\0'; i++)
    {
        *machineWord |= 1 << (A[i] - 'a');
    }
    
}

void printMachineWord(int machineWord){
    for (int i = 25; i >= 0; i--)
    {
        if (machineWord & (1 << i)) cout << static_cast <char>(i + 'a') <<" ";
    }
    cout << "\n";
}
void solverMachineWordE(int machineWordA, int machineWordB, int machineWordC,  int machineWordD, int* machineWordE){
    *machineWordE = (machineWordA &~ (machineWordB | machineWordC | machineWordD));
}

void myTester(char*A, char* B, char* C, char*D, int power){

    auto dtArray = std::chrono::duration<double>::zero();
    auto dtList = std::chrono::duration<double>::zero();
    auto dtMasBit = std::chrono::duration<double>::zero();
    auto dtMachineWord = std::chrono::duration<double>::zero();

        generator(A,B,C,D, power);
        char E[MAX+1];
        auto t1Array = std::chrono::high_resolution_clock::now();
        Esolver(A,B,C,D,E,power);
        auto t2Array = std::chrono::high_resolution_clock::now();
        dtArray += std::chrono::duration_cast<std::chrono::duration<double>>(t2Array-t1Array);

        List* listA = nullptr ;
        ConvertToList(A,power, listA);
        List* listB = nullptr;
        ConvertToList(B,power, listB);
        List* listC = nullptr;
        ConvertToList(C,power, listC);
        List* listD = nullptr ;
        ConvertToList(D,power, listD);
        List* listE = nullptr;
        auto t1List = std::chrono::high_resolution_clock::now();
        ListEsolver(&listA, &listB, &listC, &listD, &listE);
        auto t2List = std::chrono::high_resolution_clock::now();
        dtList += std::chrono::duration_cast<std::chrono::duration<double>>(t2List-t1List);
        ListDestruction(&listA);
        ListDestruction(&listB);
        ListDestruction(&listC);
        ListDestruction(&listD);
        ListDestruction(&listE);

        int masBitA[MAX] = {0};
        int masBitB[MAX] = {0};
        int masBitC[MAX] = {0};
        int masBitD[MAX] = {0};
        int masBitE[MAX] = {0};
        createMasBit(A, masBitA, power);
        createMasBit(B, masBitB, power);
        createMasBit(C, masBitC, power);
        createMasBit(D, masBitD, power);
        auto t1MasBit = std::chrono::high_resolution_clock::now();
        solverMasBitE(masBitA, masBitB, masBitC, masBitD, masBitE);
        auto t2MasBit = std::chrono::high_resolution_clock::now();
        dtMasBit += std::chrono::duration_cast<std::chrono::duration<double>>(t2MasBit-t1MasBit);

        int machineWordA, machineWordB, machineWordC, machineWordD, machineWordE;
        createMachineWord(A, &machineWordA);
        createMachineWord(B, &machineWordB);
        createMachineWord(C, &machineWordC);
        createMachineWord(D, &machineWordD);
        auto t1MachineWord = std::chrono::high_resolution_clock::now();
        solverMachineWordE(machineWordA,machineWordB, machineWordC, machineWordD, &machineWordE);
        auto t2MachineWord = std::chrono::high_resolution_clock::now();
        dtMachineWord += std::chrono::duration_cast<std::chrono::duration<double>>(t2MachineWord-t1MachineWord);

    cout << "Execution Times for 300 test with different values :" <<'\n';
    cout << "For array : " <<dtArray.count() <<'\n';
    cout << "For List : " <<dtList.count() <<'\n';
    cout << "For Massive Bits : " <<dtMasBit.count() <<'\n';
    cout << "For Machine Word : " <<dtMachineWord.count() <<'\n';

    cout << "END!!!!!";
    
}