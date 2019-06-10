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
#ifndef ERJacobiLocalizer_hpp
#define ERJacobiLocalizer_hpp


#include "OrbitalOptimization/JacobiOrbitalOptimizer.hpp"


namespace GQCP {


/**
 *  A class that localizes a set of orthonormal orbitals according to the maximization of the Edmiston-Ruedenberg localization index. A maximum is found using subsequent Jacobi rotations.
 */
class ERJacobiLocalizer : public JacobiOrbitalOptimizer {
private:
    bool are_calculated_jacobi_coefficients = false;

    double A=0.0, B=0.0, C=0.0;  // the Jacobi rotation coefficients


public:
    // CONSTRUCTORS

    /**
     *  @param N_P                  the number of electron pairs
     *  @param oo_options           the options for orbital optimization
     */
    ERJacobiLocalizer(size_t N_P, const OrbitalOptimizationOptions& oo_options);


    // PUBLIC OVERRIDDEN METHODS

    /**
     *  @param ham_par      the Hamiltonian parameters
     * 
     *  @return the value of the scalar function that should be optimized
     */
    double calculateScalarFunction(const HamiltonianParameters<double>& ham_par);

    /**
     *  Calculate the trigoniometric polynomial coefficients for the given Jacobi rotation
     *
     *  @param i            the index of spatial orbital i
     *  @param j            the index of spatial orbital j
     */
    void calculateJacobiCoefficients(const HamiltonianParameters<double>& ham_par, const size_t i, const size_t j);

    /**
     *  @param ham_par      the current Hamiltonian parameters
     *  @param i            the index of spatial orbital 1
     *  @param j            the index of spatial orbital 2
     *
     *  @return the angle for which the derivative of the scalar function after the Jacobi rotation is zero (and the second derivative is positive), using the current trigoniometric polynomial coefficients
     */
    double calculateOptimalRotationAngle(const HamiltonianParameters<double>& ham_par, const size_t i, const size_t j);

    /**
     *  @param ham_par              the current Hamiltonian parameters
     *  @param jacobi_rot_par       the Jacobi rotation parameters
     * 
     *  @return the value of the scalar function if the given Jacobi rotation parameters would be used to rotate the given Hamiltonian parameters
     */
    double calculateScalarFunctionAfterJacobiRotation(const HamiltonianParameters<double>& ham_par, const JacobiRotationParameters& jacobi_rot_par);
};


}  // namespace GQCP


#endif /* ERJacobiLocalizer_hpp */
