#!/usr/bin/env python2

from optparse import OptionParser
import math
import numpy as np
from soma import aims, aimsalgo

#folds_arg_filename = '/volatile/riviere/basetests-3.1.0/subjects/sujet01/t1mri/default_acquisition/default_analysis/folds/3.1/default_session_auto/Lsujet01_default_session_auto.arg'
#output_fold_graph_filename = '/tmp/cut_graph.arg'
label_attribute = 'label'
left_planes = (11., 43.)
right_planes = (6., 41.)

def cut_vertex(graph, vertex, plane_normal, plane_d):
    cut_line = aims.BucketMap_VOID()
    for bucket_name in ('aims_ss', 'aims_bottom', 'aims_other'):
        if not vertex.has_key(bucket_name):
            continue
        bucket = vertex[bucket_name]
        vs = (bucket.sizeX(), bucket.sizeY(), bucket.sizeZ())
        maxdist = math.sqrt(vs[0] * vs[0] + vs[1] * vs[1] + vs[2] * vs[2]) / 2
        for voxel in bucket[0].keys():
            point = (voxel[0] * vs[0], voxel[1] * vs[1], voxel[2] * vs[2])
            plane_pt = plane_normal.dot(point) + plane_d
            if abs(plane_pt) <= maxdist:
                cut_line[0][voxel] = 0
    if len(cut_line[0]) != 0:
        overseg = aims.FoldArgOverSegment(graph)
        new_vertex = overseg.splitVertex(vertex, cut_line)
        return new_vertex

def relabel_STAP_vertices(vertices, plane_normal, planes_d):
    for vertex in vertices:
        label = vertex[label_attribute]
        if label.endswith('_left'):
            side = 'left'
            planes_d_side = planes_d[0]
        else:
            side = 'right'
            planes_d_side = planes_d[1]
        bucket = vertex['aims_ss']
        vs = (bucket.sizeX(), bucket.sizeY(), bucket.sizeZ())
        centroid = np.average(np.asarray(bucket[0].keys()), axis=0) * vs
        plane_dot = plane_normal.dot(centroid)
        if plane_dot + planes_d_side[0] >= 0 \
                and plane_dot + planes_d_side[1] <= 0:
            vertex[label_attribute] = 'S.T.s.moy._%s' % side


parser = OptionParser('Split the STAP part of the superior temporal sulcus '
    '(S.T.s.med) in a sulcal graph (Leroy et al, PNAS 2015)')
parser.add_option('-i', '--input', dest='input',
                  help='input labelled sulcal graph')
parser.add_option('-o', '--output', dest='output',
                  help='output labelled sulcal graph')
parser.add_option('-l', '--label-attr', dest='label_attr',
                  help='label attribute, default: guessed grom graph, or %s'
                  % label_attribute)
parser.add_option('--la', dest='left_ant', type='float',
                  help='left anterior Y limit of the STAP (Talairach coord), '
                  'default: %f' % left_planes[0],
                  default=left_planes[0])
parser.add_option('--lp', dest='left_post', type='float',
                  help='left posterior Y limit of the STAP (Talairach coord), '
                  'default: %f' % left_planes[1],
                  default=left_planes[1])
parser.add_option('--ra', dest='right_ant', type='float',
                  help='right anterior Y limit of the STAP (Talairach coord), '
                  'default: %f' % right_planes[0],
                  default=right_planes[0])
parser.add_option('--rp', dest='right_post', type='float',
                  help='right posterior Y limit of the STAP '
                  '(Talairach coord), default: %f' % right_planes[1],
                  default=right_planes[1])

options, args = parser.parse_args()

folds_arg_filename = options.input
output_fold_graph_filename = options.output
left_planes = options.left_ant, options.left_post
right_planes = options.right_ant, options.right_post

graph = aims.read(folds_arg_filename)

if options.label_attr:
    label_attribute = options.label_attr
elif graph.has_key('label_property'):
    label_attribute = graph['label_property']
print 'label attribute:', label_attribute
print 'planes:', left_planes, right_planes

talairach_trans = aims.GraphManip.talairach(graph)
inv_talairach = talairach_trans.inverse()

transformed_normal = inv_talairach.transform((0, 1, 0)) \
    - inv_talairach.transform((0, 0, 0))
transformed_normal.normalize()
transformed_left_points = (inv_talairach.transform((0, left_planes[0], 0)),
                           inv_talairach.transform((0, left_planes[1], 0)))
transformed_right_points = (inv_talairach.transform((0, right_planes[0], 0)),
                            inv_talairach.transform((0, right_planes[1], 0)))
transformed_left_d = (-transformed_left_points[0].dot(transformed_normal),
                      -transformed_left_points[1].dot(transformed_normal))
transformed_right_d = (-transformed_right_points[0].dot(transformed_normal),
                      -transformed_right_points[1].dot(transformed_normal))

left_cuts = []
right_cuts = []
for vertex in graph.vertices():
    if not vertex.has_key(label_attribute):
        continue
    label = vertex[label_attribute]
    if label == 'S.T.s._left':
        left_cuts.append(vertex)
    elif label == 'S.T.s._right':
        right_cuts.append(vertex)

cuts = left_cuts + right_cuts
for vertex in left_cuts:
    new_vertex = cut_vertex(graph, vertex, transformed_normal,
                            transformed_left_d[0])
    new_vertex2 = cut_vertex(graph, vertex, transformed_normal,
                             transformed_left_d[1])
    if new_vertex is not None:
        cuts.append(new_vertex)
        new_vertex = cut_vertex(graph, new_vertex, transformed_normal,
                                transformed_left_d[1])
        if new_vertex is not None:
            cuts.append(new_vertex)
    if new_vertex2 is not None:
        cuts.append(new_vertex2)

for vertex in right_cuts:
    new_vertex = cut_vertex(graph, vertex, transformed_normal,
                            transformed_right_d[0])
    new_vertex2 = cut_vertex(graph, vertex, transformed_normal,
                             transformed_right_d[1])
    if new_vertex is not None:
        cuts.append(new_vertex)
        new_vertex = cut_vertex(graph, new_vertex, transformed_normal,
                                transformed_right_d[1])
        if new_vertex is not None:
            cuts.append(new_vertex)
    if new_vertex2 is not None:
        cuts.append(new_vertex2)

relabel_STAP_vertices(cuts, transformed_normal,
                      (transformed_left_d, transformed_right_d))

aims.write(graph, output_fold_graph_filename)


