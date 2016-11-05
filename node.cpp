#include <vector>
#include <utility>
#include <deque>
#include <queue>
#include <algorithm>
#include <memory>

#include "node.h"
#include "emulator.h"

size_t Edge::get_end() const
{
    return end;
}

size_t Edge::get_weight() const
{
    return weight;
}

Edge::State Edge::get_state() const
{
    return state;
}

Query::Query(size_t sender, size_t recipient) : Emulator_query(sender, recipient)
{
}

Edge::Edge(size_t end, State state, size_t weight) : end(end), weight(weight), state(state)
{
}

Node::Node() : state(Node::SLEEP), checking_postponed(false), component(Edge::UDEF, 0)
{
}

void Node::set_id(size_t id_)
{
    id = id_;
    parent = id_;
}

void Node::add_edge(size_t end, size_t weight)
{
    edges.push_back(Edge(end, Edge::UNKNOWN, weight));
}

const std::deque<std::shared_ptr<const Emulator_query>>& Node::wake_up()
{
    return tick(std::make_shared<WAKE_UP>(id, id));
}

Graph_as_vector Node::get_branches() const
{
    Graph_as_vector result;

    for(const Edge& i : edges)
        if(i.get_state() == Edge::BRANCH)
            result.add_edge(Graph_as_vector::Edge(id, i.get_end(), i.get_weight()));

    return result;
}

bool Node::ended() const
{
    return state == Node::END;
}

size_t Node::get_edge(size_t end) const
{
    return std::distance(edges.begin(), find_if(edges.begin(), edges.end(), [end](const Edge& e)
    {
        return e.get_end() == end;
    }));
}

size_t Node::find_min_edge() const
{
    std::pair<size_t, size_t> min(Edge::UDEF, Edge::INF_WEIGHT);
    for(size_t i = 0; i < edges.size(); ++i)
        if(edges[i].get_state() == Edge::UNKNOWN && edges[i].get_weight() < min.second)
        {
            min.first = i;
            min.second = edges[i].get_weight();
        }
    return min.first;
}

void Node::send(std::shared_ptr<const Query> q)
{
    result.push_back(q);
}

void Node::postpone(std::shared_ptr<const Query> q)
{
    postponed.push(q);
}

void Node::check_postponed()
{
    if(checking_postponed)
        return;

    checking_postponed = true;
    for(int i = 0, j = postponed.size(); i < j; ++i)
    {
        postponed.front()->accept(*this);
        postponed.pop();
    }
    checking_postponed = false;
}

void Node::test()
{
    size_t min_edge = find_min_edge();
    if(min_edge != Edge::UDEF)
    {
        test_node = edges[min_edge].get_end();
        send(std::make_shared<TEST>(id, edges[min_edge].get_end(), component));
    }
    else
    {
        test_node = Node::UDEF;
        report();
    }
}

void Node::report()
{
    size_t sons_num = 0;
    for(size_t i = 0; i < edges.size(); ++i)
        sons_num += edges[i].get_state() == Edge::BRANCH && edges[i].get_end() != parent;

    if(sons_num == reports && test_node == Node::UDEF)
    {
        state = Node::FOUND;
        send(std::make_shared<REPORT>(id, parent, best_edge_weight));

        check_postponed();
    }
}

void Node::change_core()
{
    if(edges[best_edge].get_state() == Edge::BRANCH)
        send(std::make_shared<CHANGE_CORE>(id, edges[best_edge].get_end()));
    else
    {
        send(std::make_shared<CONNECT>(id, edges[best_edge].get_end(), component));
        edges[best_edge].state = Edge::BRANCH;

        check_postponed();
    }
}

const std::deque<std::shared_ptr<const Emulator_query>>& Node::tick(std::shared_ptr<const Emulator_query> q)
{
    result = std::deque<std::shared_ptr<const Emulator_query>>();

    if(state == Node::SLEEP)
        WAKE_UP(id, id).accept(*this);

    std::dynamic_pointer_cast<const Query>(q)->accept(*this);

    return result;
}

void Node::visit(const INIT& q)
{
    component = q.component;
    state = q.state;
    parent = q.get_sender();
    best_edge = Edge::UDEF;
    best_edge_weight = Edge::INF_WEIGHT;

    for(const Edge& i : edges)
        if(i.get_state() == Edge::BRANCH && i.get_end() != parent)
            send(std::make_shared<INIT>(id, i.get_end(), q.component, q.state));

    if(state == Node::SEARCH)
    {
        reports = 0;
        test();
    }

    check_postponed();
}

void Node::visit(const CHANGE_CORE& q)
{
    change_core();
}

void Node::visit(const CONNECT& q)
{
    size_t connect_edge = get_edge(q.get_sender());
    if(q.component.level < component.level)
    {
        edges[connect_edge].state = Edge::BRANCH;

        send(std::make_shared<INIT>(id, q.get_sender(), component, state));
    }
    else if(edges[connect_edge].get_state() == Edge::BRANCH)
        send(std::make_shared<INIT>(id, q.get_sender(), Component(edges[connect_edge].get_weight(), component.level + 1), Node::SEARCH));
    else
        postpone(q.shared_from_this());
}

void Node::visit(const REJECT& q)
{
    size_t connect_edge = get_edge(q.get_sender());
    if(edges[connect_edge].get_state() == Edge::UNKNOWN)
        edges[connect_edge].state = Edge::REJECTED;

    test();
}

void Node::visit(const REPORT& q)
{
    size_t connect_edge = get_edge(q.get_sender());
    if(q.get_sender() != parent)
    {
        if(q.best_edge_weight < best_edge_weight)
        {
            best_edge_weight = q.best_edge_weight;
            best_edge = connect_edge;
        }

        reports++;
        report();
    }
    else
    {
        if(state == Node::SEARCH)
            postpone(q.shared_from_this());
        else if(q.best_edge_weight > best_edge_weight)
            change_core();
        else if(q.best_edge_weight == Edge::INF_WEIGHT && best_edge_weight == Edge::INF_WEIGHT)
        {
            state = Node::END;
            for(const Edge& i : edges)
                if(i.get_state() == Edge::BRANCH && i.get_end() != parent)
                    send(std::make_shared<::END>(id, i.get_end()));
        }
    }
}

void Node::visit(const TEST& q)
{
    size_t connect_edge = get_edge(q.get_sender());
    if(q.component.fragment == component.fragment)
    {
        if(edges[connect_edge].get_state() == Edge::UNKNOWN)
            edges[connect_edge].state = Edge::REJECTED;

        if(q.get_sender() != test_node)
            send(std::make_shared<REJECT>(id, q.get_sender()));
        else
            test();
    }
    else if(q.component.level <= component.level)
        send(std::make_shared<ACCEPT>(id, q.get_sender()));
    else
        postpone(q.shared_from_this());
}

void Node::visit(const WAKE_UP& q)
{
    if(state == Node::SLEEP)
    {
        best_edge = find_min_edge();
        if(best_edge != Edge::UDEF)
        {
            state = Node::FOUND;
            edges[best_edge].state = Edge::BRANCH;
            send(std::make_shared<CONNECT>(id, edges[best_edge].get_end(), component));
        }
        else
            state = Node::END;
    }
}

void Node::visit(const ACCEPT& q)
{
    size_t connect_edge = get_edge(q.get_sender());
    test_node = Node::UDEF;

    if(edges[connect_edge].get_weight() < best_edge_weight)
    {
        best_edge_weight = edges[connect_edge].get_weight();
        best_edge = connect_edge;
    }

    report();
}

void Node::visit(const ::END& q)
{
    state = Node::END;
    for(const Edge& i : edges)
        if(i.get_end() != parent && i.get_state() == Edge::BRANCH)
            send(std::make_shared<::END>(id, i.get_end()));
}

void INIT::accept(Visitor& visitor) const
{
    visitor.visit(*this);
}

void CHANGE_CORE::accept(Visitor& visitor) const
{
    visitor.visit(*this);
}

void CONNECT::accept(Visitor& visitor) const
{
    visitor.visit(*this);
}

void REJECT::accept(Visitor& visitor) const
{
    visitor.visit(*this);
}

void REPORT::accept(Visitor& visitor) const
{
    visitor.visit(*this);
}

void TEST::accept(Visitor& visitor) const
{
    visitor.visit(*this);
}

void WAKE_UP::accept(Visitor& visitor) const
{
    visitor.visit(*this);
}

void ACCEPT::accept(Visitor& visitor) const
{
    visitor.visit(*this);
}

void END::accept(Visitor& visitor) const
{
    visitor.visit(*this);
}

