#!/usr/bin/env python

''' Cortical thickness correction functions accorting to curvature.
See :func:`corrected_thick_flattened()` for most explanations.

In a few words, curvature affects cortical thickness, as observed by many studies (Bok 1929, Demirci 2022, Leprince 2015, Brodmann and so on). If we need to measure or correlate thickness with other factors (like Brodmann regions or other parcellations) we need to make it more independent from curvature.

We have basically 2 functions, :func:`corrected_thick_flattened()` which performs this correction from observed thickness and curvature, and :func:`corrected_thick_curved` which does the inverse: calculate the theoretical thickness of a curved portion of cortex for a given "flat" thickness. Not sure this second function has a real use: in most cases the first one is used.

Two other functions allow to check and optimize the ``beta`` parameter: :func:`find_best_beta` and :func:`find_best_beta_per_parcel`.
'''

import numpy as np


def corrected_thick_curved(thick, curv, beta, r0=0.1):
    ''' Calculate the thickness of a curved portion of cortex, given a "flat cortex" thickness value and a curvature value.

    curv should be positive for cortex convexities (gyri), this this the
    opposite from Freesurfer curvature

    This is not what you should apply to correct an observed thickness map, but the inverse: see :func:`corrected_thick_flattened` for that.
    '''
    # r0: virtual circle line radius (-> 0), the result should be independent
    # from it, actually (and this is quite true)
    # no curvature: v = l * t
    # curvature: v = vcone + vcalotte
    # vcone = 1/3 * disk.surf * h
    # vcal = pi/3 * h0^^2 * (3*r - h0)

    # r: external curv radius

    if np.abs(curv) <= 1e-10:
        return thick

    rmid = 1. / curv  # radius at curv measurement

    if curv < 0:
        rmid *= -1
        beta = 1 - beta
        # curvature is computed on the grey/white mesh in Freesurfer
        #rmid -= thick * 0.5
    else:
        # curvature is computed on the grey/white mesh in Freesurfer
        rmid += thick * 0.5
    rl = rmid + (beta - 0.5) * thick  # radius at iso-level

    v = np.pi * r0 * r0 * thick

    sin_a = r0 / rl
    cos_a = np.sqrt(1 - sin_a * sin_a)
    print('rmid:', rmid, ', rl:', rl, ', sin:', sin_a, ', cos:', cos_a)

    B = np.pi / 3. * (sin_a * sin_a * cos_a + (1. - cos_a) ** 2 * (2. + cos_a))
    print('B:', B, ', v:', v)

    if rl * rl * rl < beta * v / B:
        # the beta*v volume does not fit in the internal cone: rl must be
        # pushed outwards
        rl = np.power(beta * v / B, 1. / 3.)
        print('curvature too high for internal thickness, limiting')
        sin_a = r0 / rl
        cos_a = np.sqrt(1 - sin_a * sin_a)
        print('now rl:', rl, ', sin:', sin_a, ', cos:', cos_a)
        B = np.pi / 3. * (sin_a * sin_a * cos_a + (1. - cos_a) ** 2 * (2. + cos_a))
        print('B:', B)
        print(rl * rl * rl - beta * v / B)
        if rl * rl * rl < beta * v / B:
            rl = np.power(beta * v / B, 1. / 3.) + 1e-10  # add epsilon
            # to ensure positivity
            print('still pushing out rl:', rl)

    rext = np.power((1. - beta) * v / B + rl * rl * rl, 1. / 3.)

    print('pow:', rl * rl * rl - beta * v / B)
    rint = np.power(rl * rl * rl - beta * v / B, 1 / 3.)

    new_th = rext - rint
    print('rext:', rext, ', rint:', rint)

    return new_th


def corrected_thick_flattened(thick, curv, beta, r0=0.1):
    ''' Correct a cortical thickness value for the curvature, for a single
    point.

    The correction consists in "flattening" the cortex and applying the iso-volumic principle (Bok 1929, Leprince 2015) in order to calculate the thickness of a flat cortex portion corresponding to the same volume.

    This flattening has one degree of freedom, and thus depends on a parameter, which we call ``beta`` here, which specifies the position of the "iso-surface" layer (Bok 1929). The iso-surface layer is the layer height where no deformation happens: where there is no tangential shrinking nor compession. According to Bok (1929), it should be around cortical layer IV which is the most dense cortical layer in terms of neurons. However it might also depend on pressure / tension conditions which may be different in the grey matter and in the CSF, and which could participate in a shrink / compression phenomenon. Any way this parameter needs to be given.

    beta is given as a volume ratio, 0 on the inner (grey/white) surface, 1 on the outer (pial) surface, and not as a distance ratio, because it would then depend on the curvature. Here it does not, according to the iso-volume principle (Bok, Leprince).

    If we stick with Bok's observations and hypotheses, layer IV is located at about 60-70% of cortical depth from the inner surface. In our experiments beta is optimal at about 0.6 (0.59 precisely).

    Technically beta might vary according to the microstructure of the cortex, thus change from one Brodmann area to another, but it does not change very much, thus could be approximatively be taken constant. In our experiments the optimal beta calculated on each region of a Desikan parcellation of the cortex, varies between 0.5 and 0.68, and is very close to 0.6 in most regions.

    See :func:`find_best_beta_per_parcel` to optimize and verify this.

    Parameters
    ----------
    thick: float
        observed thickness value (for instance from a Freesurfer thickness
        texture map)
    curv: float
        curvature value at the same point. Curvature is signed, with positive values interpreted as a convexity (thus, positive for gyri and negative for sulci). **BE CAREFUL** this is the opposite from Freesurfer curvature maps, which should thus be inverted. The curvature is supposed to be measured on the inner surface (grey/white interface), as it is done in Freesurfer. If the curvature is negative and too strong (curvature center inside the cortical ribbon), it is corrected to push the center on the outer (pial) surface.
    beta: float
        volume ratio giving the relative position of the iso-surface layer (between 0 and 1 normally, but might be outside these boundaries).
    r0: float
        r0 is the "virtual" radius of a circle at rhe iso-surface position where the volume will be measured. Normally the correcthed thickness should not depend on this radius, but technical calculations are using it. In practice the result does actually not change much if r0 is small enough: values 1., 0.1, 0.01 give almost the same results. The default, 0.1 is OK.

    Returns
    -------
    corrected_thickness: float
        corrected thickness value
    '''

    # r0: virtual circle line radius (-> 0), the result should be independent
    # r1: internal radius (G/W)
    # r2: external radius (pial)

    if np.abs(curv) <= 1e-10:
        # no curvature: no correction
        return thick

    if curv < 0:
        curv *= -1
        beta = 1. - beta
        r = 1. / curv - thick  # radius at curv measurement (grey/white ??)
        if r < 0:
            r = 0
    else:
        r = 1. / curv  # radius at curv measurement (grey/white ??)
    # r is the internal radius

    r2 = r + thick
    r1 = np.power((beta * r2 ** 3 + (1. - beta) * r ** 3), 1. / 3.)
    sin_a = r0 / r1
    cos_a = np.sqrt(1 - sin_a * sin_a)
    B = np.pi / 3. * (sin_a * sin_a * cos_a + (1. - cos_a) ** 2 * (2. + cos_a))

    new_th = (r2 ** 3 - r ** 3) * B / (np.pi * r0 * r0)

    # print('r:', r, ', r2:', r2, ', r1:', r1, ', sin_a:', sin_a, ', cos_a:', cos_a, ', B:', B)
    return new_th


def find_best_beta(thick_map, curv_map, r0=0.1, betas=np.arange(0, 1., 0.1),
                   return_stats=False):
    ''' Determine the best beta value for use with :func: `corrected_thick_flattened` and :func:`corrected_thick_curved`.

    ``beta`` is tested iteratively in a list of tested values, for a given thickness / curvature observations sample. The optimum is the values which results in the lowest relative standard deviation (std / average) in corrected thickness.

    Parameters
    ----------
    thick_map: texture or numpy array
        thickness map (for instance a Freesurfer thickness map).
    curv_map: texture or numpy array
        curvature map. As in :func: `corrected_thick_flattened` and :func:`corrected_thick_curved`, it is considered being measured ont the inner surface (grey/white) and being positive for convex parts (gyri), thus Freesurfer maps should be inverted first.
    r0: float
        see :func: `corrected_thick_flattened`
    betas: list or array
        list of beta values to be tested
    return_stats: bool
        if True, the return value will include the average and standard deviation of the corrected thickness data for all beta values

    Returns
    -------
        beta_opt: float
            optimal beta value
        y_avg: numpy array (if return_stats is True only)
            averages of corrected thickness for all betas
        y_std: numpy array (if return_stats is True only)
            std of corrected thickness for all betas
    '''

    thick = thick_map
    if not isinstance(thick, np.ndarray):
        thick = thick_map[0].np
    curv = curv_map
    if not isinstance(curv, np.ndarray):
        curv = curv_map[0].np

    yavg = []
    ystd = []

    for beta in betas:
        # print('beta:', beta)
        y = [corrected_thick_flattened(t, c, beta, r0)
             for t, c in zip(thick, curv)]
        yavg.append(np.average(y))
        ystd.append(np.std(y))

    beta_opt = betas[np.argmin(np.array(ystd) / np.array(yavg))]
    res = beta_opt
    if return_stats:
        res = (beta_opt, yavg, ystd)

    return res


def find_best_beta_per_parcel(thick_map, curv_map, parcels, r0=0.1,
                              betas=np.arange(0, 1., 0.1), return_stats=True):
    ''' Find the best beta valuye for use with :func: `corrected_thick_flattened` and :func:`corrected_thick_curved`, on a region-per-region basis.

    Uses :func:`find_best_beta` on every region of a parcellation

    Parameters
    ----------
    thick_map: texture or numpy array
        thickness map (for instance a Freesurfer thickness map).
    curv_map: texture or numpy array
        curvature map.*
    parcels: label texture or numpy array
        parcellation, like a Desikan texture
    r0: float
        see :func: `corrected_thick_flattened`
    betas: list or array
        list of beta values to be tested
    return_stats: bool
        if True, the return value will include the optimal beta for each region

    Returns
    -------
    beta_opt: float
        average of optimal betas for all regions
    bets_opts: array (if return_stats is True)
        dict of optimal beta for each region label
    '''
    thick = thick_map
    if not isinstance(thick, np.ndarray):
        thick = thick_map[0].np
    curv = curv_map
    if not isinstance(curv, np.ndarray):
        curv = curv_map[0].np
    if not isinstance(parcels, np.ndarray):
        parcels = parcels[0].np

    beta_opts = {}
    for region in np.unique(parcels):
        reg_p = np.where(parcels == region)
        thick_r = thick[reg_p]
        curv_r = curv[reg_p]
        beta = find_best_beta(thick_r, curv_r, r0, betas=betas)
        beta_opts[region] = beta

    beta_opt = np.average(list(beta_opts.values()))

    if return_stats:
        return beta_opt, beta_opts
    else:
        return beta_opt
