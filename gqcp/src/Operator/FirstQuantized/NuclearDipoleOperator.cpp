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

#include "Operator/FirstQuantized/NuclearDipoleOperator.hpp"


namespace GQCP {


/*
 *  MARK: Constructors
 */

/**
 *  Create a `NuclearDipoleOperator` from a nuclear framework and dipole origin.
 * 
 *  @param nuclear_framework            The nuclear framework underlying the nuclear operator.
 *  @param o                            The origin of the dipole.
 */
NuclearDipoleOperator::NuclearDipoleOperator(const NuclearFramework& nuclear_framework, const Vector<double, 3>& o) :
    BaseNuclearOperator(nuclear_framework),
    BaseReferenceDependentOperator(o) {}


/*
 *  MARK: Operator value
 */

/**
 *  @return The value of this nuclear dipole operator.
 */
Vector<double, 3> NuclearDipoleOperator::value() const {

    Vector<double, 3> mu = Vector<double, 3>::Zero();

    const auto& nuclei = this->nuclearFramework().nucleiAsVector();
    for (const auto& nucleus : nuclei) {
        mu += nucleus.charge() * nucleus.position();
    }

    return mu;
}


}  // namespace GQCP
