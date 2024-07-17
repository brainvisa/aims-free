
from soma import aims, aimsalgo
import numpy as np


def trim_distance_homotopic(dist, maxdist):
    '''
    Trim a distance map: removes points under distance threshold dist,
    only if they preserve the object topology
    '''
    bin_vol = aims.Volume_S16(dist.shape, [1, 1, 1])  # border 1
    bin_vol.fill(0)
    bin_vol[dist.np < 32000] = 1
    topo = aimsalgo.TopologicalClassification_S16(bin_vol)
    pts = np.where(dist.np <= maxdist)
    apts = np.array(pts[:3])
    plist = np.argsort(dist[pts])
    tc = aimsalgo.TopologicalClassificationMeaning
    cols = list(range(len(plist)))

    nmodif = 1
    iter = 0
    while nmodif != 0:
        # print('iter:', iter)
        # print('apts:', apts.shape[1])
        # print(apts)
        nmodif = 0
        iter += 1
        next_apts = apts
        plist = [cols.index(x) for x in plist if x in cols]
        cols = list(range(len(plist)))
        # print('plist:', plist)
        for i in range(len(plist)):
            pi = plist[i]
            p = apts[:, pi]
            pt = tuple(p) + (0, )
            topo.computeLocalCCNumbers(p, 1)
            c = tc.classification(topo.Cstar(), topo.Cbar())
            # print('p:', pi, p, tc.name(topo.Cstar(), topo.Cbar()))
            if c in (tc.BorderPoint, ):
                bin_vol[pt] = 0
                neigh = np.array(np.where(bin_vol[p[0]-1:p[0]+2, p[1]-1:p[1]+2,
                                                  p[2]-1:p[2]+2, 0]))
                nc = []
                for j in range(neigh.shape[1]):
                    pn = p + neigh[:, j] - 1
                    topo.computeLocalCCNumbers(pn, 1)
                    # print(pn, tc.name(topo.Cstar(), topo.Cbar()))
                    nc.append(tc.classification(topo.Cstar(), topo.Cbar()))
                bin_vol[pt] = 1
                old_nc = []
                for j in range(neigh.shape[1]):
                    pn = p + neigh[:, j] - 1
                    topo.computeLocalCCNumbers(pn, 1)
                    old_nc.append(tc.classification(topo.Cstar(), topo.Cbar()))
                immortal = False
                for c, old_c in zip(nc, old_nc):
                    if c != old_c and c not in (tc.BorderPoint, tc.CurvePoint,
                                                tc.SurfacePoint):
                        # print('point', p, 'cannot change')
                        immortal = True
                        break
                if not immortal:
                    # print('erase point', pi, ':', p)
                    bin_vol[pt] = 0
                    npts = []
                    col = cols.index(pi)
                    for xpts in pts:
                        npts.append(np.delete(xpts, col))
                    pts = tuple(npts)
                    next_apts = np.delete(next_apts, col, axis=1)
                    del cols[col]
                    # print('cols:', cols)
                    nmodif += 1
            # print('nmodifs:', nmodif)
        apts = next_apts

    bin_vol.copyHeaderFrom(dist.header())
    return bin_vol

