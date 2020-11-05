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

#define BOOST_TEST_MODULE "Tensor"

#include <boost/test/unit_test.hpp>

#include "Mathematical/Representation/Tensor.hpp"


BOOST_AUTO_TEST_CASE(constructor_assignment) {

    // A small check to see if the interface of the constructor and assignment operator works as expected

    Eigen::Tensor<double, 3> A {2, 2, 2};
    Eigen::Tensor<double, 3> B {2, 2, 2};

    GQCP::Tensor<double, 3> T1 {A + B};
    GQCP::Tensor<double, 3> T2 = 2 * B;
}


BOOST_AUTO_TEST_CASE(hasEqualDimensionsAs) {

    GQCP::Tensor<double, 4> T1 {1, 2, 3, 4};
    GQCP::Tensor<double, 4> T2 {1, 2, 3, 4};
    GQCP::Tensor<double, 4> T3 {2, 2, 3, 4};

    BOOST_CHECK(T1.hasEqualDimensionsAs(T2));
    BOOST_CHECK(!T1.hasEqualDimensionsAs(T3));
}


BOOST_AUTO_TEST_CASE(FromBlock) {

    // Create an example tensor
    long dim1 = 3;
    GQCP::Tensor<double, 4> T1 {dim1, dim1, dim1, dim1};
    for (size_t i = 0; i < dim1; i++) {
        for (size_t j = 0; j < dim1; j++) {
            for (size_t k = 0; k < dim1; k++) {
                for (size_t l = 0; l < dim1; l++) {
                    T1(i, j, k, l) = l + 3 * k + 9 * j + 27 * i;
                }
            }
        }
    }

    // Create subtensor and check with manual reference
    auto T2 = GQCP::Tensor<double, 4>::FromBlock(T1, 1, 1, 1, 1);

    long dim2 = 2;
    for (size_t i = 0; i < dim2; i++) {
        for (size_t j = 0; j < dim2; j++) {
            for (size_t k = 0; k < dim2; k++) {
                for (size_t l = 0; l < dim2; l++) {
                    BOOST_CHECK(T2(i, j, k, l) == (l + 1) + 3 * (k + 1) + 9 * (j + 1) + 27 * (i + 1));
                }
            }
        }
    }
}


BOOST_AUTO_TEST_CASE(isApprox_throws) {

    // Check for a throw if the dimensions aren't compatible
    GQCP::Tensor<double, 4> M {2, 2, 2, 2};   // don't need to set them to zeros to check dimensions
    GQCP::Tensor<double, 4> T1 {2, 2, 3, 2};  // don't need to set them to zeros to check dimensions
    BOOST_CHECK_THROW(M.isApprox(T1), std::invalid_argument);


    // Check for no throw if correct tensors are given
    GQCP::Tensor<double, 4> T2 {2, 2, 2, 2};
    BOOST_CHECK_NO_THROW(M.isApprox(T2));
}


BOOST_AUTO_TEST_CASE(isApprox_example) {

    GQCP::Tensor<double, 4> M {2, 2, 2, 2};
    GQCP::Tensor<double, 4> T {2, 2, 2, 2};


    // Fill the two compatible tensors with random data and check if they're not equal
    M.setRandom();
    T.setRandom();

    BOOST_CHECK(!M.isApprox(T));  // probability of random tensors being equal approaches zero


    // Fill the two compatible tensors with the same data and check if they're equal
    M.setZero();
    T.setZero();
    M(0, 1, 0, 0) = 0.5;
    T(0, 1, 0, 0) = 0.5;

    BOOST_CHECK(M.isApprox(T));
}


BOOST_AUTO_TEST_CASE(print) {

    GQCP::Tensor<double, 4> T {2, 2, 2, 2};
    T.setRandom();

    std::ofstream file;
    file.open("print_output_stream_test.output");

    T.print();  // to std::cout
    T.print(file);

    file.close();
}


BOOST_AUTO_TEST_CASE(pairWiseReduced) {

    // Create an example 2x2x2x2 tensor
    GQCP::Tensor<double, 4> T1 {2, 2, 2, 2};

    for (size_t i = 0; i < 2; i++) {
        for (size_t j = 0; j < 2; j++) {
            for (size_t k = 0; k < 2; k++) {
                for (size_t l = 0; l < 2; l++) {
                    T1(i, j, k, l) = l + 2 * k + 4 * j + 8 * i;
                }
            }
        }
    }

    // Test default pairWiseReduced behavior
    GQCP::MatrixX<double> M1_ref1 {4, 4};
    // clang-format off
    M1_ref1 <<  0,  2,  1,  3,
                8, 10,  9, 11,
                4,  6,  5,  7,
               12, 14, 13, 15;
    // clang-format on

    BOOST_CHECK(M1_ref1.isApprox(T1.pairWiseReduced(), 1.0e-12));


    // Test non-default pairWiseReduced behavior
    GQCP::MatrixX<double> M1_ref2 {2, 4};
    // clang-format off
    M1_ref2 <<  4,  6,  5,  7,
               12, 14, 13, 15;
    // clang-format on

    BOOST_CHECK(M1_ref2.isApprox(T1.pairWiseReduced(0, 1, 0, 0), 1.0e-12));
    std::cout << T1.pairWiseReduced(0, 1, 0, 0) << std::endl;


    // Create an example 3x3x3x3 tensor
    GQCP::Tensor<double, 4> T2 {3, 3, 3, 3};
    for (size_t i = 0; i < 3; i++) {
        for (size_t j = 0; j < 3; j++) {
            for (size_t k = 0; k < 3; k++) {
                for (size_t l = 0; l < 3; l++) {
                    T2(i, j, k, l) = l + 3 * k + 9 * j + 27 * i;
                }
            }
        }
    }

    GQCP::MatrixX<double> M2_ref {9, 9};
    // clang-format off
    M2_ref <<  0,  3,  6,  1,  4,  7,  2,  5,  8,
              27, 30, 33, 28, 31, 34, 29, 32, 35,
              54, 57, 60, 55, 58, 61, 56, 59, 62,
               9, 12, 15, 10, 13, 16, 11, 14, 17,
              36, 39, 42, 37, 40, 43, 38, 41, 44,
              63, 66, 69, 64, 67, 70, 65, 68, 71,
              18, 21, 24, 19, 22, 25, 20, 23, 26,
              45, 48, 51, 46, 49, 52, 47, 50, 53,
              72, 75, 78, 73, 76, 79, 74, 77, 80;
    // clang-format on

    BOOST_CHECK(M2_ref.isApprox(T2.pairWiseReduced(), 1.0e-12));
}


BOOST_AUTO_TEST_CASE(addBlock_tensor) {

    // Create an example 3x3x3x3 tensor
    long dim1 = 3;
    GQCP::Tensor<double, 4> T1 {dim1, dim1, dim1, dim1};
    for (size_t i = 0; i < dim1; i++) {
        for (size_t j = 0; j < dim1; j++) {
            for (size_t k = 0; k < dim1; k++) {
                for (size_t l = 0; l < dim1; l++) {
                    T1(i, j, k, l) = l + 3 * k + 9 * j + 27 * i;
                }
            }
        }
    }

    // Create an example 2x2x2x2 tensor
    long dim2 = 2;
    GQCP::Tensor<double, 4> T2 {dim2, dim2, dim2, dim2};
    for (size_t i = 0; i < dim2; i++) {
        for (size_t j = 0; j < dim2; j++) {
            for (size_t k = 0; k < dim2; k++) {
                for (size_t l = 0; l < dim2; l++) {
                    T2(i, j, k, l) = 2 * l + j + 10 * i;
                }
            }
        }
    }

    // Add the smaller tensor and test if addition went correctly
    T1.addBlock(T2, 0, 0, 0, 0);
    for (size_t i = 0; i < dim2; i++) {
        for (size_t j = 0; j < dim2; j++) {
            for (size_t k = 0; k < dim2; k++) {
                for (size_t l = 0; l < dim2; l++) {
                    BOOST_CHECK(T1(i, j, k, l) == l + 3 * k + 9 * j + 27 * i + 2 * l + j + 10 * i);
                }
            }
        }
    }
}


BOOST_AUTO_TEST_CASE(addBlock_matrix) {

    // Create an example 3x3x3x3 tensor
    long dim1 = 3;
    GQCP::Tensor<double, 4> T1 {dim1, dim1, dim1, dim1};
    for (size_t i = 0; i < dim1; i++) {
        for (size_t j = 0; j < dim1; j++) {
            for (size_t k = 0; k < dim1; k++) {
                for (size_t l = 0; l < dim1; l++) {
                    T1(i, j, k, l) = l + 3 * k + 9 * j + 27 * i;
                }
            }
        }
    }

    // Create an example 2x2 matrix
    size_t dim2 = 2;
    GQCP::MatrixX<double> M = GQCP::MatrixX<double>::Zero(dim2, dim2);
    for (size_t i = 0; i < dim2; i++) {
        for (size_t j = 0; j < dim2; j++) {
            M(i, j) = 100 * i + 95 * j;
        }
    }

    // Copy the tensor
    auto T2 = T1;
    auto T3 = T1;

    // Add the matrix to the tensor and test if addition went correctly
    T2.addBlock<0, 1>(M, 0, 0, 0, 0);
    for (size_t i = 0; i < dim2; i++) {
        for (size_t j = 0; j < dim2; j++) {
            BOOST_CHECK(T2(i, j, 0, 0) == 9 * j + 27 * i + 100 * i + 95 * j);
        }
    }

    // Add the matrix to the tensor and test if addition went correctly
    T3.addBlock<2, 1>(M, 0, 0, 0, 0);
    for (size_t i = 0; i < dim2; i++) {
        for (size_t j = 0; j < dim2; j++) {
            BOOST_CHECK(T3(0, j, i, 0) == 3 * i + 9 * j + 100 * i + 95 * j);
        }
    }
}

BOOST_AUTO_TEST_CASE(einsum) {

    // Create an example 2x2x2x2 tensor
    long dim1 = 2;
    GQCP::Tensor<double, 4> T1 {dim1, dim1, dim1, dim1};
    for (size_t i = 0; i < dim1; i++) {
        for (size_t j = 0; j < dim1; j++) {
            for (size_t k = 0; k < dim1; k++) {
                for (size_t l = 0; l < dim1; l++) {
                    T1(i, j, k, l) = 2 * l + j + 10 * i;
                }
            }
        }
    }

    // Create an example Rank 2 tensor
    long dim2 = 2;
    GQCP::Tensor<double, 2> T2 {dim2, dim2};
    for (size_t i = 0; i < dim2; i++) {
        for (size_t j = 0; j < dim2; j++) {
            T2(i, j) = j + 5 * i;
        }
    }

    // Test the einsum API: double axis contraction
    // Start by creating a reference
    GQCP::MatrixX<double> reference {2, 2};

    // clang-format off
    reference <<   11,   35,
                  131,  155;
    // clang-format on

    const auto output = T1.einsum<2>(T2, "ijkl", "jk", "il");
    const auto test = T1.einsum<2>("ijkl,jk->il", T2);
    const GQCP::Matrix<double> output_as_matrix = output.toMatrix(2, 2);
    BOOST_CHECK(reference.isApprox(output_as_matrix, 1.0e-12));
    BOOST_CHECK(reference.isApprox(test.toMatrix(2, 2), 1e-12));

    // Test the einsum API: single axis contraction
    // Start by creating a reference
    GQCP::Tensor<double, 4> reference_tensor {dim1, dim1, dim1, dim1};
    for (size_t i = 0; i < dim1; i++) {
        for (size_t j = 0; j < dim1; j++) {
            for (size_t k = 0; k < dim1; k++) {
                for (size_t l = 0; l < dim1; l++) {
                    reference_tensor(i, j, k, l) = 0;
                }
            }
        }
    }
    GQCP::MatrixX<double> reference_block_1 {2, 2};
    // clang-format off d
    reference_block_1 << 50, 60,
        60, 74;
    // clang-format on

    GQCP::MatrixX<double> reference_block_2 {2, 2};
    // clang-format off
    reference_block_2 <<   55,   67,
                           65,   81;
    // clang-format on

    reference_tensor.addBlock<2, 3>(reference_block_1, 0, 0, 0, 0);
    reference_tensor.addBlock<2, 3>(reference_block_1, 0, 1, 0, 0);
    reference_tensor.addBlock<2, 3>(reference_block_2, 1, 0, 0, 0);
    reference_tensor.addBlock<2, 3>(reference_block_2, 1, 1, 0, 0);

    const auto output_2 = T1.einsum<1>(T2, "ijkl", "ia", "jkla");
    BOOST_CHECK(reference_tensor.isApprox(output_2, 1e-12));
}
