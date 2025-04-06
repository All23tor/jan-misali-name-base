#include "BaseName2.hpp"
#include <iostream>
#include <print>
#include <set>

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
  int parse_base = use_decimal ? 10 : 6;

  if (show_help || argc < 2) {
    std::println(help_text);
    return 0;
  }

  std::vector<std::pair<Number, Number>> ranges;
  try {
    for (const auto& argument : numbers) {
      try {
        auto separator_index = argument.find("..");
        if (separator_index == argument.npos) {
          Number val = std::stoll(std::string(argument), nullptr, parse_base);
          ranges.emplace_back(val, val);
        } else {
          auto first = std::string(argument.substr(0, separator_index));
          auto second = std::string(argument.substr(separator_index + 2));
          auto start = std::stoll(first, nullptr, parse_base);
          auto end = std::stoll(second, nullptr, parse_base);
          ranges.emplace_back(start, end);
        }
      } catch (const std::invalid_argument&) {
        throw std::invalid_argument(std::string(argument));
      } catch (const std::out_of_range&) {
        throw std::out_of_range(std::string(argument));
      }
    }
  } catch (const std::invalid_argument& ia) {
    std::println(std::cerr, "Unable to parse {} using {}", ia.what(),
                 use_decimal ? "decimal" : "seximal");
    if (use_decimal)
      std::println(std::cerr, "Use '+h' to show help");
    else
      println(std::cerr, "Use '+d' to use decimal");
    return -1;
  } catch (const std::out_of_range& oor) {
    std::println(std::cerr, "{} out of range", oor.what());
    return -1;
  }

  for (const auto& [start, end] : ranges) {
    for (Number i = start; i <= end; ++i) {
      std::string line;
      if (show_values)
        line += std::to_string(i) + " | ";
      line += base_name(i);
      if (show_prefix)
        line += " | " /* + base_prefix(i)*/;
      if (show_roots)
        line += " | " + std::to_string(base_roots(i));
      std::println("{}", line);
    }
  }
}
