#ifndef EMULATOR_H_INCLUDED
#define EMULATOR_H_INCLUDED

#include <vector>
#include <deque>
#include <memory>
#include <algorithm>
#include <unordered_set>
#include <stdexcept>

#include "graph_as_vector.h"
#include "random.h"

class bad_ghs : public std::exception
{
};

class Emulator_query
{
private:
    size_t sender, recipient;

public:
    Emulator_query(size_t sender, size_t recipient) : sender(sender), recipient(recipient) {};

    size_t get_sender() const;
    size_t get_recipient() const;

    virtual ~Emulator_query() = default;
};

class Emulator_node
{
public:
    virtual void set_id(size_t id) = 0;
    virtual const std::deque<std::shared_ptr<const Emulator_query>>& tick(std::shared_ptr<const Emulator_query> q) = 0;
    virtual void add_edge(size_t end, size_t weight) = 0;
    virtual const std::deque<std::shared_ptr<const Emulator_query>>& wake_up() = 0;
    virtual bool ended() const = 0;
    virtual ~Emulator_node() = default;
};

template<typename T>
struct Identity
{
    typedef T type;
};

class Emulator
{
private:
    std::vector<std::deque<std::shared_ptr<const Emulator_query>>> box;
    std::vector<std::shared_ptr<Emulator_node>> nodes;
    std::unordered_set<Graph_as_vector::Primitive_edge> graph;

    template<typename Id>
    Emulator(const Graph_as_vector& graph, Id obj);

    void random_wake_up();
    void process_queries(const std::deque<std::shared_ptr<const Emulator_query>>& queries);

public:
    template<typename Node>
    static Emulator create(const Graph_as_vector& graph);

    const std::shared_ptr<Emulator_node>& operator[](size_t i);

    void process();
};

template<typename Id>
Emulator::Emulator(const Graph_as_vector& graph_, Id obj) : box(graph_.get_node_num())
{
    for(size_t i = 0; i < graph_.get_edge_num(); ++i)
        graph.insert(Graph_as_vector::Primitive_edge(graph_[i]).standartize());

    nodes.reserve(graph_.get_node_num());
    for(size_t i = 0; i < graph_.get_node_num(); ++i)
    {
        nodes.push_back(std::shared_ptr<Emulator_node>(new typename Id::type));
        nodes.back()->set_id(i);
    }

    for(size_t i = 0; i < graph_.get_edge_num(); ++i)
    {
        nodes[graph_[i].get_first_node()]->add_edge(graph_[i].get_second_node(), graph_[i].get_weight());
        nodes[graph_[i].get_second_node()]->add_edge(graph_[i].get_first_node(), graph_[i].get_weight());
    }
}

template<typename Node>
Emulator Emulator::create(const Graph_as_vector& graph)
{
    return Emulator(graph, Identity<Node>());
}

#endif // EMULATOR_H_INCLUDED
