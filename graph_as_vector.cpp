#include <vector>
#include <algorithm>
#include <iostream>
#include <unordered_set>

#include "graph_as_vector.h"

namespace std
{

size_t hash<Graph_as_vector::Primitive_edge>::operator()(const Graph_as_vector::Primitive_edge& edge) const
{
    size_t half_size_t = sizeof(size_t) * 4;
    return (edge.get_first_node() << half_size_t) + (edge.get_first_node() % (1u << half_size_t)) + edge.get_second_node();
}

}

const Graph_as_vector::Primitive_edge& Graph_as_vector::Primitive_edge::standartize()
{
    if(first_node > second_node)
        std::swap(first_node, second_node);

    return *this;
}

size_t Graph_as_vector::Primitive_edge::get_first_node() const
{
    return first_node;
}

size_t Graph_as_vector::Primitive_edge::get_second_node() const
{
    return second_node;
}

bool Graph_as_vector::Primitive_edge::operator==(const Primitive_edge& e) const
{
    return first_node == e.first_node && second_node == e.second_node;
}
bool Graph_as_vector::Primitive_edge::operator!=(const Primitive_edge& e) const
{
    return !(*this == e);
}

size_t Graph_as_vector::Edge::get_weight() const
{
    return weight;
}

bool Graph_as_vector::Edge::operator==(const Edge& e) const
{
    return first_node == e.first_node && second_node == e.second_node && weight == e.weight;
}
bool Graph_as_vector::Edge::operator!=(const Edge& e) const
{
    return !(*this == e);
}

Graph_as_vector::Graph_as_vector(size_t node_num, size_t edge_num) : node_num(node_num)
{
    graph.reserve(edge_num);
}

void Graph_as_vector::sort()
{
    std::sort(graph.begin(), graph.end(), [](const Edge& a, const Edge& b)
    {
        if(a.get_weight() == b.get_weight())
        {
            if(a.get_second_node() == b.get_second_node())
                return a.get_first_node() < b.get_first_node();
            else
                return a.get_second_node() < b.get_second_node();
        }
        else
            return a.get_weight() < b.get_weight();
    });
}

void Graph_as_vector::add_edges(const Graph_as_vector& edges)
{
    graph.insert(graph.end(), edges.graph.begin(), edges.graph.end());
}

void Graph_as_vector::add_edge(const Edge& edge)
{
    graph.push_back(edge);
}

void Graph_as_vector::standartize()
{
    for(Edge& i : graph)
        i.standartize();

    sort();

    graph.erase(std::unique(graph.begin(), graph.end()), graph.end());
}

size_t Graph_as_vector::get_edge_num() const
{
    return graph.size();
}

size_t Graph_as_vector::get_node_num() const
{
    return node_num;
}

void Graph_as_vector::set_node_num(size_t node_num_)
{
    if(node_num == Graph_as_vector::NODE_NUM_UDEF)
        node_num = node_num_;
    else
        throw graph_as_vector_misuse();
}

const Graph_as_vector::Edge& Graph_as_vector::operator[](size_t i) const
{
    return graph[i];
}

bool Graph_as_vector::operator==(const Graph_as_vector& g) const
{
    return graph == g.graph;
}

bool Graph_as_vector::operator!=(const Graph_as_vector& g) const
{
    return !(*this == g);
}

std::ostream& operator<<(std::ostream& stream, const Graph_as_vector& graph)
{
    for(size_t i = 0; i < graph.get_edge_num(); ++i)
        stream << graph[i].get_first_node() << " " << graph[i].get_second_node() << " " << graph[i].get_weight() << std::endl;
    return stream;
}

std::istream& operator>>(std::istream& stream, Graph_as_vector& graph)
{
    size_t node_num, edge_num;
    stream >> node_num >> edge_num;

    graph.set_node_num(node_num);

    size_t first_node, second_node, weight;
    for(size_t i = 0; i < edge_num; ++i)
    {
        stream >> first_node >> second_node >> weight;
        graph.add_edge(Graph_as_vector::Edge(first_node, second_node, weight));
    }

    return stream;
}
