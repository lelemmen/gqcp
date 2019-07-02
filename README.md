# gqcp 0.2.0
[![Build Status](https://travis-ci.org/GQCG/gqcp.svg?branch=master)](https://travis-ci.org/GQCG/gqcp)

The Ghent Quantum Chemistry Package is a C++ library for electronic structure calculations.



## A quick example

Follow along the following documented example that calculates the FCI energy:

```cpp
#include <gqcp.hpp>


// Create the molecular Hamiltonian parameters in an AO basis
auto h2o = GQCP::Molecule::ReadXYZ("data/h2o.xyz");
auto mol_ham_par = GQCP::HamiltonianParameters<double>::Molecular(h2o, "STO-3G");


// Create a plain RHF SCF solver and solve the SCF equations
GQCP::PlainRHFSCFSolver plain_scf_solver (mol_ham_par, h2o);
plain_scf_solver.solve();
auto rhf = plain_scf_solver.get_solution();


// Transform the Hamiltonian parameters to the RHF basis
mol_ham_par.transform(rhf.get_C());


// Set up the FCI Fock space
auto K = mol_ham_par.get_K();  // number of spatial orbitials
auto N_alpha = h2o.numberOfElectrons()/2;
auto N_beta = h2o.numberOfElectrons()/2;
GQCP::ProductFockSpace fock_space (K, N_alpha, N_beta);


// Find the lowest eigenvalue using the Davidson algorithm, providing the Hartree-Fock initial guess
GQCP::FCI fci (fock_space);  // has implemented the FCI matrix-vector product

auto initial_guess = fock_space.HartreeFockExpansion();
GQCP::DavidsonSolverOptions davidson_solver_options (initial_guess);  // number of requested eigenpairs defaults to 1

GQCP::CISolver ci_solver (fci, mol_ham_par);
ci_solver.solve(davidson_solver_options);


// Retrieve the lowest eigenvalue
double fci_davidson_eigenvalue = ci_solver.get_eigenpair(0).get_eigenvalue();
```

Usage of this library can be found in the documented `tests` directory, and common use cases are explained in the [Wiki page](https://github.com/GQCG/gqcp/wiki/Common-use-cases). Full documentation can be generated using Doxygen, see below.



## Installation

### Prerequisites

Before installing gqcp, please make sure the following dependencies are available on your system:

[![Boost Dependency](https://img.shields.io/badge/Boost-1.65.1+-000000.svg)](http://www.boost.org)
[![Eigen3 Dependency](https://img.shields.io/badge/Eigen-3.3.4+-000000.svg)](http://eigen.tuxfamily.org/index.php?title=Main_Page)
[![libint2 Dependency](https://img.shields.io/badge/libint-2.3.1+-000000.svg)](https://github.com/evaleev/libint)
[![libcint Dependency](https://img.shields.io/badge/libcint-3.0.17+-000000.svg)](https://github.com/GQCG/libcint)

Note that currently the feature/cmake_refactor branch of Libcint has to be installed.

    git clone -b feature/cmake_refactor https://github.com/GQCG/libcint

As gqcp uses the bassisets packaged with libint, please set the `LIBINT_DATA_PATH` environment variable to the folder that contains these bases. In a default installation (of e.g. version v2.3.1), the data path is given by:

    export LIBINT_DATA_PATH=/usr/local/libint/2.3.1/share/libint/2.3.1/basis

Note that conda offers virtual environments that ease installation and linking to these libraries. Please consult the Travis config file for more information.

### CMake out-of-source build

For a default CMake build, the steps are the following:

1. clone the master branch, which contains the latest release

        https://github.com/GQCG/gqcp.git --branch master --single-branch
        cd gqcp

2. perform an out-of-source build:

        mkdir build && cd build
        cmake .. (CMake options)
        make && make test && sudo make install

### CMake options

In general, please set and pass the following options to the `cmake ..` command:

* `-DCMAKE_PREFIX_PATH=prefix_path`, with `prefix_path` the path to those libraries and includes that are not in default locations, but are grouped together.
   For instance, setting the prefix_path to `/usr/local` ensures that the folders `cmake`, `lib`, `lib64` and `include` can be found.
   You should set the prefix to libInt2, libCint, Eigen and Intel MKL.
   If you have chosen to use conda for some dependencies, `prefix_path` should be set to e.g. `/anaconda3/envs/.../`

* `-DCMAKE_INSTALL_PREFIX=prefix`, with `prefix` (defaulted to `/usr/local`) the installation prefix you want the library to be installed it. This option controls where the library is installed:
    * the header files will be installed in `prefix/include`
    * the compiled library will be installed in `prefix/lib`
    * drivers (optional) and benchmarks (optional) will be installed in `prefix/bin`
    * CMake target files will be installed in `prefix/cmake`

For this library, there are several extra options you can pass to the `cmake ..` command:

* `-DEIGEN_USE_MKL=TRUE` makes sure that Eigen uses Intel MKL.

* `-DBUILD_TESTS=TRUE` specifies that tests should be built and run.

* `-DBUILD_DOCS=TRUE` specifies that documentation should be built using Doxygen, in which case Graphviz is required for UML generation. A custom `docs` target will then be configured by CMake, so that

        make docs
    
    compiles the documentation. After compilation, the HTML documentation can be found in the `docs/html` directory inside your out-of-source `build` directory. Navigating the documentation is easiest if you start with the `index.html` file.


* `-DBUILD_DRIVERS=TRUE` controls that you want to build the extra drivers, which are executables for some common use-cases.


* `-DBUILD_BENCHMARKS=TRUE` makes sure CMake adds the benchmark executables as targets. This uses [Google benchmark](https://github.com/google/benchmark), so make sure you have this installed if you wish to proceed with benchmarking on your system.


### Usage in an external project

If you want to use gqcp in another project, just add

    find_package(gqcp 0.2.0 NO_MODULE)

to its CMake configuration, and it will ben provide  `gqcp_INCLUDE_DIRS` to be used in `target_include_directories` and the target library `gqcp` to be used in `target_link_libraries`. The `NO_MODULE` option specifies that you will use the CMake config script installed by GQCP.
