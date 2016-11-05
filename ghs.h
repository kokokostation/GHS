#ifndef GHS_H_INCLUDED
#define GHS_H_INCLUDED

#include "graph_as_vector.h"

class Ghs_node
{
public:
    virtual Graph_as_vector get_branches() const = 0;
    virtual ~Ghs_node() = default;
};

Graph_as_vector ghs(const Graph_as_vector& graph);

#endif // GHS_H_INCLUDED
