# C++ trait generator

## Introduction

This tool generates type-erased traits (a.k.a. type erasures) for C++.
Unlike `std::function` or similar constructs, the generated traits support multiple APIs — more like a base class with virtual functions. Since the type is erased, client code does not need to inherit from a common base class.

## Usage

### Install

```bash
uv venv && uv pip install -e .
```

### Generate traits from JSON

```bash
python -m gen_trait <json_file>
```

Or use the shell script to batch-generate from a directory of JSON files:

```bash
cd example
./gen_trait.sh [json_dir]
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
│   ├── json/                     # trait JSON configs
│   ├── generated/                # generated .hpp files
│   ├── src/                      # C++ test/example source
│   ├── gen_trait.sh
│   └── gen_trait.bat
├── src/gen_trait/                 # Python package
│   ├── __init__.py
│   ├── __main__.py
│   └── gen_trait.py
├── pyproject.toml
├── gen_trait.schema.json
└── readme.md
```

## Input format

gen_trait accepts a JSON file describing the traits to generate.
A JSON schema is provided (`gen_trait.schema.json`) to describe the required data structure.
You can also refer to the example inputs in `example/json/`.

Here is a brief description of the input format:

- **include**: optional, array of strings. `#include <memory>` is always added to the generated header.

- **namespace**: required, string. The namespace for the generated traits. Empty string = global namespace.

- **trait**: required, array of trait objects.

  - **name**: required, string. Trait name.

  - **template**: optional, array of template parameter objects.

    - **name**: required, string. Template parameter name.
    - **type**: required, string. Template parameter type.
    - **pack**: optional, boolean. Whether this is a variadic template parameter.

  - **func**: required, array of function objects.

    - **name**: required, string. Function name.
    - **ret**: required, string. Return type.
    - **args**: required, array of argument objects.
      - **name**: required, string. Argument name.
      - **type**: required, string. Argument type.
      - **wrap**: optional, string. Wrap the argument with this function call when passing to implementation. E.g. `std::move`.
      - **cvref**: optional, string. CV/ref qualifiers for the argument.
    - **cvref**: optional, string. CV/ref qualifiers for the function itself.

  - **gen**: optional, array of `"r"` (ref), `"u"` (unique), `"s"` (shared) to control which traits are generated. Defaults to all.

  - **inplace_ref**: optional, boolean. Force in-place vtable in trait reference for better performance (one less indirection). Default is `true` if the trait has only one member function, otherwise `false`.

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
