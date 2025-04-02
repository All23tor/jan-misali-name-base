#ifndef BASE_NAME_HPP
#define BASE_NAME_HPP

#include <cmath>
#include <set>
#include <string>
#include <vector>

using Number = long long;
struct Base {
  Number val;
  std::string name;
  std::string prefix;
  Number roots;
};

inline bool operator<(const Base& lhs, const Base& rhs) {
  return lhs.val < rhs.val;
}

inline bool operator<(const Base& lhs, Number num) {
  return lhs.val < num;
}

inline bool operator<(Number num, const Base& rhs) {
  return num < rhs.val;
}

namespace {
// It has now ocurred to me that perchance this algorithm kind of profundly
// sucks
bool is_prime(Number val) {
  static std::set<Number> primes = {2, 3, 5, 7, 11, 13, 17};
  static Number tested = 20;

  Number sqrt_val = std::sqrt(val);
  Number interval = std::max(1000ll, sqrt_val);
  while (tested < val) {
    std::vector<bool> vec(interval, true);
    for (Number p : primes) {
      for (Number offset = (tested % p == 0) ? 0 : (p - tested % p);
           offset < interval; offset += p)
        vec[offset] = false;
    }
    for (Number i = 0; i < interval; i++) {
      if (vec[i])
        primes.insert(i + tested);
    }
    tested += interval;
  }

  if (primes.contains(val))
    return true;

  auto upb = primes.upper_bound(sqrt_val);
  for (auto it = primes.begin(); it != upb; it++)
    if (val % *it == 0)
      return false;

  return true;
}

auto merge_strings(std::string_view left, std::string_view right) {
  static constexpr std::string_view vowels = "aeiou";
  if (left.back() == 'i' && (right[0] == 'i' || right[0] == 'u'))
    return std::string(left).append(right.substr(1));

  if ((left.back() == 'a' || left.back() == 'o') && vowels.contains(right[0]))
    return std::string(left.substr(0, left.size() - 1)).append(right);
  if (right == "decimal")
    return std::string(left).append("gecimal");
  if (right == "baker's docenal")
    return std::string(left).append("ker's dozenal");
  return std::string(left).append(right);
}

auto get_or_insert(Number val) {
  static std::set<Base, std::less<void>> bases = {
      {0, "nullary", "null", 1},      {1, "unary", "mono", 1},
      {2, "binary", "bi", 1},         {3, "trinary", "tri", 1},
      {4, "quaternary", "tetra", 1},  {5, "quinary", "penta", 1},
      {6, "seximal", "hexa", 1},      {7, "septimal", "hepta", 1},
      {8, "octal", "octo", 1},        {9, "nonary", "enna", 1},
      {10, "decimal", "deca", 1},     {11, "elevenary", "leva", 1},
      {12, "dozenal", "doza", 1},     {13, "baker's dozenal", "baker", 1},
      {16, "hex", "tesser", 1},       {17, "suboptimal", "mal", 1},
      {20, "vigesimal", "icosi", 1},  {36, "niftimal", "feta", 1},
      {100, "centesimal", "hecto", 1}};

  if (auto it = bases.find(val); it != bases.end()) {
    return it;
  }

  if (is_prime(val)) {
    auto one_less = get_or_insert(val - 1);
    std::string name = "un" + one_less->name;
    std::string prefix = "hen" + one_less->prefix + "sna";
    Base jan{val, std::move(name), std::move(prefix), one_less->roots};
    return bases.insert(std::move(jan)).first;
  }

  auto best_left = bases.end();
  auto best_right = bases.end();
  auto best_root_count = std::numeric_limits<Number>::max();
  for (Number i = std::floor(std::sqrt(val)); i >= 2; i--) {
    if (val % i != 0)
      continue;
    auto left = get_or_insert(i);
    auto right = get_or_insert(val / i);
    Number root_count = left->roots + right->roots;
    if (root_count > best_root_count)
      continue;
    if (root_count == best_root_count)
      if (right->val - left->val > best_right->val - best_left->val)
        continue;

    best_left = left;
    best_right = right;
    best_root_count = root_count;
  }
  std::string name = merge_strings(best_left->prefix, best_right->name);
  std::string prefix = merge_strings(best_left->prefix, best_right->prefix);
  Base jan{val, std::move(name), std::move(prefix), best_root_count};
  return bases.insert(std::move(jan)).first;
}
}; // namespace

inline Base base_name(long long val) {
  if (val < 0) {
    auto abs = get_or_insert(-val);
    return {val, "nega" + abs->name, "nega" + abs->prefix, abs->roots};
  }
  return *get_or_insert(val);
}

#endif // !BASE_NAME_HPP
