#!/usr/bin/env python
# -*- coding: utf-8 -*-

from soma import aims, aimsalgo


def graph_to_meshes(graph, label_att='label', nomenclature=None,
                    mni_space=False):
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

    if nomenclature is None:
        nomenc_f = aims.carto.Paths.findResourceFile(
            'nomenclature/hierarchy/sulcal_root_colors.hie')
        if nomenc_f is not None:
            nomenclature = aims.read(nomenc_f)

    for v in graph.vertices():
        label = v.get(label_att)
        if label is not None:
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

        if mni_space:
            aims.SurfaceManip.meshTransform(surface, mni)
            surface.header()['referential'] \
                = aims.StandardReferentials.mniTemplateReferentialID()
        else:
            if ref is not None:
                surface.header()['referential'] = ref
            surface.header()['referentials'] \
                = [aims.StandardReferentials.mniTemplateReferentialID()]
            surface.header()['transformations'] = [mni.toVector()]

        meshes[label] = surface

    return meshes, labelmap


def save_graph_to_meshes(graph, filename_pattern, label_att='label',
                         nomenclature=None, format=None, mni_space=False):
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
        mni_space=mni_space)

    for label, mesh in meshes.items():
        fname = filename_pattern % {'label': label}
        aims.write(mesh, fname, format=format)

    return meshes, buckets
