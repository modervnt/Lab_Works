//#include "pch.h"
#include "HT0.h"
#include <iostream>
#include <iomanip>
using std::cout;
#include <cstdlib>  
#include <ctime> 
const int SIZE = 32; 

// Retourne un itérateur pointant sur le premier élément de la table de hachage
myiter HT::begin()const {
    myiter p(nullptr);
    p.bct = bucket;
    for (; p.pos < Buckets; ++p.pos) { 
        p.Ptr = bucket[p.pos]; 
        if (p.Ptr) break;  // Trouvé un bucket non vide
    }
    return p;
}

// Opérateur de pré-incrémentation pour l'itérateur
myiter myiter::operator++()
{
    if (!Ptr) { // Itérateur invalide ?
        return *this; // Ne rien faire si déjà invalide
    }
    else
    { // Chercher le prochain élément
        if(Ptr->down) { // Si élément suivant dans le même bucket
            Ptr = Ptr->down;
            return (*this);
        }
        while(++pos < HT::Buckets) {    // Chercher dans les buckets suivants
            if(Ptr = bct[pos])
                return *this;
        }
        Ptr = nullptr;  // Fin de la table
        return (*this);
    }
}

// Destructeur de la table de hachage
HT :: ~HT()
{
   for(auto t = 0; t < Buckets; ++t) 
       delete bucket[t];    // Libérer chaque chaîne
   delete []bucket;        // Libérer le tableau de buckets
}

// Affichage de la table de hachage
void HT::Display()    
{     
    Node** P = new Node*[Buckets];
    for(auto t = 0; t < Buckets; ++t) P[t] = bucket[t];
    bool prod = true;
    cout << "\n" << tag << ":";
    while(prod) {
        prod = false; 
        for(auto t = 0; t < Buckets; ++t) {
            if(P[t]) {
                cout << setw(4) << P[t]->key; 
                P[t] = P[t]->down; 
                prod = true;
            }
            else cout << "  - ";
        }
        cout << "\n  ";
    }

    //Display the sequence;
    cout << "Sequence" << endl;
    for (int i = 0; i < seq.size(); i++)
    {
        cout << seq[i]->key << "-" ;
    }
    cout << endl;
    
}

// Recherche d'un élément dans la table
myiter HT::find(int k)const 
{ 
    auto t(hash(k));  // Calcul du hash
    Node*p(bucket[t]);
    while (p) { 
        if (p->key == k) 
            return myiter(p);
        else p = p->down; 
    }
    return end();  // Non trouvé
}

// Insertion d'un élément dans la table
pair<myiter, bool> HT::insert(int k, myiter)    
{       
    auto t(hash(k));  
    Node * p(bucket[t]);
    while(p) {
        if(p->key == k) {
            seq.push_back(p);
            return make_pair(myiter(p), false);  // Déjà présent
        }
        else p = p->down;
    }
    bucket[t] = new Node(k, bucket[t]);  // Insertion en tête
    ++count;
    //Insertion in the sequence:
    seq.push_back(bucket[t]);
    return make_pair(myiter(bucket[t]), true);  // Insertion réussie
}

// Suppression d'un élément de la table
pair<myiter, bool> HT::Remove(int k)    
{   
    
    auto t(hash(k));  
    auto p(bucket[t]), q(p);
    if (p) {
        if (p->key == k) { // Suppression en tête de liste
            bucket[t] = p->down;
            p->down = nullptr;
            delete p;
            return make_pair(myiter(q), true);
        }
        p = p->down;
        while (p) {
            if (p->key == k) {
                q->down = p->down;
                p->down = nullptr;
                delete p;
                --count;
                return make_pair(myiter(q), true);
            }
            else q = p, p = p->down;
        }
    }
    //For the sequence i decided to delete all the instance
    /*seq.erase(__remove_if(seq.begin(), seq.end(), 
            [k](const Node* p){return p->key == k; }), seq.end());*/
    for (int i = 0; i < seq.size(); i++)
    {
        if (seq[i]->key == k)
        {
            seq.erase(seq.begin() + i);
            --i;
        }
        
    }
    
    return make_pair(myiter(q), false);  // Non trouvé
}

// Constructeur à partir d'une séquence d'éléments
template<class MyIt> 
HT::HT(MyIt first, MyIt last) : HT() {
    for (; first != last; ++first) insert(*first);
}

// Opérateur d'union avec affectation
HT & HT::operator |= (const HT & rgt) {
    for (myiter x = rgt.begin(); x != rgt.end(); ++x) 
        insert(*x, nullptr);
    return *this;
}

// Opérateur d'intersection avec affectation
HT & HT::operator &= (const HT & rgt) {
    HT temp;
    for (auto x = begin(); x != end(); ++x) 
        if (rgt.find(*x) != rgt.end())
            temp.insert(*x);
    swap(temp);
    return *this;
}

// Opérateur de différence avec affectation
HT& HT::operator -= (const HT & rgt) {
    HT temp;
    for (auto x = begin(); x != end(); ++x)
        if (rgt.find(*x) == rgt.end()) temp.insert(*x);    
    swap(temp);
    return *this;
}

//Operateur de XOR avec affectation (element soit dans A, soit dans B mais pas dans les deux)
HT& HT::operator ^= (const HT& rgt){
	HT temp;
	for (auto x = begin(); x != end(); ++x)
		if (rgt.find(*x) == rgt.end()) temp.insert(*x);

	for (auto x = rgt.begin(); x != rgt.end(); ++x)
		if(find(*x) == end()) temp.insert(*x);
	
	swap(temp);
	return *this;
}

size_t HT::tags = 0;  // Compteur pour générer des tags uniques

void generator(int arr[], int min, int max) {
    srand(time(nullptr) + min + max); 
    
    for (int i = 0; i < SIZE; ) {
        int val = min + (rand() % (max - min + 1));
        bool dejaPresent = false;
        
        // Vérifie les doublons
        for (int j = 0; j < i; ++j) {
            if (arr[j] == val) {
                dejaPresent = true;
                break;
            }
        }
        
        if (!dejaPresent) {
            arr[i++] = val;
        }
    }
}
// myiter& myiter::operator++(){
// 	if (Ptr && Ptr->down){
// 		Ptr = Ptr->down;
// 	} else {
// 		while (++pos < HT::Buckets)
// 		{
// 			if (Ptr = bct[pos]) break;		
// 		}
// 	}
// 	return *this;
// };