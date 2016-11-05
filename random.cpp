#include <random>

size_t rnd(size_t from, size_t to)
{
    static std::random_device generator;

    std::uniform_int_distribution<size_t> distribution(from, to);

    return distribution(generator);
}
