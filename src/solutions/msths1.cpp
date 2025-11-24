#include <algorithm>
#include <iostream>
#include <vector>

long long ms_merge(int *l, int *m, int *r) {
    std::vector<int> b(r - l);
    auto it = b.begin();
    int *y = m;
    long long cnt = 0;
    for (int *x = l; x < m; ++x) {
        while (y < r && *y < *x) {
            *(it++) = *y;
            ++y;
        }
        cnt += y - m;
        *(it++) = *x;
    }
    while (y < r) {
        *(it++) = *y;
        ++y;
    }
    std::copy(b.begin(), b.end(), l);
    return cnt;
}

// solving a[l..r)
long long ms(int *l, int *r) {
    if (r - l <= 1) {
        return 0LL;
    }
    int *m = l + (r - l) / 2;
    long long lc = ms(l, m);
    long long rc = ms(m, r);
    return lc + rc + ms_merge(l, m, r);
}
int main() {
    int n;
    std::cin >> n;
    std::vector<int> a(n);
    for (int i = 0; i < n; ++i) {
        std::cin >> a[i];
    }
    std::cout << ms(a.data(), a.data() + n) << std::endl;
}
