#!/usr/bin/env python

from __future__ import print_function

import unittest
from soma import aims
import numpy as np
import sys

class TestTransform(unittest.TestCase):

    def test_affine_transform(self):
        tr = aims.AffineTransformation3d()
        self.assertTrue(tr.isIdentity())
        self.assertTrue(np.all(tr.toMatrix() == np.eye(4)))
        mat = np.array(
            [[ 1.111542,  0.076462,  0.019691,  0.      ],
             [-0.071167,  0.99646,   0.148041,  0.      ],
             [-0.008385, -0.167618,  1.12418,   0.      ],
             [ 0.,        0.,        0.,        1.      ]], dtype=np.float32)
        tr.fromMatrix(mat)
        self.assertTrue(np.all(tr.toMatrix() == mat))
        self.assertEqual(tr.inverse().inverse(), tr)
        self.assertTrue(
            (tr.transform([1, 0, 0]) - [1.111542, -0.071167, -0.008385]).norm()
            < 1e-5)
        self.assertTrue(
            (tr.transform([0, 1, 0]) - [0.076462, 0.99646, -0.167618]).norm()
            < 1e-5)
        self.assertTrue(
            (tr.transform([0, 0, 1]) - [0.019691, 0.148041, 1.12418]).norm()
            < 1e-5)
        self.assertTrue(np.all(mat.ravel() - tr.toVector() == 0))
        self.assertTrue(np.all(mat.T.ravel() - tr.toColumnVector() == 0))
        self.assertTrue(np.max(np.abs((tr.inverse() * tr).toMatrix()
                                      - np.eye(4))) < 1e-5)


def test():
    suite = unittest.TestLoader().loadTestsFromTestCase(TestTransform)
    runtime = unittest.TextTestRunner(verbosity=2).run(suite)
    return runtime.wasSuccessful()


if __name__ == "__main__":
    test()
