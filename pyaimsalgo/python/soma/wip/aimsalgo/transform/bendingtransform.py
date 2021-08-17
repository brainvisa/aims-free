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

#########################################################################
#
# Project : Pyaimsalgo
# Module : aimsalgo.transform
# Create date : 2007-07-19
#
# Description :
# 	This file contains BendingTransform class
#
#########################################################################
from __future__ import absolute_import
import sys
import math

from soma import aims


class BendingTransform(object):

    def __init__(self, coefficients):
        self._coefficients = coefficients

    def transform(self, point):

        x = point[0]
        y = point[1]
        z = point[2]

        alpha = self._coefficients[0]
        kapa = self._coefficients[1]

        if (kapa != 0):

            # Apply the bending transformation
            kapaInversed = kapa ** -1
            if (kapa > 0):
                kapaSign = 1
            else:
                kapaSign = -1

            gamma = z * kapa
            r = x * math.cos(alpha) + y * math.sin(alpha)
            r1 = kapaInversed - math.cos(gamma) * (kapaInversed - r)

            X = x + math.cos(alpha) * (r1 - r)
            Y = y + math.sin(alpha) * (r1 - r)
            Z = math.sin(gamma) * (kapaInversed - r)

        else:
            X = x
            Y = y
            Z = z

        return aims.Point3df(X, Y, Z)

    def inverseTransform(self, point):

        X = point[0]
        Y = point[1]
        Z = point[2]

        alpha = self._coefficients[0]
        kapa = self._coefficients[1]

        if (kapa != 0):

            # Apply inverse of the bending transformation
            kapaInversed = kapa ** -1
            if (kapa > 0):
                kapaSign = 1
            else:
                kapaSign = -1

            r1 = X * math.cos(alpha) + Y * math.sin(alpha)
            r = kapaInversed - \
                (kapaSign * math.sqrt(Z**2 + (kapaInversed - r1)**2))
            gamma = math.atan2(Z * kapaSign, (kapaInversed - r1) * kapaSign)
            x = X - math.cos(alpha) * (r1 - r)
            y = Y - math.sin(alpha) * (r1 - r)
            z = gamma * kapaInversed

        else:
            x = X
            y = Y
            z = Z

        return aims.Point3df(x, y, z)
