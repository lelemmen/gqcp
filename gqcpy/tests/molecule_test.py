import unittest
import numpy as np

# force the local gqcpy to be imported
import sys
sys.path.insert(0, '../')

# import our `pybind11`-based extension module
import gqcpy


class Molecule(unittest.TestCase):

    ''' iniates variables to be used by tests '''
    def setUp(self):
        pass

    def tearDown(self):
        pass

        ''' compare energies with reference '''
    def test_constructor(self):
        self.molecule = gqcpy.Molecule.ReadXYZ("data/ch4_crawdad.xyz", 0)
        print(self.molecule)
        


    
if __name__ == '__main__':
    unittest.main()
