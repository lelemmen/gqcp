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


#include "Mathematical/AbstractFunction/ScalarFunction.hpp"
#include "Mathematical/Grid/Field.hpp"
#include "Mathematical/Representation/Matrix.hpp"
#include "Molecule/Molecule.hpp"

#include <array>
#include <functional>
#include <type_traits>


namespace GQCP {


/**
 *  A grid type whose points are on a regular cubic lattice.
 */
class CubicGrid {
private:
    Vector<double, 3> m_origin;             // the origin of the grid
    std::array<size_t, 3> number_of_steps;  // the number of steps in the x, y, z-directions
    std::array<double, 3> step_sizes;       // the step sizes in the x, y, z-directions


public:
    // CONSTRUCTORS

    /**
     *  @param origin               the origin of the grid
     *  @param steps                the number of steps in the x, y, z-directions
     *  @param step_sizes           the step sizes in the x, y, z-directions
     */
    CubicGrid(const Vector<double, 3>& origin, const std::array<size_t, 3>& steps, const std::array<double, 3>& step_sizes);


    // NAMED CONSTRUCTORS

    /**
     *  Parse an .rgrid-file and create the CubicGrid that is contained in it. The values for the scalar field or vector field are discarded.
     * 
     *  @param filename             the name of the .igrid-file
     * 
     *  @note An integration grid (.igrid) file is a headerless file and contains the following data:
     *      - Each row relates to one grid point, where the fastest changing values are z > y > x.
     *      - Column specification:
     *          - Column 1: The index from 1 to the number of grid points
     *          - Columns 2-4: The position of the grid point: x, y, and z
     *          - Optional: Column 5 or columns 5-7: 1 value for a scalar field, 3 values for a vector field
     */
    static CubicGrid ReadRegularGridFile(const std::string& filename);


    // PUBLIC METHODS

    /**
     *  Evaluate a scalar function on every point of this grid.
     * 
     *  @param scalar_function          the scalar functions whose values should be evaluated
     * 
     *  @return a field with the calculated evaluations
     */
    template <typename Valued>
    Field<Valued> evaluate(const ScalarFunction<Valued, double, 3>& scalar_function) const {

        std::vector<Valued> values;  // the evaluated values of the scalar function
        values.reserve(this->numberOfPoints());

        this->forEach([&values, &scalar_function](const Vector<double, 3>& r) {
            values.push_back(scalar_function(r));
        });

        return Field<Valued>(values);
    }


    /**
     *  Loop over the points of this grid by index number.
     * 
     *  @param callback         the function you would like to apply to each incoming (i,j,k)-tuple of numbers of steps taken in the x,y,z-direction.
     */
    void forEach(const std::function<void(const size_t, const size_t, const size_t)>& callback) const;

    /**
     *  Loop over the points of this grid by position (relative to the origin of this grid).
     * 
     *  @param callback         the function you would like to apply to each incoming position vector
     */
    void forEach(const std::function<void(const Vector<double, 3>&)>& callback) const;

    /**
     *  @return the number of points that are in this grid
     */
    size_t numberOfPoints() const;

    /**
     *  @return the origin of this grid
     */
    const Vector<double, 3>& origin() const { return this->m_origin; }

    /**
     *  @param i        the number of steps taken in the x-direction
     *  @param j        the number of steps taken in the y-direction
     *  @param k        the number of steps taken in the z-direction
     *
     *  @return the position vector associated to the given indices
     */
    Vector<double, 3> position(const size_t i, const size_t j, const size_t k) const;

    /**
     *  @param axis         0, 1, 2 representing the x-, y-, or z-axis
     * 
     *  @return the number of steps that can be taken in the direction of the specified axis
     */
    size_t numberOfSteps(const size_t axis) const { return this->number_of_steps[axis]; }

    /**
     *  @return the number of steps in the x, y, z-directions
     */
    const std::array<size_t, 3>& numberOfSteps() const { return this->number_of_steps; }

    /**
     *  @param axis         0, 1, 2 representing the x-, y-, or z-axis
     * 
     *  @return the step size that is taken in the direction of the specified axis
     */
    double stepSize(const size_t axis) const { return this->step_sizes[axis]; }

    /**
     *  @return the step sizes in the x, y, z-directions
     */
    const std::array<double, 3>& stepSizes() const { return this->step_sizes; }

    /**
     *  Write a field's values to a GAUSSIAN Cube file (http://paulbourke.net/dataformats/cube/).
     *
     *  @param scalar_field             the scalar field that should be written to the cubefile
     *  @param filename                 the name of the cubefile that has to be generated
     *  @param molecule                 the molecule that should be placed in the cubefile
     */
    void writeToCubeFile(const Field<double>& scalar_field, const std::string& filename, const Molecule& molecule) const {

        // Prepare some variables.
        std::ofstream cubefile;
        cubefile.open(filename, std::fstream::out);

        const auto& steps = this->numberOfSteps();
        const auto& origin = this->origin();
        const auto& step_sizes = this->stepSizes();
        const auto& nuclei = molecule.nuclearFramework().nucleiAsVector();


        // Write the necessary header lines.

        // The first two lines are comment lines.
        cubefile << "COMMENT LINE -- GAUSSIAN Cube file" << std::endl;
        cubefile << "COMMENT LINE -- OUTER LOOP: X, MIDDLE LOOP: Y, INNER LOOP: Z" << std::endl;

        cubefile << std::scientific;

        // The next line has the number of atoms and the origin of the volumetric data.
        cubefile << nuclei.size() << " " << origin(0) << " " << origin(1) << " " << origin(2) << std::endl;

        // The next three lines give the number of voxels along the respective axes.
        // We're choosing the x-, y- and z-axes, and since the number of steps is positive, the units are Bohr.
        cubefile << steps[0] << " " << step_sizes[0] << " " << 0.0 << " " << 0.0 << std::endl;
        cubefile << steps[1] << " " << 0.0 << " " << step_sizes[1] << " " << 0.0 << std::endl;
        cubefile << steps[2] << " " << 0.0 << " " << 0.0 << " " << step_sizes[2] << std::endl;
        for (const auto& nucleus : nuclei) {
            cubefile << nucleus.charge() << " " << 0.0 << " " << nucleus.position()(0) << " " << nucleus.position()(1) << " " << nucleus.position()(2) << std::endl;
        }


        // Write the values of the scalar function.
        size_t index = 0;
        this->forEach([&index, &cubefile, &scalar_field](const size_t i, const size_t j, const size_t k) {
            cubefile << scalar_field.value(index) << " ";  // write one value

            // There can only be 5 values on one line.
            if (k % 6 == 5) {
                cubefile << std::endl;
            }

            if (j == 0) {
                cubefile << std::endl;
            }

            index++;  // move to the next value
        });

        cubefile.close();
    }
};


}  // namespace GQCP
