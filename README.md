# Non-deterministic Turing machine simulator

[![Build Status](https://travis-ci.org/amecava/mtndsimulator.svg?branch=master)](https://travis-ci.org/amecava/mtndsimulator)

Implementation in C, using only the standard library (libc) and the base runtime, of a non-deterministic Turing machine, in the single tape variant, that accepts or rejects strings based on a given set of transitions.

## Usage

Run:
```sh
$ ./a.out < input_output/test_1/input.test
```

Run and check results:
```sh
$ ./a.out < input_output/test_1/input.test | diff input_output/test_1/output.test -
```

## Authors

* **Amedeo Cavallo**
