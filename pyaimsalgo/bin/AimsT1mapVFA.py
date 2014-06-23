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

import sys
import numpy as np
from soma import aims, aimsalgo
from soma.aimsalgo import t1mapping
from optparse import OptionParser
import subprocess

parser = OptionParser(description= \
    'Processing of Cortical thickness attributes in Cortical Folds Graph')
parser.add_option( '-t', '--t1_low', dest='t1_lowangle',
    help='first GRE T1 with lower angle' )
parser.add_option( '-u', '--t1_high', dest='t1_highangle',
    help='second GRE T1 with higher angle' )
parser.add_option( '-a', '--bafi_amplitude', dest='bafi_ampl',
    help='BAFI, amplitude map' )
parser.add_option( '-p', '--bafi_phase', dest='bafi_phase',
    help='BAFI, phase map' )
parser.add_option( '-o', '--output', dest='output_t1map',
    help='output T1 mapping image' )

(options, args) = parser.parse_args()

BAFI_amplitude = aims.read(options.bafi_ampl)
BAFI_phase = aims.read(options.bafi_phase)

BAFI_data = t1mapping.BAFIData(BAFI_amplitude, BAFI_phase)
BAFI_data.prescribed_flip_angle = 60.0  # degrees
BAFI_data.echo_times = [3.061, 3.061, 4.5, 7.0]  # milliseconds
BAFI_data.TR_factor = 5.0
BAFI_data.tau = 1.2e-3  # RF pulse duration in seconds TODO check real value!!

B1map_array = np.abs(BAFI_data.make_B1_map())
B1map_farray = np.asfortranarray(B1map_array)
B1map_volume = aims.Volume(B1map_farray)
#B1map_volume.header().update(BAFI_amplitude.header())
B1map_volume.header()['voxel_size'] = BAFI_amplitude.header()['voxel_size']
print 'B1map done.'
#B1map_farray[np.asarray(BAFI_amplitude)[:,:,:,0]<50] = 1.
aims.write(B1map_volume, "/tmp/b1map.nii.gz")

subprocess.check_call(['AimsMorphoMath', '-i', '/tmp/b1map.nii.gz', 
    '-o', '/tmp/b1map_dil.nii.gz', '-m', 'dil', 
    '-r', str(max(BAFI_amplitude.header()['voxel_size'][:3])*2)])

#B1map_volume = aims.read('/tmp/b1map_dil.nii.gz')

GRE_5deg = aims.read(options.t1_lowangle)
GRE_20deg = aims.read(options.t1_highangle)

#B1map_volume.header()['voxel_size'][0] = 4
B1map_vs = np.array(B1map_volume.header()['voxel_size'][:3])
print 'B1map_vs:', B1map_vs
GRE_vs = np.array(GRE_5deg.header()['voxel_size'][:3])

translation = -(B1map_vs - GRE_vs) / 2
transform = aims.AffineTransformation3d()
transform.setTranslation(translation)

conv = aims.Converter(intype=B1map_volume, outtype='Volume_FLOAT')
B1map_float = conv(B1map_volume)
# nearest neighbor resampling for now to avoid invalid data proliferation
resampler = aims.ResamplerFactory_FLOAT().getResampler(1)
#resampler = aimsalgo.MaskLinearResampler_FLOAT()
#B1map_ar = np.asarray(B1map_float)
#B1map_ar[B1map_ar==0] = -32767
#B1map_ar[np.asarray(BAFI_amplitude)[:,:,:,0]<50] = -32767
resampler.setRef(B1map_float)
B1map = resampler.doit(transform, GRE_5deg.getSizeX(), GRE_5deg.getSizeY(),
    GRE_5deg.getSizeZ(), GRE_vs).volume()
B1map_ar = np.asarray(B1map)
B1map_ar[B1map_ar<0] = 0
aims.write(B1map, '/tmp/resampled_b1map.nii.gz')

GRE_data = t1mapping.GREData2FlipAngles(GRE_5deg, GRE_20deg)
GRE_data.flip_angles = [5, 20]  # degrees
GRE_data.repetition_time = 14  # milliseconds

T1map = t1mapping.t1mapping_VFA(B1map, GRE_data)

T1map_volume = aims.Volume(np.asfortranarray(T1map))
aims.write(T1map_volume, options.output_t1map)


