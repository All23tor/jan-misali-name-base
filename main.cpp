#include "BaseName.hpp"
#include <charconv>
#include <iostream>
#include <print>
#include <set>
#include <vector>

static constexpr std::string_view help_text =
    "a base-neutral system for naming numbering systems\n\n"
    "Usage: name-base  [OPTIONS]... [NUMBER]...\n\n"
    "NUMBER arguments must be of the following forms:\n"
    "\tn\tSingle number\n"
    "\tn..m\tInclusive range\n"
    "By default it only displays the given NUMBERs base name.\n\n"
    "Options:\n"
    "\t+v\tDisplay NUMBER values\n"
    "\t+a\tDisplay abbreviations\n"
    "\t+p\tDisplay prefix forms\n"
    "\t+r\tDisplay root counts\n"
    "\t+d\tDecimal mode (by default use seximal)\n"
    "\t+h\tDisplays this help text";

struct Arguments {
  std::set<char> options;
  std::vector<std::string_view> numbers;
};

static Arguments select_arguments(int argc, const char** argv) {
  Arguments arguments;
  for (int i = 1; i < argc; ++i) {
    const std::string_view argument = argv[i];
    if (argument.starts_with('+')) {
      for (char c : argument.substr(1))
        arguments.options.insert(c);
    } else
      arguments.numbers.push_back(argument);
  }
  return arguments;
}

int main(int argc, const char** argv) {
  auto [options, numbers] = select_arguments(argc, argv);
  bool show_values = options.contains('v');
  bool show_prefix = options.contains('p');
  bool show_roots = options.contains('r');
  bool show_help = options.contains('h');
  bool use_decimal = options.contains('d');
  int base = use_decimal ? 10 : 6;

  if (show_help || argc < 2) {
    std::println(help_text);
    return 0;
  }

  std::vector<std::pair<Number, Number>> ranges;
  for (const auto& number : numbers) {
    auto begin = number.data();
    auto end = begin + number.size();
    auto separator_index = number.find("..");
    auto separator = begin + separator_index;

    std::errc error{};
    Number first{};
    Number second{};
    if (separator_index == number.npos) {
      error = std::from_chars(begin, end, first, base).ec;
      second = first;
    } else {
      auto error1 = std::from_chars(begin, separator, first, base).ec;
      auto error2 = std::from_chars(separator + 2, end, second, base).ec;
      error = (error1 == std::errc{}) ? error2 : error1;
    }
    ranges.emplace_back(first, second);

    if (error == std::errc::invalid_argument) {
      std::println(std::cerr, "Unable to parse {} using {}", number,
                   use_decimal ? "decimal" : "seximal");
      if (!use_decimal)
        std::println(std::cerr, "Use '+d' if you intended to use decimal");
      std::println(std::cerr, "Use '+h' to show help");
      return -1;

    } else if (error == std::errc::result_out_of_range) {
      std::println(std::cerr, "{} out of range", number);
      return -1;
    }
  }

  for (const auto& [start, end] : ranges) {
    for (Number i = start; i <= end; ++i) {
      std::string line;
      if (show_values)
        line += std::to_string(i) + " | ";
      line += base_name(i);
      if (show_prefix)
        line += " | " + base_prefix(i);
      if (show_roots)
        line += " | " + std::to_string(base_roots(i));
      std::println("{}", line);
    }
  }
}
