"""Fit a volume with a polynomial.

Inspired by Fitpoly.m, courtesy of Alexandre Vignaud.
"""

from __future__ import division
from __future__ import print_function
from __future__ import absolute_import

import numpy as np
import itertools
from six.moves import range

def meshgrid_volume(volume):
    """Return three arrays containing each voxel's coordinates."""
    dim_x = volume.getSizeX()
    dim_y = volume.getSizeY()
    dim_z = volume.getSizeZ()
    step_x, step_y, step_z = volume.header()["voxel_size"][:3]
    #step_x, step_y, step_z = ( 4, 4, 4 )
    #print(step_x, step_y, step_z)
    grid = np.mgrid[0:step_x*dim_x:step_x, 0:step_y*dim_y:step_y, 0:step_z*dim_z:step_z]
    return (grid[0], grid[1], grid[2])
    #return np.meshgrid(step_x * np.arange(dim_x),
                       #step_y * np.arange(dim_y),
                       #step_z * np.arange(dim_z), indexing="ij")

def _build_fitpoly_matrix(volume, order, mask=None, transformation=None):
    X, Y, Z = meshgrid_volume(volume)
    if transformation is not None:
        # TODO
        raise NotImplementedError("coordinate transformation not implemented")
    if mask is None:
        maskedX = X
        maskedX = X
        maskedX = X
    else:
        maskedX = X[mask]
        maskedY = Y[mask]
        maskedZ = Z[mask]
    P = np.empty((maskedX.size, order ** 3))
    for k, m, n in itertools.product(list(range(order)), repeat=3):
        P[:, k * (order ** 2) + m * order + n] = ((maskedX ** k) *
                                                  (maskedY ** m) *
                                                  (maskedZ ** n))
    return P

def fit_poly_coefs(volume, order, mask=None):
    """Get the coefficients of the polynomial that fits the input data"""
    P = _build_fitpoly_matrix(volume, order, mask)
    #data = np.asarray(volume)[:, :, :, 0]
    data = np.array(volume, copy=True)[:, :, :, 0]
    Q = np.linalg.pinv(P)
    #c = np.dot(Q, data[mask].ravel())
    if mask is None:
        c = np.dot(Q, data.ravel())
    else:
        c = np.dot(Q, data[mask])
    return c

def apply_poly(volume_like, coefs, mask=None, transformation=None):
    """Calculate a polynomial over the domain of the supplied mask.

    Return a numpy array the same size as the input mask.

    transformation (optional) is the projective transformation from the volume
    to the referential in which the polynomial coefficients were calculated.
    """
    order = int(round(len(coefs) ** (1 / 3)))
    assert order ** 3 == len(coefs)
    volume_like_array = np.asarray(volume_like)
    if mask is not None:
        assert mask.shape == volume_like_array.shape[:3]
    P = _build_fitpoly_matrix(volume_like, order, mask, transformation)
    if mask is None:
        ret = np.dot(P, coefs)
    else:
        ret = np.zeros_like(
            volume_like_array.reshape(volume_like_array.shape[:3]),
            dtype=float)
        ret[mask] = np.dot(P, coefs)
    return ret
