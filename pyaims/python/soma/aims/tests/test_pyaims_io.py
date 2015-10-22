# -*- coding: utf-8 -*-
import tempfile
import os
import unittest
import shutil
from soma import aims
import numpy as np

class TestPyaimsIO(unittest.TestCase):

    def setUp(self):
        self.work_dir = tempfile.mkdtemp(prefix='test_pyaims')


    def compare_images(self, vol, vol2, vol1_name, vol2_name):
        #print 'comp vol, sizes:', vol.getSize(), vol2.getSize()
        #print '    vsizes:', vol.getVoxelSize(), vol2.getVoxelSize()
        msg = 'comparing %s and %s' % (vol1_name, vol2_name)
        self.assertEqual(vol.getSize(), vol2.getSize(), msg)
        self.assertTrue(
            np.max(np.abs(np.asarray(vol.getVoxelSize())
                          - vol2.getVoxelSize())) < 1e-6, msg)
        self.assertTrue(np.max(np.abs(np.asarray(vol) - np.asarray(vol2)))
                        < 1e-6, msg)

    def test_pyaims_io(self):
        formats = ['.nii', '.nii.gz', '.ima' ] #, '.mnc']
        types = ['S16', 'FLOAT']
        for dtype in types:
            vol = aims.Volume(10, 10, 10, dtype=dtype)
            vol.header()['voxel_size'] = [0.5, 0.6, 0.7]
            vol.fill(0)
            vol.setValue(35, 5, 5, 5)
            vol.setValue(18, 6, 5, 5)
            for format in formats:
                fname = os.path.join(
                    self.work_dir, 'vol_%s%s' % (dtype, format))
                aims.write(vol, fname)
                vol1_name = os.path.basename(fname) + ' (written)'

                vol2_name = os.path.basename(fname) + ' (re-read)'
                vol2 = aims.read(fname)
                self.compare_images(vol, vol2, vol1_name, vol2_name)
                del vol2

                # test native file without minf
                minf_fname = fname + '.minf'
                os.unlink(minf_fname)
                vol3_name = os.path.basename(fname) \
                    + ' (re-read without .minf)'
                vol3 = aims.read(fname)
                self.compare_images(vol, vol3, vol1_name, vol3_name)

    def tearDown(self):
        shutil.rmtree(self.work_dir)


def test_suite():
  return unittest.TestLoader().loadTestsFromTestCase(TestPyaimsIO)


if __name__ == '__main__':
    unittest.main(defaultTest='test_suite')

