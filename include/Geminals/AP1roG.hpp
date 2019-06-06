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
#ifndef AP1roG_hpp
#define AP1roG_hpp


#include "Geminals/AP1roGGeminalCoefficients.hpp"
#include "HamiltonianParameters/HamiltonianParameters.hpp"
#include "RDM/OneRDM.hpp"
#include "RDM/TwoRDM.hpp"


namespace GQCP {


/**
 *  @param G            the converged AP1roG geminal coefficients
 *  @param ham_par      Hamiltonian parameters in an orthonormal spatial orbital basis
 *
 *  @return the AP1roG electronic energy
 */
double calculateAP1roGEnergy(const AP1roGGeminalCoefficients& G, const HamiltonianParameters<double>& ham_par);

/**
 *  @param G                the AP1roG geminal coefficients
 *  @param multipliers      the AP1roG Lagrangian multipliers
 *
 *  @return the AP1roG 1-DM
 */
OneRDM<double> calculate1RDM(const AP1roGGeminalCoefficients& G, const AP1roGVariables& multipliers);

/**
 *  @param G                the AP1roG geminal coefficients
 *  @param multipliers      the AP1roG Lagrangian multipliers
 *
 *  @return the AP1roG number 2-RDM (the Delta-matrix in the notes)
 */
SquareMatrix<double> calculateNumber2RDM(const AP1roGGeminalCoefficients& G, const AP1roGVariables& multipliers);

/**
 *  @param G                the AP1roG geminal coefficients
 *  @param multipliers      the AP1roG Lagrangian multipliers
 *
 *  @return the AP1roG pair 2-RDM (the Pi-matrix in the notes)
 */
SquareMatrix<double> calculatePair2RDM(const AP1roGGeminalCoefficients& G, const AP1roGVariables& multipliers);

/**
 *  @param G                the AP1roG geminal coefficients
 *  @param multipliers      the AP1roG Lagrangian multipliers
 *
 *  @return the AP1roG 2-DM
 */
TwoRDM<double> calculate2RDM(const AP1roGGeminalCoefficients& G, const AP1roGVariables& multipliers);


}  // namespace GQCP


#endif /* AP1roG_hpp */
