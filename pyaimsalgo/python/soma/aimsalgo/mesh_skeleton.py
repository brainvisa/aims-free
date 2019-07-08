
from __future__ import print_function

from soma import aims, aimsalgo
import numpy as np
import six

inspect = () # (70989, 70988)

def mesh_skeleton(mesh, texture, curv_func=None, dist_tex=None,
                  do_timesteps=False, min_cc_size=20, min_branch_size=20,
                  debug_inspect=()):
    '''
    Process a skeleton of an object given as a binary texture.

    The current algorithm is rather simple, it erodes vertices iteratively in a
    given order until a vertex is "blocked" based on a curvature-like criterion
    funcion. The mesh vertices position and curvature are not used directly in
    the algorithm.

    Parameters
    ----------
    mesh: aims.AimsSurfaceTriangle
        triangular mesh to buils the skeleton on
    texture: aims.TimeTexture (int values)
        input object definition: binary object in a texture, all non-zero
        values are considered in the object
    curv_func: function
        "curvature" function which returns a value for each point, deciding if
        the point can be removed from the object (eroded), normally based on
        curvature or "sharpness". A negative value means that the point cannot
        be removed. The default function allows to remove a point unless it is
        a "sharp edge", connected to the object only by one triangle edge.
    dist_tex: aims.TimeTexture (float values)
        distance-like map inside the object, deciding the priority of eroded
        points. Points with the lowest values will be processed first.
        Typically if we want to build the skeleton of a thresholded curvature
        or a depth potential function (DPF), texture will be this binarized
        texture, and dist_tex will be the curvature or DPF texture itself.
    do_timesteps: bool
        if True, the output texture will have one timestep per front
        propagation iteration
    min_cc_size: int
        small connected components can be removed afterwards. Such trimming
        only happens if do_timesteps is False.
    min_branch_size: int
        small branches can be pruned afterwards. Such trimming only happens if
        do_timesteps is False.
    debug_inspect: sequence (preferably set) of ints
        list of vertices for which debug information will be printed on the
        standard output. Useful to understand what happens there.

    Returns
    -------
    skel_tex: aims.TimeTexture_S16
        output skeleton texture. Value 0 is the background, 1 is the skeleton.
        If do_timesteps is True, then one timestep per propagation step will be
        found in the texture, and value 2 will be used for the object interior
        (not belonging to the propagation front in the current step)

    '''

    if curv_func is None:
        curv_func = sharp_curve_func

    vert = mesh.vertex()
    nvert = np.asarray(vert)
    ntex = np.array(texture[0], copy=True)
    ntex[ntex!=0] = 1  # binarize ntex
    neigh = aims.SurfaceManip.surfaceNeighbours(mesh)

    out_tex = aims.TimeTexture(np.int16)
    out_tex[0].assign([0] * len(ntex))

    fg = np.where(ntex!=0)[0]
    #bg = np.where(ntex==0)[0]
    front = fg[[np.any(ntex[list(neigh[i])] == 0) for i in fg]]

    frozen = False
    p = 0
    while not frozen:
        frozen = True
        print('iter %d: front: %d' % (p, len(front)))
        # order front point by potential priority
        front = sort_potential(front, dist_tex)
        # iterate front
        new_front = []

        if do_timesteps:
            out_tex[p].assign(np.zeros(ntex.shape, dtype=np.int16))
            np.asarray(out_tex[p])[ntex!=0] = 2
            np.asarray(out_tex[p])[front] = 1

        ntex[front] = 2  # front points are part of the object
        cfront = [curv_func(nvert, ntex, neigh, v, texture) for v in front]

        for v, c in zip(front, cfront):
            #c = curv_func(nvert, ntex, neigh, v, texture)
            v2 = can_move(mesh, ntex, v, neigh, c, debug_inspect)
            if v in debug_inspect:
                print('v:', v, ', c:', c, ', v2:', v2)
            if v2 is None:
                new_front.append(v) # v still in front line
            else:
                # remove point v
                ntex[v] = 0
                v2 = [x for x in v2 if x not in front and x not in new_front]
                new_front += v2
                ntex[v2] = 2
                frozen = False
        ntex[front] = 0
        front = new_front
        if do_timesteps:
            p += 1

    print('skeleton:', len(front))
    out_tex[p].assign(np.zeros(ntex.shape, dtype=np.int16))
    np.asarray(out_tex[p])[ntex!=0] = 2
    np.asarray(out_tex[p])[front] = 1

    if not do_timesteps and (min_cc_size != 0 or min_branch_size != 0):
        out_tex = trim_skeleton(mesh, out_tex, min_cc_size=min_cc_size,
                                min_branch_size=min_branch_size)

    return out_tex


def sharp_curve_func(nvert, ntex, neigh, v, dist_tex):
    '''
    Default "curvature" function used as "curv_func" in :func:`mesh_skeleton`.

    Returns a negative value if the given vertex should not be removed
    (eroded). The current implementation freezes a vertex if it has only one
    neighbor in the object.
    '''
    n_v = np.array(list(neigh[v]))
    nval = ntex[n_v]
    active = n_v[nval!=0]
    if len(active) < 2:
        return 0
    return 1


def _same_cc(active, neigh):
    s = set([active[0]])
    todo = list(active[1:])
    while todo:
        #print('        samecc todo:', todo)
        todo_next = []
        n = len(todo)
        while todo:
            p = todo.pop(0)
            #print('test:', p)
            for nb in neigh[p]:
                if nb in s:
                    s.add(p)
                    #print('add', p)
                    break
            else:
                #print('keep', p)
                todo_next.append(p) # back in todo list
        if len(todo_next) == n:
            # no change this pass
            #print('no change')
            return False
        todo = todo_next
    return True


def can_move(mesh, ntex, v, neigh, c, debug_inspect):
    if c <= 0:
        return None
    n_v = np.array(list(neigh[v]))
    nval = ntex[n_v]
    active = n_v[nval!=0]
    n = len(active)
    if v in debug_inspect:
        print('    can_move:', v, ':', n, active, n_v)
    if n == 0:
        return None
    elif n == 1:
        return active
    else:
        if _same_cc(active, neigh) == 1:
            return active

    return None


def sort_potential(front, texture):
    '''
    Sort front points list according to texture value
    '''
    return sorted(front, key=lambda v: texture[0][v])


def trim_skeleton(mesh, skeleton, min_cc_size=20, min_branch_size=20):
    '''
    Trim a skeleton texture by removing small connected components and small
    branches.
    '''

    #print('trim_skeleton')
    trimmed = aims.TimeTexture(skeleton)
    cc = aimsalgo.AimsMeshLabelConnectedComponent(mesh, skeleton,
                                                           0, 0)
    ncc = np.asarray(cc[0])
    nskel = np.asarray(trimmed[0])
    values = np.unique(ncc)[1:]
    sizes = [len(np.where(ncc==i)[0]) for i in values]
    for i, v in enumerate(values):
        if sizes[i] < min_cc_size:
            print('remove:', v, ':', sizes[i], i)
            nskel[ncc==v] = 0

    pruned = prune_branches(mesh, trimmed, min_branch_size=min_branch_size)

    return pruned


def topo_mark(mesh, texture, neigh=None):
    '''
    Mark skeleton vertices according to their topological type:

    * 1: end point
    * 2: line point
    * 3: bifurcation
    '''

    if neigh is None:
        neigh = aims.SurfaceManip.surfaceNeighbours(mesh)
    ntex = np.asarray(texture[0])
    otex = aims.TimeTexture(texture)
    notex = np.asarray(otex[0])
    todo = np.where(ntex != 0)[0]
    for v in todo:
        n_v = np.array(list(neigh[v]))
        nval = ntex[n_v]
        active = n_v[nval!=0]
        n = len(active)
        if n <= 1:
            notex[v] = 2  # end point
        elif n > 2:
            notex[v] = 3  # bifurcation
    return otex


def prune_branches(mesh, texture, min_branch_size=20, neigh=None):
    '''
    Prune the smallest branches in a skeleton texture
    '''

    if neigh is None:
        neigh = aims.SurfaceManip.surfaceNeighbours(mesh)
    topo = topo_mark(mesh, texture, neigh)
    ntopo = np.asarray(topo[0])
    br = aims.TimeTexture(topo)
    nbr = np.asarray(br[0])
    nbr[nbr!=1] = 0
    cc = aimsalgo.AimsMeshLabelConnectedComponent(mesh, br, 0, 0)
    ncc = np.asarray(cc[0])
    br = aims.TimeTexture(topo)
    nbr = np.asarray(br[0])
    nbr[nbr!=3] = 0
    cc_bif = aimsalgo.AimsMeshLabelConnectedComponent(mesh, br, 0, 0)
    ncc_bif = np.asarray(cc_bif[0])
    ntex = np.asarray(texture[0])

    otex = aims.TimeTexture(texture)
    notex = np.asarray(otex[0])

    end_points = np.where(np.asarray(topo[0]) == 2)[0]
    branches_per_cc = {}
    for v in end_points:
        n_v = np.array(list(neigh[v]))
        nval = ntex[n_v]
        obj = n_v[nval != 0][0]
        ccobj = ncc[obj]  # (may be -1 for bifurcation)
        if ccobj == -1:
            branches_per_cc.setdefault(ncc_bif[obj], []).append((v, ccobj))
        else:
            # find bifurcation attached to this cc
            for v2 in np.where(ncc==ccobj)[0]:
                n_v2 = np.array(list(neigh[v2]))
                nval2 = ntopo[n_v2]
                obj2 = n_v2[nval2 == 3]
                if len(obj2) != 0:  # can be 0 for a line without bifurcation
                    branches_per_cc.setdefault(ncc_bif[obj2[0]], []).append(
                        (v, ccobj))
                    break

    for bif, branches in six.iteritems(branches_per_cc):
        if len(branches) >= 3:
            br_cc = [x[1] for x in branches]
            br_sz = []
            for b in br_cc:
                if b == -1:  # no cc, size 0
                    br_sz.append(0)
                else:
                    br_sz.append(len(np.where(ncc == b)[0]))
            ranks = np.argsort(br_sz)  # smallest to biggest
            for br in ranks[:-2]:  # leave at least the last 2 (biggest)
                if br_sz[br] < min_branch_size:
                    branch = branches[br]
                    #print('remove branch size', br_sz[br], 'at vertex', branch[0], bif, branches)
                    notex[branch[0]] = 0
                    if branch[1] != -1:
                        notex[ncc==branch[1]] = 0

    return otex


if __name__ == '__main__':
    from soma import aims
    from soma.aimsalgo import mesh_skeleton
    import numpy as np

    mesh = aims.read('/volatile/riviere/basetests-3.1.0/subjects/ratio_t1_dp/t1mri/default_acquisition/default_analysis/segmentation/mesh/ratio_t1_dp_Lwhite.gii')
    neigh = aims.SurfaceManip.surfaceNeighbours(mesh)
    tex = aims.read('/volatile/riviere/basetests-3.1.0/subjects/ratio_t1_dp/t1mri/default_acquisition/default_analysis/segmentation/mesh/surface_analysis/ratio_t1_dp_Lwhite_DPF.gii')
    texture = aims.TimeTexture((np.asarray(tex[0]) >= 0).astype(np.int32))
    ntex = np.asarray(texture[0])
    stex = mesh_skeleton.mesh_skeleton(mesh, texture, dist_tex=tex)
    aims.write(stex, '/tmp/stex.gii')

    bg = np.where(ntex==0)[0]
    front = np.where([np.any(ntex[list(neigh[i])]) for i in bg])[0]
