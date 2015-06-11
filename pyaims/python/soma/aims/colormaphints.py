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

from soma import aims
import numpy
import types
try:
    from soma import aimsalgo
except:
    aimsalgo = None


def checkVolume(vol):
    '''Checks colormap-related clues in a volume, and tries to determine whether it is an anatomical volume, a diffusion volume, a functional volume, or a labels volume. This is determined as "likelihoods" for each class (based on a pure empirical heurisrtic), based on, mainly, the histogram, voxel type, and voxel sizes.
    '''
    def _histogram(vol, bins):
        histo = getattr(aims, 'RegularBinnedHistogram_' + dtype)
        if not type(bins) is types.IntType:
            m, M = bins[0], bins[-1]
            bins = len(bins) - 1
            hg = histo(bins)
            hg.doit(vol, m, M)
        else:
            hg = histo(bins)
            hg.doit(vol)
        d = hg.data()
        h = numpy.array(d.volume(), copy=False).reshape(d.dimX())
        step = float(hg.maxDataValue() - hg.minDataValue()) / hg.bins()
        his = (h, numpy.arange(hg.minDataValue(), hg.maxDataValue() + step,
                               step))
        return his

    def _unique(vol):
        hg = getattr(aims, 'RegularBinnedHistogram_' + dtype)()
        return hg.unique(vol).arraydata()

    if aimsalgo is not None:
        # aims histogram is 15 times faster than numpy...
        histogram = _histogram
        # and unique is only 5 times faster than numpy
        unique = _unique
    else:
        histogram = numpy.histogram
        unique = lambda vol: numpy.unique(numpy.array(vol, copy=False))

    hints = {}
    # check data type
    tname = vol.__class__.__name__
    if tname.startswith('Volume_'):
        dtype = tname[7:]
    elif tname.startswith('AimsData_'):
        dtype = tname[9:]
        vol = vol.volume()
    else:
        raise ValueError('input parameter should be a volume')
    hints['data_type'] = dtype
    if dtype not in ('U8', 'S8', 'U16', 'S16', 'U32', 'S32', 'U64', 'S64',
                     'FLOAT', 'DOUBLE'):
        # not a scalar, we cannot perform scalar things, and the volume will have
        # no colormap, so we are done
        return hints
    u = unique(vol)
    maxv = u[-1]
    minv = u[0]
    if len(u) == 2:
        hints['binary_volume'] = True
    else:
        hints['binary_volume'] = False
    if dtype[0] == 'S' or dtype[0] == 'U' and dtype[1:] in ('8', '16', '32',
                                                            '64'):
        hints['int_type'] = True
    else:
        if len(numpy.where(u.astype('int32') == u)) != len(u):
            hints['int_type'] = False
            hints['labels_likelihood'] = 0.
        else:
            hints['int_type'] = True
    if hints['int_type']:
        labell = maxv / 250. - 1.
        labell = numpy.exp(-labell) / (numpy.exp(labell) + numpy.exp(-labell))
        hints['labels_likelihood'] = labell
    if minv < 0:
        hints['negative_values'] = True
    else:
        hints['negative_values'] = False

    # determine background color (most frequent value)
    his = histogram(vol, min(len(u), 200))
    maxhis = numpy.argmax(his[0])
    subval = numpy.where((u < his[1][maxhis + 1]).__and__(
                         u >= his[1][maxhis]))[0]
    if len(subval) == 0:
        subval = [0]  # FIXME: take the next lower used bin
    if len(subval) == 1:
        hints['background'] = u[subval[0]]
    else:
        # more precise histogram
        bins = numpy.hstack((u[subval], u[subval[-1] + 1]))
        h = histogram(vol, bins)
        hints['background'] = h[1][numpy.argmax(h[0])]
        maxhis = numpy.where(his[1] <= hints['background'])[0][-1]

    # determine cmap bounds
    # "positive" values
    chis = numpy.cumsum(his[0][maxhis:])
    chis = chis / float(chis[-1])  # proportional cummulated histo
    maxcmap = numpy.where(chis < 0.99)[0]
    if len(maxcmap) >= 1:
        maxcmap = maxcmap[-1]
        if chis[maxcmap] < 0.98:
            maxcmap += 1  # add the next bin if it is too big
    else:
        maxcmap = len(chis)
    if maxcmap < len(chis):
        hints['histo_99percent_positive'] = his[1][maxhis + maxcmap]
    else:
        hints['histo_99percent_positive'] = u[-1]
    # correct the labels likelihood
    hints['labels_likelihood'] *= float(maxcmap) / len(chis)
    if hints['binary_volume']:
        hints['labels_likelihood'] = 1.
    # "negative" values
    chis = numpy.cumsum(his[0][:maxhis + 2][-1:0:-1])
    chis = chis / float(chis[-1])  # proportional cummulated histo
    maxcmap = numpy.where(chis < 0.99)[0]
    if len(maxcmap) >= 1:
        maxcmap = maxcmap[-1]
        if chis[maxcmap] < 0.98:
            maxcmap += 1  # add the next bin if it is too big
    else:
        maxcmap = len(chis)
    if maxcmap < len(chis):
        hints['histo_99percent_negative'] = his[1][maxhis - maxcmap]
    else:
        hints['histo_99percent_negative'] = u[0]
    hdr = vol.header()
    dims = [vol.getSizeX(), vol.getSizeY(), vol.getSizeZ(), vol.getSizeT()]
    vs = hdr['voxel_size']
    # determine if it could be an anatomical / functional volume
    vsmin = numpy.min(vs[:3])
    vsmax = numpy.max(vs[:3])

    def anatlikelihoodcurve(vsize):
        if vsize < 1.:
            return 1.
        elif vsize > 4.:
            return 0.2
        else:
            return max(1. - (vsize - 1.) / 3., 0.2)

    def difflikelihoodcurve(vsize):
        if vsize < 1.:
            return 0.2
        elif vsize > 6.:
            return 0.1
        elif vsize < 2.:
            return max(vsize - 1., 0.2)
        else:
            return max(1. - (vsize - 2.) / 4., 0.1)

    def funclikelihoodcurve(vsize):
        if vsize < 1.:
            return 0.1
        elif vsize > 3.:
            return 1.
        else:
            return max((vsize - 1) / 2., 0.1)

    def normalize(vec):
        return vec / numpy.sqrt((vec ** 2).sum())
    likes = normalize(numpy.array([anatlikelihoodcurve(vsmin),
                                   difflikelihoodcurve(vsmin), funclikelihoodcurve(vsmin), 0.]))
    likes *= [anatlikelihoodcurve(vsmax),
              difflikelihoodcurve(vsmax), funclikelihoodcurve(vsmax), 0.]
    likes = normalize(likes)
    likes[3] = hints['labels_likelihood']
    if not hints['int_type']:
        if hints['negative_values']:
            likes *= [0.2, 0.3, 1., 0.]
        else:
            if dims[3] > 1:
                likes *= [0.1, 0.5, 2., 0.]
            else:
                likes *= [0.3, 0.3, 2., 0.]
    else:
        ll = 1. - hints['labels_likelihood']
        if hints['negative_values']:
            if dims[3] > 1:
                likes *= [0.2 * ll, 0.5 * ll, ll, 1.]
            else:
                likes *= [0.5 * ll, 0.2 * ll, ll, 1.]
        else:
            likes *= [ll, ll, ll, 1.]
    likes = normalize(likes)
    hints['volume_contents_likelihoods'] = list(likes)
    hints['anatomical_likelihood'] = likes[0]
    hints['diffusion_likelihood'] = likes[1]
    hints['functional_likelihood'] = likes[2]
    hints['labels_likelihood'] = likes[3]

    # get specified palette information if any

    try:
        pal = hdr['palette']
        p = pal['palette']
        hints['palette'] = p
    except KeyError:
        pass

    return hints

anatomicalColormaps = [('B-W LINEAR', (1., 1., 1.)),
                       ('Blue-White', (0., 0., 1.)),
                       ('Green-White-linear', (0., 1., 0.)),
                       ('Green-White-exponential', (0., 1., 0.)),
                       ]
'''predefined list of colormaps suitable for anatomical volumes'''

anatomicalFusionColormaps = [('B-W LINEAR-fusion', (1., 1., 1.)),
                             ('Blue-White-fusion', (0., 0., 1.)),
                             ('Green-White-linear-fusion', (0., 1., 0.)),
                             ]
'''predefined list of colormaps suitable for fusionned anatomical volumes'''

diffusionColormaps = anatomicalColormaps
'''predefined list of colormaps suitable for diffusion volumes'''

diffusionFusionColormaps = anatomicalFusionColormaps
'''predefined list of colormaps suitable for fusionned diffusion volumes'''

functionalColormaps = [('RED TEMPERATURE', (1., 0.5, 0.)),
                       ('RAINBOW', (1., 0., 0.)),
                       ('Blue-Red', (1., 0., 0.)),
                       ('actif-ret', (1., 1., 0.)),
                       ('Yellow-red', (1., 1., 0.)),
                       ]
'''predefined list of colormaps suitable for functional volumes'''

functionalFusionColormaps = [('Rainbow1-fusion', (1., 0., 0.)),
                             ('Blue-Red-fusion', (1., 0., 0.)),
                             ('Yellow-red-fusion', (1., 1., 0.)),
                             ]
'''predefined list of colormaps suitable for fusionned functional volumes'''

twotailColormaps = [('tvalues100-200-100-lfusion', (1., 0., 0.)),
                    ('tvalues100-100-100-lfusion', (1., 0., 0.)),
                    ]
'''predefined list of colormaps suitable for two-tail T-values volumes'''

twotailFusionColormaps = [('tvalues100-200-100', (1., 0., 0.)),
                          ('tvalues100-100-100', (1., 0., 0.)),
                          ]
'''predefined list of colormaps suitable for fusionned two-tail T-values
volumes'''

labelsColormaps = [('Blue-Red', (1., 0., 0.)),
                   ('Talairach', (0., .0, .0, )),
                   ]
'''predefined list of colormaps suitable for labels volumes'''

labelsFusionColormaps = [
]
'''predefined list of colormaps suitable for fusionned labels volumes'''

binaryColormaps = [('BLUE-lfusion', (0., 0., 1.)),
                   ('GREEN-lfusion', (0., 1., 0.)),
                   ('RED-lfusion', (1., 0., 0.)),
                   ('CYAN-lfusion', (0., 1., 1.)),
                   ('VIOLET-lfusion', (1., 0., 1.)),
                   ('YELLOW-lfusion', (1., 1., 0.)),
                   ('WHITE-lfusion', (1., 1., 1.)),
                   ]
'''predefined list of colormaps suitable for binary volumes'''

binaryFusionColormaps = [('BLUE-ufusion', (0., 0., 1.)),
                         ('GREEN-ufusion', (0., 1., 0.)),
                         ('RED-ufusion', (1., 0., 0.)),
                         ('CYAN-ufusion', (0., 1., 1.)),
                         ('VIOLET-ufusion', (1., 0., 1.)),
                         ('YELLOW-ufusion', (1., 1., 0.)),
                         ('Black-ufusion', (1., 1., 1.)),
                         ]
'''predefined list of colormaps suitable for fusionned binary volumes'''


def chooseColormaps(vols):
    '''Automatically chooses distinc colormaps for a list of volumes

    - returns: a list of colormaps names. They should be known from Anatomist.
    '''
    cmapbytype = [anatomicalColormaps, diffusionColormaps, functionalColormaps,
                  labelsColormaps, twotailColormaps, binaryColormaps]
    cmapbytype_fusion = [anatomicalFusionColormaps, diffusionFusionColormaps,
                         functionalFusionColormaps, labelsFusionColormaps, twotailColormaps,
                         binaryColormaps]

    def findRgbColormap(cmap):
        for cmt in cmapbytype:
            for cm in cmt:
                if cm[0] == cmap:
                    return cm[1]
        return None
    chooseColormaps.findRgbColormap = findRgbColormap

    def rgbDist(rgb1, rgb2):
        rgb = numpy.array(rgb1) - rgb2
        return numpy.sqrt((rgb ** 2).sum())
    chooseColormaps.rgbDist = rgbDist

    def chooseColormap(vtype, cmaps, rgbcmaps):
        disttoothers = []
        ct = cmapbytype[vtype]
        for cmap in ct:
            if cmap[0] in cmaps:
                disttoothers.append(0.)
                continue
            # find min distance from cmap to already used ones
            bd = numpy.sqrt(3.)
            for c in rgbcmaps:
                if c is not None:
                    dist = rgbDist(cmap[1], c)
                    if dist < bd:
                        bd = dist
            disttoothers.append(bd)
        disttoothers = numpy.array(disttoothers)
        bests = numpy.where(disttoothers == disttoothers.max())[0]
        if len(bests) == 1:
            return ct[bests[0]]
        else:
            candidates = numpy.array([ct[x][0] for x in bests])
            count = numpy.zeros(len(bests))
            for c in cmaps:
                cc = numpy.where(candidates == c)[0]
                if len(cc) == 1:
                    count[cc[0]] += 1
            return ct[bests[count.argmin()]]
    cmaps = []
    rgbcmaps = []
    typeclasses = [{}, {}, {}, {}, {}, {}]
    vtypes = []
    i = 0
    for vol in vols:
        lh = 1.
        if type(vol) is not type({}):
            vol = checkVolume(vol)
        if vol['binary_volume']:
            vtype = 5
        else:
            vtype = numpy.argmax(vol['volume_contents_likelihoods'])
            if vtype == 2 and vol['negative_values']:
                vtype = 4
            else:
                # classify by likelihood
                lh = vol['volume_contents_likelihoods'][vtype]
        tc = typeclasses[vtype]
        if tc.has_key(lh):
            tc[lh].append((vol, i))
        else:
            tc[lh] = [(vol, i)]
        vtypes.append(vtype)
        i += 1
    # re-order volumes by type/likelihood
    ordered = []
    for t in typeclasses:
        keys = t.keys()
        keys.sort()
        keys.reverse()  # highest at first
        for k in keys:
            ordered += t[k]
    # now choose cmap in ordered volumes
    for vol, i in ordered:
        pal = vol.get('palette')
        if pal:
            if pal not in cmaps:
                cmaps.append(pal)
                rgbcmaps.append(findRgbColormap(pal))
            else:
                pal = None
        if not pal:
            vtype = vtypes[i]
            cmap = chooseColormap(vtype, cmaps, rgbcmaps)
            cmaps.append(cmap[0])
            rgbcmaps.append(cmap[1])

    orderedcmaps = [None] * len(ordered)
    for i in xrange(len(ordered)):
        orderedcmaps[ordered[i][1]] = cmaps[i]
    return orderedcmaps
