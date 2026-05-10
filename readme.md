# C++ trait generator

## Introduction

This tool generates type-erased traits (a.k.a. type erasures) for C++.
Unlike `std::function` or similar constructs, the generated traits support multiple APIs — more like a base class with virtual functions. Since the type is erased, client code does not need to inherit from a common base class.

## Usage

### Install

```bash
uv venv && uv pip install -e .
```

### Generate traits

```bash
python -m gen_trait <file>
```

Accepts both `.trait` (C++-like DSL) and `.json` files. Format is auto-detected.

Or use the shell script to batch-generate from a directory:

```bash
cd example
./gen_trait.sh [dir]
```

### Build & run examples

```bash
cmake -B build example
cmake --build build
./build/gen_trait_example
```

## Project structure

```
gen_trait/
├── cmake/                        # CMake modules
│   └── GoogleBenchAndTest.cmake
├── example/                      # C++ example
│   ├── CMakeLists.txt
│   ├── json/                     # trait definitions (.trait and .json)
│   ├── generated/                # generated .hpp files
│   ├── src/                      # C++ test/example source
│   ├── gen_trait.sh
│   └── gen_trait.bat
├── src/gen_trait/                 # Python package
│   ├── __init__.py
│   ├── __main__.py
│   ├── gen_trait.py              # code generation backend
│   ├── lexer.py                  # DSL tokenizer
│   └── parser.py                 # DSL parser
├── pyproject.toml
├── gen_trait.schema.json
└── readme.md
```

## Input format

gen_trait supports two input formats: a C++-like DSL (recommended) and JSON (legacy).

### DSL format (`.trait`)

A natural C++-like syntax:

```
#include <iostream>

namespace my::lib;

trait drawable [[inplace_ref]] {
    void draw(std::ostream& os);
    void draw_cap(std::ostream& os) const;
};

template<typename R, typename... Args>
trait callable {
    R operator()(Args... args) const;
    R operator()();
};
```

#### DSL features

- **`#include`** — standard include directives; `<memory>` and `<functional>` are always added.
- **`namespace`** — qualified name with `;` terminator.
- **`template<...>`** — standard C++ template parameter syntax, including `...` for parameter packs.
- **`trait Name [[attrs]] { ... };`** — trait declaration with optional attributes.
- **Function declarations** — standard C++ signature syntax.
- **`[[attribute]]`** — C++11-style attributes for metadata.

#### Attributes

| Attribute | Applies to | Example | Meaning |
|---|---|---|---|
| `gen(modes)` | trait | `[[gen(u, s)]]` | Which variants to generate: `r`=ref, `u`=unique, `s`=shared. Default: all. |
| `inplace_ref` | trait | `[[inplace_ref]]` or `[[inplace_ref(false)]]` | In-place vtable in ref for fewer indirections. Default: true if single function. |
| `wrap(expr)` | parameter | `[[wrap(std::move)]]` | Wrap argument when forwarding to implementation. |

### JSON format (`.json`)

A JSON schema is provided (`gen_trait.schema.json`). Example inputs in `example/json/`.

- **include**: optional, array of strings. `#include <memory>` is always added.
- **namespace**: required, string. Empty string = global namespace.
- **trait**: required, array of trait objects.
  - **name**: required, string.
  - **template**: optional, array of `{type, name, pack?}` objects.
  - **func**: required, array of `{name, ret, args[], cvref?}` objects.
    - **args**: `{name, type, cvref?, wrap?}`.
  - **gen**: optional, array of `"r"`, `"u"`, `"s"`. Defaults to all.
  - **inplace_ref**: optional, boolean.

## Implementation details

For each trait named `example`, three classes are generated:

- **`example`**: uniquely owned trait. Move-only, not copyable.
- **`example_shared`**: shared trait. Copyable, reference-counted via `std::shared_ptr`.
- **`example_ref`**: non-owning reference. Implicitly constructible from `example` and `example_shared`.

Specializations of `std::hash` are also provided for all three classes.

Each trait function call incurs exactly two indirections: one for virtual table lookup and one for the function call itself. The virtual table is generated statically for each erased type and shared by all instances of the same type (not heap-allocated).

Space overhead:
- `trait` and `trait_ref`: 2 pointers
- `trait_shared`: 1 pointer + 1 `shared_ptr`
