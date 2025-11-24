#include <algorithm>
#include <iostream>
#include <numeric>
#include <random>
#include <vector>

std::mt19937 rng(std::random_device{}());
int main(int argc, char **argv) {
    int n = atoi(argv[1]);
    std::vector<int> a(n);
    iota(a.begin(), a.end(), 1);
    std::shuffle(a.begin(), a.end(), rng);
    std::cout << n << std::endl;
    for (int i = 0; i < n; ++i) {
        std::cout << a[i] << " ";
    }
    std::cout << std::endl;
}
