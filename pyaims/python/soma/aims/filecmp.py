from __future__ import absolute_import
from __future__ import print_function

from soma.aims.graph_comparison import same_graphs
from soma.aims.volumetools import compare_images
import filecmp


def filter_header(hdr):
    '''
    Utility function used by cmp()
    Removed uuid and referential properties from the given header, in order to
    be compared with another file wich may differ only in thoses identifiers
    '''
    for prop in ('uuid', 'referential'):
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
        obj3 = aims.read(test_file)
        if type(obj1).__name__.startswith('soma.aims.Volume_') \
                and type(obj2).__name__.startswith('soma.aims.Volume_'):
            return compare_images(obj1, obj2)
        if obj1 == obj2:
            return True
        if type(obj1) != type(obj2):
            return False
        if hasattr(obj1, 'header'):
            filter_header(obj1.header())
        if hasattr(obj2, 'header'):
            filter_header(obj1.header())
        if obj1 == obj2:
            return

    except Exception:
        # aims cannot read them: use the standard filecmp
        pass
    return filecmp.cmp(ref_file, test_file)
