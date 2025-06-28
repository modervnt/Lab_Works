#ifndef AICD_LAB4
#define AICD_LAB4


#include <iostream>
#include <vector>
using namespace std;


class Graph
{
private:
    int amount = 0;
    vector<vector<int>> adjList;
public:
    static int cnt;
    Graph(const vector<vector<int>>& matrix);
    explicit Graph(int n);
    void printGraph();
    vector<vector<int>> makeEquivalenceList(Graph sample);
    vector<vector<int>> updateEquivalenceList(Graph sample,vector<vector<int>> list);
    vector<vector<int>> subgraphSearch(const Graph& sample, vector<vector<int>> list, int set);
};

bool haveEmptyRows(const vector<vector<int>>& matrix);
vector<vector<int>> setElement(const vector<vector<int>> &matrix, int pos, int elem);
vector<int> deleteElement(const vector<int>& vec, int el);
void printMatrix(const vector<vector<int>>& matrix, int n);
bool elementInVector(const vector<int>& vector, int element);
vector<vector<int>> inputAdjList();


#endif
