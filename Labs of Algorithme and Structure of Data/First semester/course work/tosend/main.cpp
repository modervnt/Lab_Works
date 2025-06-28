
#include <iostream>
#include <vector>
#include "Graph.hpp"
using namespace std;

int Graph::cnt = 0;
int main()
{
    srand(time(0));
//    test1
    vector<vector<int>> matrix1 = {{1, 2, 5}, {2, 4, 5}, {3, 5}, {0, 1, 2, 3, 5}, {1, 4, 5}, {0, 1, 2, 3, 4}};
    vector<vector<int>> matrix2 = {{7, 10}, {6, 7, 8, 9}, {8}, {8, 9, 10}, {6}};
//    test2
    vector<vector<int>> matrix3 = {{1, 2, 4}, {0}, {0, 1, 3}, {0, 1, 2}, {1, 2}};
    vector<vector<int>> matrix4 = {{6}, {5, 7}, {5}, {5, 6, 7}};
// test3
    vector<vector<int>> matrix5 = {{1}, {2}, {3}, {4}, {5}, {}};
    vector<vector<int>> matrix6 = {{7}, {8}, {9}, {10}, {11}, {}};
// test4
    vector<vector<int>> matrix7 = {{0, 1, 2, 3}, {1, 2 ,3}, {0, 1, 3}, {0, 1 , 2}};
    vector<vector<int>> matrix8 = {{4, 5, 6, 7}, {6, 7}, {5, 7}, {5, 6, 7}};
//    test5
    vector<vector<int>> matrix9 = {{1, 3}, {2}, {0}, {3, 4}, {5}, {3}};
    vector<vector<int>> matrix10 = {{6, 7}, {8}, {6}};


    Graph graph(matrix1);
    graph.printGraph();
    Graph sample(matrix2);
    sample.printGraph();
    
    auto result = graph.subgraphSearch(sample, graph.makeEquivalenceList(sample), 0);



    vector<vector<int>> empty = {{}};
    if (result != empty){
        cout << "There is no isomorphic subgraph";
    }
 }
