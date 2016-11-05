#ifndef GRAPH_AS_VECTOR_H_INCLUDED
#define GRAPH_AS_VECTOR_H_INCLUDED

#include <vector>
#include <iostream>
#include <unordered_set>
#include <climits>
#include <stdexcept>

class graph_as_vector_misuse : public std::exception
{
};

class Graph_as_vector
{
public:
    class Primitive_edge
    {
    protected:
        size_t first_node, second_node;

    public:
        Primitive_edge() = default;
        Primitive_edge(size_t first_node, size_t second_node) : first_node(first_node), second_node(second_node) {};

        const Primitive_edge& standartize();

        size_t get_first_node() const;
        size_t get_second_node() const;

        bool operator==(const Primitive_edge& e) const;
        bool operator!=(const Primitive_edge& e) const;
    };

    class Edge : public Primitive_edge
    {
    private:
        size_t weight;

    public:
        Edge() = default;
        Edge(size_t first_node, size_t second_node, size_t weight) : Primitive_edge(first_node, second_node), weight(weight) {};

        size_t get_weight() const;

        bool operator==(const Edge& e) const;
        bool operator!=(const Edge& e) const;
    };

private:
    size_t node_num;
    std::vector<Edge> graph;

public:
    enum {NODE_NUM_UDEF = INT_MAX};

    Graph_as_vector(size_t node_num = NODE_NUM_UDEF, size_t edge_num = 0);

    void sort();
    void add_edges(const Graph_as_vector& edges);
    void add_edge(const Edge& edge);
    void standartize();

    size_t get_edge_num() const;
    size_t get_node_num() const;
    void set_node_num(size_t node_num_);

    bool operator==(const Graph_as_vector& g) const;
    bool operator!=(const Graph_as_vector& g) const;

    const Edge& operator[](size_t i) const;
};

std::ostream& operator<<(std::ostream& stream, const Graph_as_vector& graph);
std::istream& operator>>(std::istream& stream, Graph_as_vector& graph);

namespace std
{

template<>
struct hash<Graph_as_vector::Primitive_edge>
{
    size_t operator()(const Graph_as_vector::Primitive_edge& edge) const;
};

}

#endif // GRAPH_AS_VECTOR_H_INCLUDED
