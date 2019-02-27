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
#ifndef GQCP_ONEELECTRONOPERATOR_HPP
#define GQCP_ONEELECTRONOPERATOR_HPP


#include <Eigen/Dense>

#include "JacobiRotationParameters.hpp"
#include "math/SquareMatrix.hpp"
#include "Operator.hpp"


namespace GQCP {


/**
 *  A class that represents a one-electron operator in an orbital basis
 *
 *  @tparam Scalar      the scalar type
 */
template <typename Scalar>
class OneElectronOperator : public SquareMatrix<Scalar>, public Operator<OneElectronOperator<Scalar>> {
public:

    /*
     *  CONSTRUCTORS
     */

    /**
     *  Default constructor
     */
    OneElectronOperator() :
        SquareMatrix<Scalar>()
    {}


    /**
     *  @param matrix   the explicit matrix representation of the one-electron operator
     *
     *  Note that this should accept any Matrix<Scalar> (instead of SquareMatrix<Scalar>) because we want other Eigen return types to be accepted as well, like after a product of OneElectronOperators
     */
    explicit OneElectronOperator(const Matrix<Scalar>& matrix) :
        SquareMatrix<Scalar>(matrix)
    {}


    /*
     *  OVERRIDDEN PUBLIC METHODS
     */

    /**
     *  In-place transform the matrix representation of the one-electron operator
     *
     *  @tparam TransformationScalar        the type of scalar used for the transformation matrix
     *
     *  @param T    the transformation matrix between the old and the new orbital basis, it is used as
     *      b' = b T ,
     *   in which the basis functions are collected as elements of a row vector b
     *
     *  Note that in order to use these transformation formulas, the multiplication between TransformationScalar and Scalar should be 'enabled'. See LinearCombination.hpp for an example
     */
    template <typename TransformationScalar = Scalar>
    void transform(const SquareMatrix<TransformationScalar>& T) {
        *this = OneElectronOperator<Scalar>(T.adjoint() * (*this) * T);  // this has no aliasing issues (https://eigen.tuxfamily.org/dox/group__TopicAliasing.html)
    }


    /*
     *  PUBLIC METHODS
     */

    using Operator<OneElectronOperator<Scalar>>::rotate;  // bring over rotate from the base class


    /**
     *  In-place rotate the matrix representation of the one-electron operator using a unitary Jacobi rotation matrix constructed from the Jacobi rotation parameters. Note that this function is only available for real (double) matrix representations
     *
     *  @param jacobi_rotation_parameters       the Jacobi rotation parameters (p, q, angle) that are used to specify a Jacobi rotation: we use the (cos, sin, -sin, cos) definition for the Jacobi rotation matrix. See transform() for how the transformation matrix between the two bases should be represented
     */
    template<typename Z = Scalar>
    enable_if_t<std::is_same<Z, double>::value> rotate(const JacobiRotationParameters& jacobi_rotation_parameters) {

        auto p = jacobi_rotation_parameters.get_p();
        auto q = jacobi_rotation_parameters.get_q();
        auto angle = jacobi_rotation_parameters.get_angle();

        double c = std::cos(angle);
        double s = std::sin(angle);


        // Use Eigen's Jacobi module to apply the Jacobi rotations directly (cfr. T.adjoint() * M * T)
        Eigen::JacobiRotation<double> jacobi (c, s);

        this->applyOnTheLeft(p, q, jacobi.adjoint());
        this->applyOnTheRight(p, q, jacobi);
    }
};



}  // namespace GQCP



#endif  // GQCP_ONEELECTRONOPERATOR_HPP
