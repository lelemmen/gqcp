// This file is part of GQCG-gqcp.
//
// Copyright (C) 2017-2018  the GQCG developers
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
#define BOOST_TEST_MODULE "NRDMCalculator_test"


#include "RDM/NRDMCalculator.hpp"


#include <boost/test/unit_test.hpp>
#include <boost/test/included/unit_test.hpp>  // include this to get main(), otherwise the compiler will complain


BOOST_AUTO_TEST_CASE ( calculateElement_throws ) {

    // Create a test wave function
    size_t M = 3;
    size_t N = 1;
    GQCP::FockSpace fock_space (M, N);

    Eigen::VectorXd coeff (fock_space.get_dimension());
    coeff << 1, 2, -3;
    GQCP::NRDMCalculator d (fock_space);

    BOOST_CHECK_THROW(d.calculateElement({3}, {0}, coeff), std::invalid_argument);  // bra-index is out of bounds
    BOOST_CHECK_THROW(d.calculateElement({0}, {3}, coeff), std::invalid_argument);  // ket-index is out of bounds
}



BOOST_AUTO_TEST_CASE ( calculateElement_1RDM ) {

    // Create a test wave function
    size_t M = 3;
    size_t N = 1;
    GQCP::FockSpace fock_space (M, N);

    Eigen::VectorXd coeff (fock_space.get_dimension());
    coeff << 1, 2, -3;


    // Check some 1-RDM values
    GQCP::NRDMCalculator d (fock_space);
    BOOST_CHECK(std::abs(d.calculateElement({0}, {0}, coeff) - 1.0) < 1.0e-12);     // d(0,0) : a^\dagger_0 a_0
    BOOST_CHECK(std::abs(d.calculateElement({0}, {1}, coeff) - 2.0) < 1.0e-12);     // d(0,1) : a^\dagger_0 a_1
    BOOST_CHECK(std::abs(d.calculateElement({2}, {1}, coeff) - (-6.0)) < 1.0e-12);  // d(2,1) : a^\dagger_2 a_1
}


BOOST_AUTO_TEST_CASE ( calculateElement_2RDM ) {

    // Create a test wave function
    size_t M = 3;
    size_t N = 2;
    GQCP::FockSpace fock_space (M, N);

    Eigen::VectorXd coeff (fock_space.get_dimension());
    coeff << 1, 2, -3;


    // Check some 2-RDM values
    GQCP::NRDMCalculator d (fock_space);
    BOOST_CHECK(std::abs(d.calculateElement({0,1}, {2,1}, coeff) - (-3.0)) < 1.0e-12);  // d(0,1,1,2) : a^\dagger_0 a^\dagger_1 a_2 a_1
    BOOST_CHECK(std::abs(d.calculateElement({2,0}, {1,0}, coeff) - (-2.0)) < 1.0e-12);  // d(2,0,0,1) : a^\dagger_2 a^\dagger_0 a^1 a_0
    BOOST_CHECK(std::abs(d.calculateElement({0,2}, {0,2}, coeff) - (-4.0)) < 1.0e-12);  // d(0,2,0,2) : a^\dagger_0 a^dagger_2 a_0 a_2
    BOOST_CHECK(std::abs(d.calculateElement({0,0}, {0,2}, coeff) - 0.0) < 1.0e-12);     // d(0,2,0,0) : a^\dagger_0 a^dagger_0 a_0 a_2, double annihilation gives 0.0
}
