
from soma import aims
import numpy as np
import atexit

'''
IO formats readers / writers written in python for aims.

Currently:

Numpy format for matrices
'''

class NpyFormat(aims.FileFormat_SparseOrDenseMatrix):
    def read(self, filename, obj, context, options=None):
        mat = np.load(filename)
        # currently we need to perform a full copy of the array because
        # we cannot prevent it from deleting (when getting back to the C++
        # layer the python wrapper is destroyed and any references it holds
        # also disapear.
        # Or we should manually increment the ref counter in the numpy PyObject
        # but it would never be destroyed then.
        #vol = aims.Volume(mat)
        #vol._npy = mat
        vol = aims.Volume(mat.shape, dtype=mat.dtype)
        np.asarray(vol)[:, :, 0, 0] = mat
        if isinstance(obj, aims.carto.AllocatorContext):
            # return the object variant
            options = context
            context = obj
            obj = aims.SparseOrDenseMatrix()
            obj.setMatrix(vol)
            return obj
        # otherwise fill in obj
        obj.setMatrix(vol)
        return True

    def write(self, filename, obj, options):
        mat = np.asarray(obj.asDense())
        np.save(filename, mat)
        hdr = obj.header()
        aims.write(hdr, '%s.minf' % filename)
        return True


class NpyFinderFormat(aims.FinderFormat):
    def check(self, filename, finder):
        if filename.endswith('.npy'):
            hdr = {
                'file_type': 'NPY',
                'object_type': 'SparseMatrix',
                'data_type': 'DOUBLE',
            }
            finder.setHeader(hdr)
            finder.setObjectType('SparseMatrix')
            finder.setDataType('DOUBLE')
            return True
        return False

def remove_python_formats():
    aims.Finder.unregisterFormat('NUMPY')
    aims.FileFormatDictionary_SparseOrDenseMatrix.unregisterFormat('NUMPY')


aims.Finder.registerFormat('NUMPY', NpyFinderFormat(), ['npy'])
aims.FileFormatDictionary_SparseOrDenseMatrix.registerFormat(
    'NUMPY', NpyFormat(), ['npy'])

atexit.register(remove_python_formats)


