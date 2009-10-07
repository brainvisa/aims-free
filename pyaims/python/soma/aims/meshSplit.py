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

def meshSplit(mesh,tex,graph):
  print "type(mesh) : ",type(mesh),", type(tex) : ",type(tex)
  labels = numpy.unique(tex[0].arraydata())
  labels = labels.tolist()
  labels.remove(0)
  num_labels = len(labels)
  tex2 = aims.TimeTexture_S16()
  tx = tex[0]
  tx2 = tex2[0]
  tx2.reserve( tx.size() )
  for i in tx:
    tx2.push_back( round(i * num_labels) )
  for label in labels:
    v = graph.addVertex( 'roi' )
    label_int = round(label*num_labels)
    v['name']=str(label_int)
    sub_mesh = aims.SurfaceManip.meshExtract( mesh, tex2, label_int )
    aims.GraphManip.storeAims( graph, v.get(),'roi_mesh',  aims.rc_ptr_AimsTimeSurface_3(sub_mesh[0]))
  return graph

def meshSplit2(mesh, tex, graph, voxel_size):
  """
  inputs:
            mesh : cortex mesh for example
            tex: texture of labels (parcellation of the mesh, labels between 1 and nb_labels, background = 0), type : aims.TimeTexture_S16
            graph: soma.aims.Graph, __syntax__:'roi'
  outputs:
            void: modify the input graph: add vertex : submeshes (one per texture label) and associated buckets
                                          add vertex "others" : void
  """
  labels = numpy.unique(tex[0].arraydata())
  print labels
  labels = labels.tolist()
  if labels.count(0) > 1:
    labels.remove(0)
  for i in xrange(int(voxel_size.size())):
    if voxel_size[i]== 0:
      print "voxel_size of ", i, " egal zero, substituted by 1"
      voxel_size[i] = 1
  num_labels = len(labels)
  for label in labels:
    v = graph.addVertex( 'roi' )
    v['name']=str(label)
    v['roi_label'] = label
    sub_mesh = aims.SurfaceManip.meshExtract( mesh, tex, label )
    aims.GraphManip.storeAims( graph, v.get(),'roi_mesh',  aims.rc_ptr_AimsTimeSurface_3(sub_mesh[0]) )
    #associated bucket creation:
    bucketMap = aims.BucketMap_VOID()
    bucket = bucketMap[0]
    sub_mesh_vertex = sub_mesh[0].vertex()
    countVertex = 0
    for vertex in sub_mesh_vertex:
      px = round(vertex[0]/voxel_size[0])
      py = round(vertex[1]/voxel_size[1])
      pz = round(vertex[2]/voxel_size[2])
      p3d = aims.Point3d(px, py, pz)
      bucket[p3d] = 1
      countVertex+=1
    if int(voxel_size.size())>=3:
      bucketMap.setSizeXYZT(voxel_size[0],voxel_size[1], voxel_size[2],1)
    else:
      bucketMap.setSizeXYZT(1,1,1,1)
    aims.GraphManip.storeAims( graph, v.get(),'aims_roi', aims.rc_ptr_BucketMap_VOID(bucketMap) )