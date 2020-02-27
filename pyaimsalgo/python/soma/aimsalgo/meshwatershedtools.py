#! /usr/bin/env python
############################################################################
# This software and supporting documentation are distributed by
# CEA/NeuroSpin, Batiment 145, 91191 Gif-sur-Yvette cedex, France.
# This software is governed by the CeCILL license version 2 under
# French law and abiding by the rules of distribution of free software.
# You can  use, modify and/or redistribute the software under the
# terms of the CeCILL license version 2 as circulated by CEA, CNRS
# and INRIA at the following URL "http://www.cecill.info".
############################################################################

"""
This modles features the following:

* generate a reduced profile by basins using the watershed algorithm
* merge the items if necessary (measured criteria validating the basins)

Main dependencies: PyAims library
"""


#----------------------------Imports-------------------------------------------


# import system
from __future__ import absolute_import
from __future__ import print_function
import numpy
import sys

# soma import
from soma import aims, aimsalgo
from six.moves import range

#----------------------------Functions-----------------------------------------


def _clean_after_merge(labels, parents, valid):
    """Remove the non-valid components and reorder the ROI list.append

    Parameters
    ----------
    labels:
        labels of basins of watershed (after merge)
    parents:
        parent local according to local maximum (after merge)
    valid:
        measured criteria validating the basins

    Returns
    -------
    labels:
        clean labels of basins of watershed
    parents:
        clean parent local according to local maximum
    """
    k = numpy.size(valid)
    if k != 0:
        ## remove invalid null components
        for j in range(k):
            if not valid[parents[j]]:
                parents[j] = j

        iconvert = numpy.nonzero(valid)
        iconvert = numpy.reshape(iconvert, numpy.size(iconvert))
        convert = -numpy.ones(k).astype('i')
        aux = (numpy.cumsum(valid.astype('i')) - 1).astype('i')
        convert[valid] = aux[valid]
        k = numpy.size(iconvert)

        q = numpy.nonzero(labels > -1)
        q = numpy.reshape(q, numpy.size(q))
        labels[q] = convert[labels[q]]
        parents = convert[parents[iconvert]]
        parents = parents[:k]

        return labels, parents


def _merge_ascending(labels, parents, valid):
    """Remove the non-valid items by including them in their parents when
    it exists.

    Parameters
    ----------
    labels:
        labels of basins of watershed
    parents:
        parent local according to local maximum
    valid:
        measured criteria validating the basins

    Returns
    -------
    labels:
        clean labels of basins of watershed
    parents:
        clean parents according to local maximum
    """
    labels = numpy.copy(labels)
    parents = numpy.copy(parents)
    for j in range(len(valid)):
        if valid[j] == 0:
            fj = parents[j]
            if fj != j:
                parents[parents == j] = fj
                labels[labels == j] = fj
            else:
                valid[j] = 1

    labels, parents = _clean_after_merge(labels, parents, valid)

    return labels, parents


def watershedWithBasinsMerging(
        tex, mesh, size_min=0, depth_min=0, tex_threshold=0.01, mode="and"):
    """Generate a texture of merged basins: watershed texture.

    The basins are merged according to two criteria:
        (1) the size of basins
        (2) the depth of basins

    Parameters
    ----------
    tex: (TimeTexture_S16)
        texture of boundaries between regions
    mesh: (aimsTimeSurface_3)
        associated mesh
    size_min: (int)
        number of basins > size_min
    depth_min: (float)
        number of basins > depth_min
    tex_threshold: (float)
        threshold on the input intensity texture
    mode: (str)
        two cases:
            (1) and --> merge basins with its parent
                        if size < k_size and depth < k_depth
            (2) or --> merge basins with its parent
                       if size < k_size or depth < k_depth
    Returns
    -------
    output_tex: (TimeTexture_S16)
        watershed results according to the thresholds
        indicated by k_size and k_depth
    """
    ## Watershed:
    idxWt, depthWt, majorWt, labelWt = aimsalgo.meshWatershed(
        mesh, tex, tex_threshold)
    # idxWt  : array of shape (nbasins) indices of the vertices that are
    #          local maxima
    # depthWt: array of shape (nbasins) topological the depth of the basins
    #          depth[idx[i]] = q means that idx[i] is a q-order maximum
    #          This is also the diameter of the basins associated
    #          with local maxima
    # majorWt: array of shape (nbasins)
    #          label of the maximum which dominates each local maximum
    #          i.e. it describes the hierarchy of the local maxima
    # labelWt: array of shape (number of vertices)
    #          labelling of the vertices according to their basin

    data = tex[0].arraydata()
    idxW = idxWt[0].data().arraydata()
    majorW = majorWt[0].data().arraydata()
    labelW = labelWt[0].data().arraydata()

    if size_min != 0 or depth_min != 0:
        size = numpy.asarray([len(numpy.where(labelW == x)[0])
                             for x in range(idxW.size)])
        print("criteria size: ", size)

        # Blobs
        blobindices = aimsalgo.blobsHeights(
            mesh, tex[0].data(), labelWt[0].data())
        blobheights = data[blobindices.arraydata()]
        blobdepths = data[idxW] - blobheights
        print("criteria depth: ", blobdepths)

        # criteria to regroup basins
        valid_size = size > size_min
        valid_depth = blobdepths > depth_min
        if mode == "or":
            valid = valid_size * valid_depth
        elif mode == "and":
            valid = valid_size + valid_depth
        else:
            raise ValueError('invalid mode')

        # Remove the non-valid items by including them in their parents when
        # it exists.
        labelW, majorW = _merge_ascending(labelW, majorW, valid)

    # create the final texture with the items valid
    output_tex = aims.TimeTexture_S16()
    output_text = output_tex[0]
    output_text.assign(labelW + 1)

    return output_tex
