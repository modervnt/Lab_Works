
#include "Graph.hpp"
#include <algorithm>

Graph :: Graph(const vector<vector<int>>& matrix) {
    amount = matrix.size();
    adjList.resize(amount);
    adjList = (matrix);
    cnt += amount;
}

Graph :: Graph(int n) {
    amount = n;
    adjList.resize(amount);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++){
            if (rand() %  2 == 0 ) {
                adjList[i].push_back(cnt + j);
            }
        }
    }
    cnt += amount;
}

void Graph :: printGraph()
{
    for (int i = 0; i < amount; i++)
    {
        cout << cnt - amount + i << " --> ";
        for (int v: adjList[i]) {
            cout << v << " ";
        }
        cout <<  endl;
    }
    cout<< endl;
}

vector<vector<int>> Graph::makeEquivalenceList(Graph sample) {
    vector<vector<int>> list(sample.amount, vector<int>());
    for (int i = 0; i < sample.amount; ++i){
        for (int j = 0; j < amount; ++j){
            if (sample.adjList[i].size() <= adjList[j].size()){
                list[i].push_back(j);
            }
        }
    }
    return list;
}

vector<vector<int>> Graph::updateEquivalenceList(Graph sample,vector<vector<int>> list) {
    vector<vector<int>> newList(sample.amount, vector<int>());
    for (int i = 0; i < sample.amount; ++i) {
        if (!sample.adjList[i].empty()){
            for (int j = 0; j < list[i].size(); j++) {
                bool conclusion = true;
                int testedElememt = list[i][j];
                for (auto el: sample.adjList[i]) {
                    bool flag = false;
                    int num = el - amount;
                    for (auto k: list[num]) {
                        if (elementInVector(adjList[testedElememt], k)) {flag = true;}
                    }
                    if (!flag){
                        conclusion = false;
                    }
                }
                if (conclusion) newList[i].push_back(list[i][j]);
            }
        }
        else {
            newList[i] = list[i];
        }
    }
    return newList;
}


vector<vector<int>> Graph:: subgraphSearch(const Graph& sample, vector<vector<int>> list, int set){
    auto checkList = updateEquivalenceList(sample, list);
    while (checkList != list){
        list = checkList;
        checkList = updateEquivalenceList(sample, list);
    }
    list = checkList;
    if (haveEmptyRows(list)) {
        return list;
    }
    else {
//        printMatrix(list, amount);
        if (set == sample.amount){
            vector<vector<int>> empty = {{}};
            cout << "result" << endl;
            printMatrix(list, amount);
            return empty;
        }
        else{
            for (int i = set; i < sample.amount; i++){
                if (haveEmptyRows(list)){
                    return list;
                }
                else{
                    auto checkingList = list;
                    for (int g : checkingList[i]){
                        auto testList = setElement(checkingList, i, g);
                        auto newList = subgraphSearch(sample, testList, set + 1);
                        vector<vector<int>> empty = {{}};
                        if (newList == empty){
                            return empty;
                        }
                        if (haveEmptyRows(newList)){
                            list[i] = deleteElement(list[i], g);
                        }
                    }
                }
            }
            return list;
        }
    }
}

bool haveEmptyRows(const vector<vector<int>>& matrix){
//    any_of(matrix.cbegin(), matrix.cend(), [](vector<int> vector){return vector.empty();});
    for (auto i : matrix){
        if (i.empty()){
            return true;
        }
    }
    return false;
}

vector<vector<int>> setElement(const vector<vector<int>> &matrix, int pos, int elem) {
    vector<vector<int>> newMatrix(matrix.size());
    for (int i =0; i < matrix.size(); i++){
        if (i == pos) newMatrix[i].push_back(elem);
        else {
            for (int j = 0; j < matrix[i].size(); j++){
                if (matrix[i][j] != elem) newMatrix[i].push_back(matrix[i][j]);
            }
        }
    }
    return newMatrix;
}


vector<int> deleteElement(const vector<int>& vec, int el){
    vector<int> newVec;
    for (int i : vec){
        if (i != el)   newVec.push_back(i);
    }
    return newVec;
}

void printMatrix(const vector<vector<int>>& matrix, int n){
    int printingElement = n;
    for (const auto& i : matrix){
        cout << printingElement << " : ";
        for (auto j : i){
            cout << j << " ";
        }
        cout << endl;
        printingElement += 1;
    }
    cout << endl;
}

bool elementInVector(const vector<int>& vector, int element){
    //    any_of(vector.cbegin(), vector.cend(), [](int el){return int el == element;});
    for (auto el : vector){
        if (el == element){
            return true;
        }
    }
    return false;
}

vector<vector<int>> inputAdjList(){
    vector<vector<int>> matrix;
    vector<int> row;
    int n, k, element;
    cin >> n;
    cout << "Enter number of rows in adjList";
    for (int i = 0; i < n; i++){
        cout << "Enter number of neighbours of " << i;
        cin >> k;
        for (int j = 0; j < k; j++){
            cout << "Enter an element";
            cin >> element;
            row.push_back(element);
        }

    }
    return matrix;
}