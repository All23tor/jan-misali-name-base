#include <cmath>
#include <limits>
#include <string>
#include <unordered_map>

namespace {
using Number = long long;

struct factorization {
  Number roots;
  Number best_factor;
};

std::unordered_map<Number, const factorization> factors = {
    {0, {0, 0}},   {1, {0, 1}},   {2, {1, 2}},   {3, {1, 3}},    {4, {1, 4}},
    {5, {1, 5}},   {6, {1, 6}},   {7, {1, 7}},   {8, {1, 8}},    {9, {1, 9}},
    {10, {1, 10}}, {11, {1, 11}}, {12, {1, 12}}, {13, {1, 13}},  {16, {1, 16}},
    {17, {1, 17}}, {20, {1, 20}}, {36, {1, 36}}, {100, {1, 100}}};

// Returns the smallest of the best pair of factors
auto get_factorization(Number n) {
  if (auto it = factors.find(n); it != factors.end())
    return it;

  Number best_number = 1;
  Number best_root_count = std::numeric_limits<Number>::max();
  for (Number i = std::floor(std::sqrt(n)); i >= 2; --i) {
    if (n % i)
      continue;
    auto left = get_factorization(i)->second;
    auto right = get_factorization(n / i)->second;
    Number root_count = left.roots + right.roots;
    if (root_count > best_root_count)
      continue;
    if (root_count == best_root_count &&
        (n / i - i) > (n / best_number - best_number))
      continue;

    best_root_count = root_count;
    best_number = i;
  }

  if (best_number == 1) // n is prime
    best_root_count = get_factorization(n - 1)->second.roots;

  return factors.insert({n, {best_root_count, best_number}}).first;
};

static const std::unordered_map<Number, std::pair<const char*, const char*>> affixes{
    {0, {"nullary", "null"}},      {1, {"unary", "mono"}},
    {2, {"binary", "bi"}},         {3, {"trinary", "tri"}},
    {4, {"quaternary", "tetra"}},  {5, {"quinary", "penta"}},
    {6, {"seximal", "hexa"}},      {7, {"septimal", "hepta"}},
    {8, {"octal", "octo"}},        {9, {"nonary", "enna"}},
    {10, {"gesimal", "deca"}},     {11, {"elevenary", "leva"}},
    {12, {"dozenal", "doza"}},     {13, {"ker's dozenal", "baker"}},
    {16, {"hex", "tesser"}},       {17, {"suboptimal", "mal"}},
    {20, {"vigesimal", "icosi"}},  {36, {"niftimal", "feta"}},
    {100, {"centesimal", "hecto"}}};

const char* to_suffix(Number n) {
  return affixes.at(n).first;
}

const char* to_prefix(Number n) {
  return affixes.at(n).second;
}

void append_and_format(std::string& left, const char* right) {
  static constexpr std::string_view vowels = "aeiou";
  if (left.back() == 'i' && (right[0] == 'i' || right[0] == 'u')) {
    right++;
  } else if ((left.back() == 'a' || left.back() == 'o') &&
             vowels.contains(right[0])) {
    left.pop_back();
  }
  left.append(right);
}

void insert_factors_prefix(std::string& s, Number n) {
  Number factor = get_factorization(n)->second.best_factor;
  if (factor == n) // n is a root
    append_and_format(s, to_prefix(factor));
  else if (factor == 1) { // n is prime
    append_and_format(s, "hen");
    insert_factors_prefix(s, n - 1);
    append_and_format(s, "sna");
  } else { // n is composite
    insert_factors_prefix(s, factor);
    insert_factors_prefix(s, n / factor);
  }
}

void insert_factors_suffix(std::string& s, Number n) {
  Number factor = get_factorization(n)->second.best_factor;
  if (factor == n) // n is a root
    append_and_format(s, to_suffix(factor));
  else if (factor == 1) { // n is prime
    append_and_format(s, "un");
    insert_factors_suffix(s, n - 1);
  } else { // n is composite
    insert_factors_prefix(s, factor);
    insert_factors_suffix(s, n / factor);
  }
}

} // namespace

inline std::string base_name(Number n) {
  std::string s;
  if (n < 0)          // Doing this before insertion may merge the 'a'
    s.append("nega"); // This is intentional, as BASEOFF does the same
  n = std::abs(n);
  insert_factors_suffix(s, n);
  if (n == 10)
    s.replace(s.rfind("ges"), 3, "dec");
  else if (n == 13)
    s.insert(s.rfind("ker"), "ba");
  return s;
}

inline std::string base_prefix(Number n) {
  std::string s;
  if (n < 0)          // Doing this before insertion may merge the 'a'
    s.append("nega"); // This is intentional, as BASEOFF does the same
  n = std::abs(n);
  insert_factors_prefix(s, n);
  return s;
}

inline long long base_roots(long long n) {
  return get_factorization(n)->second.roots;
}
