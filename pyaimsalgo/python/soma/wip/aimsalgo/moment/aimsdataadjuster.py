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

import math, numpy
import sys

from soma import aimsalgo, aims
from soma.wip.aimsalgo import *

from soma.aims import AimsData_S16, BucketMap_VOID, Converter_AimsData_S16_BucketMap_VOID, Converter_BucketMap_VOID_AimsData_S16, Motion, Reader, ResamplerFactory_S16, Writer, Volume_S16
from soma.aimsalgo import MomentBase, GeometricMoment_S16, MomentInvariant_S16
from soma.wip.aimsalgo.moment import Moment

if sys.version_info[0] >= 3:
    xrange = range


class AimsDataAdjuster :

	def adjust( self, dataSource, dataTarget ) :
		# Process geometric moments on source and target
		geometricMomentSource = GeometricMoment_S16( )
		geometricMomentSource.doit( dataSource )
		geometricMomentTarget = GeometricMoment_S16( )
		geometricMomentTarget.doit( dataTarget )

		# Get the resampling sizes for the transformation
		#sizes = self.getResamplingSizes( dataSource, geometricMomentSource, geometricMomentTarget )
		factor = self.getScaleFactor( geometricMomentSource, geometricMomentTarget )
		sizes = self.getMaximumSize( [ dataSource, dataTarget ], [ factor ] )
		
		# Readjustment
		factory = ResamplerFactory_S16()
		resampler = factory.getResampler( ResamplerFactory_S16.NearestNeighbor )

		# Get the readjust motion from moments using the moment
		matrix = self.getAdjustMatrix( geometricMomentSource, geometricMomentTarget )
		transformation = self.getMotion( matrix )
		resampler.setRef( dataSource )
		result = resampler.doit( transformation,
									sizes[0],
									sizes[1],
									sizes[2],
									aims.Point3df(1) )
		return aims.AimsData(result)

	def getMaximumSize( self, volumes, factors = [] ) :
	
		sizes = numpy.zeros( (len(volumes), 4) )
		for index in xrange( len(volumes) ) :
			if (index < len( factors ) ) :
				factor = factors[index]
			else :
				factor = 1
				
			volume = volumes[index]
			sizes[index] = [volume.dimX() * factor, volume.dimY() * factor, volume.dimZ() * factor, volume.dimT() * factor]
	
		maxima = sizes.max(0)
		result = 0
		for maximum in maxima :
			result += maximum ** 2
		
		return aims.Point3df(math.sqrt(result))
		
	def getResamplingSizes( self, volume, geometricMomentSource, geometricMomentTarget ) :
		matrix = self.getAdjustMatrix( geometricMomentSource, geometricMomentTarget )
		result = numpy.array( [ volume.dimX(), volume.dimY(), volume.dimZ(), 1 ] )
		
		result = result * matrix
		result /= result.take((3,))[0]
		result = result.tolist()[0]

		return result
		
		
	def getEigenVector( self, moment, index ) :
	
		return aims.Point3df(
			moment.eigenVector().value(0, index),
			moment.eigenVector().value(1, index),
			moment.eigenVector().value(2, index)
			)
		
	def getEigenVectors( self, moment ) :
		result = list()
		result.append( self.getEigenVector( moment, 0 ) )
		result.append( self.getEigenVector( moment, 1 ) )
		result.append( self.getEigenVector( moment, 2 ) )
	
		return result
	
	def getQuaternion( self, source, target ) :
		# Process rotation quaternion to apply
		scalar = source.dot( target )
		if scalar < 0 :
			source = aims.Point3df(0) - source
			scalar = -scalar
		
		theta = math.acos( scalar / (target.norm() * source.norm() ) )
		axis = source.crossed( target )	
		quaternion = [ axis[0] * math.sin(theta / 2), axis[1] * math.sin(theta / 2), axis[2] * math.sin(theta / 2), math.cos(theta / 2)  ]
	
		return self.normalizeQuaternion( quaternion )

	def normalizeQuaternion( self, quaternion ) :
		norm = math.sqrt(quaternion[0] ** 2 + quaternion[1] ** 2 + quaternion[2] ** 2 + quaternion[3] ** 2)
		return [quaternion[0] / norm, quaternion[1] / norm, quaternion[2] / norm, quaternion[3] / norm]

	def getRotationMatrix( self, quaternion ) :
		# Process rotation matrix from quaternion
		rotation = numpy.matrix( [
			[(1 - 2 * quaternion[1] ** 2 - 2 * quaternion[2] ** 2),
			(2 * quaternion[0] * quaternion[1] + 2 * quaternion[2] * quaternion[3]),
			(2 * quaternion[0] * quaternion[2] - 2 * quaternion[1] * quaternion[3]),
			0],
			[(2 * quaternion[0] * quaternion[1] - 2 * quaternion[2] * quaternion[3]),
			(1 - 2 * quaternion[0] ** 2 - 2 * quaternion[2] ** 2),
			(2 * quaternion[1] * quaternion[2] + 2 * quaternion[0] * quaternion[3]),
			0],
			[(2 * quaternion[0] * quaternion[2] + 2 * quaternion[1] * quaternion[3]),
			(2 * quaternion[1] * quaternion[2] - 2 * quaternion[0] * quaternion[3]),
			(1 - 2 * quaternion[0] ** 2 - 2 * quaternion[1] ** 2),
			0],
			[ 0,
			0,
			0,
			1]
		] )
	
		return rotation.transpose()
	
	def getTranslationMatrix( self, vector ) :
		# Process translation matrix
		translation = numpy.matrix( [
			[ 1, 0, 0, vector[0]],
			[ 0, 1, 0, vector[1]],
			[ 0, 0, 1, vector[2]],
			[ 0, 0, 0, 1]
		] )
		
		return translation
	
	def getScaleMatrix( self, vector ) :
		# Process scale matrix
		scale = numpy.matrix( [
			[ vector[0], 0, 0, 0],
			[ 0, vector[1], 0, 0],
			[ 0, 0, vector[2], 0],
			[ 0, 0, 0, 1]
		] )
		
		return scale
	
	def getOrthogonalProjectionVector( self, vector1, vector2 ):
		return vector1 - ( ( vector2 / vector2.norm() ) * vector1.dot( vector2 ) )
		
	def getScaleFactor( self, geometricMomentSource, geometricMomentTarget ) :
		return ( geometricMomentTarget.m0() / geometricMomentSource.m0() ) ** (1 / 3.0)
		
	def getMotion( self, transformation ) :
		
		transformationArray = numpy.array(transformation.flatten())
		transformationArray = transformationArray.reshape((transformationArray.shape[0] * transformationArray.shape[1]))
		
		return Motion(transformationArray)

	def getGravityTranslationMatrix( self, geometricMoment, direction = 1 ) :
		
		# Get the translation matrix using gravity center
		vector = aims.Point3df( geometricMoment.gravity() ) * direction
		return self.getTranslationMatrix( vector )

	def getAdjustMatrix( self, geometricMomentSource, geometricMomentTarget ) :
	
		# Get the translation matrix using gravity centers
		translation1 = self.getGravityTranslationMatrix( geometricMomentSource, -1 )
		
		# Get the translation matrix using gravity centers
		translation2 = self.getGravityTranslationMatrix( geometricMomentTarget )
		
		# Process the rotation done on eigen vectors
		eigenVectorsSource = self.getEigenVectors( geometricMomentSource )
		eigenVectorsTarget = self.getEigenVectors( geometricMomentTarget )
		
		# Get the transformations to apply between axis
		# Using 1st axis
		quaternion = self.getQuaternion( eigenVectorsSource[0], eigenVectorsTarget[0] )
		rotation1 = self.getRotationMatrix( quaternion )
		
		# Apply the transformation to the 2nde eigen vector of the source object 
		# It is necessary to project eigen vectors (that are not necessarly orthogonals) 
		# on orthogonal plan before processing the rotation to apply
		eigenVectorSource = (list(eigenVectorsSource[1]) + [1]) * rotation1
		eigenVectorSource = eigenVectorSource.tolist()[0]
		eigenVectorSource = aims.Point3df( eigenVectorSource[:3] ) / eigenVectorSource[3]
		eigenVectorSource = self.getOrthogonalProjectionVector( eigenVectorSource, eigenVectorsTarget[0] )
		
		eigenVectorTarget = self.getOrthogonalProjectionVector( eigenVectorsTarget[1], eigenVectorsTarget[0] )

		quaternion = self.getQuaternion( eigenVectorSource, eigenVectorTarget )
		rotation2 = self.getRotationMatrix( quaternion )
				
		# Get the scale matrix based on volumes
		factor = self.getScaleFactor( geometricMomentSource, geometricMomentTarget )
		scale = self.getScaleMatrix( aims.Point3df(factor) )
		
		transformation = translation2 * scale * rotation2 * rotation1 * translation1
		
		return transformation
