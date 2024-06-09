// #2
#include <iostream>
#include <vector>
#include <algorithm>

std::vector<long long> primes(100001);

class ClosedLinearUnMap {
private:
    std::vector<std::vector<long long>> data;
    size_t size;
    size_t capacity;

    long long hashFunc(long long key) {
        return (key * 2654435761) % capacity;
    }

    void rehash() {
        capacity *= 2;
        std::vector<std::vector<long long>> temp(capacity);

        for (auto& chain : data) {
            for (long long key : chain) {
                long long h = hashFunc(key);
                temp[h].push_back(key);
            }
        }
        std::swap(data, temp);
    }

public:
    ClosedLinearUnMap() 
        : size(0)
        , capacity(100)
    {
        data.resize(capacity);
    }

    void insert(long long key) {
        long long h = hashFunc(key);
        data[h].push_back(key);
        ++size;
        if (size > capacity) {
            rehash();
        }
    }

    bool find(long long key) {
        long long h = hashFunc(key);
        for (long long k : data[h]) {
            if (k == key) return true;
        }
        return false;
    }
};

long long primeHashFunc(const std::vector<long long>& array, size_t start, size_t end) {
    long long h = 1;
    for (size_t i = start; i <= end; ++i) {
        h *= primes[array[i]];
    }
    return h;
}

bool isPrime(size_t n) {
    if (n == 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;

    for (size_t i = 2; i * i <= n; ++i) {
        if (n % i == 0) return false;
    }
    return true;
}

std::vector<long long> fillWithPrimes(size_t count) {
    std::vector<long long> primes;
    size_t num = 3;
    while (primes.size() < count) {
        if (isPrime(num)) primes.push_back(num);
        ++num;
    }
    return primes;
}

int main() {

    long long n;
    std::cin >> n;
    
    ClosedLinearUnMap UnMap;
    std::vector<long long> arr_1(n);
    for (long long i = 0; i < n; ++i) {
        std::cin >> arr_1[i];
    }

    std::vector<long long> original_primes = fillWithPrimes(1000);
    for (size_t i = 0; i < 100001; ++i) {
        primes[i] = original_primes[i % 1000];
    }

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = i; j < n; ++j) {
            long long h = primeHashFunc(arr_1, i, j);
            UnMap.insert(h);
        }
    }

    long long m;
    std::cin >> m;

    std::vector<long long> arr_2(m);
    for (long long i = 0; i < m; ++i) {
        std::cin >> arr_2[i];
    }

    size_t result = 0;
    for (size_t i = 0; i < m; ++i) {
        for (size_t j = i; j < m; ++j) {
            long long h = primeHashFunc(arr_2, i, j);
            if (UnMap.find(h)) {
                result = std::max(result, j - i + 1);
            }
        }
    }

    std::cout << result;
    return 0;
}