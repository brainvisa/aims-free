#!/usr/bin/env python
# -*- coding: utf-8 -*-

from soma import aims, aimsalgo
import numpy as np


def graph_to_meshes(graph, label_att='label', nomenclature=None,
                    mni_space=False, transform=None, referential=None,
                    removed_labels=None):
    ''' Make one mesh for each sucus in a labelled sulcal graph.

    graph may be either a Graph object, or a filename.
    If nomenclature is not given, an attempt will be made to read the standard
    sulci nomenclature in BrainVisa shared data. If available, the nomenclature
    will be used to assign colors to meshes.
    '''
    if isinstance(graph, str):
        graph = aims.read(graph)

    meshes = {}
    labelmap = {}
    if removed_labels is None:
        removed_labels = set()
    elif not isinstance(removed_labels, set):
        removed_labels = set(removed_labels)

    if nomenclature is None:
        nomenc_f = aims.carto.Paths.findResourceFile(
            'nomenclature/hierarchy/sulcal_root_colors.hie')
        if nomenc_f is not None:
            nomenclature = aims.read(nomenc_f)

    for v in graph.vertices():
        label = v.get(label_att)
        if label is not None and label not in removed_labels:
            bck = labelmap.setdefault(label, aims.BucketMap_VOID())
            bk = bck[0]
            for bname in ('aims_ss', 'aims_bottom', 'aims_other'):
                b = v.get(bname)
                if b is not None:
                    for p in b[0].keys():
                        bk[p] = 1

            for e in v.edges():
                if e.getSyntax() in ('junction', 'plidepassage',
                                     'hull_junction'):
                    for bname in ('aims_junction', 'aims_plidepassage'):
                        b = e.get(bname)
                        if b is not None:
                            for p in b[0].keys():
                                bk[p] = 1

    mni = aims.GraphManip.getICBMTransform(graph)
    ref = graph.get('referential')

    mesher = aimsalgo.Mesher()
    mesher.setDecimation(0.99, 3., 0.2, 180.)
    mesher.setSmoothing(mesher.LOWPASS, 50, 0.4)
    mesher.setVerbose(False)

    print('meshing buckets...')
    n = len(labelmap)
    vs = graph['voxel_size']

    for i, (label, bck) in enumerate(labelmap.items()):
        print('meshing sulcus', i, '/', n)
        bck.setSizeXYZT(vs[0], vs[1], vs[2], 1.)

        surfaces = mesher.doit(bck)
        surface = aims.AimsSurfaceTriangle()
        for sl in surfaces.values():
            for s in sl:
                aims.SurfaceManip.meshMerge(surface, s)
        # additional decimation/smothing
        mesher.decimate(surface)
        mesher.smooth(surface)
        mesher.decimate(surface)
        mesher.smooth(surface)
        mesher.decimate(surface)

        if nomenclature is not None:
            color = nomenclature.find_color(label, None)
            if color is not None:
                surface.header()['material'] = {'diffuse': list(color) + [1.]}

        if 'GIFTI_labels_table' in surface.header():
            del surface.header()['GIFTI_labels_table']

        if mni_space:
            aims.SurfaceManip.meshTransform(surface, mni)
            surface.header()['referential'] \
                = aims.StandardReferentials.mniTemplateReferentialID()
        elif transform is not None:
            if referential is not None:
                surface.header()['referential'] = referential
            aims.SurfaceManip.meshTransform(surface, transform)
            to_mni = mni * transform.inverse()
            surface.header()['referentials'] \
                = [aims.StandardReferentials.mniTemplateReferentialID()]
            surface.header()['transformations'] = [to_mni.toVector()]
        else:
            if ref is not None:
                surface.header()['referential'] = ref
            surface.header()['referentials'] \
                = [aims.StandardReferentials.mniTemplateReferentialID()]
            surface.header()['transformations'] = [mni.toVector()]

        meshes[label] = surface

    return meshes, labelmap


def save_graph_to_meshes(graph, filename_pattern, label_att='label',
                         nomenclature=None, format=None, mni_space=False,
                         transform=None, referential=None,
                         removed_labels=None):
    ''' Call graph_to_meshes and save mesh files according to a filename
    pattern.

    filename_pattern should contain %(label)s as a placeholder for the sulcus
    label, ex::

        filename_pattern = '/tmp/sulci_meshes/Lsubject_%(label)s.gii'

    format will be used to force a particular format, especially if the
    filename extension is ambiguous (like ``.obj`` which may be Wavefront or
    MNI mesh), ex: ``WAVERONT``).
    '''
    meshes, buckets = graph_to_meshes(
        graph, label_att=label_att, nomenclature=nomenclature,
        mni_space=mni_space, transform=transform, referential=referential,
        removed_labels=removed_labels)

    if format in ('GLTF', 'GLB') or \
            (format is None and (filename_pattern.endswith('.glb')
                                 or filename_pattern.endswith('.gltf'))):
        fname = filename_pattern
        aims.write(list(meshes.values()), fname, format=format)
    else:
        for label, mesh in meshes.items():
            fname = filename_pattern % {'label': label}
            aims.write(mesh, fname, format=format)

    return meshes, buckets


def spam_to_graphs(spam, labels=None):
    def new_graph(tal_trans):
        graph = aims.Graph('CorticalFoldArg')
        graph['filename_base'] = '*'
        graph['voxel_size'] = [1., 1., 1.]
        graph['boundingbox_min'] = [-90, -80, -90]
        graph['boundingbox_max'] = [90, 120, 60]
        graph['fold.global.tri'] = 'fold aims_Tmtktri Tmtktri_label'
        graph['label_property'] = 'name'
        graph['type.global.tri'] = 'fold.global.tri'
        graph['referential'] = 'a2a820ac-a686-461e-bcf8-856400740a6c'
        aims.GraphManip.storeTalairach(graph, tal_trans)

        return graph

    def new_vertex(graph, label):
        v = graph.addVertex('fold')
        v['bottom_point_number'] = 0
        v['name'] = label
        v['point_number'] = 0
        v['size'] = 0
        v['ss_point_number'] = 0

        return v

    if labels is None:
        labels = spam.header()['GIFTI_labels_table']

    vol = aims.Volume(spam.shape[0], spam.shape[1], spam.shape[2], 1, 1,
                      dtype='S16')
    vol.copyHeaderFrom(spam.header())
    vol.fillBorder(-1)
    mesher = aimsalgo.Mesher()
    mesher.setDecimation(0.99, 3., 0.2, 180.)
    mesher.setSmoothing(mesher.LOWPASS, 50, 0.4)
    mesher.setVerbose(False)

    try:
        tali = spam.header()['referentials'].index(
            aims.StandardReferentials.acPcReferentialID())
        tal = aims.AffineTransformation3d(spam.header()[
            'transformations'][tali])
    except ValueError:
        try:
            icbmi = spam.header()['referentials'].index(
                aims.StandardReferentials.mniTemplateReferential())
        except ValueError:
            icbmi = spam.header()['referentials'].index(
                aims.StandardReferentials.mniTemplateReferentialID())
        icbm = aims.AffineTransformation3d(spam.header()['transformations'][
            icbmi])
        tal = aims.StandardReferentials.talairachToICBM().inverse() * icbm

    allmeshes = {0.2: {}, 0.4: {}, 0.7: {}}
    graphs = {0.2: [new_graph(tal), new_graph(tal)],
              0.4: [new_graph(tal), new_graph(tal)],
              0.7: [new_graph(tal), new_graph(tal)]}

    bboxes = {}
    for i, labeld in labels.items():
        label = labeld['Label']
        lmax = np.max(spam[:, :, :, i])
        for th, amesh in allmeshes.items():
            vol.fill(0)
            vol[spam[:, :, :, i] >= th * lmax] = 1
            meshes = mesher.doit(vol)
            mesh = aims.AimsSurfaceTriangle()
            mesh.header()['label'] = label
            color = labeld['RGB'] + [th + 0.3]
            mesh.header()['material'] = {'diffuse': color}

            for sl in meshes.values():
                for s in sl:
                    aims.SurfaceManip.meshMerge(mesh, s)
            # additional decimation/smothing
            mesher.decimate(mesh)
            mesher.smooth(mesh)
            mesher.decimate(mesh)
            mesher.smooth(mesh)
            mesher.decimate(mesh)
            if label.endswith('_right'):
                side = 1
            else:
                side = 0
            graph = graphs[th][side]
            glabel = label
            if glabel.startswith('unknown'):
                glabel = 'unknown'
            v = new_vertex(graph, glabel)
            aims.GraphManip.storeAims(graph, v, 'aims_Tmtktri', mesh)
            v['skeleton_label'] = i

            amesh[label] = mesh
            bbox = bboxes.setdefault(th, [None, None])[side]
            bbmin = np.min(mesh.vertex(), axis=0)
            bbmax = np.max(mesh.vertex(), axis=0)
            if bbox is None:
                bbox = [bbmin, bbmax]
            else:
                bbox = [np.min((bbox[0], bbmin), axis=0),
                        np.max((bbox[1], bbmax), axis=0)]
            bboxes[th][side] = bbox

    for th, graphs in graphs.items():
        for side, graph in enumerate(graphs):
            vs = graph['voxel_size'][:3]
            bbox = bboxes[th][side]
            bboxi = [np.floor(bbox[0][:3] / vs).astype(int),
                     np.ceil(bbox[1][:3] / vs).astype(int)]
            graph['boundingbox_min'] = bboxi[0]
            graph['boundingbox_max'] = bboxi[1]

    return graphs, allmeshes


def build_hierarchy_for_labels(labels):
    ''' labels is a GIFTI-like labels table
    ({index: {"Label": label, "RGB": color}})
    '''
    h = aims.Hierarchy()
    h.setSyntax('hierarchy')
    h['graph_syntax'] = 'CorticalFoldArg'
    t = aims.Tree()
    t.setSyntax('fold_name')
    t['name'] = 'brain'
    h.insert(t)
    tl = aims.Tree()
    tl.setSyntax('fold_name')
    tl['name'] = 'hemisph_left'
    t.insert(tl)
    tr = aims.Tree()
    tr.setSyntax('fold_name')
    tr['name'] = 'hemisph_right'
    t.insert(tr)

    for i, ldef in labels.items():
        label = ldef['Label']
        col = ldef['RGB']
        col = [int(round(c * 255)) for c in col]
        if label.endswith('_right'):
            parent = tr
        else:
            parent = tl
        tc = aims.Tree()
        tc.setSyntax('fold_name')
        tc['name'] = label
        tc['label'] = '%02d' % i
        tc['color'] = col
        parent.insert(tc)

    return h


def symmetrize_hierarchy_colors(hierarchy, left_to_right=True):
    def get_other_name(name):
        if name.endswith('_left'):
            return name[:-4] + 'right'
        else:
            return name[:-5] + 'left'

    brain = [c for c in hierarchy.children() if c.get('name') == 'brain'][0]
    lh = [c for c in brain.children() if c.get('name') == 'hemisph_left'][0]
    rh = [c for c in brain.children() if c.get('name') == 'hemisph_right'][0]
    if left_to_right:
        src = lh
        dst = rh
    else:
        src = rh
        dst = lh

    tree_map = {}
    todo = [dst]
    while todo:
        item = todo.pop(0)
        name = item.get('name')
        if name is not None:
            tree_map[name] = item
        todo += item.children()
    print('dest map:', len(tree_map))
    print(tree_map)

    todo = [src]
    while todo:
        item = todo.pop(0)
        name = item.get('name')
        if name is not None:
            color = item.get('color')
            if color is not None:
                other_name = get_other_name(name)
                ditem = tree_map.get(other_name)
                if ditem is not None:
                    ditem['color'] = color
                else:
                    print('cannot find match for:', other_name)
        todo += item.children()

