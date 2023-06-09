# C++ trait generator

## Introduction

This little tool can be used to generate traits aka type erasures for C++.
Unlike std::function or similar constructs, the generated traits support multiple apis, that is, more like a base class with virtual functions. Since type erased, client code does not need to inherit from common base class.

## Usage

The tool is a python script so you can just copy it to your project and use your automation tools or build system to invoke it.
Provided is also a bash script that can be used to scan all json files in a directory and generate traits for them: ```./gen_trait.sh <directory>```

## Input format

gen_trait.py accept a json file as input which describe the traits to be generated.
A json schema is provided to describe the required data structure.
You can also refer to the example input in the example directory.
Here is a brief description of the input format:

- include: optional, should be an array of strings, ```#include <memory>``` is always added to the generated header file.

- namespace: required, should be a string, the namespace of the generated traits enclosed in. If empty string, they are put in global namespace.

- pragma: optional, array of strings to provide pragma directives for the generated header file.

- trait: required, describes the traits to be generated.

  - name: required, string, the name of the trait.

  - template: optional, array of objects to describe template parameters of the trait.

    - name: required, string, template parameter name.

    - type: required, string, template parameter type.

    - pack: optional, boolean, whether the template parameter is a variadic template parameter.

  - func: required, array of objects to describe trait functions.

    - name: required, string, function name.

    - ret: required, string, return type of the function.

    - args: required, array of objects to describe function arguments.

      - name: required, string, argument name.

      - type: required, string, argument type.

      - wrap: optional, string, when passed to actual implementation, wrap the argument with this function call.
      For example, specify wrap as ```std::move``` to make ```x``` pass as ```std::move(x)```.

      - cvref: optional, string, add cv ref qualifiers.


## Implementation details

For each trait named ```example```, three classes are generated:

- ```example```: uniquely owned trait. It is move only and cannot be copied.

- ```example_shared```: shared trait. It is copyable and is reference counted as underlying implemented by ```std::shared_ptr```.

- ```example_ref```: reference to trait. It does not hold ownership and can be implicitly converted by ```example``` and ```example_shared```.

Specializations of ```std::hash``` are also provided for all three classes.

Regarding to overhead, each trait function call is exactly two indirection, one for virtual table lookup and another one for the function call itself. Virtual table is generated statically for each erased type and is shared by all instances of the same type, thus not allocated on heap.

As for space, both trait and trait ref are 2 pointers and trait shared is 1 pointer and 1 shared_ptr. It is possible to implement the shared trait with intrusive ref counting to object creation and deletion maybe a little bit faster. It is up to further investigation.

