# -*- coding: utf-8 -*-
#  This software and supporting documentation are distributed by
#      Institut Federatif de Recherche 49
#      CEA/NeuroSpin, Batiment 145,
#      91191 Gif-sur-Yvette cedex
#      France
#
# This software is governed by the CeCILL-B license under
# French law and abiding by the rules of distribution of free software.
# You can  use, modify and/or redistribute the software under the
# terms of the CeCILL-B license as circulated by CEA, CNRS
# and INRIA at the following URL "http://www.cecill.info".
#
# As a counterpart to the access to the source code and  rights to copy,
# modify and redistribute granted by the license, users are provided only
# with a limited warranty  and the software's author,  the holder of the
# economic rights,  and the successive licensors  have only  limited
# liability.
#
# In this respect, the user's attention is drawn to the risks associated
# with loading,  using,  modifying and/or developing or reproducing the
# software by the user in light of its specific status of free software,
# that may mean  that it is complicated to manipulate,  and  that  also
# therefore means  that it is reserved for developers  and  experienced
# professionals having in-depth computer knowledge. Users are therefore
# encouraged to load and test the software's suitability as regards their
# requirements in conditions enabling the security of their systems and/or
# data to be ensured and,  more generally, to use and operate it in the
# same conditions as regards security.
#
# The fact that you are presently reading this means that you have had
# knowledge of the CeCILL-B license and that you accept its terms.

from __future__ import absolute_import
import sys
if sys.version_info[:2] <= (2, 6):
    # @unittest.expectedFailure decorator is not available on python 2.6:
    # we need a backport of a newer version of unittest.
    import unittest2 as unittest
else:
    import unittest

import numpy as np

from soma import aims
from soma import aimsalgo


def create_reference_volume(dtype, shape=(4, 5, 6, 1)):
    vol = aims.Volume(shape[0], shape[1], shape[2], shape[3], dtype=dtype)
    arr = np.asarray(vol)
    arr[...] = np.arange(np.prod(shape), dtype=dtype).reshape(shape)
    return vol


def empty_volume_like(ref):
    return aims.Volume(ref.getSizeX(), ref.getSizeY(), ref.getSizeZ(),
                       ref.getSizeT(), dtype=np.asarray(ref).dtype)


def array_approx_equal(out, ref, border=0, atol=1e-6):
    out, ref = np.asarray(out), np.asarray(ref)
    if out.shape != ref.shape:
        return False
    if border == 0:
        return np.allclose(out, ref, atol=atol)
    else:
        return np.allclose(out[border : -border,
                               border : -border,
                               border : -border],
                           ref[border : -border,
                               border : -border,
                               border : -border], atol=atol)


identity_transform = aims.AffineTransformation3d()
assert identity_transform.isIdentity()


class ResamplingTestCase(unittest.TestCase):
    def do_identity_resampling_test(self, resampler, dtype, border=0):
        """Helper function to test resampling with identity transform"""
        ref = create_reference_volume(dtype)
        out = empty_volume_like(ref)
        resampler.setRef(ref)
        resampler.doit(identity_transform, out)
        self.assertTrue(array_approx_equal(out, ref, border))

    def test_identity_nn_resampling_float32(self):
        self.do_identity_resampling_test(
            aimsalgo.NearestNeighborResampler_FLOAT(),
            np.float32
        )

    def test_identity_linear_resampling_float32(self):
        self.do_identity_resampling_test(
            aimsalgo.LinearResampler_FLOAT(),
            np.float32
        )

    def test_identity_cubic_resampling_float32(self):
        self.do_identity_resampling_test(
            aimsalgo.CubicResampler_FLOAT(),
            np.float32
        )

    def test_identity_nn_resampling_int16(self):
        self.do_identity_resampling_test(
            aimsalgo.NearestNeighborResampler_S16(),
            np.int16
        )

    def test_identity_linear_resampling_int16(self):
        self.do_identity_resampling_test(
            aimsalgo.LinearResampler_S16(),
            np.int16
        )

    def test_identity_cubic_resampling_int16(self):
        self.do_identity_resampling_test(
            aimsalgo.CubicResampler_S16(),
            np.int16
        )

    def test_4d_resampling(self):
        resampler = aimsalgo.NearestNeighborResampler_S16()
        ref = create_reference_volume(np.int16, shape=(1, 1, 1, 2))
        out = empty_volume_like(ref)
        resampler.setRef(ref)
        resampler.doit(identity_transform, out)
        self.assertTrue(np.array_equal(out, ref))

    # known issue: the last element along each axis is -32768 (the mask value)
    def test_identity_masklin_resampling_int16_except_last_elem(self):
        self.do_identity_resampling_test(
            aimsalgo.MaskLinearResampler_S16(),
            np.int16, border=1
        )

    def test_scaling(self):
        ref = create_reference_volume(np.int16)
        out = empty_volume_like(ref)
        ref.header()["voxel_size"] = [0.2, 0.3, 0.4]
        transform = aims.AffineTransformation3d()
        transform.scale([1, 1, 1], ref.header()["voxel_size"])
        resampler = aims.ResamplerFactory_S16().getResampler(0)
        resampler.resample(ref, transform, 0, out)
        self.assertTrue(np.array_equal(out, ref))

    def test_nearestneighbor_rounding(self):
        ref = create_reference_volume(np.int16)
        out = empty_volume_like(ref)
        transform = aims.AffineTransformation3d()
        transform.setTranslation([0.49, 0.49, -0.49])
        resampler = aims.ResamplerFactory_S16().getResampler(0)
        resampler.resample(ref, transform, 0, out)
        self.assertTrue(np.array_equal(out, ref))

    def test_resample_one_value(self):
        ref = aims.Volume(np.arange(2, dtype=np.float32).reshape(2, 1, 1, 1))
        resampler = aims.ResamplerFactory_FLOAT().getResampler(1)
        out = resampler.resample(ref, identity_transform, 0, [0.5, 0, 0], 0)
        self.assertTrue(np.isclose(out, 0.5))

    def test_masklinresampler_one_value(self):
        shape = (3, 3, 3)
        ref = aims.Volume(np.arange(0, 2 * np.prod(shape), 2, dtype=np.int16)
                          .reshape(shape))
        resampler = aimsalgo.MaskLinearResampler_S16()
        out = resampler.resample(ref, identity_transform, -1, [0.5, 0, 0], 0)
        self.assertEqual(out, 1)

    def test_resample_inv_identity(self):
        ref = create_reference_volume(np.float32)
        out = empty_volume_like(ref)
        resampler = aimsalgo.LinearResampler_FLOAT()
        resampler.resample_inv(ref, identity_transform, -1, out)
        self.assertTrue(array_approx_equal(out, ref))

    def test_resample_inv_to_vox_identity(self):
        ref = create_reference_volume(np.float32)
        out = empty_volume_like(ref)
        resampler = aimsalgo.LinearResampler_FLOAT()
        resampler.resample_inv_to_vox(ref, identity_transform, -1, out)
        self.assertTrue(array_approx_equal(out, ref))

    def test_resample_inv_scaling(self):
        ref = create_reference_volume(np.int16)
        out = empty_volume_like(ref)
        ref.header()["voxel_size"] = [0.2, 0.3, 0.4]
        transform = aims.AffineTransformation3d()
        transform.scale(ref.header()["voxel_size"], [1, 1, 1])
        resampler = aims.ResamplerFactory_S16().getResampler(0)
        resampler.resample_inv(ref, transform, 0, out)
        self.assertTrue(np.array_equal(out, ref))

    def test_resample_inv_to_vox_scaling(self):
        ref = create_reference_volume(np.int16)
        out = empty_volume_like(ref)
        ref.header()["voxel_size"] = [0.2, 0.3, 0.4]
        resampler = aims.ResamplerFactory_S16().getResampler(0)
        resampler.resample_inv_to_vox(ref, identity_transform, 0, out)
        self.assertTrue(np.array_equal(out, ref))


if __name__ == "__main__":
    unittest.main()
