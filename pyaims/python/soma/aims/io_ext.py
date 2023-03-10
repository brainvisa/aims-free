
import numpy as np
import atexit
import sys
from . import gltf_io
aims = sys.modules['soma.aims']

'''
IO formats readers / writers written in python for aims.

Currently:

Numpy format for matrices
YAML format for Object
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


aims.Finder.registerFormat('NUMPY', NpyFinderFormat(), ['npy'])
aims.FileFormatDictionary_SparseOrDenseMatrix.registerFormat(
    'NUMPY', NpyFormat(), ['npy'])


# ---

class YamlFormat(aims.FileFormat_Object):
    def read(self, filename, obj, context, options=None):
        import yaml
        with open(filename) as f:
            ydict = yaml.safe_load(f)

        if isinstance(obj, aims.carto.AllocatorContext):
            # return the object variant
            options = context
            context = obj
            obj = aims.Object(ydict)
            return obj

        obj.update(ydict)

        return True

    def write(self, filename, obj, options):
        import yaml

        # yaml cannot automatically represent aims.Object instances
        class MyDumper(yaml.Dumper):
            def represent_data(self, data):
                if isinstance(data, (aims.Object, aims.carto.GenericObject)):
                    try:
                        if data.keys():
                            data = dict(data)
                        else:
                            data = list(data)
                    except:
                        pass
                elif type(data).__module__.startswith('soma.aims') \
                        and hasattr(type(data), '__getitem__'):
                    data = list(data)
                return super().represent_data(data)

        with open(filename, 'w') as f:
            yaml.dump(obj, f, Dumper=MyDumper)
        return True


class YamlFinderFormat(aims.FinderFormat):
    def check(self, filename, finder):
        if filename.endswith('.yaml'):
            hdr = {
                'file_type': 'YAML',
                'object_type': 'genericobject',
                'data_type': 'any',
            }
            finder.setHeader(hdr)
            finder.setObjectType('genericobject')
            finder.setDataType('any')
            return True
        return False


aims.Finder.registerFormat('YAML', YamlFinderFormat(), ['yaml'])
aims.FileFormatDictionary_Object.registerFormat(
    'YAML', YamlFormat(), ['yaml'])


class GLTFFormat(aims.FileFormat_Object):
    def read(self, filename, obj, context, options=None):
        meshes = gltf_io.load_gltf(filename)

        if isinstance(obj, aims.carto.AllocatorContext):
            # return the object variant
            options = context
            context = obj
            obj = aims.Object(meshes)
            return obj

        obj.update(meshes)

        return True

    def write(self, filename, obj, options):
        opts = {}
        for opt in ('matrix', 'tex_format', 'images_as_buffers',
                    'single_buffer'):
            if opt in options:
                opts[opt] = options[opt]

        gltf = gltf_io.meshes_dict_to_gltf(obj, **opts)
        save_opt = {}
        use_draco = options.get('use_draco', True)
        gltf_io.save_gltf(gltf, filename, use_draco=use_draco)

        return True

class GLTFMeshFormat_3(aims.FileFormat_AimsTimeSurface_3_VOID):
    def read(self, filename, obj, context, options=None):
        meshes = gltf_io.load_gltf(filename)
        mesh = meshes['objects'][0]
        while isinstance(mesh, list):
            mesh = mesh[0]
        mesh = mesh['mesh']

        if isinstance(obj, aims.carto.AllocatorContext):
            # return the object variant
            options = context
            context = obj
            obj = mesh
            return obj

        obj.vertex().assign(mesh.vertex())
        obj.normal().assign(mesh.normal())
        obj.polygon().assign(mesh.polygon())
        h = obj.header()
        for k in h:
            del h[k]
        h.update(mesh.header())

        return True

    def write(self, filename, obj, options):
        meshes = {'objects': [{'mesh': obj}]}
        return GLTFFormat().write(filename, meshes, options)


class GLTFMeshFormat_2(aims.FileFormat_AimsTimeSurface_2_VOID):
    def read(self, filename, obj, context, options=None):
        meshes = gltf_io.load_gltf(filename)
        mesh = meshes['objects'][0]
        while isinstance(mesh, list):
            mesh = mesh[0]
        mesh = mesh['mesh']

        if isinstance(obj, aims.carto.AllocatorContext):
            # return the object variant
            options = context
            context = obj
            obj = mesh
            return obj

        obj.vertex().assign(mesh.vertex())
        obj.normal().assign(mesh.normal())
        obj.polygon().assign(mesh.polygon())
        h = obj.header()
        for k in h:
            del h[k]
        h.update(mesh.header())

        return True

    def write(self, filename, obj, options):
        meshes = {'objects': [{'mesh': obj}]}
        return GLTFFormat().write(filename, meshes, options)


class GLTFFinderFormat(aims.FinderFormat):
    def check(self, filename, finder):
        if filename.endswith('.gltf') or filename.endswith('.glb'):
            hdr = {
                'file_type': 'GLTF',
                'object_type': 'genericobject',
                'data_type': 'any',
            }
            finder.setHeader(hdr)
            finder.setObjectType('genericobject')
            finder.setDataType('any')
            finder.setFormat('GLTF')
            return True
        return False

aims.Finder.registerFormat('GLTF', GLTFFinderFormat(), ['gltf', 'glb'])
aims.FileFormatDictionary_Object.registerFormat(
    'GLTF', GLTFFormat(), ['gltf', 'glb'])
aims.FileFormatDictionary_AimsTimeSurface_3_VOID.registerFormat(
    'GLTF', GLTFMeshFormat_3(), ['gltf', 'glb'])
aims.FileFormatDictionary_AimsTimeSurface_2_VOID.registerFormat(
    'GLTF', GLTFMeshFormat_2(), ['gltf', 'glb'])


def remove_python_formats():
    aims.Finder.unregisterFormat('NUMPY')
    aims.Finder.unregisterFormat('YAML')
    aims.Finder.unregisterFormat('GLTF')
    aims.FileFormatDictionary_SparseOrDenseMatrix.unregisterFormat('NUMPY')
    aims.FileFormatDictionary_Object.unregisterFormat('YAML')
    aims.FileFormatDictionary_Object.unregisterFormat('GLTF')


atexit.register(remove_python_formats)
