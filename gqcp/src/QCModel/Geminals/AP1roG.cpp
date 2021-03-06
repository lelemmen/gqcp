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

#include "QCModel/Geminals/AP1roG.hpp"


namespace GQCP {


/*
 *  STATIC PUBLIC METHODS
 */

/**
 *  @param G                    the converged AP1roG geminal coefficients
 *  @param sq_hamiltonian       the Hamiltonian expressed in an orthonormal basis
 *
 *  @return the AP1roG electronic energy
 */
double QCModel::AP1roG::calculateEnergy(const AP1roGGeminalCoefficients& G, const RSQHamiltonian<double>& sq_hamiltonian) {

    // Prepare some variables.
    const auto& h = sq_hamiltonian.core().parameters();
    const auto& g = sq_hamiltonian.twoElectron().parameters();

    const auto orbital_space = G.orbitalSpace();


    // KISS implementation of the AP1roG energy
    double E = 0.0;
    for (const auto& j : orbital_space.indices(OccupationType::k_occupied)) {
        E += 2 * h(j, j);

        for (const auto& k : orbital_space.indices(OccupationType::k_occupied)) {
            E += 2 * g(k, k, j, j) - g(k, j, j, k);
        }

        for (const auto& b : orbital_space.indices(OccupationType::k_virtual)) {
            E += g(j, b, j, b) * G(j, b);
        }
    }

    return E;
}


/**
 *  @param sq_hamiltonian           the Hamiltonian expressed in an orthonormal basis
 *  @param G                        the AP1roG geminal coefficients
 *  @param i                        the subscript for the coordinate function
 *  @param a                        the superscript for the coordinate function
 *
 *  @return the PSE coordinate function with given indices (i,a) at the given geminal coefficients
 */
double QCModel::AP1roG::calculatePSECoordinateFunction(const RSQHamiltonian<double>& sq_hamiltonian, const AP1roGGeminalCoefficients& G, const size_t i, const size_t a) {

    // Prepare some variables.
    const auto N_P = G.numberOfElectronPairs();
    const auto K = G.numberOfSpatialOrbitals();

    const auto& h = sq_hamiltonian.core().parameters();
    const auto& g = sq_hamiltonian.twoElectron().parameters();

    const auto orbital_space = G.orbitalSpace();


    // A KISS implementation of the AP1roG PSE equations
    double value = 0.0;
    value += g(a, i, a, i) * (1 - std::pow(G(i, a), 2));

    for (const auto& j : orbital_space.indices(OccupationType::k_occupied)) {
        if (j != i) {
            value += 2 * ((2 * g(a, a, j, j) - g(a, j, j, a)) - (2 * g(i, i, j, j) - g(i, j, j, i))) * G(i, a);
        }
    }

    value += 2 * (h(a, a) - h(i, i)) * G(i, a);

    value += (g(a, a, a, a) - g(i, i, i, i)) * G(i, a);

    for (const auto& b : orbital_space.indices(OccupationType::k_virtual)) {
        if (b != a) {
            value += (g(a, b, a, b) - g(i, b, i, b) * G(i, a)) * G(i, b);
        }
    }

    for (const auto& j : orbital_space.indices(OccupationType::k_occupied)) {
        if (j != i) {
            value += (g(j, i, j, i) - g(j, a, j, a) * G(i, a)) * G(j, a);
        }
    }

    for (const auto& b : orbital_space.indices(OccupationType::k_virtual)) {
        if (b != a) {
            for (const auto& j : orbital_space.indices(OccupationType::k_occupied)) {
                if (j != i) {
                    value += g(j, b, j, b) * G(j, a) * G(i, b);
                }
            }
        }
    }

    return value;
}


/**
 *  @param sq_hamiltonian           the Hamiltonian expressed in an orthonormal basis
 *  @param G                        the AP1roG geminal coefficients
 *
 *  @return the PSEs, evaluated at the given geminal coefficients
 */
ImplicitMatrixSlice<double> QCModel::AP1roG::calculatePSECoordinateFunctions(const RSQHamiltonian<double>& sq_hamiltonian, const AP1roGGeminalCoefficients& G) {

    // Prepare some variables.
    const auto N_P = G.numberOfElectronPairs();
    const auto K = G.numberOfSpatialOrbitals();

    const auto orbital_space = G.orbitalSpace();

    auto F = orbital_space.initializeRepresentableObjectFor<double>(OccupationType::k_occupied, OccupationType::k_virtual);  // create a mathematical representation for an occupied-virtual object


    // Loop over all the elements of F to construct it.
    for (const auto& i : orbital_space.indices(OccupationType::k_occupied)) {
        for (const auto& a : orbital_space.indices(OccupationType::k_virtual)) {
            F(i, a) = QCModel::AP1roG::calculatePSECoordinateFunction(sq_hamiltonian, G, i, a);
        }
    }

    return F;
}


/**
 *  @param sq_hamiltonian           the Hamiltonian expressed in an orthonormal basis
 *  @param N_P                      the number of electron pairs
 * 
 *  @return a callable (i.e. with operator()) expression for the coordinate functions: the accepted VectorX<double> argument should contain the geminal coefficients in a column-major representation
 */
VectorFunction<double> QCModel::AP1roG::callablePSECoordinateFunctions(const RSQHamiltonian<double>& sq_hamiltonian, const size_t N_P) {

    VectorFunction<double> callable = [&sq_hamiltonian, N_P](const VectorX<double>& x) {
        const auto K = sq_hamiltonian.numberOfOrbitals();  // the number of spatial orbitals

        const auto G = AP1roGGeminalCoefficients::FromColumnMajor(x, N_P, K);
        return QCModel::AP1roG::calculatePSECoordinateFunctions(sq_hamiltonian, G).asVector();
    };

    return callable;
}


/**
 *  @param sq_hamiltonian       the Hamiltonian expressed in an orthonormal basis
 *  @param G                    the AP1roG geminal coefficients
 *  @param i                    the subscript for the coordinate function
 *  @param a                    the superscript for the coordinate function
 *  @param j                    the subscript for the geminal coefficient
 *  @param b                    the superscript for the geminal coefficient
 *
 *  @return the Jacobian element with compound indices (i,a) and (j,b) at the given geminal coefficients
 */
double QCModel::AP1roG::calculatePSEJacobianElement(const RSQHamiltonian<double>& sq_hamiltonian, const AP1roGGeminalCoefficients& G, const size_t i, const size_t a, const size_t j, const size_t b) {

    // Prepare some variables.
    const auto& h = sq_hamiltonian.core().parameters();
    const auto& g = sq_hamiltonian.twoElectron().parameters();

    const auto orbital_space = G.orbitalSpace();


    // KISS implementation of the calculation of Jacobian elements.
    double value = 0.0;
    if (i == j) {
        value += g(a, b, a, b) - 2 * g(j, b, j, b) * G(j, a);

        for (const auto& k : orbital_space.indices(OccupationType::k_occupied)) {
            value += g(k, b, k, b) * G(k, a);
        }
    }


    if (a == b) {
        value += g(j, i, j, i) - 2 * g(j, b, j, b) * G(i, b);

        for (const auto& c : orbital_space.indices(OccupationType::k_virtual)) {
            value += g(j, c, j, c) * G(i, c);
        }
    }


    if ((i == j) && (a == b)) {
        value += 2 * (h(a, a) - h(i, i));

        value -= 2 * (2 * g(a, a, i, i) - g(a, i, i, a));

        for (const auto& k : orbital_space.indices(OccupationType::k_occupied)) {
            value += 2 * (2 * g(k, k, a, a) - g(a, k, k, a)) - 2 * (2 * g(i, i, k, k) - g(i, k, k, i));

            if (k != i) {
                value -= 2 * g(k, a, k, a) * G(k, a);
            }
        }

        for (const auto& c : orbital_space.indices(OccupationType::k_virtual)) {
            if (c != a) {
                value -= 2 * g(i, c, i, c) * G(i, c);
            }
        }
    }

    return value;
}


/**
 *  @param sq_hamiltonian       the Hamiltonian expressed in an orthonormal basis
 *  @param G                    the AP1roG geminal coefficients
 *
 *  @return the Jacobian J_{ia,jb} of the PSEs, i.e. df_i^a/dG_j^b, evaluated at the given geminal coefficients
 */
ImplicitRankFourTensorSlice<double> QCModel::AP1roG::calculatePSEJacobian(const RSQHamiltonian<double>& sq_hamiltonian, const AP1roGGeminalCoefficients& G) {

    // Prepare some variables.
    const auto N_P = G.numberOfElectronPairs();
    const auto K = G.numberOfSpatialOrbitals();

    const auto orbital_space = G.orbitalSpace();

    auto J = orbital_space.initializeRepresentableObjectFor<double>(OccupationType::k_occupied, OccupationType::k_virtual, OccupationType::k_occupied, OccupationType::k_virtual);  // initialize an occupied-virtual, occupied-virtual tensor


    // Loop over all elements of J to construct it.
    for (const auto& i : orbital_space.indices(OccupationType::k_occupied)) {
        for (const auto& a : orbital_space.indices(OccupationType::k_virtual)) {
            for (const auto& j : orbital_space.indices(OccupationType::k_occupied)) {
                for (const auto& b : orbital_space.indices(OccupationType::k_virtual)) {
                    J(i, a, j, b) = QCModel::AP1roG::calculatePSEJacobianElement(sq_hamiltonian, G, i, a, j, b);
                }
            }
        }
    }

    return J;
}


/**
 *  @param sq_hamiltonian           the Hamiltonian expressed in an orthonormal basis
 *  @param N_P                      the number of electron pairs
 * 
 *  @return a callable (i.e. with operator()) expression for the Jacobian: the accepted VectorX<double> argument should contain the geminal coefficients in a column-major representation
 */
MatrixFunction<double> QCModel::AP1roG::callablePSEJacobian(const RSQHamiltonian<double>& sq_hamiltonian, const size_t N_P) {

    MatrixFunction<double> callable = [&sq_hamiltonian, N_P](const VectorX<double>& x) {
        const auto K = sq_hamiltonian.numberOfOrbitals();  // the number of spatial orbitals

        const auto G = AP1roGGeminalCoefficients::FromColumnMajor(x, N_P, K);
        return QCModel::AP1roG::calculatePSEJacobian(sq_hamiltonian, G).asMatrix();
    };

    return callable;
}

}  // namespace GQCP
