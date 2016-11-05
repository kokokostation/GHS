#include <vector>
#include <deque>
#include <memory>
#include <algorithm>

#include "emulator.h"
#include "graph_as_vector.h"

size_t Emulator_query::get_sender() const
{
    return sender;
}

size_t Emulator_query::get_recipient() const
{
    return recipient;
}

const std::shared_ptr<Emulator_node>& Emulator::operator[](size_t i)
{
    return nodes[i];
}

void Emulator::process_queries(const std::deque<std::shared_ptr<const Emulator_query>>& queries)
{
    for(typename std::deque<std::shared_ptr<const Emulator_query>>::const_iterator i = queries.begin(); i != queries.end(); ++i)
    {
        if(graph.find(Graph_as_vector::Primitive_edge((*i)->get_sender(), (*i)->get_recipient()).standartize()) != graph.end())
            box[(*i)->get_recipient()].push_back(*i);
        else
            throw bad_ghs();
    }
}

void Emulator::random_wake_up()
{
    size_t num = rnd(1, nodes.size());
    for(size_t i = 0; i < num; ++i)
    {
        size_t rnode = rnd(0, nodes.size() - 1);
        process_queries(nodes[rnode]->wake_up());
    }
}

void Emulator::process()
{
    bool go = true, all_boxes_empty = true;

    std::vector<size_t> indexes(box.size());
    for(size_t i = 0; i < indexes.size(); i++)
        indexes[i] = i;

    while(go)
    {
        if(all_boxes_empty)
            random_wake_up();
        else
            all_boxes_empty = true;

        std::random_shuffle(indexes.begin(), indexes.end());
        size_t box_num = rnd(1, box.size());

        for(std::vector<size_t>::iterator i = indexes.begin(); i != indexes.begin() + box_num; ++i)
        {
            if(box[*i].empty())
                continue;

            std::shared_ptr<const Emulator_query> q = box[*i].front();
            box[*i].pop_front();

            process_queries(nodes[*i]->tick(q));
        }

        for(const std::deque<std::shared_ptr<const Emulator_query>>& i : box)
            if(!i.empty())
            {
                all_boxes_empty = false;
                break;
            }

        go = false;
        for(const std::shared_ptr<Emulator_node>& i : nodes)
            go = go || !i->ended();
    }
}
