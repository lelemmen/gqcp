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
#define BOOST_TEST_MODULE "TwoElectronOperator"


#include <boost/test/unit_test.hpp>
#include <boost/test/included/unit_test.hpp>  // include this to get main(), otherwise the compiler will complain

#include "Operator/TwoElectronOperator.hpp"

#include "utilities/io.hpp"
#include "utilities/linalg.hpp"

#include "utilities/miscellaneous.hpp"


BOOST_AUTO_TEST_CASE ( TwoElectronOperator_constructor ) {

    // Check a correct constructor
    Eigen::Tensor<double, 4> tensor (3, 3, 3, 3);
    GQCP::TwoElectronOperator<double> O (tensor);


    // Check a faulty constructor
    Eigen::Tensor<double, 4> tensor2 (3, 3, 3, 2);
    BOOST_CHECK_THROW(GQCP::TwoElectronOperator<double> O2 (tensor2), std::invalid_argument);
}


BOOST_AUTO_TEST_CASE ( TwoElectronOperator_transform_trivial ) {

    // Let's test a trivial transformation: i.e. with T being a unit matrix
    Eigen::Tensor<double, 4> g (3, 3, 3, 3);
    g.setRandom();
    GQCP::TwoElectronOperator<double> G (g);

    Eigen::MatrixXd T = Eigen::MatrixXd::Identity(3, 3);
    G.transform(GQCP::SquareMatrix<double>(T));

    BOOST_CHECK(GQCP::areEqual(g, g, 1.0e-12));
}


BOOST_AUTO_TEST_CASE ( TwoElectronOperator_transform_olsens ) {

    // We can find a reference algorithm in the olsens module from Ayer's lab
    Eigen::Tensor<double, 4> g_transformed_ref (2, 2, 2, 2);
    GQCP::readArrayFromFile("data/rotated_two_electron_integrals_olsens.data", g_transformed_ref);

    // Set an example transformation matrix and two-electron integrals tensor
    Eigen::MatrixXd T (2, 2);
    T << 1, 2, 3, 4;

    Eigen::Tensor<double, 4> g (2, 2, 2, 2);
    for (size_t i = 0; i < 2; i++) {
        for (size_t j = 0; j < 2; j++) {
            for (size_t k = 0; k < 2; k++) {
                for (size_t l = 0; l < 2; l++) {
                    g(i, j, k, l) = l + 2*k + 4*j + 8*i;
                }
            }
        }
    }
    GQCP::TwoElectronOperator<double> G (g);
    G.transform(GQCP::SquareMatrix<double>(T));

    BOOST_CHECK(GQCP::areEqual(G, g_transformed_ref, 1.0e-12));
}


BOOST_AUTO_TEST_CASE ( TwoElectronOperator_rotate_throws ) {

    // Create a random TwoElectronOperator
    size_t dim = 3;
    Eigen::Tensor<double, 4> g (dim, dim, dim, dim);
    g.setRandom();
    GQCP::TwoElectronOperator<double> G (g);


    // Check if a non-unitary matrix as transformation matrix causes a throw
    Eigen::MatrixXd U (Eigen::MatrixXd::Random(dim, dim));
    BOOST_CHECK_THROW(G.rotate(GQCP::SquareMatrix<double>(U)), std::invalid_argument);


    // Check if a unitary matrix as transformation matrix is accepted
    G.rotate(GQCP::SquareMatrix<double>(Eigen::MatrixXd::Identity(dim, dim)));
}


BOOST_AUTO_TEST_CASE ( TwoElectronOperator_rotate_JacobiRotationParameters ) {

    // Create a random TwoElectronOperator
    size_t dim = 5;
    Eigen::Tensor<double, 4> g (dim, dim, dim, dim);
    g.setRandom();
    GQCP::TwoElectronOperator<double> G1 (g);
    GQCP::TwoElectronOperator<double> G2 (g);


    // Check that using a Jacobi transformation (rotation) matrix as U is equal to the custom transformation (rotation)
    // with custom JacobiRotationParameters
    GQCP::JacobiRotationParameters jacobi_rotation_parameters (4, 2, 56.81);

    auto U = GQCP::jacobiRotationMatrix(jacobi_rotation_parameters, dim);


    G1.rotate(jacobi_rotation_parameters);
    G2.rotate(GQCP::SquareMatrix<double>(U));


    BOOST_CHECK(GQCP::areEqual(G1, G2, 1.0e-12));
}
