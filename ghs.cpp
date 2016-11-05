#include <memory>

#include "graph_as_vector.h"
#include "emulator.h"
#include "node.h"

Graph_as_vector ghs(const Graph_as_vector& graph)
{
    Emulator e = Emulator::create<Node>(graph);
    e.process();

    Graph_as_vector result(graph.get_node_num());
    for(size_t i = 0; i < graph.get_node_num(); ++i)
        result.add_edges(std::dynamic_pointer_cast<const Ghs_node>(e[i])->get_branches());

    result.standartize();

    return result;
}
