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
#include "RHF/DIISRHFSCFSolver.hpp"


namespace GQCP {


/*
 *  PRIVATE METHODS
 */

/**
 *  Update the Fock matrix, i.e. calculate the Fock matrix to be used in the next iteration of the SCF procedure, according to the DIIS step
 *
 *  @param D_AO     the RHF density matrix in AO basis
 *
 *  @return the new Fock matrix (expressed in AO basis)
 */
OneElectronOperator<double> DIISRHFSCFSolver::calculateNewFockMatrix(const OneRDM<double>& D_AO) {

    auto S = this->ham_par.get_S();

    // Calculate the Fock matrix based off the density matrix
    auto f_AO = calculateRHFAOFockMatrix(D_AO, this->ham_par);


    // Update deques for the DIIS procedure
    this->fock_matrix_deque.emplace_back(f_AO);
    OneElectronOperator<double> error_matrix = f_AO * D_AO * S - S * D_AO * f_AO;
    this->error_matrix_deque.emplace_back(error_matrix);


    // Enable DIIS when the current subspace dimension is large enough
    size_t n = error_matrix_deque.size();  // n is the current subspace dimension
    if (n >= this->minimum_subspace_dimension) {

        // Initialize and calculate the augmented B matrix
        SquareMatrix<double> B = -1 * SquareMatrix<double>::Ones(n+1,n+1);  // +1 for the multiplier
        B(n,n) = 0;
        for (size_t i = 0; i < n; i++) {
            for (size_t j = 0; j < n; j++) {
                // B(i,j) = Tr(e_i^T e_j)
                B(i,j) = (this->error_matrix_deque[i].transpose() * this->error_matrix_deque[j]).trace();
            }
        }

        // Initialize the RHS of the system of equations
        VectorX<double> b = VectorX<double>::Zero(n+1);  // +1 for the multiplier
        b(n) = -1;  // the last entry of b is accessed through n: dimension of b is n+1 - 1 because of computers


        // Solve the DIIS linear equations B y = b
        Eigen::HouseholderQR<Eigen::MatrixXd> lin_solver (B);
        VectorX<double> y = lin_solver.solve(b);


        // Use the coefficients that are in y to construct 'the best' Fock matrix as a linear combination of previous Fock matrices
        f_AO = OneElectronOperator<double>::Zero(S.cols(), S.cols());
        for (size_t i = 0; i < n; i++) {  // n is the current subspace dimension (not equal to the size of the augmented B matrix)
            f_AO += y(i) * this->fock_matrix_deque[i];
        }


    }  // subspace collapse


    // Collapse the subspace if it becomes too large, discard the oldest entry
    if (n > this->maximum_subspace_dimension) {
        this->fock_matrix_deque.pop_front();
        this->error_matrix_deque.pop_front();
    }


    return f_AO;
}



/*
 *  CONSTRUCTORS
 */
/**

 *  @param ham_par                          the Hamiltonian parameters in AO basis
 *  @param molecule                         the molecule used for the SCF calculation
 *  @param minimum_subspace_dimension       the minimum number of Fock matrices that have to be in the subspace before enabling DIIS
 *  @param maximum_subspace_dimension       the maximum DIIS subspace dimension before the oldest Fock matrices get discarded (one at a time)
 *  @param threshold                        the convergence treshold on the Frobenius norm on the AO density matrix
 *  @param maximum_number_of_iterations     the maximum number of iterations for the SCF procedure
 */
DIISRHFSCFSolver::DIISRHFSCFSolver(HamiltonianParameters<double> ham_par, Molecule molecule, size_t minimum_subspace_dimension, size_t maximum_subspace_dimension, double threshold, size_t maximum_number_of_iterations) :
    RHFSCFSolver(ham_par, molecule, threshold, maximum_number_of_iterations),
    minimum_subspace_dimension (minimum_subspace_dimension),
    maximum_subspace_dimension (maximum_subspace_dimension)
{}


}  // namespace GQCP
