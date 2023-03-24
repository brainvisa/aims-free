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
        tr2 = aims.AffineTransformation3d()
        tr2.setTranslation([10, 20, 30])
        tr3 = tr * tr2
        mat3 = np.array(
            [[ 1.11154199e+00,  7.64620006e-02,  1.96909998e-02,
               1.32353888e+01],
             [-7.11669996e-02,  9.96460021e-01,  1.48040995e-01,
               2.36587601e+01],
             [-8.38499982e-03, -1.67618006e-01,  1.12417996e+00,
               3.02891884e+01],
             [ 0.00000000e+00,  0.00000000e+00,  0.00000000e+00,
               1.00000000e+00]])
        self.assertTrue(np.max(np.abs(tr3.toMatrix() - mat3)) < 1e-5)
        tr *= tr2
        self.assertEqual( tr, tr3 )

    def test_lightresampler(self):
      dims = (10, 15, 20)
      vs = (0.8, 1.3, 2.1)
      vol = aims.Volume(dims, dtype='S16')
      vol.setVoxelSize(vs)
      vol.np.ravel(order='K')[:] = np.arange(vol.np.size, dtype=np.int16)

      tr = aims.AffineTransformation3d()
      tr.affine()[:3, :4, 0, 0] = [[0, -1, 0, (dims[1] - 1) * vs[1]],
                                   [0, 0, -1, (dims[2] - 1) * vs[2]],
                                   [-1, 0, 0, (dims[0] - 1) * vs[0]]]

      odim = aims.LightResampler_S16.getTransformedDims(vol.shape, tr)
      self.assertEqual(odim, [15, 20, 10, 1])
      ovol = aims.LightResampler_S16.allocateResampledVolume(vol, tr)
      self.assertEqual(ovol.shape, (15, 20, 10, 1))
      self.assertTrue(np.max(np.abs(ovol.getVoxelSize().np
                                    - (1.3, 2.1, 0.8, 1.))) < 1e-5)

      aims.LightResampler_S16.resampleVolume(vol, ovol, tr)
      self.assertEqual(list(vol.np[:, 0, 0, 0]),
                       list(ovol.np[-1, -1, ::-1, 0]))
      self.assertEqual(list(vol.np[0, :, 0, 0]),
                       list(ovol.np[::-1, -1, -1, 0]))
      self.assertEqual(list(vol.np[0, 0, :, 0]),
                       list(ovol.np[-1, ::-1, -1, 0]))


def test():
    suite = unittest.TestLoader().loadTestsFromTestCase(TestTransform)
    runtime = unittest.TextTestRunner(verbosity=2).run(suite)
    return runtime.wasSuccessful()


if __name__ == "__main__":
    test()
