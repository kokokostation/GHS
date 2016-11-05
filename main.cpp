#include <iostream>
#include <sstream>
#include <cmath>
#include <utility>

#include "graph_as_vector.h"
#include "emulator.h"
#include "kruskal.h"
#include "node.h"
#include "random"

//#define TEST
//#define GENERATE_PRIMITIVE_TEST

int main()
{
#ifdef TEST
    freopen("input.txt", "r", stdin);
#endif // TEST

#ifndef GENERATE_PRIMITIVE_TEST
    std::istream& stream = std::cin;
#endif

#ifdef GENERATE_PRIMITIVE_TEST
    const size_t node_num_ = 228;

    std::vector<std::pair<size_t, size_t>> pairs;

    pairs.reserve(node_num_ * (node_num_ - 1) / 2);
    for(size_t i = 0; i < node_num_; ++i)
        for(size_t j = i + 1; j < node_num_; ++j)
            pairs.push_back(std::make_pair(i, j));
    std::random_shuffle(pairs.begin(), pairs.end());

    pairs.erase(pairs.begin() + rnd(pairs.size()/2, pairs.size()), pairs.end());

    std::vector<size_t> weights;

    weights.reserve(pairs.size());
    for(size_t i = 0; i < pairs.size(); ++i)
        weights.push_back(i);
    std::random_shuffle(weights.begin(), weights.end());

    std::stringstream stream;
    stream << node_num_ << " " << pairs.size() << std::endl;
    for(size_t i = 0; i < pairs.size(); ++i)
        stream << pairs[i].first << " " << pairs[i].second << " " << weights[i] << std::endl;
#endif // GENERATE_PRIMITIVE_TEST

    Graph_as_vector g;

    stream >> g;

    std::cout << (ghs(g) == mst(g));

    return 0;
}
