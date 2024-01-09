'''
GLTF (and GLB) format support for AIMS

GLTF is a 3D scene format which contains in a single file (or in few files)
meshes, textures and materials for a complete 3D scene. It is a JSON file,
which has a binary (GLB) alternative.

To be fully operational, the following modules should be installed:

- pygltflib (python module) for complete GLTF parsing and GLB support
- DracoPy (python module) for compressed meshes readiing
- webp (python module) for texture compression and webp format support for
  textures (used both in reading and writing)
- gltf-transform tool for writing compressed meshes. It is a javascript node.js
  tool, which can be installed using ``npm``, the node.js install tool::

        npm install -g @gltf-transform/core @gltf-transform/extensions @gltf-transform/functions @gltf-transform/cli

Without these modules, some functionalities will be missing, and it will not be
possible to read some GLTF/GLB files, depending on the formats used in them.
'''


import base64
import numpy as np
import os
import os.path as osp
import tempfile
import shutil
import uuid
from functools import partial
from soma import aims
import json
import subprocess
try:
    import DracoPy
except ImportError:
    DracoPy = None
try:
    import webp
except ImportError:
    webp = None


def vec_to_bytes(vector):
    # (to be rather defined in C++)
    if hasattr(vector, 'np'):
        vector = vector.np
    if vector is None:
        return b''
    return vector.tobytes(order='C')


def image_as_buffer(image, format):
    if format == 'webp':
        if webp is not None:
            arr = np.asarray(image.np['v'][:, :, 0, 0, :].transpose(1, 0, 2),
                             order='C')
            webp_data = webp.WebPPicture.from_numpy(arr)
            webp_data = webp_data.encode()
            data = bytes(webp_data.buffer())
            return data
        else:
            format = 'png'

    tmpd = tempfile.mkdtemp(prefix='aims_gltf_')
    try:
        tmpf = osp.join(tmpd, 'image.%s' % format)
        aims.write(image, tmpf)
        with open(tmpf, 'rb') as f:
            data = f.read()
        return data
    finally:
        # print('tmpd:', tmpd)
        shutil.rmtree(tmpd)


def add_object_to_gltf_dict(vert, norm, poly, material=None, matrix=None,
                            textures=[], teximages=[], name=None, gltf=None,
                            tex_format='webp', images_as_buffers=True,
                            single_buffer=True, samplers=None):
    ''' Export a mesh with optional texture to a GLTF JSON dictionary.

    The gltf dict may already contain a scene with meshes, the new mesh will be
    appended to it.

    Parameters
    ----------
    vert: array(n,3), dtype=np.float32) or aims.vector_FLOAT
        mesh vertices array
    norm: (may be None or empty)
        mesh normals array
    poly:
        mesh polygons array
    material: dict (optional)
        material dict (as in Anatomist Material.genericDescription()).
    matrix: list (optional)
        An optional transformation matrix may be passed (column major vector).
    textures: list (optional)
        texture coords arrays list
    teximages: list(RGBA Volume) (optional)
        only if textures are used, the teture image as an aims Volume. If
        textures are used, this parameter is mandatory.
    name: str (optional)
        The mesh name.
    gltf: dict (optional)
        The GLTF dictionary. It will be updated and returned by the function.
        If it is not provided, a new one will be created.
    tex_format: str (optional)
        image format for textures. Default: "webp", supported only if the webp
        module is installed. Otherwise it will fallback to "png".
    images_as_buffers: bool (optional)
        if True, texture images will be stored in buffers instead of images
    single_buffer: bool (optional)
        if True, all binary data are appended in the same buffer object,
        instead of each having a separate buffer. The binary GLB format is
        better optimized for this situation. If this option is used, the buffer
        will not be base64-encoded on-the-fly, the function
        gltf_encode_buffers() should be called when the scene is finished in
        order to avoid many encoding/decoding overheads.
    '''
    if gltf is None:
        gltf = {}
    if vert is None:
        return gltf

    if tex_format == 'webp' and webp is None:
        tex_format = 'png'
    if tex_format == 'webp':
        images_as_buffers = True

    meshes = gltf.setdefault('meshes', [])
    nmesh = len(meshes)
    buffers = gltf.setdefault('buffers', [])
    nbuff = len(buffers)
    buffviews = gltf.setdefault('bufferViews', [])
    nbuffv = len(buffviews)
    accessors = gltf.setdefault('accessors', [])
    naccess = len(accessors)
    nodes = gltf.setdefault('nodes', [])
    nnodes = len(nodes)
    materials = gltf.setdefault('materials', [])
    nmat = len(materials)
    gtextures = gltf.setdefault('textures', [])
    ntex = len(gtextures)
    user_samplers = samplers  # use another var for this
    samplers = gltf.setdefault('samplers', [])
    nsamplers = len(samplers)
    buf_offset = 0

    poly = np.asarray(poly)
    # gltf doesn't handle quads, we have to transform
    if poly.shape[1] == 4:
        poly2 = poly[:, :3]
        poly3 = poly[:, (0, 2, 3)]
        poly = np.vstack((poly2, poly3))

    ps = poly.shape[1]

    hasnorm = True
    if norm is None:
        norm = []
        hasnorm = False

    if hasnorm:
        data = vec_to_bytes(vert) + vec_to_bytes(norm) + vec_to_bytes(poly)
    else:
        data = vec_to_bytes(vert) + vec_to_bytes(poly)
    if single_buffer:
        if len(buffers) == 0:
            buf = {'data': b''}
            buffers.append(buf)
        else:
            buf = buffers[-1]
            nbuff -= 1
            if 'data' not in buf:
                buf['data'] = b''
        buf_offset = len(buf['data']) + buf.get('byteLength', 0)
        buf['data'] += data
    else:
        buffers.append(
            {
                # base64 should not contain "\n"
                "uri":
                    ("data:application/octet-stream;base64,"
                     + base64.encodebytes(data).decode()
                    ).replace('\n', ''),
                "byteLength": len(data)
            })

    # polygons
    buffviews.append(
        {
            "buffer": nbuff,
            "byteOffset": buf_offset + len(vert) * 3 * 4 + len(norm) * 3 * 4,
            "byteLength": len(poly) * ps * 4
        })

    # vertices
    buffviews.append(
        {
            "buffer": nbuff,
            "byteOffset": buf_offset,
            "byteLength": len(vert) * 3 * 4
        })

    if hasnorm:
        # normals
        buffviews.append(
            {
                "buffer": nbuff,
                "byteOffset": buf_offset + len(vert) * 3 * 4,
                "byteLength": len(norm) * 3 * 4
            })
    buf_offset += len(data)

    accessors.append(
        {
            "bufferView": nbuffv,
            "componentType": 5125,
            "count": len(poly) * ps,
            "type": "SCALAR",
        })

    accessors.append(
        {
            "bufferView": nbuffv + 1,
            "componentType": 5126,
            "count": len(vert),
            "type": "VEC3",
        })

    if hasnorm:
        accessors.append(
            {
                "bufferView": nbuffv + 2,
                "componentType": 5126,
                "count": len(norm),
                "type": "VEC3",
            })

    pa = {
        "POSITION": naccess + 1,
    }
    if hasnorm:
        pa['NORMAL'] = naccess + 2

    # polygon size -> mesh mode id conversion
    modes = {
        1: 0,
        2: 1,
        3: 4,
    }

    mesh = {
        "primitives": [
            {
                "indices": naccess,
                "attributes": pa,
                "mode": modes[poly.shape[1]]
            }
        ]
    }
    if name:
        mesh['name'] = name
    meshes.append(mesh)
    if hasnorm:
        naccess += 3
        nbuffv += 3
    else:
        naccess += 2
        nbuffv += 2
    if not single_buffer:
        nbuff += 1

    node = {
            "mesh": nmesh,
    }
    if matrix is not None:
        if isinstance(matrix, aims.AffineTransformation3d):
            if matrix.isIdentity():
                matrix = None
            else:
                matrix = list(matrix.affine().np.transpose().ravel())
        elif isinstance(matrix, np.ndarray):
            matrix = list(matrix.transpose().ravel())

        if matrix is not None \
                and not aims.AffineTransformation3d(matrix).isIdentity():
            node["matrix"] = list(matrix)

    gmat = {
        'pbrMetallicRoughness': {
            'baseColorFactor': [1., 0.8, 0.8, 1.],
            'metallicFactor': 0.,
            'roughnessFactor': 0.5
        }
    }
    if material is not None:
        diffuse = material.get('diffuse')
        if diffuse is not None:
            gmat['pbrMetallicRoughness']['baseColorFactor'] \
                = list(diffuse)
            if len(diffuse) >= 4 and diffuse[3] < 1.:
                gmat['alphaMode'] = 'BLEND'
        if not material.get('face_culling', True):
            gmat['doubleSided'] = True
        sh = material.get('shininess', 20.)
        # we use a split curve which matches 20 shininess to 0.4 metal
        if sh <= 20.:
            shininess = 1. - sh / 33.33
        else:
            shininess = 0.4 - (sh - 20.) / 270.
        gmat['pbrMetallicRoughness']['metallicFactor'] = shininess
        spec = material.get('specular')
        if spec:
            spec = np.sqrt(spec[0] * spec[0] + spec[1] * spec[1]
                           + spec[2] * spec[2]) / np.sqrt(3.)
            gmat['pbrMetallicRoughness']['roughnessFactor'] = 1. - spec
    materials.append(gmat)
    mesh['primitives'][0]['material'] = nmat

    vtype = {
        1: 'SCALAR',
        2: 'VEC2',
        3: 'VEC3'
    }

    # filter out empty textures
    textures = [tc for tc in textures if tc is not None]

    if len(textures) != 0:
        tex_prop = None
        for tex, teximage in enumerate(teximages):
            if tex_prop is None:
                tex_prop = teximage.header().get('gltf_properties', {})
            image_data_buf = get_image_view(gltf, teximage, tex_format,
                                            images_as_buffers, single_buffer)
            tex_image_i = image_data_buf[2]

        if tex_format == 'webp' and len(teximages) != 0:
            extused = gltf.setdefault('extensionsUsed', [])
            if 'EXT_texture_webp' not in extused:
                extused.append('EXT_texture_webp')
            extreq = gltf.setdefault('extensionsRequired', [])
            if 'EXT_texture_webp' not in extreq:
                extreq.append('EXT_texture_webp')

        buffers = gltf.setdefault('buffers', [])
        buf_offset = 0
        nbuff = len(buffers)
        if single_buffer:
            buf = buffers[-1]
            buf_offset = len(buf.get('data', b''))
            nbuff -= 1
        nbuffv = len(buffviews)

        # note: only one baseColorTexture is allowed.
        texnames = ['pbrMetallicRoughness.baseColorTexture',
                    'emissiveTexture']

        for tex, tc in enumerate(textures):
            b = vec_to_bytes(tc)
            if single_buffer:
                buffers[-1]['data'] += b
            else:
                buffers.append({
                    "uri":
                        ("data:application/octet-stream;base64,"
                         + base64.encodebytes(b).decode()
                        ).replace('\n', ''),
                    "byteLength": int(np.prod(tc.shape) * 4)
                })
            buffviews.append({
                "buffer": nbuff,
                "byteOffset": buf_offset,
                "byteLength": int(np.prod(tc.shape) * 4)
            })
            if single_buffer:
                buf_offset += len(b)
            accessors.append({
                "bufferView": nbuffv,
                "componentType": 5126,
                "count": len(tc),
                "type": vtype[tc.shape[1]],
            })
            mesh['primitives'][0]['attributes']['TEXCOORD_%s' % tex] \
                = naccess
            if not single_buffer:
                nbuff += 1
            nbuffv += 1
            naccess += 1

            sampler = {
                "magFilter": 9729,
                "minFilter": 9987,
                "wrapS": 10497,  # mirroredrepeat: 33648,
                "wrapT": 10497  # mirroredrepeat: 33648
            }
            if user_samplers is not None and len(user_samplers) > tex:
                ssampler = user_samplers[tex]
                if ssampler is not None:
                    sampler.update(ssampler)
            elif 'sampler' in tex_prop:
                ssampler = tex_prop['sampler']
                if ssampler is not None:
                    sampler.update(ssampler)
            samplers.append(sampler)

            if tex_format == 'webp':
                gtextures.append({
                    'extensions': {
                        'EXT_texture_webp': {'source': tex_image_i}
                    },
                    'sampler': nsamplers,
                    'source': None
                })
            else:
                gtextures.append({
                    'source': tex_image_i,
                    'sampler': nsamplers
                })
            nsamplers += 1
            # note: only one baseColorTexture is allowed.
            if len(texnames) > tex:
                mat_id = texnames[tex]
                gm = gmat
                for tn in mat_id.split('.'):
                    gm = gm.setdefault(tn, {})
                gm.update({
                    'index': ntex,
                    'texCoord': tex
                })
            ntex += 1

    nodes.append(node)
    if 'children' in nodes[0]:
        nodes[0]['children'].append(nnodes)

    return gltf


def get_image_view(gltf, teximage, tex_format, images_as_buffers,
                   single_buffer):
    ''' Internal function used to get the GLTF buffer or image view for a
    texture image (given as an aims Volume_RGBA). Texture images are re-used,
    meaning that if we call several times this function using the same
    teximage, we get a reference to the same buffer.
    '''
    aims_teximages = gltf.setdefault('_teximages', {})
    tex_buf_ref = aims_teximages.get(id(teximage))
    if tex_buf_ref is not None:
        # teximage has already been used: take it in cache
        return tex_buf_ref

    b = image_as_buffer(teximage, tex_format)
    buf_offset = 0
    if images_as_buffers:
        buffers = gltf.setdefault('buffers', [])
        if single_buffer:
            if len(buffers) == 0:
                buf = {'data': b''}
                buffers.append(buf)
            else:
                buf = buffers[-1]
            buf_offset = len(buf['data'])  # + buf.get('byteLength', 0)
            buf['data'] += b
        else:
            image_bytes = base64.encodebytes(b).decode().replace(
                '\n', '')
            buf = {
                "uri": "data:application/octet-stream;base64," + image_bytes,
                "byteLength": len(b)
            }
            buffers.append(buf)
        buffviews = gltf.setdefault('bufferViews', [])
        buffviews.append({
            "buffer": len(buffers) - 1,
            "byteOffset": buf_offset,
            "byteLength": len(b)
        })
        images = gltf.setdefault('images', [])
        images.append({
            'mimeType': 'image/%s' % tex_format,
            'bufferView': len(buffviews) - 1
        })
        buf_offset += len(b)
    else:
        image_bytes = base64.encodebytes(b).decode().replace('\n', '')
        image_bytes = "data:image/%s;base64," % tex_format \
            + image_bytes
        images.append({
            'uri': image_bytes,
        })

    del b

    ret_ref = (images_as_buffers, single_buffer, len(images) - 1)
    aims_teximages[id(teximage)] = ret_ref
    return ret_ref


def mesh_to_gltf(mesh, matrix=None, name=None, gltf=None, tex_format='webp',
                 images_as_buffers=True, single_buffer=True):
    ''' Export a mesh to a GLTF JSON dictionary.

    The gltf dict may already contain a scene with meshes, the new mesh will be
    appended to it.

    Parameters
    ----------
    mesh: Aims mesh
        The mesh to be added to the GLTF scene
    matrix: list (optional)
        An optional transformation matrix may be passed (column major vector).
    name: str (optional)
        The mesh name.
    gltf: dict (optional)
        The GLTF dictionary. It will be updated and returned by the function.
        If it is not provided, a new one will be created.
    tex_format: str (optional)
        image format for textures. Default: "webp", supported only if the webp
        module is installed. Otherwise it will fallback to "png".
    images_as_buffers: bool (optional)
        if True, texture images will be stored in buffers instead of images
    single_buffer: bool (optional)
        if True, all binary data are appended in the same buffer object,
        instead of each having a separate buffer. The binary GLB format is
        better optimized for this situation. If this option is used, the buffer
        will not be base64-encoded on-the-fly, the function
        gltf_encode_buffers() should be called when the scene is finished in
        order to avoid many encoding/decoding overheads.
    '''
    if gltf is None:
        gltf = {}
    vert = mesh.vertex()
    norm = mesh.normal()
    poly = mesh.polygon()
    material = mesh.header().get('material')
    return add_object_to_gltf_dict(vert, norm, poly, material=material,
                                   name=name, gltf=gltf, tex_format=tex_format,
                                   images_as_buffers=images_as_buffers)


def tex_mesh_to_gltf(mesh, textures, matrix=None, name=None, gltf=None,
                     tex_format='webp', images_as_buffers=True,
                     single_buffer=True):
    ''' Same as :func:`mesh_to_gltf`, but with textures.

    textures should be a list of aims TimeTexture objects. Each texture must
    have in its header a dict named `gltf_texture` which contains at least a
    `teximage` field which is a texture image as an aims RGBA volume::

        texture.header()['gltf_texture'] = {
            'teximage': aims.Volume_RGBA(256, 256)
        }
    '''
    if gltf is None:
        gltf = {}
    vert = mesh.vertex()
    norm = mesh.normal()
    poly = mesh.polygon()
    tex_coords = []
    teximages = []
    for tex in textures:
        tex_coords.append(tex[0].np)
        teximage = tex.header().get('gltf_texture', {}).get('teximage')
        teximages.append(teximage)
    material = mesh.header().get('material')
    return add_object_to_gltf_dict(vert, norm, poly, textures=tex_coords,
                                   teximages=teximages, material=material,
                                   name=name, gltf=gltf, tex_format=tex_format,
                                   images_as_buffers=images_as_buffers)


def gltf_encode_buffers(gltf):
    ''' Replace raw binary data buffers with base64-encoded URIs
    '''
    for buf in gltf.get('buffers', []):
        data = buf.get('data')
        if data is not None:
            # if an existing URI is present, contcatenate it
            uri = buf.get('uri')
            if uri is not None:
                old_data = base64.decodebytes(uri[uri.find(','):].encode())
                data = old_data + data
            uri = 'data:application/octet-stream;base64,' \
                + base64.encodebytes(data).decode().replace('\n', '')
            buf['uri'] = uri
            buf['byteLength'] = len(data)
            del buf['data']


def default_gltf_scene(matrix=None, gltf=None):
    ''' Create a default GLTF dics scene
    '''
    if gltf is None:
        gltf = {}
    gltf.update({
        "asset": {
            "version": "2.0"
        },
        "scene": 0,
        "scenes": [
            {
                "nodes": [0]
            }
        ],
        "nodes": [
            {
                "children": []
            }
        ],

        "meshes": [
        ],
    })

    if matrix is not None:
        if isinstance(matrix, aims.AffineTransformation3d):
            if matrix.isIdentity():
                matrix = None
            else:
                matrix = list(matrix.affine().np.transpose().ravel())
        elif isinstance(matrix, np.ndarray):
            matrix = list(matrix.transpose().ravel())
        elif not isinstance(matrix, list):
            matrix = list(matrix)

        if matrix is not None \
                and not aims.AffineTransformation3d(matrix).isIdentity():
            gltf['nodes'][0]['matrix'] = matrix

    return gltf


def save_gltf(gltf, filename, use_draco=True):
    ''' Save the GLTF dict as a .gltf or .glb file, optionally using Draco
    compression.

    Returns
    -------
    filename: str
        the actually saved filename: it may have changed from .glb to .gltf if
        binary save is not supported by modules and extenal commands.
    '''
    try:
        from pygltflib import GLTF2, BufferFormat, ImageFormat
    except ImportError:
        GLTF2 = None  # no GLTF/GLB conversion support

    # ensure all buffers are encoded in base64
    gltf_encode_buffers(gltf)

    glb_saved = False
    gltf_filename = filename
    if filename.endswith('.glb'):
        if GLTF2 is None:
            print('warning: pygltflib is not found. Cannot save binary GLB. '
                  'Falling back to GLTF.')
            filename = filename[:-4] + '.gltf'
        else:
            gltf2 = GLTF2().from_dict(gltf)
            gltf2.convert_images(ImageFormat.BUFFERVIEW)
            gltf2.convert_buffers(BufferFormat.BINARYBLOB)
            gltf2.save(filename)
        glb_saved = True

    if not glb_saved:
        with open(filename, 'w') as f:
            json.dump(gltf, f, indent=4)

    if use_draco:
        try:
            if not gltf_convert_draco(filename, gltf_filename, fail=False):
                print('warning: gltf-transform is not found. Cannot compress '
                      'using Draco.')
            else:
                filename = gltf_filename
        except subprocess.CalledProcessError:
            print('draco compression failed for', filename)

    return filename


def meshes_dict_to_gltf(meshes, gltf=None, matrix=None, tex_format='webp',
                        images_as_buffers=True, single_buffer=True):
    if gltf is None:
        gltf = default_gltf_scene(matrix)
    root_ref = meshes.get('root_referential')
    trans_dict = meshes.get('transformation_graph', {})

    todo = [(obj, root_ref) for obj in meshes['objects']]

    while todo:
        obj, parent_ref = todo.pop(0)
        if isinstance(obj, list):
            todo += [(o, parent_ref) for o in obj]
            continue
        mesh = obj['mesh']  # mesh is mandatory
        tex = obj.get('textures')
        ref = mesh.header().get('referential')
        tr = None
        if ref is not None:
            tr = trans_dict.get(parent_ref, {}).get(ref)
            if tr is None:
                refs = mesh.header().get('referentials', [])
                if parent_ref in refs:
                    ri = refs.index(parent_ref)
                    tl = mesh.header().get('transformations', [])
                    tr = aims.AffineTransformation3d(tl[ri])
        if tex:
            tex_mesh_to_gltf(mesh, tex, matrix=tr, name=None, gltf=gltf,
                             tex_format=tex_format,
                             images_as_buffers=images_as_buffers,
                             single_buffer=single_buffer)
        else:
            mesh_to_gltf(mesh, matrix=tr, name=None, gltf=gltf,
                         tex_format=tex_format,
                         images_as_buffers=images_as_buffers,
                         single_buffer=single_buffer)

    return gltf


def gltf_convert_draco(infilename, outfilename=None, fail=True):
    # compress meshes using Draco
    # gltf-transform optimize <filename>.gltf <filename>.glb --texture-compress webp
    # gltf-transform draco [options] filename filename
    # we use the following options to avoid loss and texture shifts:
    # * --quantize-position 0
    # * --quantize-texcoord 0
    gltf_trans = shutil.which('gltf-transform')
    if not gltf_trans:
        if fail:
            raise ValueError('gltf-transform command is not found')
        return
    opts = ['--quantize-position', '0',  # don't quantize position
            '--quantize-texcoord', '0']  # don't quantize texcoords
    if outfilename is None:
        outfilename = infilename
    cmd = [gltf_trans, 'draco'] + opts + [infilename, outfilename]
    if 'LD_PRELOAD' in os.environ:
        # in VirtualGL processes (anatomist headless for instance), LD_PRELOAD
        # is used, and this causes a warning when running gltf-transform.
        # So we remove it.
        del os.environ['LD_PRELOAD']
    subprocess.check_call(cmd)
    if infilename != outfilename:
        os.unlink(infilename)
    return True


class BufferView:
    def __init__(self, gltf, bufferview, arrays):
        self.gltf = gltf
        self.arrays = arrays
        self.ddef = bufferview
        self._data = None

    @staticmethod
    def get_buffer(buff_i, gltf, arrays):
        buffs = arrays.setdefault('buffers', [])
        if len(buffs) > buff_i:
            buff = buffs[buff_i]
            if buff is not None:
                return buff
        if len(buffs) <= buff_i:
            buffs += [None] * (buff_i + 1 - len(buffs))
        cbuff = gltf['buffers'][buff_i]
        uri = cbuff.get('uri')
        binbuf = 'data:application/octet-stream;base64,'
        if uri is None or not uri.startswith(binbuf):
            raise ValueError('uri in buffer %d is not supported' % buff_i)

        bdata = base64.decodebytes(uri[len(binbuf):].encode())
        buffs[buff_i] = bdata

        return bdata

    def data(self):
        if self._data is not None:
            return self._data
        buff = self.get_buffer(self.ddef['buffer'], self.gltf, self.arrays)
        bo = self.ddef.get('byteOffset', 0)
        bl = self.ddef['byteLength']
        bs = self.ddef.get('byteStride', 1)
        # self._data = buff[bo:bo + bl:bs]
        # strides are between elements, not between bytes - but we don't
        # know elements size for now.
        self._data = buff[bo:bo + bl]
        return self._data


class Accessor:
    def __init__(self, gltf, accessor, arrays):
        self.gltf = gltf
        self.arrays = arrays
        self.ddef = accessor
        self._data = None

    def data(self):
        if self._data is not None:
            return self.data
        bo = self.ddef.get('byteOffset', 0)
        ctype = self.ddef['componentType']
        if ctype == 5120:
            nptype = np.int8
            itsize = 1
        elif ctype == 5121:
            nptype = np.uint8
            itsize = 1
        elif ctype == 5122:
            nptype = np.int16
            itsize = 2
        elif ctype == 5123:
            nptype = np.uint16
            itsize = 2
        elif ctype == 5125:
            nptype = np.uint32
            itsize = 4
        elif ctype == 5126:
            nptype = np.float32
            itsize = 4
        else:
            raise ValueError('unsupported component type: %d in accessor'
                             % ctype)
        atype = self.ddef['type']
        if atype == 'SCALAR':
            nc = 1
        elif atype == 'VEC2':
            nc = 2
        elif atype == 'VEC3':
            nc = 3
        elif atype == 'VEC4':
            nc = 4
        else:
            raise ValueError('unsupported accessor type: %d' % atype)
        n = self.ddef['count']

        bvn = self.ddef.get('bufferView')
        if bvn is None:
            self._data = np.zeros((n, nc), dtype=nptype)
        else:
            bv = self.gltf['bufferViews'][bvn]
            bufferview = BufferView(self.gltf, bv, self.arrays)
            bvdata = bufferview.data()
            size = len(bvdata) - bo
            stride = bv.get('byteStride')
            if stride is None:
                stride = itsize * nc
            if n == 0:
                n = int(size / stride)
                print('null count in accessor - fixing it to:', n)
                self.ddef['count'] = n
            if size < (n-1) * stride + nc * itsize:
                print('size error on bufferView', bvn, ': buffer is:', size,
                      ', expecting:', n * stride, '(%d x %d)' % (n, stride))
                if size == n:
                    print('count seems to be in bytes')
                # fix bufferView
                bv['byteLength'] = n * stride
                print('fixed len:', n * stride)
                bufferview._data = None
                bvdata = bufferview.data()
            if size < n * stride:
                # add missing part of stride to rebuild the array
                bvdata += b'\0' * (n * stride - size)
                size = len(bvdata) - bo
            ncs = stride // itsize
            ba = np.frombuffer(bvdata, dtype=nptype, offset=bo, count=n * ncs)
            ba = ba.reshape((n, ncs))[:, :nc]
            if nc == 1:
                ba = ba.reshape((n, ))
            self._data = ba

        return self._data


class GLTFParser:
    def __init__(self, base_uri=''):
        self.base_uri = base_uri

    def parse(self, gltf, mesh, arrays=None):
        pmesh = {}
        if 'name' in mesh:
            pmesh['name'] = mesh['name']
        pmeshes = []
        pmesh['meshes'] = pmeshes
        primitives = mesh.get('primitives', {})
        global DracoPy
        for prim in primitives:
            mat_i = prim.get('material')
            ext = prim.get('extensions', {})
            mesh_def = {}
            pmeshes.append(mesh_def)
            attrib = prim.get('attributes', {})
            # attrib may be an Attributes instance in pygltflib
            attget = getattr(attrib, 'get', partial(getattr, attrib))
            attdict = getattr(attrib, '__dict__', attrib)

            if 'KHR_draco_mesh_compression' in ext:
                # Draco-compressed mesh
                if DracoPy is None:
                    # raise an ImportError
                    import DracoPy
                draco = ext['KHR_draco_mesh_compression']
                bv = gltf['bufferViews'][draco['bufferView']]
                bv = BufferView(gltf, bv, arrays)
                draco_mesh = DracoPy.decode(bv.data())
                mesh_def['polygons'] = draco_mesh.faces
                mesh_def['vertices'] = draco_mesh.points
                mesh_def['normals'] = draco_mesh.normals
                # tex coords ??
                texcoord = getattr(draco_mesh, 'tex_coord', None)
                if texcoord is not None and len(texcoord) != 0:
                    mesh_def['texcoords'] = {0: texcoord.astype(np.float32)}
            else:
                mode = prim.get('mode', 4)
                indices_i = prim.get('indices')
                pos_i = attget('POSITION')
                normal_i = attget('NORMAL')
                if indices_i is not None:
                    acc = gltf['accessors'][indices_i]
                    accessor = Accessor(gltf, acc, arrays)
                    indices = accessor.data()
                    ns = 1
                    if mode == 1:
                        ns = 2
                    elif mode == 4:
                        ns = 3
                    if ns != 1:
                        indices = indices.reshape((int(indices.shape[0]/ns),
                                                   ns))
                    mesh_def['polygons'] = indices
                if pos_i:
                    acc = gltf['accessors'][pos_i]
                    accessor = Accessor(gltf, acc, arrays)
                    vert = accessor.data()
                    mesh_def['vertices'] = vert
                if normal_i:
                    acc = gltf['accessors'][normal_i]
                    accessor = Accessor(gltf, acc, arrays)
                    norm = accessor.data()
                    mesh_def['normals'] = norm
                texcoords_i = {}
                for k, v in attdict.items():
                    if k.startswith('TEXCOORD_') and v is not None:
                        texcoords_i[int(k[9:])] = v
                if texcoords_i:
                    texcoords = {}
                    for tex, tci in texcoords_i.items():
                        acc = gltf['accessors'][tci]
                        accessor = Accessor(gltf, acc, arrays)
                        tc = accessor.data()
                        texcoords[tex] = tc
                    mesh_def['texcoords'] = texcoords

            if mat_i is not None:
                material = gltf.get('materials', [])[mat_i]
                mat = {}
                if material.get('doubleSided', False):
                    mat['face_culling'] = 0
                emiss = material.get('emissiveFactor')
                if emiss:
                    mat['emission'] = emiss
                col = material.get('pbrMetallicRoughness',
                                   {}).get('baseColorFactor')
                if col is not None:
                    mat['diffuse'] = col
                shininess = material.get('pbrMetallicRoughness',
                                         {}).get('metallicFactor')
                if shininess is not None:
                    # we use a split curve which matches 20 shininess to 0.4
                    # metal
                    if shininess <= 0.4:
                        sh = 128. - shininess * 270.
                    else:
                        sh = 33.33 - shininess * 33.33
                    mat['shininess'] = sh
                roughness = material.get('pbrMetallicRoughness',
                                         {}).get('roughnessFactor')
                if roughness is not None:
                    spec = 1. - roughness
                    mat['specular'] = [spec, spec, spec, 1.]

                if mesh_def.get('texcoords'):
                    mattex = {}
                    coltex = material.get('pbrMetallicRoughness',
                                          {}).get('baseColorTexture')
                    if coltex:
                        mattex0 = self.get_texture(coltex, gltf, arrays)
                        mattex.update(mattex0)
                    emtex = material.get('emissiveTexture')
                    if emtex:
                        mattex0 = self.get_texture(emtex, gltf, arrays)
                        mattex.update(mattex0)

                    if mattex:
                        mat['textures'] = mattex

                mesh_def['material'] = mat

        return pmesh

    def get_texture(self, coltex, gltf, arrays):
        mattex = {}
        tnum = coltex['index']
        tcoord = coltex.get('texCoord', 0)
        texdef = gltf.get('textures', [])[tnum]
        tsamp = texdef.get('sampler')
        mtc = {}
        mattex[tcoord] = mtc
        if tsamp is not None:
            sampler = gltf.get('samplers', [])[tsamp]
            mtc['sampler'] = sampler
        ext = texdef.get('extensions', {})
        teximage_i = None
        if 'EXT_texture_webp' in ext:
            ext_w = ext['EXT_texture_webp']
            teximage_i = ext_w.get('source')
        else:
            teximage_i = texdef.get('source')
        if teximage_i is not None:
            teximage = self.get_teximage(teximage_i, gltf, arrays)
            mtc['teximage'] = teximage
        return mattex

    def image_from_buffer(self, data, format):
        if format == 'webp':
            global webp
            if webp is None:
                import webp  # raise an ImportError
            webp_data = webp.WebPData.from_buffer(data)
            arr = webp_data.decode(
                color_mode=webp.WebPColorMode.RGBA).transpose((1, 0, 2))
            teximage = aims.Volume_RGBA(arr.shape[:2])
            teximage.np['v'][:, :, 0, 0, :] = arr
            return teximage

        tmpd = tempfile.mkdtemp(prefix='aims_gltf_')
        try:
            tmpf = osp.join(tmpd, 'image.%s' % format)
            with open(tmpf, 'wb') as f:
                f.write(data)
            teximage = aims.read(tmpf)
            return teximage
        finally:
            # print('tmpd:', tmpd)
            shutil.rmtree(tmpd)

    def get_teximage(self, texnum, gltf, arrays):
        # print('get_teximage', texnum, arrays is not None)
        if arrays is not None:
            images = arrays.get('images', [])
            # print('images cache:', len(images))
            if len(images) > texnum:
                teximage = images[texnum]
                if teximage is not None:
                    # print('get_teximage from cache', texnum)
                    return teximage

        texdef = gltf.get('images', [])[texnum]
        uri = texdef.get('uri')
        if uri is not None:
            if uri.startswith('data:'):
                start = uri.find(',') + 1
                data = base64.decodebytes(uri[start:].encode())
                endf = uri.find(';', 11)
                format = uri[11: endf]
                teximage = self.image_from_buffer(data, format)
            else:
                url = osp.join(self.base_uri, uri)
                teximage = aims.read(url)

        else:  # uri is None
            bv = gltf['bufferViews'][texdef['bufferView']]
            bv = BufferView(gltf, bv, arrays)
            data = bv.data()
            mtype = texdef.get('mimeType')
            # image in buffer in any format
            # mtype should be 'image/<format>'
            format = mtype[6:]
            teximage = self.image_from_buffer(data, format)

        if arrays is not None:
            images = arrays.setdefault('images', [])
            if len(images) <= texnum:
                images += [None] * (texnum + 1 - len(images))
            # print('set cache image')
            images[texnum] = teximage

        return teximage

    def set_object_referential(self, obj, ref):
        obj['referential'] = ref
        return obj

    def polish_result(self, mesh_dict):
        # do nothing
        return mesh_dict


class AimsGLTFParser(GLTFParser):
    def __init__(self, base_uri=''):
        super().__init__(base_uri=base_uri)
        self.refs = {}

    def aims_trans(self, matrix):
        return aims.AffineTransformation3d(matrix)

    def parse(self, gltf, mesh, arrays=None):
        if arrays is None:
            arrays = {}
        pmeshes = super().parse(gltf, mesh, arrays=arrays)
        name = pmeshes.get('name')

        objects = []
        for obj in pmeshes['meshes']:
            aimsobj = self.parse_object(obj, name)
            objects.append(aimsobj)

        return objects

    def parse_object(self, mesh, name):
        aimsobj = {}
        poly = mesh.get('polygons')
        pdim = poly.shape[1]
        # Note: in Aims <= 5.2, aims.vector.assign() is slow because it
        # converts its argument to a std::vector, then goes to the = operator.
        # Here we assign a numpy array, so we can use np operator in a much
        # more efficient way

        amesh = aims.AimsTimeSurface(pdim)
        # amesh.vertex().assign(mesh.get('vertices'))
        v = mesh.get('vertices')
        amesh.vertex().resize(len(v))
        amesh.vertex().np[:] = v
        # amesh.polygon().assign(poly)
        amesh.polygon().resize(len(poly))
        amesh.polygon().np[:] = poly
        norm = mesh.get('normals')
        if norm is not None:
            # amesh.normal().assign(norm)
            amesh.normal().resize(len(norm))
            amesh.normal().np[:] = norm
        aimsobj['mesh'] = amesh
        if name:
            amesh.header()['name'] = name
        textures = mesh.get('texcoords')
        mat = mesh.get('material')
        if mat:
            amesh.header()['material'] = {k: v for k, v in mat.items()
                                          if k != 'textures'}
        if textures:
            dt = {
                np.dtype('float32'): 'F',
                np.dtype('float64'): 'D',
                np.dtype('int16'): '',
                np.dtype('uint32'): 'U',
                np.dtype('int64'): 'L'
            }
            atexs = []
            aimsobj['textures'] = atexs
            wrap_values = {
                10497: 'repeat',
                33071: 'clamp_to_edge',
                33648: 'mirrored_repeat'
            }
            for tex in sorted(textures.keys()):
                texture = textures[tex]
                ttype = texture.dtype
                if len(texture.shape) > 1 and texture.shape[1] > 1:
                    ttype = 'POINT%dD%s' % (texture.shape[1], dt[ttype])
                else:
                    ttype = aims.typeCode(ttype)
                atex = aims.TimeTexture(ttype)
                # atex[0].data().assign(texture)
                atex[0].data().resize(len(texture))
                atex[0].data().np[:] = texture
                atex.header()['allow_normalize_coords'] = 0
                mattex = mat.get('textures', {}).get(tex, {})
                if mattex:
                    atex.header()['gltf_texture'] = mattex
                    sampler = mattex.get('sampler')
                    if sampler is not None:
                        wrap = []
                        wraps = sampler.get('wrapS')
                        wv = 'clamp_to_edge'
                        if wraps is not None:
                            wv = wrap_values.get(wraps, wv)
                        wrap.append(wv)
                        wrapt = sampler.get('wrapT')
                        wv = 'clamp_to_edge'
                        if wrapt is not None:
                            wv = wrap_values.get(wrapt, wv)
                        wrap.append(wv)
                        if wrap != ['clamp_to_edge', 'clamp_to_edge']:
                            atex.header()['texture_properties'] = [{
                                'wrapmode': wrap
                            }]
                atexs.append(atex)
        return aimsobj

    def aims_transforms(self, trans):
        aimstrans = {}
        refs = self.refs
        refs.update({r: str(uuid.uuid4()) for r in trans if r not in refs})
        for r, tdef in trans.items():
            for r2, t in tdef.items():
                s = refs[r]
                d = refs.get(r2)
                if d is None:
                    d = str(uuid.uuid4())
                    refs[r2] = d
                aimstrans.setdefault(s, {})[d] = self.aims_trans(t)

        return refs, aimstrans

    def get_aims_referential(self, ref):
        aimsref = self.refs.get(ref)
        if aimsref:
            return aimsref
        aimsref = str(uuid.uuid4())
        self.refs[ref] = aimsref
        return aimsref

    def set_object_referential(self, obj, ref):
        aimsref = self.get_aims_referential(ref)
        for sobj in obj:
            sobj['mesh'].header()['referential'] = aimsref
        return obj

    def polish_result(self, mesh_dict):
        refs, aimstrans = \
            self.aims_transforms(mesh_dict.get('transformation_graph', {}))
        objects = mesh_dict['objects']

        aims_meshes = {
            'objects': objects,
            'transformation_graph': aimstrans
        }
        return aims_meshes


def gltf_to_meshes(gltf, object_parser=AimsGLTFParser()):
    ''' Parse a GLTF dict (from a JSON file) and build Aims meshes from it

    Parameters
    ----------
    gltf: dict
        GLTF dictionary
    object_parser: :class:`GLTFParser` instance
        The parser will build mesh objects. It can be inherited to produce
        other kinds of mesh objects (like Anatomist objects). The default parser is an :class:`AimsGLTFParser` instance.

    Returns
    -------
    meshes: dict
        The meshes dict will have the following structure::

            {
              "objects": [object1, object2, ...],
              "transformation_graph": <transform_dict>
            }

        Objects will be the result of object_parser.parse_object() for each
        mesh in the GLTF dict.

        `<transform_dict>` is a dictionary with referentials / transformations
        description, with the stucture::

            {
                source_ref1: {
                    dest_ref2: trans_ref1_to_ref2,
                    dest_ref3: trans_ref1_to_ref2,
                    ...
                },
                source_ref2: {...},
                ...
            }

        The shape of referentials and transformation objects also depends on
        the parser.
    '''
    scene_i = gltf.get('scene', 0)
    scene = gltf.get('scenes', [])[scene_i]
    sc_nodes = scene.get('nodes', [0])
    objects = []
    all_nodes = gltf.get('nodes', [])

    root_ref = object()
    trans = {}
    todo = [(objects, sc_nodes, root_ref)]
    arrays = {}  # decrypted buffers, images

    while todo:
        objects, nodes, nref = todo.pop(0)
        for node_i in nodes:
            if isinstance(node_i, list):
                sub_obj = []
                objects.append(sub_obj)
                todo.append((sub_obj, node_i, nref))
                continue

            node = all_nodes[node_i]
            ref = nref
            matrix = node.get('matrix')
            if matrix is not None:
                ref = object()
                trans.setdefault(nref, {})[ref] = matrix
            children = node.get('children')
            if children:
                sub_obj = []
                objects.append(sub_obj)
                todo.append((sub_obj, children, ref))

            mesh_i = node.get('mesh')
            if mesh_i is not None:
                mesh = gltf['meshes'][mesh_i]
                obj = object_parser.parse(gltf, mesh, arrays=arrays)
                obj = object_parser.set_object_referential(obj, ref)
                objects.append(obj)

    result = {
        'objects': objects,
        'transformation_graph': trans
    }

    result = object_parser.polish_result(result)

    return result


def load_gltf(filename, object_parser=AimsGLTFParser()):
    ''' Load a GLTF or GLB file, and parse it to produce meshes.

    GLB files are open using the pygltflib module, which should be present in
    order to enable this format.

    Parsing is done using :func:`gltf_to_meshes`.
    '''
    if not osp.exists(filename):
        for ext in ('gltf', 'glb'):
            filename2 = '{}.{}'.format(filename, ext)
            if osp.exists(filename2):
                filename = filename2
                break
        else:
            with open(filename):
                # just raise a FileNotFound error
                pass

    try:
        from pygltflib import GLTF2, BufferFormat, ImageFormat
    except ImportError:
        GLTF2 = None  # no GLTF/GLB conversion support

    if GLTF2 is not None:
        gltf_o = GLTF2().load(filename)
        # convert buffers from GLB blobs or Draco compressed
        gltf_o.convert_buffers(BufferFormat.DATAURI)
        gltf = gltf_o.to_dict()
        del gltf_o
    else:
        with open(filename) as f:
            gltf = json.load(f)

    meshes = gltf_to_meshes(gltf, object_parser=object_parser)
    return meshes
