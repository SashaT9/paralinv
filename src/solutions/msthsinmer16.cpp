#include <algorithm>
#include <iostream>
#include <thread>
#include <vector>

const int MAX_THREADS = 16;
const int PARALLEL_THRESHOLD = 1 << 17;

// merge [l1, r1), [l2, r2)
void ms_merge_core(int *l1, int *r1, int *l2, int *r2, int *buf,
                   long long &cnt) {
    while (l1 < r1 && l2 < r2) {
        if (*l1 < *l2) {
            *(buf++) = *(l1++);
        } else {
            cnt += r1 - l1;
            *(buf++) = *(l2++);
        }
    }
    while (l1 < r1) {
        *(buf++) = *(l1++);
    }
    while (l2 < r2) {
        *(buf++) = *(l2++);
    }
}

long long ms_merge(int *l, int *m, int *r) {
    long long cnt = 0;
    std::vector<int> b(r - l);
    int *p = b.data();
    if (r - l <= PARALLEL_THRESHOLD) {
        ms_merge_core(l, m, m, r, p, cnt);
    } else {
        const int BLOCK_LEN = (m - l + MAX_THREADS - 1) / MAX_THREADS;
        std::vector<std::thread> ths;
        std::vector<long long> cnts;
        cnts.reserve(MAX_THREADS);
        int *j = m;
        for (int *i = l; i < m; i += BLOCK_LEN) {
            int *nxt = std::min(m, i + BLOCK_LEN);
            int val = *(nxt - 1);
            auto it = std::lower_bound(j, r, val);
            cnts.push_back(0);
            ths.emplace_back(ms_merge_core, i, nxt, j, it, p,
                             std::ref(cnts.back()));
            p += nxt - i;
            p += it - j;
            cnt += (it - j) * (m - nxt);
            j = it;
        }
        if (j < r) {
            std::copy(j, r, p);
        }
        for (auto &th : ths) {
            th.join();
        }
        for (auto &c : cnts) {
            cnt += c;
        }
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
