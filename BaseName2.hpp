#include <cmath>
#include <limits>
#include <map>
#include <string>
#include <vector>

namespace {
using Number = long long;

struct factorization {
  Number roots;
  Number best_factor;
};

std::map<Number, const factorization> factors = {
    {1, {0, 1}},   {2, {1, 2}},   {3, {1, 3}},    {4, {1, 4}},   {5, {1, 5}},
    {6, {1, 6}},   {7, {1, 7}},   {8, {1, 8}},    {9, {1, 9}},   {10, {1, 10}},
    {11, {1, 11}}, {12, {1, 12}}, {13, {1, 13}},  {16, {1, 16}}, {17, {1, 17}},
    {20, {1, 20}}, {36, {1, 36}}, {100, {1, 100}}};

// Returns the smallest of the best pair of factors
auto get_factorization(Number n) {
  if (auto it = factors.find(n); it != factors.end())
    return it;

  Number best_number = 1;
  Number best_root_count = std::numeric_limits<Number>::max();
  for (Number i = std::ceil(std::sqrt(n)); i >= 2; --i) {
    if (n % i)
      continue;
    Number root_count = get_factorization(i)->second.roots +
                        get_factorization(n / i)->second.roots;
    if (root_count < best_root_count) {
      best_root_count = root_count;
      best_number = i;
    }
  }
  return factors.insert({n, {best_root_count, best_number}}).first;
};

void insert_factors(std::vector<Number>& v, Number n) {
  Number factor = get_factorization(n)->second.best_factor;
  if (factor == n) // n is a root
    v.push_back(factor);
  else if (factor == 1) { // n is prime
    v.push_back(1);
    insert_factors(v, n - 1);
    v.push_back(-1);
  } else { // n is composite
    insert_factors(v, factor);
    insert_factors(v, n / factor);
  }
}
} // namespace

inline std::string base_name(long long n) {
  std::string ans;
  std::vector<long long> factors;
  insert_factors(factors, n);
  for (auto&& e : factors) {
    ans += std::to_string(e);
    ans += ' ';
  }
  return ans;
}

inline long long base_roots(long long n) {
  return get_factorization(n)->second.roots;
}
