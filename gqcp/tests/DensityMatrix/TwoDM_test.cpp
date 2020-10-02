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

#define BOOST_TEST_MODULE "TwoDM"

#include <boost/test/unit_test.hpp>

#include "DensityMatrix/TwoDM.hpp"


/*
 *  HELPER FUNCTIONS
 */

/**
 *  @return a toy 2-DM where
 *      d(i,j,k,l) = l + 2k + 4j + 8i
 */
GQCP::TwoDM<double> calculateToy2DMTensor() {

    GQCP::TwoDM<double> d(2);

    for (size_t i = 0; i < 2; i++) {
        for (size_t j = 0; j < 2; j++) {
            for (size_t k = 0; k < 2; k++) {
                for (size_t l = 0; l < 2; l++) {
                    auto i_ = static_cast<double>(i);
                    auto j_ = static_cast<double>(j);
                    auto k_ = static_cast<double>(k);
                    auto l_ = static_cast<double>(l);

                    d(i, j, k, l) = l_ + 2 * k_ + 4 * j_ + 8 * i_;
                }
            }
        }
    }

    return d;
};


/*
 *  UNIT TESTS
 */

BOOST_AUTO_TEST_CASE(TwoDM_constructor) {

    // Check a correct constructor
    GQCP::Tensor<double, 4> tensor {3, 3, 3, 3};
    BOOST_CHECK_NO_THROW(GQCP::TwoDM<double> d {tensor});


    // Check a faulty constructor
    GQCP::Tensor<double, 4> tensor2 {3, 3, 3, 2};
    BOOST_CHECK_THROW(GQCP::TwoDM<double> d2 {tensor2}, std::invalid_argument);
}


BOOST_AUTO_TEST_CASE(trace) {

    auto d = calculateToy2DMTensor();

    BOOST_CHECK(std::abs(d.trace() - 30.0) < 1.0e-12);  // manual calculation
}


BOOST_AUTO_TEST_CASE(reduce) {

    auto d = calculateToy2DMTensor();

    GQCP::OneDM<double> D_ref = GQCP::OneDM<double>::Zero(2);

    // clang-format off
    D_ref <<  3, 11,
             19, 27;
    // clang-format on

    BOOST_CHECK(D_ref.isApprox(d.reduce(), 1.0e-12));
}