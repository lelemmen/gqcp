#ifndef RHFSCFSolver_hpp
#define RHFSCFSolver_hpp


#include "HamiltonianParameters/HamiltonianParameters.hpp"
#include "RHF.hpp"
#include "Molecule.hpp"


namespace GQCG {


/**
 *  Base class for RHF SCF solvers. This class contains the solve()-method, which is the RHF SCF procedure.
 *
 *  Derived classes should implement the pure virtual function calculateNewFockMatrix.
 */
class RHFSCFSolver {
protected:
    const size_t maximum_number_of_iterations;
    const double threshold;
    bool is_converged = false;

    const GQCG::HamiltonianParameters ham_par;  // Hamiltonian parameters expressed in an AO basis
    const GQCG::Molecule molecule;

    GQCG::RHF solution;

    // PROTECTED METHODS
    /**
     *  Calculate a new Fock matrix (expressed in AO basis). This is the function that causes the different behaviour between derived classes.
     */
    virtual Eigen::MatrixXd calculateNewFockMatrix(const Eigen::MatrixXd& D_AO) = 0;

public:
    // CONSTRUCTORS
    /**
     *  Constructor based on given Hamiltonian parameters @param ham_par, @param molecule, @param maximum_number_of_iterations and @param SCF threshold
     */
    RHFSCFSolver(GQCG::HamiltonianParameters ham_par, GQCG::Molecule molecule, double threshold=1.0e-08, size_t maximum_number_of_iterations=128);

    // GETTERS
    GQCG::RHF get_solution() const { return this->solution; }

    /**
     *  Solve the RHF SCF equations. This function internally uses the pure virtual calculateNewFockMatrix.
     */
    void solve();
};


}  // namespace GQCG


#endif /* RHFSCFSolver_hpp */