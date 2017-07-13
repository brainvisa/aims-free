# -*- coding: utf-8 -*-
from __future__ import print_function

import tempfile
import os
import sys
import unittest
import shutil
from soma import aims
import numpy as np

class TestPyaimsIO(unittest.TestCase):

    verbose = False


    def setUp(self):
        self.work_dir = tempfile.mkdtemp(prefix='test_pyaims')


    def compare_images(self, vol, vol2, vol1_name, vol2_name, thresh=1e-6):
        #print('comp vol, sizes:', vol.getSize(), vol2.getSize())
        #print('    vsizes:', vol.getVoxelSize(), vol2.getVoxelSize())
        msg = 'comparing %s and %s' % (vol1_name, vol2_name)
        self.assertEqual(vol.getSize(), vol2.getSize(),
                         msg + ': %s != %s'
                         % (str(vol.getSize()), str(vol2.getSize())))
        self.assertTrue(
            np.max(np.abs(np.asarray(vol.getVoxelSize())
                          - vol2.getVoxelSize())) < 1e-6, msg)
        self.assertTrue(np.max(np.abs(np.asarray(vol) - np.asarray(vol2)))
                        < thresh,
                        msg + ', max diff: %f'
                        % np.max(np.abs(np.asarray(vol) - np.asarray(vol2))))

    def check_open_files(self, fnames):
        proc_dir = '/proc/self/fd'
        failing = []
        if os.path.exists(proc_dir):
            for fd in os.listdir(proc_dir):
                fdpath = os.path.join(proc_dir, fd)
                if os.path.islink(fdpath):
                    fpath = os.readlink(fdpath)
                    if fpath in fnames:
                        failing.append((fd, fpath))
        if self.verbose and failing:
            print('    ! remaining open files:', failing)
        return failing


    def use_type(self, dtype):
        formats = ['.nii', '.nii.gz', '.ima', '.mnc', '.v', '.tiff']

        # create test volume
        vol = aims.Volume(10, 10, 10, dtype=dtype)
        vol.header()['voxel_size'] = [0.5, 0.6, 0.7]
        np.asarray(vol)[:,:,:,:] = np.ogrid[0:1000].reshape(10, 10, 10, 1)
        failing_files = set()
        for format in formats:
            failing_files.update(self.use_format(vol, format))
        return failing_files


    def use_format(self, vol, format):
        suffixes = {'.dcm': '1', '.tiff': '_0000'}
        partial_read = ['.nii', '.nii.gz', '.ima', '.dcm']
        partial_write = ['.nii', '.ima']
        default_epsilon = 1e-6
        # ecat scaling is far from exact...
        epsilon = {'.v': 5e-2}
        dtype = aims.typeCode(np.asarray(vol).dtype)

        if self.verbose:
            print('testing type: %s, format: %s' % (dtype, format))
        # write volume
        fname = os.path.join(
            self.work_dir, 'vol_%s%s' % (dtype, format))
        aims.write(vol, fname)
        vol1_name = os.path.basename(fname) + ' (written)'

        # re-read it
        fname = os.path.join(
            self.work_dir, 'vol_%s%s%s'
            % (dtype, suffixes.get(format, ''), format))
        vol2_name = os.path.basename(fname) + ' (re-read)'
        vol2 = aims.read(fname)
        thresh = epsilon.get(format, default_epsilon)

        # ensure we get the same
        self.compare_images(vol, vol2, vol1_name, vol2_name, thresh)
        del vol2

        # test native file without minf
        minf_fname = fname + '.minf'
        minf = aims.read(minf_fname)
        if 'filenames' not in minf:
            os.unlink(minf_fname)
            vol3_name = os.path.basename(fname) + ' (re-read without .minf)'
            vol3 = aims.read(fname)
            self.compare_images(vol, vol3, vol1_name, vol3_name, thresh)

        if format in partial_read:
            if self.verbose:
                print('    testing partial reading:', format)
            vol3 = aims.read(fname + '?ox=2&&sx=7&&oy=3&&sy=5&&oz=4&&sz=6')
            self.assertEqual(vol3.getSize(), (7, 5, 6, 1))
            vol4 = aims.VolumeView(vol, (2, 3, 4, 0), (7, 5, 6, 1))
            self.compare_images(vol4, vol3, 'sub-volume', 'patially read')

        if format in partial_write:
            if self.verbose:
                print('    testing partial writing:', format)
            vol2 = aims.VolumeView(vol, (3, 3, 3, 0), (5, 6, 4, 1))
            aims.write(vol2, fname + '?partial_writing=1&ox=2&&oy=4&&oz=5')
            vol3 = aims.read(fname)
            self.assertEqual(vol3.getSize(), (10, 10, 10, 1))
            vol4 = aims.VolumeView(vol3, (2, 4, 5, 0), (5, 6, 4, 1))
            # compare the written view
            self.compare_images(vol4, vol2, 'sub-volume %s (write, format %s)'
                                % (aims.typeCode(vol), format),
                                'patially written')
            # compare a part of the original volume
            vol2 = aims.VolumeView(vol, (0, 0, 0, 0), (10, 10, 5, 1))
            vol4 = aims.VolumeView(vol3, (0, 0, 0, 0), (10, 10, 5, 1))
            self.compare_images(vol4, vol2, 'sub-volume %s (write, format %s)'
                                % (aims.typeCode(vol), format),
                                'original part')

        # disable this test for now.
        ## test IO for VolumeView
        #vol2 = aims.VolumeView(vol, (2, 3, 4, 0), (7, 5, 6, 1))
        #aims.write(vol2, fname)
        #vol3 = aims.read(fname)
        #self.assertEqual(vol3.getSize(), (7, 5, 6, 1))
        #self.compare_images(vol2, vol3, 'volume view', 're-read volume view')

        # check if files remain open
        failing_files = self.check_open_files([fname, minf_fname])
        return failing_files


    def test_pyaims_io(self):
        types = ['S16', 'FLOAT']
        failing_files = set()
        for dtype in types:
            failing_files.update(self.use_type(dtype))
        if failing_files:
            raise RuntimeError('There are still open files: %s'
                % repr(failing_files) )


    def tearDown(self):
        shutil.rmtree(self.work_dir)


def test_suite():
  if '-v' in sys.argv[1:] or '--verbose' in sys.argv[1:]:
      TestPyaimsIO.verbose = True
  return unittest.TestLoader().loadTestsFromTestCase(TestPyaimsIO)


if __name__ == '__main__':
    unittest.main(defaultTest='test_suite')

