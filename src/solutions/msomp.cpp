#include <algorithm>
#include <iostream>
#include <omp.h>
#include <vector>

const int PARALLEL_THRESHOLD = 1 << 17;

// merge [l1, r1), [l2, r2)
long long ms_merge_core(int *l1, int *r1, int *l2, int *r2, int *buf) {
    long long cnt = 0;
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
    return cnt;
}

long long ms_merge(int *l, int *m, int *r) {
    long long cnt = 0, cntloc = 0;
    std::vector<int> b(r - l);
    int *p = b.data();
    if (r - l <= PARALLEL_THRESHOLD) {
        cnt = ms_merge_core(l, m, m, r, p);
        std::copy(b.begin(), b.end(), l);
        return cnt;
    }
    int nths = omp_get_max_threads();
    const int BLOCK_LEN = (m - l + nths - 1) / nths;
    int *j = m;
    for (int *i = l; i < m; i += BLOCK_LEN) {
        int *nxt = std::min(m, i + BLOCK_LEN);
        int val = *(nxt - 1);
        auto it = std::lower_bound(j, r, val);
#pragma omp task shared(b, cnt) firstprivate(i, nxt, j, it, p)
        {
            long long c = ms_merge_core(i, nxt, j, it, p);
#pragma omp atomic
            cnt += c;
        }
        p += nxt - i;
        p += it - j;
        cntloc += (long long)(it - j) * (m - nxt);
        j = it;
    }
    if (j < r) {
        std::copy(j, r, p);
    }
#pragma omp taskwait
#pragma omp parallel for
    for (int k = 0; k < r - l; ++k) {
        l[k] = b[k];
    }
    return cnt + cntloc;
}

// solving a[l..r)
long long ms(int *l, int *r) {
    if (r - l <= 1) {
        return 0LL;
    }
    int *m = l + (r - l) / 2;
    long long lc = 0, rc = 0;
    if (r - l <= PARALLEL_THRESHOLD) {
        lc = ms(l, m);
        rc = ms(m, r);
    } else {
#pragma omp task shared(lc)
        lc = ms(l, m);
#pragma omp task shared(rc)
        rc = ms(m, r);
#pragma omp taskwait
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
    long long cnt = 0;
#pragma omp parallel
    {
#pragma omp single
        cnt = ms(a.data(), a.data() + n);
    }
    std::cout << cnt << std::endl;
}
