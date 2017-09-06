#!/usr/bin/env python2
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

import sys
import os
from optparse import OptionParser
from soma import aims
import numpy as np

if sys.version_info[0] >= 3:
    xrange = range

parser = OptionParser(
    description='Project volume voxels valus on a mesh into a texture. Each vertex simply gets the volume value at its position.')
parser.add_option('-i', '--input', dest='input',
                  help='input volume')
parser.add_option("-m", "--mesh", dest="mesh", action='store',
                  help="mesh file")
parser.add_option('-o', '--output', dest='output', action='store',
                  help='output texture file')
parser.add_option('-t', '--transform', dest='transform', action='store',
                  help='optional transformation file between the volume and mesh spaces')

(options, args) = parser.parse_args()
if not options.input and len(args) != 0:
    options.input = args.pop(0)
if not options.mesh and len(args) != 0:
    options.mesh = args.pop(0)
if not options.output and len(args) != 0:
    options.output = args.pop(0)
if not options.transform and len(args) != 0:
    options.transform = args.pop(0)

if not options.input or not options.mesh or not options.output \
        or len(args) != 0:
    parser.parse_args([sys.argv[0], '-h'])

vol = aims.read(options.input)
mesh = aims.read(options.mesh)
trans = None

tex = aims.TimeTexture(str(vol.arraydata().dtype))

if options.transform:
    trans = aims.read(options.transform)
    vert = [trans.transform(v) for v in mesh.vertex()]
else:
    vert = mesh.vertex()
nv = vert.size()
vs = vol.header()['voxel_size'][:3]
ivert = np.round(np.array(vert) / vs).astype('int')
# clamp indices
ivert[ivert < 0] = 0
ivert[np.where(ivert[:, 0] >= vol.getSizeX())[0], 0] = vol.getSizeX() - 1
ivert[np.where(ivert[:, 1] >= vol.getSizeY())[0], 1] = vol.getSizeY() - 1
ivert[np.where(ivert[:, 2] >= vol.getSizeZ())[0], 2] = vol.getSizeZ() - 1
ar = np.array(vol, copy=False)


for t in xrange(vol.getSizeT()):
    t = tex[t]
    t.reserve(nv)
    t.assign(ar[ivert[:, 0], ivert[:, 1], ivert[:, 2], 0])

aims.write(tex, options.output)
