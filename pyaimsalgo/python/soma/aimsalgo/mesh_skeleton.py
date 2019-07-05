
from __future__ import print_function

from soma import aims
import numpy as np

inspect = (70989, 70988)

def mesh_skeleton(mesh, texture, curv_func=None):

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
        front = sort_potential(front)
        # iterate front
        new_front = []

        out_tex[p].assign(np.zeros(ntex.shape, dtype=np.int16))
        np.asarray(out_tex[p])[ntex!=0] = 2
        np.asarray(out_tex[p])[front] = 1

        ntex[front] = 2  # front points are part of the object

        for v in front:
            c = curv_func(nvert, ntex, neigh, v)
            v2 = can_move(mesh, ntex, v, neigh, c)
            if v in inspect:
                print('v:', v, ', c:', c, ', v2:', v2)
            if v2 is None:
                new_front.append(v) # v still in front line
            else:
                #remove_point(mesh, ntex, v)
                ntex[v] = 0
                v2 = [x for x in v2 if x not in front and x not in new_front]
                new_front += v2
                ntex[v2] = 2
                frozen = False
        ntex[front] = 0
        front = new_front
        p += 1

    print('skeleton:', len(front))
    #print(front)
    #notex = np.asarray(out_tex[p])
    #notex[front] = 1
    out_tex[p].assign(np.zeros(ntex.shape, dtype=np.int16))
    np.asarray(out_tex[p])[ntex!=0] = 2
    np.asarray(out_tex[p])[front] = 1

    return out_tex


def sharp_curve_func(nvert, ntex, neigh, v):
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


#def is_changing_topology(mesh, ntex, v, neigh):
def can_move(mesh, ntex, v, neigh, c):
    if c <= 0:
        return None
    n_v = np.array(list(neigh[v]))
    nval = ntex[n_v]
    active = n_v[nval!=0]
    n = len(active)
    if v in inspect:
        print('    can_move:', v, ':', n, active, n_v)
    if n == 0:
        return None
    elif n == 1:
        return active
    else:
        if _same_cc(active, neigh) == 1:
            return active

    return None


#def remove_point(mesh, ntex, v):
    #ntex[v] = 0

def sort_potential(front):
    return front


if __name__ == '__main__':
    from soma import aims
    from soma.aimsalgo import mesh_skeleton
    import numpy as np

    mesh = aims.read('/volatile/riviere/basetests-3.1.0/subjects/ratio_t1_dp/t1mri/default_acquisition/default_analysis/segmentation/mesh/ratio_t1_dp_Lwhite.gii')
    neigh = aims.SurfaceManip.surfaceNeighbours(mesh)
    tex = aims.read('/volatile/riviere/basetests-3.1.0/subjects/ratio_t1_dp/t1mri/default_acquisition/default_analysis/segmentation/mesh/surface_analysis/ratio_t1_dp_Lwhite_DPF.gii')
    texture = aims.TimeTexture((np.asarray(tex[0]) >= 0).astype(np.int32))
    ntex = np.asarray(texture[0])
    bg = np.where(ntex==0)[0]
    stex = mesh_skeleton.mesh_skeleton(mesh, texture)
    aims.write(stex, '/tmp/stex.gii')
    front = np.where([np.any(ntex[list(neigh[i])]) for i in bg])[0]
