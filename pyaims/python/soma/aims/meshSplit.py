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
    aims.GraphManip.storeAims( graph, v.get(),'roi_mesh',  aims.rc_ptr_AimsTimeSurface_3(sub_mesh))
  return graph

