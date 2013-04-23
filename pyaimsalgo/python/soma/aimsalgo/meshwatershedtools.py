#!/usr/bin/env python
# -*- coding: utf-8 -*-
from soma import aims, aimsalgo
import numpy as N


def _clean_after_merge( labels, parents, valid ):
    """
    remove the non-valid compnents and reorder the ROI list
    """
    # taken from nipy
    k = N.size(valid)
    if k != 0:
        ## remove invalid null components
        for j in range( k ):
            if not valid[ parents[j] ]:
                parents[ j ] = j

        iconvert = N.nonzero( valid )
        iconvert = N.reshape( iconvert, N.size(iconvert) )
        convert = -N.ones( k ).astype('i')
        aux = ( N.cumsum( valid.astype('i') ) - 1 ).astype('i')
        convert[ valid ] = aux[ valid ]
        k = N.size( iconvert )

        q = N.nonzero( labels > -1 )
        q = N.reshape( q, N.size(q) )
        labels[q] = convert[ labels[q] ]
        parents = convert[ parents[iconvert] ]
        parents = parents[:k]

        return labels, parents


def _merge_ascending( labels, parents, valid ):
    """
    _merge_ascending(valid)
    Remove the non-valid items by including them in
    their parents when it exists
    """
    # taken from nipy
    labels = N.copy( labels )
    parents = N.copy( parents )
    for j in xrange( len( valid ) ):
        if valid[j] == 0:
            fj =  parents[j]
            if fj != j:
                parents[ parents==j ] = fj
                labels[ labels==j ] = fj
            else:
                valid[j] = 1

    labels, parents = _clean_after_merge( labels, parents, valid )

    return labels, parents


def watershedWithBasinsMerging(
        tex, mesh, size_min=0, depth_min=0, tex_threshold=0.01, mode = "and" ):
    """
    inputs:
            tex : texture of boundaries between regions
            mesh : associated mesh
            k_size : number of basins > size_min (rough)
            k_depth : number of basins > depth_min (rough)
            tex_threshold : threshold on the input intensity texture
            mode = "and": merge basins with its parent if size < k_size and depth < k_depth
                  "or": merge basins with its parent if size < k_size or depth < k_depth
    output:
            parcellated texture : watershed results according to the thresholds indicated by k_size and k_depth
    """

    data = tex[0].arraydata()

    ## Watershed:
    idxWt, depthWt, majorWt, labelWt = aimsalgo.meshWatershed( mesh, tex,
        tex_threshold )
    #   idxWt: array of shape ( nbasins ) indices of the vertices that are local maxima
    # depthWt: array of shape (nbasins) topological the depth of the basins
    #          depth[ idx[i] ] = q means that idx[i] is a q-order maximum
    #          This is also the diameter of the basins associated with local maxima
    # majorWt: array of shape ( nbasins )
    #          label of the maximum which dominates each local maximum
    #          i.e. it describes the hierarchy of the local maxima
    # labelWt: array of shape ( number of vertices )
    #          labelling of the vertices according to their basin

    idxW = idxWt[0].data().arraydata()
    depthW = depthWt[0].data().arraydata()
    majorW = majorWt[0].data().arraydata()
    labelW = labelWt[0].data().arraydata()
    kW=idxW.size

    if size_min != 0 or depth_min != 0:
        size = N.asarray( [ len( N.where( labelW == x )[0] ) \
            for x in xrange( kW ) ] )

        # Blobs
        blobindices = aimsalgo.blobsHeights( mesh, tex[0].data(),
            labelWt[0].data() )
        blobheights = data[ blobindices.arraydata() ]
        blobdepths = data[ idxW ] - blobheights

        #Criteria to regroup basins
        valid_size = size > size_min
        valid_depth = blobdepths > depth_min
        if mode == "or":
            valid = valid_size * valid_depth
        elif mode == "and":
            valid = valid_size + valid_depth
        else:
            raise ValueError( 'invalid mode' )
        labelW, majorW = _merge_ascending( labelW, majorW, valid )

    output_tex = aims.TimeTexture_S16()
    output_text = output_tex[0]
    output_text.assign( labelW + 1 )

    label_list = N.unique( labelW + 1 )
    print 'Final number of labels: ', len( label_list )
    return output_tex


