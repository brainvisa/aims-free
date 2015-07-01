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
    inputs:
            mesh
            tex: aimsTimeTexture_S16 (one time step) labeled between 1 and LabelsNb, background = 0, ignored_vertex = -1
            areas_mode: if = 1: computing area measures of the connected components, if = 0: no measure (by default)
    output:
            cctex: onnectedComponentTex: aimsTimeTexture_S16, time step = LabelsNb, for each time step (label in the tex), texture of the connected components corresponding to this label (background = -1, and connected components = values between 1 and ccNb)

            areas_measures = python dictionary, areas_measures[label] = [16.5, 6.0] (numpy array) if label (in tex) has two connected Components 1 and 2 with area = 16.5 and 6.0 respectively, areas are in square mm
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
                c = c + 1
                ccMesh = aims.SurfaceManip.meshExtract(mesh, labelcctex, c)[0]
                ccArea = aims.SurfaceManip.meshArea(ccMesh)
                areas_measures[label][c - 1] = ccArea
        cctex[label - 1].resize(meshVertexNb, 0)
        cctex[label - 1].assign(labelcctex[0].arraydata())

    if areas_mode:
        return cctex, areas_measures
    else:
        return cctex


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

