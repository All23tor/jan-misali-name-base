use std::{collections::HashSet, env, num::IntErrorKind, ops::RangeInclusive};

mod name_base;
use crate::name_base::*;

const HELP_TEXT: &str = "\
  a base-neutral system for naming numbering systems

  Usage: name-base  [OPTIONS]... [NUMBER]...

  NUMBER arguments must be of the following forms:
  \tn\tSingle number
  \tn..m\tInclusive range
  \tBy default it only displays the given NUMBERs base name.

  Options:
  \t+v\tDisplay numeric values
  \t+p\tDisplay prefix forms
  \t+r\tDisplay root counts
  \t+d\tDecimal mode (by default use seximal)
  \t+h\tDisplays this help text
";

struct Options {
  show_values: bool,
  show_prefix: bool,
  show_roots: bool,
  show_help: bool,
  use_decimal: bool,
}

impl Options {
  pub fn new(options: HashSet<char>) -> Self {
    Self {
      show_values: options.contains(&'v'),
      show_prefix: options.contains(&'p'),
      show_roots: options.contains(&'r'),
      show_help: options.contains(&'h'),
      use_decimal: options.contains(&'d'),
    }
  }

  pub fn print(&self, n: i64) {
    if self.show_values {
      print!("{} | ", n);
    }
    print!("{}", base_name(n));
    if self.show_prefix {
      print!(" | {}", base_prefix(n));
    }
    if self.show_roots {
      print!(" | {}", base_roots(n));
    }
    println!();
  }
}

fn select_arguments() -> (Options, Vec<String>) {
  let mut options = HashSet::new();
  let mut numbers = Vec::new();
  for arg in env::args().skip(1) {
    if let Some(opts) = arg.strip_prefix('+') {
      for c in opts.chars() {
        options.insert(c);
      }
    } else {
      numbers.push(arg);
    }
  }
  (Options::new(options), numbers)
}

fn parse_range(s: &str, radix: u32) -> Result<RangeInclusive<i64>, IntErrorKind> {
  Ok(if let Some((first, last)) = s.split_once("..") {
    let first_num = i64::from_str_radix(first, radix).map_err(|e| *e.kind())?;
    let last_num = i64::from_str_radix(last, radix).map_err(|e| *e.kind())?;
    first_num..=last_num
  } else {
    let num = i64::from_str_radix(s, radix).map_err(|e| *e.kind())?;
    num..=num
  })
}

fn main() {
  let (options, num_strings) = select_arguments();
  let radix = if options.use_decimal { 10 } else { 6 };

  if options.show_help || num_strings.is_empty() {
    print!("{HELP_TEXT}");
    return;
  }

  for num_string in num_strings {
    match parse_range(&num_string, radix) {
      Ok(range) => range.for_each(|n| options.print(n)),
      Err(error_kind) => match error_kind {
        IntErrorKind::Empty | IntErrorKind::InvalidDigit => {
          let base = if options.use_decimal {
            "decimal"
          } else {
            "seximal"
          };
          eprintln!("Unable to parse {num_string} using {base}");
          eprintln!("Use '+d' if you intended to use decimal");
          eprintln!("Use '+h' to show help");
        }
        IntErrorKind::PosOverflow | IntErrorKind::NegOverflow => {
          eprintln!("{num_string} out of range")
        }
        _ => panic!(),
      },
    };
  }
}
