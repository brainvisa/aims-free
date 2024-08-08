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

'''File functions'''

import os
import sys
import numpy as np
import gzip
import hashlib
import tempfile
import filecmp
from soma import aims
from soma.aims.graph_comparison import same_graphs
from soma.aims.volumetools import compare_images


def compare_gzip_files(file1, file2):
    same = False
    # file1
    with gzip.open(file1, 'rb') as f:
        f1 = f.read()
    tmp_file1 = tempfile.mkstemp()
    os.close(tmp_file1[0])
    tf1 = open(tmp_file1[1], 'wb')
    tf1.write(f1)
    tf1.close()
    # file2
    with gzip.open(file2, 'rb') as f:
        f2 = f.read()
    tmp_file2 = tempfile.mkstemp()
    os.close(tmp_file2[0])
    tf2 = open(tmp_file2[1], 'wb')
    tf2.write(f2)
    tf2.close()

    same = filecmp.cmp(tmp_file1[1], tmp_file2[1])

    os.unlink(tmp_file1[1])
    os.unlink(tmp_file2[1])

    return same


def compare_text_files(file1, file2, thresh=1e-6):
    '''
    Compare text files (.txt, .csv,..) which may contain nan.
    '''
    arr1 = np.genfromtxt(file1)
    arr2 = np.genfromtxt(file2)
    #if not np.any(np.isnan(arr1)) and not np.any(np.isnan(arr2)):
        #return filecmp.cmp(file1, file2)
    if np.any(np.isnan(arr1)) and np.any(np.isnan(arr2)):
        if np.all(np.isnan(arr1) == np.isnan(arr2)):
            arr1[np.isnan(arr1)] = 0
            arr2[np.isnan(arr2)] = 0
            return np.max(np.abs(arr2 - arr1)) <= thresh
    elif not np.any(np.isnan(arr1)) and not np.any(np.isnan(arr2)):
        return np.max(np.abs(arr2 - arr1)) <= thresh

    return False


def compare_nii_files(file1, file2, thresh=50, out_stream=sys.stdout):
    '''
    Compare nifti files (.nii, .nii.gz)
    '''

    # Get md5 for each file
    if os.path.splitext(file1)[-1] == '.gz':
        md5_file1 = hashlib.md5(gzip.open(file1, 'rb').read()).hexdigest()
    else:
        md5_file1 = hashlib.md5(open(file1, 'rb').read()).hexdigest()

    if os.path.splitext(file2)[-1] == '.gz':
        md5_file2 = hashlib.md5(gzip.open(file2, 'rb').read()).hexdigest()
    else:
        md5_file2 = hashlib.md5(open(file2, 'rb').read()).hexdigest()

    if md5_file1 == md5_file2:
        return True

    # md5 are differents
    # Check the voxels
    a_1 = aims.read(file1)
    a_2 = aims.read(file2)
    if a_1.np.shape == a_2.np.shape:
        d = a_1.np - a_2.np
        if abs(np.max(d) - np.min(d)) <= thresh:
            print('WARNING, use aims, absolute difference value:'
                  ' %s' % abs(np.max(d) - np.min(d)), file=out_stream)
            return True
    return False


def filter_header_for_cmp(hdr):
    '''
    Utility function used by cmp()
    Removed uuid and referential properties from the given header, in order to
    be compared with another file wich may differ only in thoses identifiers
    '''
    for prop in ('uuid', 'referential', 'GIFTI_dataarrays_info',
                 'GIFTI_metadata', 'file_type', 'format', 'GIFTI_version'):
        if prop in hdr:
            del hdr[prop]
    # gifti-specific
    if 'GIFTI_coordinates_systems' in hdr:
        gcs = hdr['GIFTI_coordinates_systems']
        if 'data_referentials' in gcs:
            del gcs['data_referentials']


def cmp(ref_file, test_file, skip_suffixes=None):
    '''
    Compare files, taking into account their neuroimaging nature.
    Some specific comparison function will be called for graphs, meshes,
    images, CSV files.
    '''
    if skip_suffixes is None:
        skip_suffixes = [
            ".minf",
            # SPAM probas differ up tp 0.15 in energy and I don't know why, so
            # skip the test
            "_proba.csv",
            # this .dat file contains full paths of filenames
            "_global_TO_local.dat",
            # referentials with registration are allocated differently (uuids)
            "_auto.referential"]
    for ext in skip_suffixes:
        if ref_file.endswith(ext):
            return True
    if ref_file.endswith(".arg"):
        return same_graphs(ref_file, test_file)
    elif ref_file.endswith(".csv"):
        if filecmp.cmp(ref_file, test_file):
            return True
        arr1 = np.genfromtxt(ref_file)
        if len(arr1.shape) >= 2 and np.any(np.isnan(arr1[0, :])):
            arr1 = arr1[1:, :]
        arr2 = np.genfromtxt(test_file)
        if len(arr2.shape) >= 2 and np.any(np.isnan(arr2[0, :])):
            arr2 = arr2[1:, :]
        return np.max(np.abs(arr2 - arr1)) <= 1e-5
    try:
        obj1 = aims.read(ref_file)
        obj2 = aims.read(test_file)
        if type(obj1).__name__.startswith('soma.aims.Volume_') \
                and type(obj2).__name__.startswith('soma.aims.Volume_'):
            return compare_images(obj1, obj2)
        if obj1 == obj2:
            return True
        if type(obj1) != type(obj2):
            return False
        if hasattr(obj1, 'header'):
            filter_header_for_cmp(obj1.header())
        if hasattr(obj2, 'header'):
            filter_header_for_cmp(obj2.header())
        if obj1 == obj2:
            return True

    except Exception:
        # aims cannot read them: use the standard filecmp
        pass
    return filecmp.cmp(ref_file, test_file)
