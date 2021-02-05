#!/usr/bin/env python
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

from __future__ import print_function

from __future__ import absolute_import
from soma import aims, aimsalgo
from optparse import OptionParser
import numpy, math
from six.moves import range

parser = OptionParser( description = 'Creates a cone or other mesh '
  'representing the TMS active zone at the specified target position along '
  'the head mesh' )
parser.add_option( '-i', '--input', dest='headmeshfile',
  help='input head mesh' )
parser.add_option( '-o', '--output', dest='conefile',
  help='output mesh' )
parser.add_option( '-s', '--stimpoint', dest='stimpoint', type='float',
  nargs=3, help='stimulation position in mm on head or cortex ' \
    '(3 float numbers)' )
parser.add_option( '-r', '--radius', dest='baseradius', type='float',
  help='stimulation cone base radius' )
parser.add_option( '-d', '--depth', dest='depth', type='float',
  help='stimulation cone depth' )
parser.add_option( '-n', '--normalradius', dest='normalradius', type='float',
  help='radius around stimulation point to average normal [optional, ' \
  'default=20] ' )
parser.add_option( '-m', '--normalmesh', dest='normalmesh',
  help='output mesh for the normal line (just for visualization purpose) ' \
    '[optional]' )
parser.add_option( '-t', '--targetlinemesh', dest='targetlinemesh',
  help='output mesh for the target-head projection line, which ' \
  'might be slightly different from the normal. If the target is specified ' \
  'on the head, this line is by defninition the normal. [optional] ' )
parser.add_option( '-b', '--brain', dest='brain',
  help='brain (or grey/white cortex) mask [optional, -z option needed]' )
parser.add_option( '-c', '--cortex', dest='cortexvalue', type='int', 
  help='cortex value in grey/white mask image, [default: all non-zero]' )
parser.add_option( '-z', '--zone', dest='stimzone',
  help='stimulation zone output filename (.bck) [optional, -b option needed]' )
parser.add_option( '-w', '--writeresults', dest='writeresults',
  help='output text file where to write results (points and directions) ' \
  '[optional]' )

parser.set_defaults( normalradius = 20, cortexvalue=None )

(options, args) = parser.parse_args()

#print(options)
if options.headmeshfile is None or options.conefile is None \
  or options.stimpoint is None or options.baseradius is None \
  or options.depth is None \
  or (options.brain is None and options.stimzone is not None):
  parser.parse_args( [ '-h' ] )

targetpoint = aims.Point3df( options.stimpoint )
meshfile = options.headmeshfile
depth = options.depth
baseradius = options.baseradius
outfile = options.conefile
normalradius = options.normalradius
if normalradius < 1:
  normalradius = 1

headmesh = aims.read( meshfile )
# project stimpoint to the head mesh
stimpoint = aims.SurfaceManip.nearestPointToMesh( targetpoint, headmesh,
  10 )[0]
# find closest vertices to stimpoint
dist = numpy.array( [ (p - stimpoint).norm() for p in headmesh.vertex() ] )
normals = headmesh.normal()
snormals = [ normals[n] for n in numpy.where( dist <= normalradius )[0] ]
ntarget = aims.Point3df( numpy.mean( snormals, 0 ) )
ntarget.normalize()
# target line
tline = stimpoint - targetpoint
if tline.norm() < 5:
  tline = ntarget
  print('target is on the head: target line is the normal')
else:
  tline.normalize()
cosalpha = tline.dot( ntarget )
if cosalpha > 1.:
  cosalpha = 1.
alpha = math.acos( cosalpha )
print('stimulation position on head:', stimpoint.list())
print('normal:', ntarget.list())
print('target line:', tline.list())
print('(normal, target line) angle:', alpha)
print()
endpoint = stimpoint - ntarget * depth
if options.writeresults:
  f = open( options.writeresults, 'w' )
  print('target:', targetpoint.list(), file=f)
  print('coil:', stimpoint.list(), file=f)
  print('normal:', ntarget.list(), file=f)
  print('target_line:', tline.list(), file=f)
  print('normal_target_angle:', alpha, file=f)
  f.close()
  del f

stimarea = aims.SurfaceGenerator.cone( endpoint, stimpoint, baseradius, 50,
  False, True )

aims.write( stimarea, outfile )
if options.normalmesh:
  nmesh = aims.AimsTimeSurface_2()
  v0 = nmesh.vertex(0)
  v0.assign( [ endpoint, stimpoint + ntarget * 50 ] )
  p0 = nmesh.polygon(0)
  p0.append( [ 0, 1 ] )
  aims.write( nmesh, options.normalmesh )

if options.targetlinemesh:
  tlmesh = aims.AimsTimeSurface_2()
  v0 = tlmesh.vertex(0)
  tortho = aims.vectProduct( tline, [ 0., 0., 1. ] )
  if tortho.norm() < 1.e-4:
    tortho = aims.vectProduct( tline, [ 1., 0., 0. ] )
  tortho.normalize()
  tortho2 = aims.vectProduct( tline, tortho )
  tortho2.normalize() # (should be already normalized)
  v0.assign( [ aims.Point3df( targetpoint ) - tline * 50,
    targetpoint,
    stimpoint + tline * 50,
    targetpoint - tortho * 20,
    targetpoint + tortho * 20,
    targetpoint - tortho2 * 20,
    targetpoint + tortho2 * 20 ] )
  p0 = tlmesh.polygon(0)
  p0.append( [ 0, 1 ] )
  p0.append( [ 1, 2 ] )
  p0.append( [ 3, 4 ] )
  p0.append( [ 5, 6 ] )
  aims.write( tlmesh, options.targetlinemesh )

if options.stimzone:
  print('masking stimulation zone...')
  # read brain mask
  brain = aims.read( options.brain )
  vs = brain.header()[ 'voxel_size' ]
  dims = brain.header()[ 'volume_dimension' ]
  dimm = [ dims[i] * vs[i] for i in range( 3 ) ]
  # intersection with the stimulation zone
  #  (take a rough bounding box)
  bmin = numpy.min( ( stimpoint, endpoint ), axis=0 )
  bmax = numpy.max( ( stimpoint, endpoint ), axis=0 )
  bmin -= baseradius
  bmax += baseradius
  bmin = numpy.max( ( bmin, [ 0, 0, 0 ] ), axis=0 )
  bmax = numpy.min( ( bmax, dimm ), axis=0 )

  cs = aims.ConeSamplable( endpoint, stimpoint, baseradius )
  sampler = aimsalgo.BucketMapSampler_FLOAT_3()
  zonespl = sampler.sample( cs, bmin, bmax - bmin, vs[:3] ).getPython()
  if options.cortexvalue is None:
    stzone = aims.maskWithVolume( zonespl._get(), brain )
  else:
    stzone = aims.maskWithVolume( zonespl._get(), brain, options.cortexvalue,
      True )

  zone = stzone[0]
  print('stimulation zone:', zone.size(), 'voxels (',
        zone.size() * vs[0] * vs[1] * vs[2], 'mm3)')
  aims.write( stzone, options.stimzone )

