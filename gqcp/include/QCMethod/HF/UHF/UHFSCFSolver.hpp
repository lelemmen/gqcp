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


#include "Mathematical/Algorithm/CompoundConvergenceCriterion.hpp"
#include "Mathematical/Algorithm/IterativeAlgorithm.hpp"
#include "Mathematical/Optimization/ConsecutiveIteratesNormConvergence.hpp"
#include "QCMethod/HF/UHF/UHFDensityMatrixCalculation.hpp"
#include "QCMethod/HF/UHF/UHFElectronicEnergyCalculation.hpp"
#include "QCMethod/HF/UHF/UHFErrorCalculation.hpp"
#include "QCMethod/HF/UHF/UHFFockMatrixCalculation.hpp"
#include "QCMethod/HF/UHF/UHFFockMatrixDIIS.hpp"
#include "QCMethod/HF/UHF/UHFFockMatrixDiagonalization.hpp"
#include "QCMethod/HF/UHF/UHFSCFEnvironment.hpp"


namespace GQCP {


/**
 *  A factory class that can construct UHF SCF solvers in an easy way.
 * 
 *  @tparam _Scalar             The scalar type that is used for the coefficient matrix/expansion coefficients.
 */
template <typename _Scalar>
class UHFSCFSolver {
public:
    using Scalar = _Scalar;


public:
    /*
     *  PUBLIC STATIC METHODS
     */

    /**
     *  @param minimum_subspace_dimension           The minimum number of Fock matrices that have to be in the subspace before enabling DIIS.
     *  @param maximum_subspace_dimension           The maximum number of Fock matrices that can be handled by DIIS.
     *  @param threshold                            The threshold that is used in comparing both the alpha and beta density matrices.
     *  @param maximum_number_of_iterations         The maximum number of iterations the algorithm may perform.
     * 
     *  @return A DIIS UHF SCF solver that uses the combination of norm of the difference of two consecutive alpha and beta density matrices as a convergence criterion.
     */
    static IterativeAlgorithm<UHFSCFEnvironment<Scalar>> DIIS(const size_t minimum_subspace_dimension = 6, const size_t maximum_subspace_dimension = 6, const double threshold = 1.0e-08, const size_t maximum_number_of_iterations = 128) {

        // Create the iteration cycle that effectively 'defines' a DIIS UHF SCF solver.
        StepCollection<UHFSCFEnvironment<Scalar>> diis_uhf_scf_cycle {};
        diis_uhf_scf_cycle
            .add(UHFDensityMatrixCalculation<Scalar>())
            .add(UHFFockMatrixCalculation<Scalar>())
            .add(UHFErrorCalculation<Scalar>())
            .add(UHFFockMatrixDIIS<Scalar>(minimum_subspace_dimension, maximum_subspace_dimension))  // This also calculates the next coefficient matrix.
            .add(UHFElectronicEnergyCalculation<Scalar>());

        // Create a convergence criterion on the norm of subsequent density matrices.
        const std::function<std::deque<SpinResolved1DM<Scalar>>(const UHFSCFEnvironment<Scalar>&)> density_matrix_extractor = [](const UHFSCFEnvironment<Scalar>& environment) { return environment.density_matrices; };

        using ConvergenceType = ConsecutiveIteratesNormConvergence<SpinResolved1DM<Scalar>, UHFSCFEnvironment<Scalar>>;
        const ConvergenceType convergence_criterion {threshold, density_matrix_extractor, "the UHF spin resolved density matrix in AO basis"};

        return IterativeAlgorithm<UHFSCFEnvironment<Scalar>>(diis_uhf_scf_cycle, convergence_criterion, maximum_number_of_iterations);
    }


    /**
     *  @param threshold                            The threshold that is used in comparing both the alpha and beta density matrices.
     *  @param maximum_number_of_iterations         The maximum number of iterations the algorithm may perform.
     * 
     *  @return A plain UHF SCF solver that uses the combination of norm of the difference of two consecutive alpha and beta density matrices as a convergence criterion.
     */
    static IterativeAlgorithm<UHFSCFEnvironment<Scalar>> Plain(const double threshold = 1.0e-08, const size_t maximum_number_of_iterations = 128) {

        // Create the iteration cycle that effectively 'defines' a plain UHF SCF solver.
        StepCollection<UHFSCFEnvironment<Scalar>> plain_uhf_scf_cycle {};
        plain_uhf_scf_cycle
            .add(UHFDensityMatrixCalculation<Scalar>())
            .add(UHFFockMatrixCalculation<Scalar>())
            .add(UHFFockMatrixDiagonalization<Scalar>())
            .add(UHFElectronicEnergyCalculation<Scalar>());

        // Create a convergence criterion on the norm of subsequent density matrices.
        const std::function<std::deque<SpinResolved1DM<Scalar>>(const UHFSCFEnvironment<Scalar>&)> density_matrix_extractor = [](const UHFSCFEnvironment<Scalar>& environment) { return environment.density_matrices; };

        using ConvergenceType = ConsecutiveIteratesNormConvergence<SpinResolved1DM<Scalar>, UHFSCFEnvironment<Scalar>>;
        const ConvergenceType convergence_criterion {threshold, density_matrix_extractor, "the UHF spin resolved density matrix in AO basis"};

        return IterativeAlgorithm<UHFSCFEnvironment<Scalar>>(plain_uhf_scf_cycle, convergence_criterion, maximum_number_of_iterations);
    }
};


}  // namespace GQCP
