#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED

#include <vector>
#include <deque>
#include <queue>
#include <climits>
#include <memory>

#include "graph_as_vector.h"
#include "emulator.h"
#include "ghs.h"

struct Query;
struct INIT;
struct CHANGE_CORE;
struct CONNECT;
struct REJECT;
struct REPORT;
struct TEST;
struct WAKE_UP;
struct ACCEPT;
struct END;

class Node;

struct Component
{
    size_t fragment, level;
    Component(size_t fragment, size_t level) : fragment(fragment), level(level) {};
};

class Edge
{
    friend Node;

public:
    enum State {UNKNOWN, REJECTED, BRANCH};
    enum {INF_WEIGHT = INT_MAX, UDEF = INT_MAX};

private:
    size_t end, weight;
    State state;

public:
    Edge(size_t end, State state, size_t weight);

    size_t get_end() const;
    size_t get_weight() const;
    State get_state() const;
};

class Visitor
{
public:
    virtual void visit(const INIT& q) = 0;
    virtual void visit(const CHANGE_CORE& q) = 0;
    virtual void visit(const CONNECT& q) = 0;
    virtual void visit(const REJECT& q) = 0;
    virtual void visit(const REPORT& q) = 0;
    virtual void visit(const TEST& q) = 0;
    virtual void visit(const WAKE_UP& q) = 0;
    virtual void visit(const ACCEPT& q) = 0;
    virtual void visit(const ::END& q) = 0;
    virtual ~Visitor() = default;
};

class Node : public Visitor, public Emulator_node, public Ghs_node
{
public:
    enum State {SLEEP, FOUND, SEARCH, END};

private:
    enum {UDEF = INT_MAX};
    size_t id, parent, best_edge_weight, best_edge, test_node, reports;
    State state;
    bool checking_postponed;
    Component component;

    std::vector<Edge> edges;
    std::deque<std::shared_ptr<const Emulator_query>> result;
    std::queue<std::shared_ptr<const Query>> postponed;

    size_t get_edge(size_t end) const;
    size_t find_min_edge() const;
    void send(std::shared_ptr<const Query> q);
    void postpone(std::shared_ptr<const Query> q);
    void test();
    void report();
    void change_core();
    void check_postponed();

    virtual void visit(const INIT& q) override;
    virtual void visit(const CHANGE_CORE& q) override;
    virtual void visit(const CONNECT& q) override;
    virtual void visit(const REJECT& q) override;
    virtual void visit(const REPORT& q) override;
    virtual void visit(const TEST& q) override;
    virtual void visit(const WAKE_UP& q) override;
    virtual void visit(const ACCEPT& q) override;
    virtual void visit(const ::END& q) override;

public:
    Node();

    virtual void set_id(size_t id_) override;
    virtual const std::deque<std::shared_ptr<const Emulator_query>>& tick(std::shared_ptr<const Emulator_query> q) override;
    virtual Graph_as_vector get_branches() const override;
    virtual void add_edge(size_t end, size_t weight) override;
    virtual const std::deque<std::shared_ptr<const Emulator_query>>& wake_up() override;
    virtual bool ended() const override;
};

struct Query : public Emulator_query, public std::enable_shared_from_this<Query>
{
    Query(size_t sender, size_t recipient);

    virtual void accept(Visitor& visitor) const = 0;
    virtual ~Query() = default;
};

struct INIT : public Query
{
    Node::State state;
    Component component;

    INIT() = default;
    INIT(size_t sender, size_t recipient, const Component& component, Node::State state) : Query(sender, recipient), state(state), component(component) {};
    virtual void accept(Visitor& visitor) const override;
};

struct CHANGE_CORE : public Query
{
    CHANGE_CORE() = default;
    CHANGE_CORE(size_t sender, size_t recipient) : Query(sender, recipient) {};
    virtual void accept(Visitor& visitor) const override;
};

struct CONNECT : public Query
{
    Component component;

    CONNECT() = default;
    CONNECT(size_t sender, size_t recipient, const Component& component) : Query(sender, recipient), component(component) {};
    virtual void accept(Visitor& visitor) const override;
};

struct REJECT : public Query
{
    REJECT() = default;
    REJECT(size_t sender, size_t recipient) : Query(sender, recipient) {};
    virtual void accept(Visitor& visitor) const override;
};

struct REPORT : public Query
{
    size_t best_edge_weight;

    REPORT() = default;
    REPORT(size_t sender, size_t recipient, size_t best_edge_weight) : Query(sender, recipient), best_edge_weight(best_edge_weight) {};
    virtual void accept(Visitor& visitor) const override;
};

struct TEST : public Query
{
    Component component;

    TEST() = default;
    TEST(size_t sender, size_t recipient, const Component& component) : Query(sender, recipient), component(component) {};
    virtual void accept(Visitor& visitor) const override;
};

struct WAKE_UP : public Query
{
    WAKE_UP() = default;
    WAKE_UP(size_t sender, size_t recipient) : Query(sender, recipient) {};
    virtual void accept(Visitor& visitor) const override;
};

struct ACCEPT : public Query
{
    ACCEPT() = default;
    ACCEPT(size_t sender, size_t recipient) : Query(sender, recipient) {};
    virtual void accept(Visitor& visitor) const override;
};

struct END : public Query
{
    END() = default;
    END(size_t sender, size_t recipient) : Query(sender, recipient) {};
    virtual void accept(Visitor& visitor) const override;
};

#endif // NODE_H_INCLUDED
