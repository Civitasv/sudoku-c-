#include <ctime>
#include "shuffle.hpp"

void shuffle(int *array, size_t n)
{
    srand(time(NULL));
    if (n > 1)
    {
        size_t i;
        int t;
        for (i = 0; i < n - 1; i++)
        {
            size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
            t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}