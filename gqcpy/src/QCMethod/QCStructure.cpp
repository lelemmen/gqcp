// This file is part of GQCG-gqcp.
// 
// Copyright (C) 2017-2019  the GQCG developers
// 
// GQCG-gqcp is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// GQCG-gqcp is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with GQCG-gqcp.  If not, see <http://www.gnu.org/licenses/>.
// 
#include "QCMethod/QCStructure.hpp"

#include "QCModel/CI/LinearExpansion.hpp"
#include "QCModel/HF/RHF.hpp"

#include <pybind11/eigen.h>
#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace gqcpy {


/**
 *  Since QCStructure is a class template, we must provide bindings for each of its associated types. In order to avoid duplicating code, we use a templated binding approach.
 */

/**
 *  Bind a quantum chemical model to the given module.
 * 
 *  @tparam QCModel             the type of the quantum chemical model
 * 
 *  @param module               the Pybind11 module
 *  @param suffix               the suffix that the Python class should receive, i.e. "QCStructure" + suffix
 *  @param description          the Python class description
 */
template <typename QCModel>
void bindQCStructure(py::module& module, const std::string& suffix, const std::string& description) {

    py::class_<GQCP::QCStructure<QCModel>>(module,
        ("QCStructure" + suffix).c_str(),
        description.c_str()
    )

        .def("energy",
            [ ] (const GQCP::QCStructure<QCModel>& qc_structure, const size_t i) {
                return qc_structure.energy(i);
            },
            py::arg("i") = 0,
            "Return the electronic energy corresponding to the i-th excited state."
        )

        .def("groundStateEnergy",
            [ ] (const GQCP::QCStructure<QCModel>& qc_structure) {
                return qc_structure.groundStateEnergy();
            },
            "Return the ground state electronic energy for this quantum chemical structure."
        )

        .def("groundStateParameters",
            [ ] (const GQCP::QCStructure<QCModel>& qc_structure) {
                return qc_structure.groundStateParameters();
            },
            "Return the ground state model parameters for this quantum chemical structure."
        )

        .def("parameters",
            [ ] (const GQCP::QCStructure<QCModel>& qc_structure, const size_t i) {
                return qc_structure.parameters(i);
            },
            py::arg("i") = 0,
            "Return the parameters corresponding to the i-th excited state."
        )
    ;
}


void bindQCStructures(py::module& module) {

    bindQCStructure<GQCP::LinearExpansion>(module, "LinearExpansion", "A quantum chemical structure for linear expansions.");
    bindQCStructure<GQCP::QCModel::RHF<double>>(module, "RHF", "A quantum chemical structure for RHF parameters.");
}


}  // namespace gqcpy