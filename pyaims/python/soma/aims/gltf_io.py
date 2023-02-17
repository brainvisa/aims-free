import base64
import numpy as np
import os
import os.path as osp
import tempfile
import shutil
from soma import aims


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

    # gltf doesn't handle quads, we have to transform
    if poly.shape[1] == 4:
        poly2 = poly[:, :3]
        poly3 = poly[:, (0, 2, 3)]
        poly = np.vstack((poly2, poly3))

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
        print('textures:', len(textures))
        tcmap = tempfile.mkdtemp(prefix='anat_export')
        for teximage in teximages:
            cmapname = osp.join(tcmap, 'texture_0000.png')
            aims.write(teximage, cmapname)
            with open(cmapname, 'rb') as f:
                b = f.read()
            shutil.rmtree(tcmap)
            image_bytes = "data:image/png;base64," \
                + base64.encodebytes(b).decode().replace('\n', '')
            images.append({
                'uri': image_bytes,
            })
            del b

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
            gmat['pbrMetallicRoughness']['baseColorTexture'] = {
                'index': ntex,
                'texCoord': tex
            }
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


