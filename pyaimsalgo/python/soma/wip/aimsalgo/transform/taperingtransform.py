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
# 	This file contains TaperingTransform class
#
#########################################################################
import sys, math

from soma import aims

class TaperingTransform :

	def __init__( self, coefficients) :
		self._coefficients = coefficients

	def transform( self, point ) :
		
		x = point[0]
		y = point[1]
		z = point[2]

		zlimit = self._coefficients[0]
		k1 = self._coefficients[1]
		k2 = self._coefficients[2]
		k3 = self._coefficients[3]

		if ( zlimit != 0 ) :
			
			# Apply the tapering transformation
			X = ( ( k1 * z + k3 ) / zlimit + 1) * x
			Y = ( ( k2 * z + k3 )  / zlimit + 1) * y
			Z = z

		else :
			X = x
			Y = y
			Z = z
		
		return aims.Point3df(X, Y, Z)
		
	def inverseTransform( self, point ) :
		
		X = point[0]
		Y = point[1]
		Z = point[2]

		zlimit = self._coefficients[0]
		k1 = self._coefficients[1]
		k2 = self._coefficients[2]
		k3 = self._coefficients[3]

		coef1 = ( ( k1 * Z + k3 ) / zlimit + 1)
		coef2 = ( ( k2 * Z + k3 )  / zlimit + 1)
		
		if ( ( coef1 != 0 ) and ( coef2 != 0 ) ) :
			
			# Apply inverse of the tapering transformation
			x = X / coef1
			y = Y / coef2
			z = Z

		else :
			x = X
			y = Y
			z = Z
		
		return aims.Point3df(x, y, z)
