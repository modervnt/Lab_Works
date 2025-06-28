// MyCont00.cpp : Хеш-таблица + итераторы 
// Вставка, удаление, копирование и двуместные операции
// Демонстрационная программа: (C)lgn, 17-23.03.19


//include "pch.h"
#include <iostream>
#include <clocale>
#include <vector>
#include <list>
#include <stack>
#include <iterator>
#include <algorithm>
#include "HT0.h"
using namespace std;
const int SIZE = 32;

int main()
{


	// Initialisation des tableaux avec des plages différentes
    int AT[SIZE], BT[SIZE], CT[SIZE], DT[SIZE], ET[SIZE];
    cout << "Creation of sets..." << endl;
    generator(AT, 10, 50);  
    generator(BT, 20, 60);   
    generator(CT, 30, 70);   
    generator(DT, 40, 80);   
    generator(ET, 50, 90);   

    HT A, B, C, D, E;
    for (int i = 0; i < SIZE; ++i) {
        A.insert(AT[i]);
        B.insert(BT[i]);
        C.insert(CT[i]);
        D.insert(DT[i]);
        E.insert(ET[i]);
    }
    cout << "== TABLE A ==" << endl; A.Display();
    cout << "== TABLE B ==" << endl; B.Display();
    cout << "== TABLE C ==" << endl; C.Display();
    cout << "== TABLE D ==" << endl; D.Display();
    cout << "== TABLE E ==" << endl; E.Display();

    // Calcul de R = (A \ B) XOR (C \ D \ E)
    cout << "== (A \\ B)==" << endl;
    HT one = A - B;
    one.Display();
    cout << "== C \\ D \\ E ==" << endl;
    HT two = C - D - E;
    two.Display();  
    HT R = (A - B) ^ (C - D - E);
    cout << "\nRESULTAT: R = (A ∖ B) ⊖ (C ∖ D ∖ E)" << endl;
    R.Display();

}