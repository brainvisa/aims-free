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

parser = OptionParser(description= \
    'Processing of Cortical thickness attributes in Cortical Folds Graph')
parser.add_option('-t', '--t1_low', dest='t1_lowangle',
    help='first GRE T1 with lower angle')
parser.add_option('-u', '--t1_high', dest='t1_highangle',
    help='second GRE T1 with higher angle')
parser.add_option('-a', '--bafi_amplitude', dest='bafi_ampl',
    help='BAFI, amplitude map')
parser.add_option('-p', '--bafi_phase', dest='bafi_phase',
    help='BAFI, phase map')
parser.add_option('-o', '--output', dest='output_t1map',
    help='output T1 mapping image')
parser.add_option('-s', '--smoothtype', dest='smooth_type', default='linear',
    help='extrapolation / smoothing type for B1 map. allowed values: nearest, linear, masked_linear, polynomial, median, dilated. default: linear')
parser.add_option('-g', '--gaussian', dest='gaussian', default=0., type=float,
    help='additional gaussian smoothing for the B1 correction map. default: 0mm: not applied')
parser.add_option('--inv', dest='inv', action='store_true',
    help='invert T1 map image to look like a T1-weighted')
parser.add_option('--out_b1map_raw', dest='out_b1map_raw',
    help='output the raw B1 map in the specified file')
parser.add_option('--out_b1map_median', dest='out_b1map_median',
    help='output the B1 map with median filter in the specified file '
    '(if median option is used)')
parser.add_option('--out_b1map', dest='out_b1map',
    help='output the final B1 map in the specified file')

(options, args) = parser.parse_args()

# build B1 correction map from BAFI data

BAFI_amplitude = aims.read(options.bafi_ampl)
BAFI_phase = aims.read(options.bafi_phase)

BAFI_data = t1mapping.BAFIData(BAFI_amplitude, BAFI_phase)
BAFI_data.prescribed_flip_angle = 60.0  # degrees
BAFI_data.echo_times = [3.061, 3.061, 4.5, 7.0]  # milliseconds
BAFI_data.TR_factor = 5.0
BAFI_data.tau = 1.2e-3  # RF pulse duration in seconds TODO check real value!!

#B1map_array = np.abs(BAFI_data.make_B1_map())
B1map_array = np.abs(BAFI_data.make_flip_angle_map())
B1map_farray = np.asfortranarray(B1map_array)
B1map_volume = aims.Volume(B1map_farray)
#B1map_volume.header().update(BAFI_amplitude.header())
B1map_volume.header()['voxel_size'] = BAFI_amplitude.header()['voxel_size']
#B1map_farray[np.asarray(BAFI_amplitude)[:,:,:,0]<50] = 1.
B1map_farray[B1map_farray>1.77] = 0
if options.out_b1map_raw:
    aims.write(B1map_volume, options.out_b1map_raw)

# extrapolate / smooth and resample the B1 map

if options.smooth_type == 'dilated':
    morpho = getattr(aimsalgo,
        'MorphoGreyLevel_' + aims.typeCode(B1map_volume.at(0)))()
    B1map_volume = morpho.doDilation(
        B1map_volume,
        max(BAFI_amplitude.header()['voxel_size'][:3])*2)
elif options.smooth_type == 'median':
    # median filtering is meant to fill small holes in the B1 map
    # use a larger volume (border) to get it done in the border layer
    volume_type = B1map_volume.__class__
    vol_border = volume_type(B1map_volume.getSizeX(),
        B1map_volume.getSizeY(),
        B1map_volume.getSizeZ(),
        B1map_volume.getSizeT(), 1)
    np.asarray(vol_border)[:,:,:,0] = np.asarray(B1map_volume)
    vol_border.copyHeaderFrom(B1map_volume.header())
    vol_border.refVolume().copyHeaderFrom(B1map_volume.header())
    median = getattr( aimsalgo,
        'MedianSmoothing_' + aims.typeCode(B1map_volume.at(0)))
    # apply the filter on the larger image since the filter actually only 
    # applies to the interior limited by the mask size
    B1map_volume_med_border = median().doit(vol_border.refVolume()).volume()
    # get a smaller view in the result
    B1map_volume_med = volume_type(B1map_volume_med_border, 
        volume_type.Position4Di(1,1,1,0), 
        volume_type.Position4Di(*B1map_volume.getSize()))
    B1map_volume_med_arr = np.asarray(B1map_volume_med)
    B1map_volume_med_arr[np.isnan(B1map_volume_med_arr)] = 0
    if options.out_b1map_median:
        print 'writing median.'
        aims.write(B1map_volume_med, options.out_b1map_median)
    print 'median done.'
    # dilate the map to extrapolate outside the brain
    morpho = getattr(aimsalgo,
        'MorphoGreyLevel_' + aims.typeCode(B1map_volume.at(0)))()
    B1map_volume = morpho.doDilation(
        B1map_volume_med,
        max(BAFI_amplitude.header()['voxel_size'][:3])*4)
    # "un-filter" the part which already had valid data
    B1map_ar = np.asarray(B1map_volume_med)
    np.asarray(B1map_volume)[B1map_ar>1e-2] = B1map_ar[B1map_ar>1e-2]
if options.gaussian != 0:
    gsmooth = getattr( aimsalgo,
        'Gaussian3DSmoothing_' + aims.typeCode(B1map_volume.at(0)))
    B1map_volume = gsmooth(options.gaussian, options.gaussian,
        options.gaussian).doit(B1map_volume).volume()
if options.out_b1map:
    aims.write(B1map_volume, options.out_b1map)


GRE_5deg = aims.read(options.t1_lowangle)
GRE_20deg = aims.read(options.t1_highangle)

B1map_vs = np.array(B1map_volume.header()['voxel_size'][:3])
GRE_vs = np.array(GRE_5deg.header()['voxel_size'][:3])

translation = (B1map_vs - GRE_vs) / 2
transform = aims.AffineTransformation3d()
transform.setTranslation(translation)

# resampling
if options.smooth_type == 'nearest':
    resample_order = 0
elif options.smooth_type == 'masked_linear':
    resampler = getattr(aimsalgo,
        'MaskLinearResampler_' + aims.typeCode(B1map_volume.at(0)))()
    B1map_ar = np.asarray(B1map_volume)
    B1map_ar[B1map_ar==0] = -32767
    B1map_ar[np.asarray(BAFI_amplitude)[:,:,:,0]<50] = -32767
    resample_order = -1
else:
    resample_order = 1
if resample_order >= 0:
    resampler = getattr(aims,
        'ResamplerFactory_' + aims.typeCode(B1map_volume.at(0))
        )().getResampler(resample_order)
resampler.setRef(B1map_volume)
B1map = resampler.doit(transform, GRE_5deg.getSizeX(), GRE_5deg.getSizeY(),
    GRE_5deg.getSizeZ(), GRE_vs).volume()
B1map_ar = np.asarray(B1map)
B1map_ar[B1map_ar<0] = 0

GRE_data = t1mapping.GREData2FlipAngles(GRE_5deg, GRE_20deg)
GRE_data.flip_angles = [5, 20]  # degrees
GRE_data.repetition_time = 14  # milliseconds

T1map = t1mapping.t1mapping_VFA(B1map, GRE_data)
#T1map = np.log(T1map+1) * 500
sup_bound = 4000
T1map[T1map>sup_bound] = sup_bound
inf_bound = -1000
T1map[T1map<inf_bound] = inf_bound
#T1map[np.isnan(T1map)] = 0
#T1map[T1map<0] = 0
T1map[np.isnan(T1map)] = sup_bound
if options.inv:
    T1_max = np.max(T1map)
    T1map = T1_max - T1map
    # threshold from T1-weighted
    #T1map[np.asarray(GRE_5deg)<=40] *= 0.1
    gre_arr = np.asarray(GRE_5deg).astype(float)
    gre_scl = (gre_arr-40)/5.
    factor = (1. - np.exp(-gre_scl)/(1.+np.exp(-gre_scl)))
    T1map = np.multiply(T1map, factor)
T1map[np.isnan(T1map)] = 0

#T1map_volume = aims.Volume(np.asfortranarray(T1map))
T1map_volume = aims.Volume(T1map)
T1map_volume.header()['voxel_size'] = GRE_5deg.header()['voxel_size']

aims.write(T1map_volume, options.output_t1map)


