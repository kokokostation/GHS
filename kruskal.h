#ifndef KRUSKAL_H_INCLUDED
#define KRUSKAL_H_INCLUDED

#include <vector>
#include <unordered_map>

#include "graph_as_vector.h"

class Dsu
{
private:
    std::unordered_map<int, int> parent, rank;

public:
    void make_set(int v);
    int find_set(int v);
    void union_sets(int a, int b);
    bool node_in_dsu(int v) const;
};

Graph_as_vector mst(Graph_as_vector graph);

#endif // KRUSKAL_H_INCLUDED
