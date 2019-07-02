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
#define BOOST_TEST_MODULE "OO-AP1roG"

#include <boost/test/unit_test.hpp>

#include "OrbitalOptimization/AP1roGJacobiOrbitalOptimizer.hpp"

#include "Geminals/AP1roGPSESolver.hpp"
#include "Geminals/AP1roG.hpp"
#include "RHF/PlainRHFSCFSolver.hpp"


BOOST_AUTO_TEST_CASE ( lih_6_31G_calculateEnergyAfterRotation ) {

    // We have implemented a formula to calculate the rotated AP1roG energy directly, but we have to test it
    // It should be equal to the energy we obtain by rotating the one- and two-electron integrals first


    // Construct the molecular Hamiltonian parameters in the RHF basis
    auto lih = GQCP::Molecule::ReadXYZ("data/lih_olsens.xyz");
    auto ao_mol_ham_par = GQCP::HamiltonianParameters<double>::Molecular(lih, "6-31G");

    GQCP::PlainRHFSCFSolver plain_scf_solver (ao_mol_ham_par, lih);
    plain_scf_solver.solve();
    auto rhf = plain_scf_solver.get_solution();
    auto mol_ham_par = GQCP::HamiltonianParameters<double>(ao_mol_ham_par, rhf.get_C());


    // Loop over all possible Jacobi pairs for a given (random) angle and check if the analytical result matches the numerical result
    double theta = 56.71;
    size_t K = mol_ham_par.get_K();

    for (size_t q = 0; q < K; q++) {  // p and q loop over spatial orbitals
        for (size_t p = q + 1; p < K; p++) {  // p > q
            GQCP::JacobiRotationParameters jacobi_rot_par {p, q, theta};

            // Construct a new AP1roG instance, since AP1roG.calculateEnergyAfterRotation overwrites this->so_basis
            // AP1roG.calculateEnergyAfterRotation is a function that is only used in testing
            GQCP::AP1roGPSESolver pse_solver (lih, mol_ham_par);
            pse_solver.solve();
            auto G = pse_solver.get_geminal_coefficients();


            // Calculate the analytical energy after rotation
            GQCP::AP1roGJacobiOrbitalOptimizer orbital_optimizer (G, 1.0e-04);
            orbital_optimizer.calculateJacobiCoefficients(mol_ham_par, p, q);
            double E_correction_analytical = orbital_optimizer.calculateScalarFunctionChange(mol_ham_par, jacobi_rot_par);


            // Calculate the energy after a numerical rotation (using a Jacobi rotation matrix)
            double E_before = GQCP::calculateAP1roGEnergy(G, mol_ham_par);
            auto mol_ham_par_copy = mol_ham_par;
            mol_ham_par_copy.rotate(jacobi_rot_par);
            double E_after = GQCP::calculateAP1roGEnergy(G, mol_ham_par_copy);
            double E_correction_numerical = E_after - E_before;


            BOOST_CHECK(std::abs(E_correction_analytical - E_correction_numerical) < 1.0e-08);
        }
    }
}


BOOST_AUTO_TEST_CASE ( lih_6_31G_orbitalOptimize ) {

    // Construct the molecular Hamiltonian parameters in the RHF basis
    auto lih = GQCP::Molecule::ReadXYZ("data/lih_olsens.xyz");
    auto ao_mol_ham_par =  GQCP::HamiltonianParameters<double>::Molecular(lih, "6-31G");

    GQCP::PlainRHFSCFSolver plain_scf_solver (ao_mol_ham_par, lih);
    plain_scf_solver.solve();
    auto rhf = plain_scf_solver.get_solution();
    auto mol_ham_par = GQCP::HamiltonianParameters<double>(ao_mol_ham_par, rhf.get_C());


    // Get the initial AP1roG energy
    GQCP::AP1roGPSESolver pse_solver (lih, mol_ham_par);
    pse_solver.solve();
    double initial_energy = pse_solver.get_electronic_energy();
    const auto initial_G = pse_solver.get_geminal_coefficients();


    // Do an AP1roG orbital optimization using Jacobi rotations
    GQCP::AP1roGJacobiOrbitalOptimizer orbital_optimizer (initial_G, 1.0e-04);
    orbital_optimizer.optimize(mol_ham_par);
    double optimized_energy = orbital_optimizer.get_electronic_energy();


    // We don't have reference data, so all we can do is check if orbital optimization lowers the energy
    BOOST_CHECK(optimized_energy < initial_energy);
}
