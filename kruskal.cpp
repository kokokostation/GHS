#include <vector>
#include <algorithm>

#include "kruskal.h"
#include "graph_as_vector.h"

void Dsu::make_set(int v)
{
    parent[v] = v;
    rank[v] = 0;
}

int Dsu::find_set(int v)
{
    if(v == parent[v])
        return v;
    return parent[v] = find_set(parent[v]);
}

void Dsu::union_sets(int a, int b)
{
    a = find_set(a);
    b = find_set(b);
    if(a != b)
    {
        if(rank[a] < rank[b])
            std::swap(a, b);
        parent[b] = a;
        if(rank[a] == rank[b])
            rank[a]++;
    }
}

bool Dsu::node_in_dsu(int v) const
{
    return parent.find(v) != parent.end();
}

Graph_as_vector mst(Graph_as_vector graph)
{
    Graph_as_vector result;
    Dsu dsu;

    graph.sort();

    for(size_t i = 0; i < graph.get_node_num(); i++)
        dsu.make_set(i);

    for(size_t i = 0; i < graph.get_edge_num() && result.get_edge_num() < graph.get_node_num(); ++i)
        if(dsu.find_set(graph[i].get_first_node()) != dsu.find_set(graph[i].get_second_node()))
        {
            result.add_edge(graph[i]);
            dsu.union_sets(graph[i].get_first_node(), graph[i].get_second_node());
        }

    result.standartize();
    return result;
}
