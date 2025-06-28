#include <iostream>
#include <fstream>
#include <chrono>
#include <bits/stdc++.h> // Includes most standard libraries (not recommended for production code)
#include "HT0.h"
using namespace std;

const int VAL_MIN = 0;
const int VAL_MAX = 500;

int main() {
    // Preparation for operations:
    srand((unsigned int)time(nullptr));
    auto MaxMul = 5; // Nahuya
    size_t middle_power = 0, set_count = 0;
    auto Used = [&](HT &t) {
        middle_power += t.power(); // Ensure `power` is implemented
        ++set_count;
    };
    ofstream fout("in.txt");

    for (size_t SIZE = 8; SIZE < 250; SIZE++) { 
        middle_power = 0; set_count = 0;
        std::vector<int> AT(SIZE), BT(SIZE), CT(SIZE), DT(SIZE), ET(SIZE);
        cout << "Creation of sets..." << endl;
        generator(AT, VAL_MIN, VAL_MAX, SIZE);
        generator(BT, VAL_MIN, VAL_MAX, SIZE);
        generator(CT, VAL_MIN, VAL_MAX, SIZE);
        generator(DT, VAL_MIN, VAL_MAX, SIZE);
        generator(ET, VAL_MIN, VAL_MAX, SIZE);

        HT A, B, C, D, E;
        for (int i = 0; i < SIZE; ++i) {
            A.insert(AT[i]);
            B.insert(BT[i]);
            C.insert(CT[i]);
            D.insert(DT[i]);
            E.insert(ET[i]);
        }
        // cout << "== TABLE A ==" << endl;
        // A.Display();
        // cout << "== TABLE B ==" << endl;
        // B.Display();
        // cout << "== TABLE C ==" << endl;
        // C.Display();
        // cout << "== TABLE D ==" << endl;
        // D.Display();
        // cout << "== TABLE E ==" << endl;
        // E.Display();

        // Calcul de R = (A \ B) XOR (C \ D \ E)
        auto t1 = chrono::high_resolution_clock::now();
        //cout << "== (A \\ B)==" << endl;
        Used(A);
        Used(B);
        HT one = A - B;
        //one.Display();
        //cout << "== C \\ D \\ E ==" << endl;
        Used(C);
        Used(D);
        Used(E);
        HT two = C - D - E;
        //two.Display();
        Used(one);
        Used(two);
        HT R = one ^ two;
        auto t2 = chrono::high_resolution_clock::now();

        int dt = std::chrono::duration_cast<std::chrono::duration<double, std::micro>>(t2 - t1).count();
        //cout << "\nRESULTAT: R = (A ∖ B) ⊖ (C ∖ D ∖ E)" << endl; //Ne pas oublier les fout;
        //R.Display();

        middle_power /= set_count;
        fout << middle_power << " " << dt << endl;
        //cout << "MIDDLE POWER: " << middle_power << endl;
        //cout << "EXECUTION TIME: " << dt << endl;
    }
    fout.close();
    return 0;
}