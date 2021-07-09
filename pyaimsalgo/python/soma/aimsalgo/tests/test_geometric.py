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

from __future__ import absolute_import

import unittest
from soma import aims
from soma import aimsalgo


class GeometricTestCase(unittest.TestCase):

    def test_delaunay(self):
        plist = [
            [0, 0],
            [1, 0],
            [1, 1],
            [.5, 1.2],
            [0, 1]]
        d = aimsalgo.simple_delaunay_triangulation(plist)
        self.assertEqual(d, [[3, 0, 1], [3, 1, 2], [4, 0, 3]])
        plist.append([0.5, 0.5])
        d = aimsalgo.simple_delaunay_triangulation(plist)
        self.assertEqual(d, [[5, 0, 1], [5, 1, 2], [5, 2, 3], [5, 3, 4]])
        # NOTE: scipy.spatial.Delaunay gives:
        # from scipy.spatial import Delaunay
        # d2 = Delaunay(plist).simplices
        # array([[1, 5, 0],
        #       [2, 5, 1],
        #       [5, 2, 3],
        #       [5, 4, 0],
        #       [4, 5, 3]], dtype=int32)
        # but simple_delaunay_triangulation triangulates an ordered polygon,
        # not a cloud points convex hull.


if __name__ == "__main__":
    unittest.main()

