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
    Number root_count = get_factorization(i)->second.roots +
                        get_factorization(n / i)->second.roots;
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
  static const std::unordered_map<Number, const char*> prefixes{
      {0, "null"},  {1, "mono"},   {2, "bi"},    {3, "tri"},    {4, "tetra"},
      {5, "penta"}, {6, "hexa"},   {7, "hepta"}, {8, "octo"},   {9, "enna"},
      {10, "deca"}, {11, "leva"},  {12, "doza"}, {13, "baker"}, {16, "tesser"},
      {17, "mal"},  {20, "icosi"}, {36, "feta"}, {100, "hecto"}};

  Number factor = get_factorization(n)->second.best_factor;
  if (factor == n) // n is a root
    append_and_format(s, prefixes.at(factor));
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
  static const std::unordered_map<Number, const char*> suffixes{
      {0, "nullary"},     {1, "unary"},          {2, "binary"},
      {3, "trinary"},     {4, "quaternary"},     {5, "quinary"},
      {6, "seximal"},     {7, "septimal"},       {8, "octal"},
      {9, "nonary"},      {10, "gesimal"},       {11, "elevenary"},
      {12, "dozenal"},    {13, "ker's dozenal"}, {16, "hex"},
      {17, "suboptimal"}, {20, "vigesimal"},     {36, "niftimal"},
      {100, "centesimal"}};

  Number factor = get_factorization(n)->second.best_factor;
  if (factor == n) // n is a root
    append_and_format(s, suffixes.at(factor));
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
