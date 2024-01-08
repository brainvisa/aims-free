# -*- coding: utf-8 -*-
#  This software and supporting documentation are distributed by
#      Institut Federatif de Recherche 49
#      CEA/NeuroSpin, Batiment 145,
#      91191 Gif-sur-Yvette cedex
#      France
#
# This software is governed by the CeCILL-B license under
# French law and abiding by the rules of distribution of free software.
# You can  use, modify and/or redistribute the software under the
# terms of the CeCILL-B license as circulated by CEA, CNRS
# and INRIA at the following URL "http://www.cecill.info".
#
# As a counterpart to the access to the source code and  rights to copy,
# modify and redistribute granted by the license, users are provided only
# with a limited warranty  and the software's author,  the holder of the
# economic rights,  and the successive licensors  have only  limited
# liability.
#
# In this respect, the user's attention is drawn to the risks associated
# with loading,  using,  modifying and/or developing or reproducing the
# software by the user in light of its specific status of free software,
# that may mean  that it is complicated to manipulate,  and  that  also
# therefore means  that it is reserved for developers  and  experienced
# professionals having in-depth computer knowledge. Users are therefore
# encouraged to load and test the software's suitability as regards their
# requirements in conditions enabling the security of their systems and/or
# data to be ensured and,  more generally, to use and operate it in the
# same conditions as regards security.
#
# The fact that you are presently reading this means that you have had
# knowledge of the CeCILL-B license and that you accept its terms.

import numpy
from soma import aims


def meshSplit(mesh, tex, graph, tex_time_step=0):
    '''Splits a mesh into patches corresponding to a labels texture. Patches are
    organized into a graph.

    The graph must preexist, and nodes will be inserted into it.
    '''
    labels = numpy.unique(tex[tex_time_step])
    labels = labels.tolist()
    labels.remove(0)
    num_labels = len(labels)
    tex2 = aims.TimeTexture_S16()
    tx = tex[tex_time_step]
    tx2 = tex2[0]
    tx2.reserve(tx.size())
    for i in tx:
        tx2.push_back(round(i * num_labels))
    for label in labels:
        v = graph.addVertex('roi')
        label_int = round(label * num_labels)
        v['name'] = str(label_int)
        sub_mesh = aims.SurfaceManip.meshExtract(mesh, tex2, label_int)
        aims.GraphManip.storeAims(
            graph, v._get(), 'roi_mesh', aims.rc_ptr_AimsTimeSurface_3_VOID(sub_mesh[0]))
    return graph


def meshSplit2(mesh, tex, graph, voxel_size=None, tex_time_step=None):
    """ Split mesh according to texture patches

    Compared to meshSplit, this version also adds buckets (voxels lists) in each graph node.

    Parameters
    ----------
    mesh: cortex mesh for example
    tex: aims.TimeTexture_S16
        texture of labels (parcellation of the mesh, labels between 1 and nb_labels, background = 0)
    graph: Graph
        the graph __syntax__ attribute should be: 'roi'
    voxel_size: (optional)
        if a voxel size is given, a bucket will be built with the specified
        voxel size to follow the mesh. Otherwise there will be no bucket.
    tex_time_step: int (optional)
        time step to be used in the texture for regions split. default: 0

    Outputs
    -------
    None:
        modify the input graph: add vertex : submeshes (one per texture
        label) and associated buckets add vertex "others" : void
    """

    # filter texture time step
    if tex_time_step is not None and tex.size() != 0:
        tex_tstep = aims.TimeTexture_S16()
        tex_tstep[0] = tex[tex_time_step]
    else:
        tex_tstep = tex
    # labels in the texture
    labels = (numpy.unique(tex_tstep[0].arraydata())).tolist()
    #if 0 in labels:
        #labels.remove(0)

    # 3D resolution: replace 0 by 1
    if voxel_size is not None:
        voxel_size = [1. if x == 0 else x for x in voxel_size]
        while len(voxel_size) < 3:
            voxel_size.append(1.)

    # for each label, roi_mesh and aims_roi are added in graph

    for label in labels:
        # (1) roi_mesh
        v = graph.addVertex('roi')
        v['name'] = str(label)
        v['roi_label'] = label

        # mesh corresponding to texture of label
        sub_mesh = aims.SurfaceManip.meshExtract(mesh, tex_tstep, label)
        aims.GraphManip.storeAims(graph, v._get(), 'roi_mesh',
                                  aims.rc_ptr_AimsTimeSurface_3_VOID(sub_mesh[0]))

        # (2) aims_roi
        if voxel_size is not None:
            bucketMap = aims.BucketMap_VOID()
            sub_mesh_vertex = sub_mesh[0].vertex()

            for vertex in sub_mesh_vertex:
                px = round(vertex[0] / voxel_size[0])
                py = round(vertex[1] / voxel_size[1])
                pz = round(vertex[2] / voxel_size[2])
                p3d = aims.Point3d(px, py, pz)
                bucketMap[0][p3d] = 1

            if len(voxel_size) >= 3:
                bucketMap.setSizeXYZT(voxel_size[0], voxel_size[1],
                                      voxel_size[2], 1)
            else:
                bucketMap.setSizeXYZT(1, 1, 1, 1)
            aims.GraphManip.storeAims(graph, v._get(), 'aims_roi',
                                      aims.rc_ptr_BucketMap_VOID(bucketMap))
