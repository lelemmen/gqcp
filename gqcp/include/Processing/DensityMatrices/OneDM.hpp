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


#include "Mathematical/Representation/QCMatrix.hpp"


namespace GQCP {


/**
 *  A type that represents a one-electron density matrix.
 *
 *  @tparam _Scalar     the scalar type
 */
template <typename _Scalar>
class OneDM: public QCMatrix<_Scalar> {
public:
    using Scalar = _Scalar;

    using BaseRepresentation = QCMatrix<Scalar>;
    using Self = OneDM<Scalar>;


public:
    /*
     *  CONSTRUCTORS
     */

    using QCMatrix<Scalar>::QCMatrix;  // inherit base constructors

    /*
     * PUBLIC METHODS
     */
    
    /**
     *  @param T          transformation matrix for the spin unresolved 1-DM
     * 
     *  @return the transformed density matrix.
     */
    OneDM<Scalar> transform(const TransformationMatrix<double>& T) const { return this->basisTransform(); }
};


}  // namespace GQCP
