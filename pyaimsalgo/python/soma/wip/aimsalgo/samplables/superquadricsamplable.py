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
# Module : aimsalgo.samplables
# Create date : 2007-07-13
#
# Description :
# 	This file contains SuperQuadricSamplable class
#
#########################################################################
from __future__ import absolute_import
import sys
import math
import types

import numpy
from soma import aimsalgo, aims

from soma.aimsalgo import Samplable_FLOAT_3
from soma.wip.aimsalgo.transform import BendingTransform
from soma.wip.aimsalgo.transform import TaperingTransform
from six.moves import range


class SuperQuadricSamplable( Samplable_FLOAT_3 ) :

	"""
	Constructor of the class.

	@type coefficients: list
	@param checkActivated: list of the 10 deformable superquadric coefficients.
		- e1 : first shape parameter
		- e2 : second shape parameter
		- a1 : x axis ray
		- a2 : y axis ray
		- a3 : z axis ray
		- k1 : first tapering parameter
		- k2 : second tapering parameter
		- k3 : third tapering parameter
		- alpha : angle of bending plan
		- kapa : curvature radius for the bending transform
	"""
	def __init__( self, coefficients, bending = True, tapering = True, checkActivated = True, maxBoundingBoxVolume = 10 ** 7) :
		Samplable_FLOAT_3.__init__( self )
		self._checking = dict()
		self._coefficients = coefficients
		self._bending = bending
		self._bendingTransform = BendingTransform(coefficients[8:10])
		self._tapering = tapering
		self._taperingTransform = TaperingTransform(coefficients[4:8])
		self._maxBoundingBoxVolume = maxBoundingBoxVolume
		self._offset = self.getBoundingBoxStart( )
		self._checked = self.check( checkActivated )

	"""
	Check that the deformable superquadric parameters are consistents.

	@type checkActivated: bool
	@param checkActivated: Specify if the check is activated.
	"""
	def check( self, checkActivated ) :

		e1 = self._coefficients[0]
		e2 = self._coefficients[1]
		a1 = self._coefficients[2]
		a2 = self._coefficients[3]
		a3 = self._coefficients[4]
		k1 = self._coefficients[5]
		k2 = self._coefficients[6]
		k3 = self._coefficients[7]
		alpha = self._coefficients[8]
		kapa = self._coefficients[9]

		if (checkActivated) :
			# Check that parameters verify constraints
			# Check for a3 parameter
			checked = ( a3 != 0 )
			self._checking['a3'] = checked
			result = checked
			
			# Check for e1 parameter
			checked = ( e1 != 0 )
			self._checking['e1'] = checked
			result &= checked
			
			# Check for e1 parameter
			checked = ( e2 != 0 )
			self._checking['e2'] = checked
			result &= checked
	
			if (self._tapering) :
				# Check for k1 parameter
				if (result) :
					checked = ( ( ( -1 * k3 * ( a3 ** -1 ) ) - 1 ) <= k1 )
					checked &= ( k1 <= ( ( k3 * ( a3 ** -1 ) ) + 1 ) )
					self._checking['k1'] = checked
					result = checked
				
				# Check for k2 parameter
				if (result) :
					checked = ( ( ( -1 * k3 * ( a3 ** -1 ) ) - 1 ) <= k2 )
					checked &= ( k2 <= ( ( k3 * ( a3 ** -1 ) ) + 1 ) )
					self._checking['k2'] = checked
					result &= checked
	
			if (self._bending) :
				# Check for kappa parameter
				if ( kapa != 0 ) :
					kapaInversed = kapa ** -1
					checked = ( ( a3 / math.pi ) < math.fabs( kapaInversed ) )
					checked &= ( ( math.sqrt ( a1**2 * ( ( math.cos(alpha) ** 2) ** e2 )
								+ a2**2 * ( ( math.sin(alpha) ** 2) ** e2 ) ) ) < math.fabs( kapaInversed ) )
				else :
					checked = True
							
				self._checking['kapa'] = checked
				result &= checked
				
			sizes = self.getBoundingBoxSizes( )
			if ( self._maxBoundingBoxVolume is not None ) :
				# Check for bounding box volume
				volume = sizes[0] * sizes[1] * sizes[2]
				checked = (volume <= self._maxBoundingBoxVolume)
				self._checking['boundingboxvolume'] = checked
				result &= checked
			
			# Check that no overflow occurs
			try :
				self._checked = checked
				self.contains( sizes )
			except :
				checked = False
				self._checking['overflow'] = checked
				result &= checked
				
			return result
				
		else :
			return True

	"""
	Check that a point is contained in the current deformable superquadric.
	This method is called by samplers during sampling to check that the point
	is contained by the object.

	@type point: bool
	@param point: The point to check.
	"""
	def isChecked( self ) :
		return self._checked

	"""
	Check that a point is contained in the current deformable superquadric.
	This method is called by samplers during sampling to check that the point
	is contained by the object.

	@type point: aims.Point3df
	@param point: The point to check.
	"""
	def contains( self, point ) :
		
		if ( self._checked ) :

			# The superquadric has always its bounding box start point in  (0, 0, 0)
			# this allow to always sample BucketMap
			
			point += self._offset

			e1 = self._coefficients[0]
			e2 = self._coefficients[1]
			a1 = self._coefficients[2]
			a2 = self._coefficients[3]
			a3 = self._coefficients[4]

			if ( self._bending ) :
				# Apply inverse of the bending transformation
				transformed = self._bendingTransform.inverseTransform( point )
			else :
				transformed = point

			if (self._tapering) :
				transformed = self._taperingTransform.inverseTransform( transformed )

			x = transformed[0]
			y = transformed[1]
			z = transformed[2]
			
			# Check that point belongs to superquadric object
			result = ( (math.fabs(x/a1)**(2/e2) ) + (math.fabs(y/a2)**(2/e2) ) )**(e2/e1) + (math.fabs(z/a3)**(2/e1) )

			return (result <= 1)

		else :
			return False

	"""
	Get bounding box start for the current superquadric.

	@rtype: aims.Point3df
	@return: start of the bounding box for the superquadric.
	"""
	def getBoundingBoxStart( self ):
		a1 = self._coefficients[2]
		a2 = self._coefficients[3]
		a3 = self._coefficients[4]
		alpha = self._coefficients[8]
		kapa = self._coefficients[9]

		sizes = aims.Point3df( a1, a2, a3 )
		if (self._tapering) :
			# Apply the tapering transformation
			sizes = self._taperingTransform.transform( sizes )
			
		if ( self._bending  and ( kapa != 0 ) ) :
			results = self.getBendingMinMax( sizes )
			start = results[0]
			start[2] = - self.getZMax()
		else :
			start = sizes * -1
			
		return start

	"""
	Get bounding box sizes for the current superquadric.

	@rtype: aims.Point3df
	@return: sizes of the bounding box for the superquadric.
	"""
	def getBoundingBoxSizes( self ):

		e1 = self._coefficients[0]
		e2 = self._coefficients[1]
		a1 = self._coefficients[2]
		a2 = self._coefficients[3]
		a3 = self._coefficients[4]
		alpha = self._coefficients[8]
		kapa = self._coefficients[9]
		
		sizes = aims.Point3df( a1, a2, a3 )
		if (self._tapering) :
			# Apply the tapering transformation
			sizes = self._taperingTransform.transform( sizes )
		
		if ( self._bending and ( kapa != 0 ) ) :
			results = self.getBendingMinMax( sizes )
			minimum = results[0]
			maximum = results[1]
			sizes[0] = (maximum[0] - minimum[0])
			sizes[1] = (maximum[1] - minimum[1])
			sizes[2] = 2 * self.getZMax()
		else :
			sizes *= 2

		return sizes

	"""
	Get maximum z value for the deformable superquadric after transformation.

	@rtype: float
	@return: maximum z value for the superquadric.
	"""
	def getZMax( self ):
		
		e1 = self._coefficients[0]
		e2 = self._coefficients[1]
		a1 = self._coefficients[2]
		a2 = self._coefficients[3]
		a3 = self._coefficients[4]
		alpha = self._coefficients[8]
		kapa = self._coefficients[9]
		
		sizes = aims.Point3df( a1, a2, a3 )
		if (self._tapering) :
			# Apply the tapering transformation to coefficients
			sizes = self._taperingTransform.transform( sizes )
			a1 = sizes[0]
			a2 = sizes[1]
			a3 = sizes[2]
			
		if (kapa != 0):
			boundingRay = math.sqrt ( a1**2 * ( math.cos(alpha) ** 2 ) ** e2
								+ a2**2 * ( math.sin(alpha) ** 2 ) ** e2 ) / 2
			zmax = ( boundingRay + 1 / kapa )
		else:
			zmax = a3
			
		return zmax

	"""
	Get minimum and maximum values for points.
	
	@type sizes: aims.Point3df
	@param sizes: list that contains 8 points of bounding box to get the bounding box
				after bending transform.
	
	@rtype: list
	@return: minimum and maximum points after bending transformation applied.
	"""
	def getBendingMinMax(self, sizes):
		# Get the 4 points of the bounding box in the plan z=0
		point1 = aims.Point3df( sizes[0], sizes[1], 0)
		point2 = aims.Point3df( sizes[0], -sizes[1], 0)
		point3 = aims.Point3df( -sizes[0], -sizes[1], 0)
		point4 = aims.Point3df( -sizes[0], sizes[1], 0)
		# Get the 4 top points of the bounding box plan z=sizes[2]
		point5 = aims.Point3df( sizes[0], sizes[1], sizes[2])
		point6 = aims.Point3df( sizes[0], -sizes[1], sizes[2])
		point7 = aims.Point3df( -sizes[0], -sizes[1], sizes[2])
		point8 = aims.Point3df( -sizes[0], sizes[1], sizes[2])
		
		# Apply the bending transformation to the 4 top points 
		# of the bounding box
		point5 = self._bendingTransform.transform( point5 )
		point6 = self._bendingTransform.transform( point6 )
		point7 = self._bendingTransform.transform( point7 )
		point8 = self._bendingTransform.transform( point8 )
		# Get the maximum and minimum values for x, y, z
		points = [point1, point2, point3, point4, point5, point6, point7, point8]
		result = self.getMinMax( points )

		return result

	"""
	Get minimum and maximum values for points.
	
	@type points: list
	@param points: The list of points to get minimum and maximum.
	
	@rtype: list
	@return: minimum and maximum points.
	"""
	def getMinMax(self, points):
		if points is not None :
			minimum = aims.Point3df(points[0])
			maximum = aims.Point3df(points[0])
			for point in points :
				for index in range(len(point)) :
					if point[index] < minimum[index] :
						minimum[index] = point[index]
					if point[index] > maximum[index] :
						maximum[index] = point[index]
			result = [minimum, maximum]
		else :
			result = None
		
		return result
