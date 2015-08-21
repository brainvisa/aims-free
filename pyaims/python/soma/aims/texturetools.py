#!/usr/bin/env python
# -*- coding: utf-8 -*-
from soma import aims, aimsalgo
import exceptions
import numpy
import os


def mergeLabelsFromTexture(tex, labels_list, new_label):
    """
    inputs:
          tex: labeled texture ( from FreeSurfer or an other )
          labels_list, new_label: you can overwrite numbers ( labels_list ) with your own number ( new_label )
    ouput:
          otex: labeled texture with merged regions
    """
    otex = aims.TimeTexture_S16()
    tex_ar = tex[0].arraydata()
    otex[0].assign(tex_ar)
    otex_ar = otex[0].arraydata()
    for i in labels_list:
        otex_ar[otex_ar == int(i)] = new_label
    return otex


def extractLabelsFromTexture(tex, labels_list, new_label):
    """
    inputs:
          tex: labeled texture ( from FreeSurfer or an other )
          labels_list, new_label: you can overwrite numbers ( labels_list ) with your own number ( new_label )
    output:
          otex: labeled texture with merged regions only
    """
    otex = aims.TimeTexture_S16()
    otex[0].reserve(tex[0].nItem())
    for i in xrange(tex[0].nItem()):
        otex[0].push_back(0)
    tex_ar = tex[0].arraydata()
    otex_ar = otex[0].arraydata()
    for i in labels_list:
        otex_ar[tex_ar == int(i)] = new_label
    return otex


def connectedComponents(mesh, tex, areas_mode=0):
    """
    Parameters
    ----------
    mesh
    tex: aimsTimeTexture_S16
        (one time step) labeled between 1 and LabelsNb, background = 0,
        ignored_vertex = -1.

    areas_mode:
        if = 1: computing area measures of the connected components,
        if = 0: no measure (by default).

    Returns
    -------
    cctex: connectedComponentTex: aimsTimeTexture_S16
        time step = LabelsNb, for each time step (label in the tex), texture of
        the connected components corresponding to this label (background = -1,
        and connected components = values between 1 and ccNb).
    areas_measure: python dictionary
        areas_measures[label] = [16.5, 6.0]
        (numpy array) if label (in tex) has two connected Components 1 and 2
        with area = 16.5 and 6.0 respectively, areas are in square mm
    """
    meshVertexNb = int(mesh.vertex().size())
    print 'Vertices number of mesh: ', meshVertexNb
    areas_measures = {}

    if (meshVertexNb != tex.nItem()):
        raise exceptions.ValueError(
            'mesh and input texture have not the same dimension...')

    dtex = tex[0].arraydata()
    labels = numpy.unique(dtex)
    labelsList = labels.tolist()
    if labelsList.count(0) != 0:
        labelsList.remove(0)
    if labelsList.count(-1) != 0:
        labelsList.remove(-1)
    print 'Labels list: ', labelsList

    dtex = tex[0].arraydata()
    otex = aims.TimeTexture(dtype='S16')
    cctex = aims.TimeTexture_S16()
    for label in labelsList:
        otex[0].assign((dtex == label).astype('int16'))
        labelcctex = aimsalgo.AimsMeshLabelConnectedComponent(
            mesh, otex, 0, 0)
        if areas_mode:
            ccNb = labelcctex[0].arraydata().max()
            areas_measures[label] = numpy.zeros(ccNb)
            for c in xrange(ccNb):
                ccMesh = aims.SurfaceManip.meshExtract(
                    mesh, labelcctex, c + 1)[0]
                ccArea = aims.SurfaceManip.meshArea(ccMesh)
                areas_measures[label][c] = ccArea
        cctex[label - 1].resize(meshVertexNb, 0)
        cctex[label - 1].assign(labelcctex[0].arraydata())

    if areas_mode:
        return cctex, areas_measures
    else:
        return cctex


def remove_non_principal_connected_components(mesh, tex, trash_label):
    """Keep only the largest connected component in each label, for a label
    texture.

    Parameters
    ----------
    mesh:
    tex: label texture (S16, int)
    trash_label: value to replace non-principal components

    Returns
    -------
    out_tex: label texture
    """
    t0 = tex[0].arraydata()
    t0 += 1  # 0 is a real label
    conn_comp, areas = connectedComponents(mesh, tex, areas_mode=True)
    t0 -= 1
    dtype = tex[0].arraydata().dtype
    out_tex = aims.TimeTexture(dtype=dtype)
    out_tex[0].assign(numpy.zeros(tex[0].size(), dtype=dtype))
    out_arr = out_tex[0].arraydata()
    out_arr[:] = trash_label
    for label in conn_comp.keys():
        comps = conn_comp[label]
        largest = numpy.argmax(areas[label + 1]) + 1
        comp_arr = comps.arraydata()
        out_arr[comp_arr==largest] = label
    return out_tex


def meshDiceIndex(mesh, texture1, texture2, timestep1=0,
                  timestep2=0, labels_table1=None, labels_table2=None):
    """DICE index calculation between two sets of regions defined by label textures on a common mesh.
    texture1, texture2: aims.TimeTexture instances, should be int (labels).
    timestep1, timestep2: timestep to use in texture1 and texture2.
    labels_table1, labels_table2: optional labels translation tables (dicts or arrays) to translate values of texture1 and/or texture2.

    return
    """
    tex1 = texture1[timestep1].arraydata()
    tex2 = texture2[timestep2].arraydata()
    if labels_table1 is not None:
        tex1 = numpy.array([labels_table1[x] for x in tex1])
    if labels_table2 is not None:
        tex2 = numpy.array([labels_table2[x] for x in tex2])
    regions = max(numpy.max(tex1), numpy.max(tex2)) + 1
    areas1 = numpy.zeros((regions, ))
    areas2 = numpy.zeros((regions, ))
    inter = numpy.zeros((regions, ))
    vertices = mesh.vertex()
    polygons = mesh.polygon()
    for poly in polygons:
        p = vertices[poly[0]]
        u1 = vertices[poly[1]] - p
        u2 = vertices[poly[2]] - p
        area = u1.crossed(u2).norm() / 6  # 1/3 area for each vertex
        l1 = tex1[poly[0]]
        l2 = tex2[poly[0]]
        areas1[l1] += area
        areas2[l2] += area
        if l1 == l2:  # intersection
            inter[l1] += area
        l1 = tex1[poly[1]]
        l2 = tex2[poly[1]]
        areas1[l1] += area
        areas2[l2] += area
        if l1 == l2:  # intersection
            inter[l1] += area
        l1 = tex1[poly[2]]
        l2 = tex2[poly[2]]
        areas1[l1] += area
        areas2[l2] += area
        if l1 == l2:  # intersection
            inter[l1] += area
    dice = inter * 2 / (areas1 + areas2)
    return dice, areas1, areas2, inter


def average_texture(output, inputs):
    """
    Create average gyri texture from a group of subject.
    """
    # read textures
    tex = []
    for fname in inputs:
        tex.append(aims.read(fname))
    # make a 2D array from a series of textures
    ar = numpy.vstack([t[0].arraydata() for t in tex])
    # count occurrences
    N = numpy.max(ar)

    def bin_resized(x):
        y = numpy.bincount(x)
        y.resize(N + 1)  # labels: 1 to 72
        return y
    cnt = numpy.apply_along_axis(bin_resized, 0, ar)
    # get max of occurrences in each vertex
    maj = numpy.argmax(cnt, axis=0)
    # make an aims texture from result (numpy array)
    otex = aims.TimeTexture('S16')
    otex[0].assign(maj)

    aims.write(otex, output)


def nomenclature_to_colormap(hierarchy, labels_list, as_float=True,
                             default_color=[0.3, 0.6, 1., 1.]):
    """
    Make a colormap from labels and colors of a nomenclature (hierarchy),
    following a labels_list order.

    Parameters
    ----------
    hierarchy: Hierarchy object
        nomenclature
    labels_list: list of strings
        labels with order. The returned colormap will follow this ordering.
    as_float: bool (optional, default: True)
        if True, colors will be float values in the [0-1] range.
        If False, they will be int values in the [0-255] range.
    default_color: list (4 floats) (optional)
        Color used for labels not found in the nomenclature. It is given as
        floats ([0-1] range).

    Returns
    -------
    colormap: numpy array
        array of colors (4 float values in [0-1] range)
    """
    colors = []
    for label in labels_list:
        try:
            color = hierarchy.find_color(label, default_color=default_color)
            color = list(color)
            if len(color) < 4:
                color.append(1.)
        except:
            color = default_color
        if not as_float:
            color = [int(c*255.9) for c in color]
        colors.append(list(color))
    return numpy.array(colors)


def vertex_texture_to_polygon_texture(mesh, tex, allow_cut=False):
    """Make a "polygon texture" from a vartex-based label texture.
    A polygon texture has a value for each polygon.

    For a given polygon the value is taken as the majority of values on its
    vertices. If an absolute majority cannot be obtained, the mesh polygons may
    be cut to avoid losing precision. This is done if allow_cut is True.

    When allow_cut is False, the returned value is the polygon texture.
    It may work on meshes of any polygon size (triangles, quads, segments...)

    When allow_cut is True, the returned value is a tuple:
      * polygon texture
      * new mesh with possibly split triangles
    It only works for meshes of triangles.
    """
    dtype = tex[tex.keys()[0]].arraydata().dtype
    poly_tex = aims.TimeTexture(dtype=dtype)

    if allow_cut:
        out_mesh = mesh.__class__(mesh)

    for t, tex0 in tex.iteritems():
        tdata = tex0.arraydata()
        ptex0 = poly_tex[t]
        ptex0.resize(len(mesh.polygon(t)))
        poly_labels = ptex0.arraydata()
        if allow_cut:
            added_vert = {}
            vertex = out_mesh.vertex(t)
            polygon = out_mesh.polygon(t)

        for p, poly in enumerate(mesh.polygon(t)):
            D = len(poly)
            labels = [tdata[v] for v in poly]
            ulabels = numpy.unique(labels)
            ilabels = [numpy.where(labels==u)[0] for u in ulabels]
            nlabels = [len(u) for u in ilabels]
            if not allow_cut:
                maj = ulabels[numpy.argmax(nlabels)]
                poly_labels[p] = maj
            else:
                # WARNING this only works for triangles.
                if len(ulabels) == 1:
                    poly_labels[p] = ulabels[0]
                elif len(ulabels) == 2:
                    # cut off one vertex
                    iother = labels.index(ulabels[numpy.argmin(nlabels)])
                    ikeep = [i for i in range(D) if i!=iother]
                    iv0 = poly[iother]
                    iv1 = poly[(iother-1) % D]
                    ind = (min((iv0, iv1)), max((iv0, iv1)))
                    ivn1 = added_vert.get(ind)
                    if ivn1 is None:
                        v1 = (vertex[iv0] + vertex[iv1]) * 0.5
                        ivn1 = len(vertex)
                        vertex.append(v1)
                        added_vert[ind] = ivn1

                    iv2 = poly[(iother+1) % D]
                    ind = (min((iv0, iv2)), max((iv0, iv2)))
                    ivn2 = added_vert.get(ind)
                    if ivn2 is None:
                        v2 = (vertex[iv0] + vertex[iv2]) * 0.5
                        ivn2 = len(vertex)
                        vertex.append(v2)
                        added_vert[ind] = ivn2
                    polygon[p][(iother-1) % D] = ivn1
                    polygon[p][(iother+1) % D] = ivn2
                    polygon.append(poly.__class__(iv1, ivn1, ivn2))
                    polygon.append(poly.__class__(ivn2, iv2, iv1))
                    poly_labels[p] = tdata[iv0]
                    ptex0.append(tdata[iv1])
                    ptex0.append(tdata[iv2])
                else:
                    # cut in 3 regions
                    bs0 = (min(poly[0], poly[1]), max(poly[0], poly[1]))
                    bs1 = (min(poly[1], poly[2]), max(poly[1], poly[2]))
                    bs2 = (min(poly[2], poly[0]), max(poly[2], poly[0]))
                    bi0 = added_vert.get(bs0)
                    if bi0 is None:
                        v0 = (vertex[poly[0]] + vertex[poly[1]]) * 0.5
                        bi0 = len(vertex)
                        added_vert[bs0] = bi0
                        vertex.append(v0)
                    bi1 = added_vert.get(bs1)
                    if bi1 is None:
                        v1 = (vertex[poly[1]] + vertex[poly[2]]) * 0.5
                        bi1 = len(vertex)
                        added_vert[bs1] = bi1
                        vertex.append(v1)
                    bi2 = added_vert.get(bs2)
                    if bi2 is None:
                        v2 = (vertex[poly[2]] + vertex[poly[0]]) * 0.5
                        bi2 = len(vertex)
                        added_vert[bs2] = bi2
                        vertex.append(v2)
                    bi3 = len(vertex)
                    v3 = (vertex[poly[0]] + vertex[poly[1]]
                          + vertex[poly[2]]) / 3.
                    vertex.append(v3)
                    polygon[p][1] = bi0
                    polygon[p][2] = bi3
                    polygon.append(poly.__class__(bi3, bi2, poly[0]))
                    polygon.append(poly.__class__(poly[1], bi1, bi3))
                    polygon.append(poly.__class__(poly[1], bi3, bi0))
                    polygon.append(poly.__class__(poly[2], bi2, bi3))
                    polygon.append(poly.__class__(poly[2], bi3, bi1))

                    poly_labels[p] = labels[0]
                    ptex0.append(labels[0])
                    ptex0.append(labels[1])
                    ptex0.append(labels[1])
                    ptex0.append(labels[2])
                    ptex0.append(labels[2])

    if allow_cut:
        return (poly_tex, out_mesh)
    else:
        return poly_tex


def mesh_to_polygon_textured_mesh(mesh, poly_tex):
    """
    """
    out_mesh = mesh.__class__()
    out_tex = poly_tex.__class__()
    polygons = mesh.polygon()
    dtype = poly_tex[poly_tex.keys()[0]].arraydata().dtype
    for t, tex0 in poly_tex.iteritems():
        print 't:', t
        overt = out_mesh.vertex(t)
        overt.assign(mesh.vertex())
        onorm = out_mesh.normal(t)
        onorm.assign(mesh.vertex())
        opoly = out_mesh.polygon(t)
        opoly.assign(mesh.polygon())
        otex = out_tex[t]
        otex.assign(numpy.zeros(mesh.vertex().size(), dtype=dtype) - 1)
        #otex_arr = otex.arraydata()
        tex_arr = tex0.arraydata()
        added = {}
        for p in xrange(len(mesh.polygon())):
            plabel = tex_arr[p]
            poly = opoly[p]
            for i, v in enumerate(poly):
                if otex[v] < 0:
                    otex[v] = plabel
                elif otex[v] != plabel:
                    old_new = added.get((v, plabel))
                    if old_new:
                        # already added, just change triangle
                        poly[i] = old_new
                    else:
                        # add a new vertex, and change triangle
                        vb = overt.size()
                        overt.append(overt[v])
                        poly[i] = vb
                        otex.data().append(plabel)
                        added[(v, plabel)] = vb
                        #otex_arr = otex.arraydata()

    out_mesh.updateNormals()
    return out_mesh, out_tex
