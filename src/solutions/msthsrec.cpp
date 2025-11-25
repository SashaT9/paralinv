#include <algorithm>
#include <atomic>
#include <iostream>
#include <thread>
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

static std::atomic<int> num_threads(0);
const int MAX_THREADS = std::thread::hardware_concurrency();

// solving a[l..r)
long long ms(int *l, int *r) {
    if (r - l <= 1) {
        return 0LL;
    }
    int *m = l + (r - l) / 2;
    long long lc, rc;
    if (num_threads >= MAX_THREADS) {
        lc = ms(l, m);
        rc = ms(m, r);
    } else {
        num_threads++;
        std::thread t([=, &lc] {
            lc = ms(l, m);
            num_threads--;
        });
        rc = ms(m, r);
        t.join();
    }
    return lc + rc + ms_merge(l, m, r);
}
int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);
    int n;
    std::cin >> n;
    std::vector<int> a(n);
    for (int i = 0; i < n; ++i) {
        std::cin >> a[i];
    }
    std::cout << ms(a.data(), a.data() + n) << std::endl;
}
