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

#include "ONVBasis/SpinResolvedONV.hpp"

#include <pybind11/eigen.h>
#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace gqcpy {


void bindSpinResolvedONV(py::module& module) {
    py::class_<GQCP::SpinResolvedONV>(module, "SpinResolvedONV", "An occupation number vector that is spin-resolved into alpha- and beta-constituents.")

        // CONSTRUCTORS

        .def_static(
            "RHF",
            [](const size_t K, const size_t N_P) {
                return GQCP::SpinResolvedONV::RHF(K, N_P);
            },
            py::arg("K"),
            py::arg("N_P"),
            "Create a spin-resolved ONV that represents the RHF single Slater determinant.")

        .def_static(
            "UHF",
            [](const size_t K, const size_t N_alpha, const size_t N_beta) {
                return GQCP::SpinResolvedONV::UHF(K, N_alpha, N_beta);
            },
            py::arg("K"),
            py::arg("N_alpha"),
            py::arg("N_beta"),
            "Create a spin-resolved ONV that represents the UHF single Slater determinant.")


        // PUBLIC METHODS

        .def("__repr__",
             &GQCP::SpinResolvedONV::asString)

        .def(
            "calculateProjection",
            [](const GQCP::SpinResolvedONV& onv_of, const GQCP::SpinResolvedONV& onv_on, const Eigen::MatrixXd& C_alpha, const Eigen::MatrixXd& C_beta, const Eigen::MatrixXd& C, const Eigen::MatrixXd& S) {
                return onv_of.calculateProjection(onv_on, GQCP::UTransformationMatrix<double>(C_alpha, C_beta), GQCP::TransformationMatrix<double>(C), GQCP::SquareMatrix<double>(S));
            },
            py::arg("onv_on"),
            py::arg("C_alpha"),
            py::arg("C_beta"),
            py::arg("C"),
            py::arg("S"),
            "Calculate the overlap <on|of>: the projection of between this spin-resolved ONV ('of') and another spin-resolved ONV ('on'), expressed in different R/U-spinor bases. The 'on'-ONV is supposed to be expressed in restricted spin-orbitals, and the 'of'-ONV is supposed to be expressed in unrestricted spin-orbitals.")

        .def(
            "onv",
            [](const GQCP::SpinResolvedONV& spin_resolved_onv, const GQCP::Spin sigma) {
                return spin_resolved_onv.onv(sigma);
            },
            py::arg("sigma"),
            "Return the ONV that describes the occupations of the sigma-spin orbitals.");
}


}  // namespace gqcpy
