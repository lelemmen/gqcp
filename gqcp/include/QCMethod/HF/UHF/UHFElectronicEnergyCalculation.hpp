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

#pragma once


#include "Mathematical/Algorithm/Step.hpp"
#include "QCMethod/HF/UHF/UHFSCFEnvironment.hpp"
#include "QCModel/HF/UHF.hpp"


namespace GQCP {


/**
 *  An iteration step that calculates the current electronic UHF energy.
 * 
 *  @tparam _Scalar              the scalar type used to represent the expansion coefficient/elements of the transformation matrix
 */
template <typename _Scalar>
class UHFElectronicEnergyCalculation:
    public Step<UHFSCFEnvironment<_Scalar>> {

public:
    using Scalar = _Scalar;
    using Environment = UHFSCFEnvironment<Scalar>;


public:
    /*
     *  PUBLIC OVERRIDDEN METHODS
     */

    /**
     *  @return a textual description of this algorithmic step
     */
    std::string description() const override {
        return "Calculate the current electronic UHF energy and place it in the environment.";
    }


    /**
     *  Calculate the current electronic UHF energy and place it in the environment.
     * 
     *  @param environment              the environment that acts as a sort of calculation space
     */
    void execute(Environment& environment) override {

        const auto& H_core = environment.sq_hamiltonian.core();  // the core Hamiltonian matrix: in zero-field calculations, alpha and beta are equal

        const auto& P_alpha = environment.density_matrices_alpha.back();  // the most recent alpha density matrix
        const auto& P_beta = environment.density_matrices_beta.back();    // the most recent beta density matrix

        const ScalarSQOneElectronOperator<Scalar> F_alpha {environment.fock_matrices_alpha.back()};  // the most recent alpha Fock matrix
        const ScalarSQOneElectronOperator<Scalar> F_beta {environment.fock_matrices_beta.back()};    // the most recent beta Fock matrix

        const auto E_electronic_alpha = QCModel::UHF<double>::calculateElectronicEnergy(P_alpha, H_core, F_alpha);
        const auto E_electronic_beta = QCModel::UHF<double>::calculateElectronicEnergy(P_beta, H_core, F_beta);

        const auto E_electronic = E_electronic_alpha + E_electronic_beta;
        environment.electronic_energies.push_back(E_electronic);
    }
};


}  // namespace GQCP