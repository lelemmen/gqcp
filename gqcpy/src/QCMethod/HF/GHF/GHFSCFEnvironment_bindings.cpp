// This file is part of GQCG-GQCP.
//
// Copyright (C) 2017-2020  the GQCG developers
//
// GQCG-GQCP is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// GQCG-GQCP is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with GQCG-GQCP.  If not, see <http://www.gnu.org/licenses/>.

#include "QCMethod/HF/GHF/GHFSCFEnvironment.hpp"

#include <pybind11/eigen.h>
#include <pybind11/pybind11.h>


namespace gqcpy {


// Provide some shortcuts for frequent namespaces.
namespace py = pybind11;
using namespace GQCP;


void bindGHFSCFEnvironment(py::module& module) {
    py::class_<GHFSCFEnvironment<double>>(module, "GHFSCFEnvironment", "An algorithm environment that can be used with standard GHF SCF solvers.")

        // CONSTRUCTORS

        .def_static(
            "WithCoreGuess",
            [](const size_t N, const GSQHamiltonian<double>& sq_hamiltonian, const Eigen::MatrixXd& S) {  // use an itermediary Eigen matrix for the Python binding, since Pybind11 doesn't accept our types that are derived from Eigen::Matrix
                return GHFSCFEnvironment<double>::WithCoreGuess(N, sq_hamiltonian, SquareMatrix<double> {S});
            },
            "Initialize an GHF SCF environment with an initial coefficient matrix that is obtained by diagonalizing the core Hamiltonian matrix.")


        // Bind read-write members/properties, exposing intermediary environment variables to the Python interface.
        .def_readwrite("N", &GHFSCFEnvironment<double>::N)

        .def_readwrite("electronic_energies", &GHFSCFEnvironment<double>::electronic_energies)

        .def_readwrite("orbital_energies", &GHFSCFEnvironment<double>::orbital_energies)

        .def_property(
            "S",
            [](const GHFSCFEnvironment<double>& environment) {
                return environment.S;
            },
            [](GHFSCFEnvironment<double>& environment, const Eigen::MatrixXd& S) {
                environment.S = SquareMatrix<double>(S);
            })


        // Define read-only 'getters'.
        .def_readonly(
            "coefficient_matrices",
            &GHFSCFEnvironment<double>::coefficient_matrices)

        .def_readonly(
            "density_matrices",
            &GHFSCFEnvironment<double>::density_matrices)

        .def_readonly(
            "fock_matrices",
            &GHFSCFEnvironment<double>::fock_matrices)

        .def_readonly(
            "error_vectors",
            &GHFSCFEnvironment<double>::error_vectors)


        // Bind methods for the replacement of the most current iterates.
        .def("replace_current_coefficient_matrix",
             [](GHFSCFEnvironment<double>& environment, const Eigen::MatrixXd& new_coefficient_matrix) {
                 environment.coefficient_matrices.pop_back();
                 environment.coefficient_matrices.push_back(SquareMatrix<double>(new_coefficient_matrix));
             })

        .def("replace_current_density_matrix",
             [](GHFSCFEnvironment<double>& environment, const Eigen::MatrixXd& new_density_matrix) {
                 environment.density_matrices.pop_back();
                 environment.density_matrices.push_back(SquareMatrix<double>(new_density_matrix));
             })

        .def("replace_current_fock_matrix",
             [](GHFSCFEnvironment<double>& environment, const Eigen::MatrixXd& new_fock_matrix) {
                 environment.fock_matrices.pop_back();
                 environment.fock_matrices.push_back(SquareMatrix<double>(new_fock_matrix));
             })

        .def("replace_current_error_vectors",
             [](GHFSCFEnvironment<double>& environment, const Eigen::MatrixXd& new_error_vectors) {
                 environment.fock_matrices.pop_back();
                 environment.fock_matrices.push_back(SquareMatrix<double>(new_error_vectors));
             });
}


}  // namespace gqcpy