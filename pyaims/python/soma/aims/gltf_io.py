import base64
import numpy as np
import os
import os.path as osp
import tempfile
import shutil
import uuid
from functools import partial
from soma import aims
try:
    import DracoPy
except ImportError:
    DracoPy = None


def vec_to_bytes(vector):
    # (to be rather defined in C++)
    if hasattr(vector, 'np'):
        vector = vector.np
    if vector is None:
        return b''
    return vector.tobytes(order='C')


def add_object_to_gltf_dict(vert, norm, poly, material=None, matrix=None,
                            textures=[], teximages=[], name=None, gltf={}):
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
    '''
    if vert is None:
        return gltf

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
    images = gltf.setdefault('images', [])
    nimages = len(images)
    samplers = gltf.setdefault('samplers', [])
    nsamplers = len(samplers)

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
        buffers.append(
            {
                # base64 should not contain "\n"
                "uri" :
                    ("data:application/octet-stream;base64,"
                      + base64.encodebytes(vec_to_bytes(vert)).decode()
                      + base64.encodebytes(vec_to_bytes(norm)).decode()
                      + base64.encodebytes(vec_to_bytes(poly)).decode()
                    ).replace('\n', ''),
                "byteLength" :
                    len(vert) * 12 + len(norm) * 12 + len(poly) * 4 * ps
            })
    else:
        buffers.append(
            {
                # base64 should not contain "\n"
                "uri" :
                    ("data:application/octet-stream;base64,"
                      + base64.encodebytes(vec_to_bytes(vert)).decode()
                      + base64.encodebytes(vec_to_bytes(poly)).decode()
                    ).replace('\n', ''),
                "byteLength" : len(vert) * 12 + len(poly) * 4 * ps
            })

    # polygons
    buffviews.append(
        {
            "buffer": nbuff,
            "byteOffset": len(vert) * 3 * 4 + len(norm) * 3 * 4,
            "byteLength": len(poly) * ps * 4
        })

    # vertices
    buffviews.append(
        {
            "buffer": nbuff,
            "byteOffset": 0,
            "byteLength": len(vert) * 3 * 4
        })

    if hasnorm:
        # normals
        buffviews.append(
            {
                "buffer": nbuff,
                "byteOffset": len(vert) * 3 * 4,
                "byteLength": len(norm) * 3 * 4
            })

    accessors.append(
        {
            "bufferView" : nbuffv,
            "componentType" : 5125,
            "count": len(poly) * ps,
            "type": "SCALAR",
        })

    accessors.append(
        {
            "bufferView" : nbuffv + 1,
            "componentType" : 5126,
            "count" : len(vert),
            "type" : "VEC3",
        })

    if hasnorm:
        accessors.append(
            {
                "bufferView" : nbuffv + 2,
                "componentType" : 5126,
                "count" : len(norm),
                "type" : "VEC3",
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
        if 'diffuse' in material:
            gmat['pbrMetallicRoughness']['baseColorFactor'] \
                = list(material['diffuse'])
        if not material.get('face_culling', True):
            gmat['doubleSided'] = True
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
        tcmap = tempfile.mkdtemp(prefix='anat_export')
        try:
            for teximage in teximages:
                cmapname = osp.join(tcmap, 'texture_0000.png')
                aims.write(teximage, cmapname)
                with open(cmapname, 'rb') as f:
                    b = f.read()
                image_bytes = "data:image/png;base64," \
                    + base64.encodebytes(b).decode().replace('\n', '')
                images.append({
                    'uri': image_bytes,
                })
                del b
        finally:
            shutil.rmtree(tcmap)

        # note: only one baseColorTexture is allowed.
        texnames = ['pbrMetallicRoughness.baseColorTexture',
                    'emissiveTexture']

        for tex, tc in enumerate(textures):
            buffers.append({
                "uri" :
                    ("data:application/octet-stream;base64,"
                     + base64.encodebytes(vec_to_bytes(tc)).decode()
                    ).replace('\n', ''),
                "byteLength" : int(np.prod(tc.shape) * 4)
            })
            buffviews.append({
                "buffer": nbuff,
                "byteOffset": 0,
                "byteLength": int(np.prod(tc.shape) * 4)
            })
            accessors.append({
                "bufferView" : nbuffv,
                "componentType" : 5126,
                "count" : len(tc),
                "type" : vtype[tc.shape[1]],
            })
            mesh['primitives'][0]['attributes']['TEXCOORD_%s' % tex] \
                = naccess
            nbuff += 1
            nbuffv += 1
            naccess += 1

            samplers.append({
                "magFilter" : 9729,
                "minFilter" : 9987,
                "wrapS" : 33648,
                "wrapT" : 33648
            })
            gtextures.append({
                'source': nimages,
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

            nimages += 1

    nodes.append(node)
    if 'children' in nodes[0]:
        nodes[0]['children'].append(nnodes)

    return gltf


def mesh_to_gltf(mesh, matrix=None, name=None, gltf={}):
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
    '''
    vert = mesh.vertex()
    norm = mesh.normal()
    poly = mesh.polygon()
    material = mesh.header().get('material')
    return add_object_to_gltf_dict(vert, norm, poly, material=material,
                                   name=name, gltf=gltf)


def default_gltf_scene(matrix=None, gltf={}):
    ''' Create a default GLTF dics scene
    '''
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

        if matrix is not None \
                and not aims.AffineTransformation3d(matrix).isIdentity():
            gltf['nodes'][0]['matrix'] = matrix

    return gltf


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
        self._data = buff[bo:bo + bl:bs]
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
        elif ctype == 5121:
            nptype = np.uint8
        elif ctype == 5122:
            nptype = np.int16
        elif ctype == 5123:
            nptype = np.uint16
        elif ctype == 5125:
            nptype = np.uint32
        elif ctype == 5126:
            nptype = np.float32
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
            ba = np.frombuffer(bvdata, dtype=nptype, offset=bo, count=n * nc)
            if nc != 1:
                ba = ba.reshape((n, nc))
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
                col = material.get('pbrMetallicRoughness',
                                   {}).get('baseColorFactor')
                if col is not None:
                    mat['diffuse'] = col
                if texcoords_i:
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


    def get_teximage(self, texnum, gltf, arrays):
        if arrays is not None:
            images = arrays.get('images', [])
            if len(images) > texnum:
                teximage = images[texnum]
                if teximage is not None:
                    return teximage

        texdef = gltf.get('images', [])[texnum]
        uri = texdef.get('uri')
        if uri is not None:
            if uri.startswith('data:image/'):
                start = uri.find(',') + 1
                data = base64.decodebytes(uri[start:].encode())
                tmpd = tempfile.mkdtemp(prefix='aims_gltf_')
                endf = uri.find(';', 11)
                format = uri[11: endf]
                try:
                    tmpf = osp.join(tmpd, 'image.%s' % format)
                    with open(tmpf, 'wb') as f:
                        f.write(data)
                    teximage = aims.read(tmpf)
                finally:
                    # print('tmpd:', tmpd)
                    shutil.rmtree(tmpd)
            else:
                url = osp.join(self.base_uri, uri)
                teximage = aims.read(url)
            if arrays is None:
                arrays = {}
            images = arrays.setdefault('images', [])
            if len(images) <= texnum:
                images += [None] * (texnum + 1 - len(images))
            return teximage
        else: # uri is None
            bv = gltf['bufferViews'][texdef['bufferView']]
            bv = BufferView(gltf, bv, arrays)
            data = bv.data()
            mtype = texdef.get('mimeType')
            if mtype == 'image/webp':
                import webp
                webp_data = webp.WebPData.from_buffer(data)
                arr = webp_data.decode(color_mode=webp.WebPColorMode.RGBA)
                teximage = aims.Volume_RGBA(arr.shape[:2])
                teximage.np['v'][:, :, 0, 0, :] = arr
                return teximage

    def set_object_referential(self, obj, ref):
        obj['referential'] = ref
        return obj

    def polish_result(self, mesh_dict):
        # do nothing
        return mesh_dict


class AimsGLTFParser(GLTFParser):
    def __init__(self):
        self.refs = {}

    def aims_trans(self, matrix):
        return aims.AffineTransformation3d(matrix)

    def parse(self, gltf, mesh, arrays={}):
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
        amesh = aims.AimsTimeSurface(pdim)
        amesh.vertex().assign(mesh.get('vertices'))
        amesh.polygon().assign(poly)
        norm = mesh.get('normals')
        if norm is not None:
            amesh.normal().assign(norm)
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
            for tex in sorted(textures.keys()):
                texture = textures[tex]
                ttype = texture.dtype
                if len(texture.shape) > 1 and texture.shape[1] > 1:
                    ttype = 'POINT%dD%s' % (texture.shape[1], dt[ttype])
                else:
                    ttype = aims.typeCode(ttype)
                atex = aims.TimeTexture(ttype)
                atex[0].data().assign(texture)
                mattex = mat.get('textures', {}).get(tex, {})
                if mattex:
                    atex.header()['gltf_texture'] = mattex
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
