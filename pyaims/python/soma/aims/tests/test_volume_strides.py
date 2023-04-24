#!/usr/bin/env python

from __future__ import print_function

import unittest
from soma import aims
import numpy as np
import sys


class TestVolumeStrides(unittest.TestCase):

    def _test_numpy_conversion(self, vol):
        arr = vol.arraydata()
        test_vol = aims.Volume(arr)
        dims = tuple(vol.getSize())
        rdims = tuple(reversed(dims))
        self.assertEqual(rdims, arr.shape)
        self.assertEqual((36499400, 125860, 434, 2), arr.strides)
        self.assertEqual(rdims, test_vol.getSize())
        # Here, vol and test_vol are *NOT* equal: test_vol is transposed

        arr = np.asarray(vol)
        test_vol = aims.Volume(arr)
        self.assertEqual(dims, arr.shape)
        self.assertEqual((2, 434, 125860, 36499400), arr.strides)
        self.assertEqual(dims, test_vol.getSize())
        dif = (vol == test_vol)
        self.assertTrue(np.all((dif).np))

        arr = np.asfortranarray(vol)
        test_vol = aims.Volume(arr)
        self.assertEqual(dims, arr.shape)
        self.assertEqual((2, 434, 125860, 36499400), arr.strides)
        self.assertEqual(dims, test_vol.getSize())
        dif = (vol == test_vol)
        self.assertTrue(np.all((dif).np))

        arr = np.ascontiguousarray(vol)
        test_vol = aims.Volume(arr)
        self.assertEqual(dims, arr.shape)
        self.assertEqual((168200, 580, 2, 2), arr.strides)
        self.assertEqual(dims, test_vol.getSize())
        dif = (vol == test_vol)
        self.assertTrue(np.all((dif).np))

    def test_volume_strides(self):
        vol = aims.Volume(4, 5, 6, dtype='S16')
        self.assertEqual(vol.shape, (4, 5, 6, 1))
        vol.np[:, :, :, 0] = np.arange(4*5*6).reshape((4, 5, 6))
        self.assertEqual(vol.at(2, 3, 4), 82)
        self.assertEqual(vol.np.strides, (2, 8, 40, 240))
        self.assertEqual(vol.getStrides(), [1, 4, 20, 120])
        # now build another volume with different strides and same content
        vol2 = aims.Volume(np.ascontiguousarray(vol.np))
        self.assertEqual(vol2.shape, (4, 5, 6, 1))
        self.assertEqual(vol2.np.strides, (60, 12, 2, 2))
        self.assertTrue(np.all((vol == vol2).np))
        self.assertEqual(vol2.at(2, 3, 4), 82)
        # test operators
        vol2 *= 2
        self.assertTrue(np.all(vol2.np == vol.np * 2))
        vol3 = vol + vol2 * 2
        self.assertEqual(vol3.shape, (4, 5, 6, 1))
        self.assertTrue(np.all(vol3.np == vol.np * 5))

    def test_numpy_conversion(self):
        vol = aims.Volume((217, 290, 290, 1), dtype='S16')
        vol.np[:] = np.arange(vol.np.size).reshape(vol.np.shape)
        self._test_numpy_conversion(vol)

    def test_numpy_conversion_neg_striodes(self):
        vol = aims.Volume_FLOAT(10, 15, 20, 4)
        vol.flipToOrientation('LPI', 'PTRS')
        vol2 = aims.Volume(vol.np)
        self.assertEqual(list(vol2.getStrides()), [-60, 1, -600, 15])


def test():
    suite = unittest.TestLoader().loadTestsFromTestCase(TestVolumeStrides)
    runtime = unittest.TextTestRunner(verbosity=2).run(suite)
    return runtime.wasSuccessful()


if __name__ == "__main__":
    test()
