#include <cmath>
#include <iostream>
#include <limits>
#include <map>
#include <vector>

struct factorization {
  long long roots;
  long long best_factor;
};
static std::map<long long, const factorization> factors = {
    {1, {0, 1}},   {2, {1, 2}},   {3, {1, 3}},    {4, {1, 4}},   {5, {1, 5}},
    {6, {1, 6}},   {7, {1, 7}},   {8, {1, 8}},    {9, {1, 9}},   {10, {1, 10}},
    {11, {1, 11}}, {12, {1, 12}}, {13, {1, 13}},  {16, {1, 16}}, {17, {1, 17}},
    {20, {1, 20}}, {36, {1, 36}}, {100, {1, 100}}};

// Returns the smallest of the best pair of factors
static auto get_factorization(long long n) {
  if (auto it = factors.find(n); it != factors.end())
    return it;

  long long best_number = 1;
  long long best_root_count = std::numeric_limits<long long>::max();
  for (long long i = std::ceil(std::sqrt(n)); i >= 2; --i) {
    if (n % i)
      continue;
    long long root_count = get_factorization(i)->second.roots +
                           get_factorization(n / i)->second.roots;
    if (root_count < best_root_count) {
      best_root_count = root_count;
      best_number = i;
    }
  }
  return factors.insert({n, {best_root_count, best_number}}).first;
};

inline void insert_factors(std::vector<long long>& v, long long n) {
  long long factor = get_factorization(n)->second.best_factor;
  if (factor == n)
    v.push_back(factor);
  else if (factor == 1) {
    v.push_back(1);
    insert_factors(v, n - 1);
    v.push_back(-1);
  } else {
    insert_factors(v, factor);
    insert_factors(v, n / factor);
  }
}
