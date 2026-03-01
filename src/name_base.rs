use std::{
  collections::HashMap,
  sync::{LazyLock, RwLock},
};

macro_rules! define_roots {
  ($($variant:ident { $value:expr, suffix: $suffix:expr, prefix: $prefix:expr },)*) => {
    #[derive(Clone, Copy)]
    enum Root { $($variant),* }

    impl Root {
      const fn suffix(self) -> &'static str {
        match self { $(Self::$variant => $suffix,)* }
      }

      const fn prefix(self) -> &'static str {
        match self { $(Self::$variant => $prefix,)* }
      }
    }

    fn initial_cache() -> RwLock<HashMap<u64, Factorization>> {
      RwLock::new(HashMap::from([ $(($value, Factorization::Root(Root::$variant)),)*]))
    }
  };
}

define_roots! {
  Zero { 0, suffix: "nullary", prefix: "null" },
  One {  1, suffix: "unary", prefix: "mono" },
  Two { 2, suffix: "binary", prefix: "bi" },
  Three { 3, suffix: "trinary", prefix: "tri" },
  Four { 4, suffix: "quaternary", prefix: "tetra" },
  Five { 5, suffix: "quinary", prefix: "penta" },
  Six { 6, suffix: "seximal", prefix: "hexa" },
  Seven { 7, suffix: "septimal", prefix: "hepta" },
  Eight { 8, suffix: "octal", prefix: "octo" },
  Nine { 9, suffix: "nonary", prefix: "enna" },
  Ten { 10, suffix: "gesimal", prefix: "deca" },
  Eleven { 11, suffix: "elevenary", prefix: "leva" },
  Twelve { 12, suffix: "dozenal", prefix: "dozen" },
  Thirteen { 13, suffix: "ker's dozenal", prefix: "baker" },
  Sixteen { 16, suffix: "hex", prefix: "tesser" },
  Seventeen { 17, suffix: "suboptimal", prefix: "mal" },
  Twenty { 20, suffix: "vigesimal", prefix: "icosi" },
  Thirtysix { 36, suffix: "niftimal", prefix: "feta" },
  Hundred { 100, suffix: "centesimal", prefix: "hecto" },
}

#[derive(Clone, Copy)]
struct Composite {
  roots: u64,
  factor: u64,
}

#[derive(Clone, Copy)]
enum Factorization {
  Root(Root),
  Prime { pred_roots: u64 },
  Composite(Composite),
}

impl Factorization {
  fn roots(self) -> u64 {
    match self {
      Self::Root(root) => !matches!(root, Root::Zero | Root::One) as u64,
      Self::Prime { pred_roots: roots } => roots,
      Self::Composite(composite) => composite.roots,
    }
  }
}

static CACHED_FACTORS: LazyLock<RwLock<HashMap<u64, Factorization>>> = LazyLock::new(initial_cache);

fn factorize(n: u64) -> Factorization {
  if let Some(cached) = CACHED_FACTORS.read().expect("Lock poisoned").get(&n) {
    return *cached;
  }

  let divisors = (2..n.isqrt()).rev().filter(|&i| n.is_multiple_of(i));
  let composites = divisors.map(|factor| Composite {
    factor,
    roots: factorize(factor).roots() + factorize(n / factor).roots(),
  });

  let best = composites.min_by_key(|&Composite { roots, factor }| (roots, n / factor - factor));

  let factorization = if let Some(composite) = best {
    Factorization::Composite(composite)
  } else {
    let pred_roots = factorize(n - 1).roots();
    Factorization::Prime { pred_roots }
  };

  let mut cache = CACHED_FACTORS.write().expect("Lock poisoned");
  *cache.entry(n).or_insert(factorization)
}

fn add_smooth(mut left: String, mut right: &str) -> String {
  const VOWELS: [char; 5] = ['a', 'e', 'i', 'o', 'u'];

  if left.ends_with('i') && right.starts_with(['i', 'u']) {
    right = &right[1..];
  } else if left.ends_with(['a', 'o']) && right.starts_with(VOWELS) {
    left.pop();
  }
  left + right
}

fn add_prefix(s: String, n: u64) -> String {
  match factorize(n) {
    Factorization::Root(root) => add_smooth(s, root.prefix()),
    Factorization::Prime { .. } => {
      let hen_prefixed = add_smooth(s, "hen");
      let pred_added = add_prefix(hen_prefixed, n - 1);
      add_smooth(pred_added, "sna")
    }
    Factorization::Composite(composite) => {
      let prefixed = add_prefix(s, composite.factor);
      add_prefix(prefixed, n / composite.factor)
    }
  }
}

fn add_suffix(s: String, n: u64) -> String {
  match factorize(n) {
    Factorization::Root(root) => add_smooth(s, root.suffix()),
    Factorization::Prime { .. } => {
      let un_prefixed = add_smooth(s, "un");
      add_suffix(un_prefixed, n - 1)
    }
    Factorization::Composite(composite) => {
      let prefixed = add_prefix(s, composite.factor);
      add_suffix(prefixed, n / composite.factor)
    }
  }
}

// Doing this before insertion may merge the 'a'
// This is intentional, as BASEOFF does the same
fn nega_prefix(n: i64) -> String {
  String::from(if n.is_negative() { "nega" } else { "" })
}

pub fn base_name(n: i64) -> String {
  match n {
    10 => "decimal".to_string(),
    13 => "baker's dozenal".to_string(),
    n => add_suffix(nega_prefix(n), n.unsigned_abs()),
  }
}

pub fn base_prefix(n: i64) -> String {
  add_prefix(nega_prefix(n), n.unsigned_abs())
}

pub fn base_roots(n: i64) -> u64 {
  factorize(n.unsigned_abs()).roots()
}
