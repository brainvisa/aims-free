#!/usr/bin/env python

from soma import aims, aimsalgo
from soma.aimsalgo.topo_distance import trim_distance_homotopic
import numpy as np
import os.path as osp


def get_bottom_image(skeleton, graph):
    # get bottom from graph.
    bottom = aims.Volume(skeleton)
    bottom[:] = 0
    c = aims.RawConverter_BucketMap_VOID_rc_ptr_Volume_S16()
    for v in graph.vertices():
        if 'aims_bottom' in v:
            c.printToVolume(v['aims_bottom'], bottom)
    return bottom


def get_hj_image(skeleton, graph):
    # get HJ from graph. However these HJ are thick (2-3 voxels)
    # so we have to re-skeletonize them.
    # we use topological classif junction points to make them immortals
    hj = aims.Volume(skeleton)
    hj[:] = 0
    c = aims.RawConverter_BucketMap_VOID_rc_ptr_Volume_S16()
    for e in graph.edges():
        if e.getSyntax() == 'hull_junction':
            c.printToVolume(e['aims_junction'], hj)

    hj2 = aims.Volume(hj)
    # aims.write(hj2, '/tmp/hj_dmap_init.nii.gz')
    hj2[np.logical_and(skeleton.np == 80, hj2.np != 0)] = 30000
    hj2[hj2.np == 0] = 32500
    # aims.write(hj2, '/tmp/hj_dmap.nii.gz')
    hj = trim_distance_homotopic(hj2, 150)

    return hj


def get_junction_image(skeleton, graph):
    junc = aims.Volume(skeleton)
    junc[:] = 0
    c = aims.RawConverter_BucketMap_VOID_rc_ptr_Volume_S16()
    for e in graph.edges():
        if e.getSyntax() == 'junction':
            c.printToVolume(e['aims_junction'], junc)
    return junc


def get_ss_image(skeleton, graph):
    ss = aims.Volume(skeleton)
    ss[:] = 0
    c = aims.RawConverter_BucketMap_VOID_rc_ptr_Volume_S16()
    for v in graph.vertices():
        for bck in ('aims_ss', 'aims_other', 'aims_bottom'):
            if bck in v:
                c.printToVolume(v[bck], ss)
    ss[ss.np != 0] = 60  # use value 60

    return ss


def trim_extremities(skeleton, graph, tminss, junc_dilation=2):
    # start: skeleton, graph

    # Idea:
    # 1. trim bottom lines, removing ends of bottom line.
    # The removed voxels will be seeds in the last step: distance map in SS
    # 2. trim hull junction lines, the same way
    # Regular Junction lines should stay immportals (and perhaps dilated for
    # this)
    # However to remove spurious end points in botom and HJ lines (BHJ), which
    # can be interrupted at some places, we want to add additional constraints
    # on ends of lines:
    # a. BHJ ends which are too far one from the others (bottom from HJ) are
    # not really endpoints. To do this we make distance maps in SS from line
    # end points.
    # b. Regular junction lines should prevent BHJ ends in their vicinity
    # c. however we should not elagate too much: we have to keep at least 2
    # points in each connected component. This is done approximately, for now.

    max_bottom_hj_dist = 3.

    # 1. trim bottom:
    # keep bottom image
    # topo classif of bottom points
    # distance map from end points of bottom lines
    # trim distance < tmin with topology preservation
    # remaining border voxels become immortals in the later SS processing
    # the distance map and remaining steps is postponed after pruning (a, b)

    print('1. trim bottom')
    dmap_sc = 50
    tmin_sc = int(tminss * dmap_sc)
    bottom = get_bottom_image(skeleton, graph)
    # aims.write(bottom, '/tmp/bottom.nii.gz')
    tcls = aimsalgo.TopologicalClassifier_Volume_S16()
    bottom_cls = tcls.doit(bottom)
    bottom_cls[np.logical_and(bottom_cls.np != 30, bottom_cls.np != 0)] = 1
    # 1: line, 30: end points
    # aims.write(bottom_cls, '/tmp/bottom_seeds1.nii.gz')

    # 2. same for hull junction
    # hj should be skeletonized before their topo classif can be done
    # junctions should be entirely immortals
    # we also add junctions as entirely immortals in order to stabilize inner
    # holes

    print('2. trim hj')
    hj = get_hj_image(skeleton, graph)
    junc = get_junction_image(skeleton, graph)
    # aims.write(hj, '/tmp/hj.nii.gz')
    tcls = aimsalgo.TopologicalClassifier_Volume_S16()
    hj_cls = tcls.doit(hj)
    hj_cls[np.logical_and(hj_cls.np != 30, hj_cls.np != 0)] = 1
    # 1: line, 30: end points
    # aims.write(hjcls, '/tmp/hj_seeds1.nii.gz')

    ss = get_ss_image(skeleton, graph)

    print('a. filter hj and bottom')
    # dist map in SS with bottom ends as seeds
    ssseeds = aims.Volume(ss)
    ssseeds[bottom_cls.np == 30] = 100
    ssseeds[hj_cls.np != 0] = 60
    aimsalgo.AimsDistanceFrontPropagation(ssseeds, 60, 0, 3, 3, 3, dmap_sc,
                                          False)
    dhj = np.where(hj_cls.np == 30)
    # dhjn = np.array(dhj).T
    # hjs = sorted(zip(dhjn, ssseeds[dhj]), key=lambda x: x[1])
    # filter out hj points too far from bottom
    dhj_filt = ssseeds[dhj] > max_bottom_hj_dist * dmap_sc
    hj_cls[tuple(x[dhj_filt] for x in dhj)] = 1

    # now reverse to filter bottom
    ssseeds = aims.Volume(ss)
    ssseeds[hj_cls.np != 0] = 60
    ssseeds[hj_cls.np == 30] = 100
    aimsalgo.AimsDistanceFrontPropagation(ssseeds, 60, 0, 3, 3, 3, dmap_sc,
                                          False)
    db = np.where(bottom_cls.np == 30)
    db_filt = ssseeds[db] > max_bottom_hj_dist * dmap_sc
    bottom_cls[tuple(x[db_filt] for x in db)] = 1
    # aims.write(bottom_cls, '/tmp/bottom_seeds2.nii.gz')

    print('b. remove junctions')
    junc_dil_org = junc
    for i in range(junc_dilation):
        junc_dil = aims.Volume(junc_dil_org)
        junc_dil[1:, :, :, 0][junc_dil_org[:-1, :, :, 0] != 0] = 1
        junc_dil[:-1, :, :, 0][junc_dil_org[1:, :, :, 0] != 0] = 1
        junc_dil[:, 1:, :, 0][junc_dil_org[:, :-1, :, 0] != 0] = 1
        junc_dil[:, :-1, :, 0][junc_dil_org[:, 1:, :, 0] != 0] = 1
        junc_dil[:, :, 1:, 0][junc_dil_org[:, :, :-1, 0] != 0] = 1
        junc_dil[:, :, :-1, 0][junc_dil_org[:, :, 1:, 0] != 0] = 1
        junc_dil_org = junc_dil

    bottom_cls[np.logical_and(junc_dil.np != 0, bottom_cls.np != 0)] = 1
    hj_cls[np.logical_and(junc_dil.np != 0, hj_cls.np != 0)] = 1

    print('2.c. trim, continued')
    # distance map from end points of bottom lines
    aimsalgo.AimsDistanceFrontPropagation(bottom_cls, 1, 0, 3, 3, 3, dmap_sc,
                                          False)
    # un-attained points are still valid: set distance 5000 (not inf)
    bottom_cls[np.logical_and(bottom_cls.np == 32500, bottom.np != 0)] = 5000
    # aims.write(bottom_cls, '/tmp/bottom_dmap.nii.gz')
    tbottom = trim_distance_homotopic(bottom_cls, tmin_sc)
    # aims.write(tbottom, '/tmp/trimmed_bottom.nii.gz')

    # distance map from end points of HJ
    aimsalgo.AimsDistanceFrontPropagation(hj_cls, 1, 0, 3, 3, 3, dmap_sc,
                                          False)
    # un-attained points are still valid: set distance 5000 (not inf)
    hj_cls[np.logical_and(hj_cls.np == 32500, hj.np != 0)] = 5000
    # print junctions as immortals
    hj_cls[junc.np != 0] = 32000
    thj = trim_distance_homotopic(hj_cls, tmin_sc)
    # aims.write(thj, '/tmp/trimmed_hj.nii.gz')

    # 3. set the remaining voxels of bottom and hull_junctions as immortals
    # in the ss image
    # set borders (excluding immortals) of ss as seeds for distance map
    # distance map in ss
    # trim distance < tminss with topology preservation

    print('3. trim ss')
    # aims.write(ss, '/tmp/ss.nii.gz')
    ss[bottom.np != 0] = 100
    ss[hj.np != 0] = 100
    ss[tbottom.np != 0] = 60
    ss[thj.np != 0] = 60
    # aims.write(ss, '/tmp/seeds.nii.gz')
    aimsalgo.AimsDistanceFrontPropagation(ss, 60, 0, 3, 3, 3, dmap_sc, False)
    # aims.write(ss, '/tmp/skel_dist.nii.gz')
    eroded = trim_distance_homotopic(ss, tmin_sc)

    return ss, eroded


if __name__ == '__main__':

    dist = aims.Volume_FLOAT([20, 20, 20])
    dist.fill(1e31)
    dist[2, 3:8, 4] = 0
    dist[17, 3:8, 4] = 0
    dist[3, 3:8, 4] = 1
    dist[16, 3:8, 4] = 1
    dist[4, 3:8, 4] = 2
    dist[15, 3:8, 4] = 2
    dist[5, 3:8, 4] = 3
    dist[14, 3:8, 4] = 3
    dist[6, 3:8, 4] = 4
    dist[13, 3:8, 4] = 4
    dist[7, 3:8, 4] = 5
    dist[12, 3:8, 4] = 5
    dist[8, 3:8, 4] = 6
    dist[11, 3:8, 4] = 6
    dist[9, 3:8, 4] = 7
    dist[10, 3:8, 4] = 7
    dist[9, 8:16, 4] = 3
    dist[5:16, 16, 4] = 4
    dist[3, 16, 4] = 1
    dist[16, 16, 4] = 1
    dist[4, 16, 4] = 10
    dist[12, 16, 4] = 10
    dist[15, 5, 4] = 1e31
    dist[6, 8, 4] = 0
    dist[5, 9, 4] = 1
    dist[4, 10, 4] = 2
    dist[3, 11, 4] = 3
    dist[2, 12, 4] = 4

    # aims.write(dist, '/tmp/dist.nii.gz')

    maxdist = 5

    trimmed = trim_distance_homotopic(dist, maxdist)
    # aims.write(trimmed, '/tmp/trimmed.nii.gz')

    dpaths = ['/volatile/riviere/basetests-3.1.0',
              '/volatile/home/dr144257/data/baseessai']
    dpath = [p for p in dpaths if osp.exists(p)][0]
    tminss = 3.
    skeleton = aims.read(osp.join(
        dpath,
        'subjects/sujet01/t1mri/default_acquisition/default_analysis/segmentation/Lskeleton_sujet01.nii'))
    graph = aims.read(osp.join(
        dpath,
        'subjects/sujet01/t1mri/default_acquisition/default_analysis/folds/3.1/Lsujet01.arg'))
    ss, trimmed = trim_extremities(skeleton, graph, tminss, junc_dilation=2)
    aims.write(trimmed, '/tmp/skel_trimmed.nii.gz')
    aims.write(ss, '/tmp/ss_dist.nii.gz')
