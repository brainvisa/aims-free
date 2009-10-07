# Copyright IFR 49 (1995-2009)
#
#  This software and supporting documentation were developed by
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

# ce truc me sert a aplatir temporelement le mesh (c'est a dire que je passe
# de morceaux de mesh (aux indices temporels croissants) vers un seul mesh)

# Script sous-optimal car il travaille sur des numpyarray (donc conversion en
# entree et en sortie), alors qu'on doit pouvoir faire l'equivalent sur les
# aimsmesh via les fonctions de aims.surfacemanip.
# Et car il charge tout en memoire.

from soma import aims
from numpy import *
v = aims.Reader().read('/home/riviere/data/ra_head.mesh')

def getFromMesh(v, t):
	vertex = array(v.vertex(t))  
	normal = array(v.normal(t))
	polygon = array(v.polygon(t))
	return(vertex, normal, polygon)

def meshlistMerge(meshlist):
        outv = vstack([x[0] for x in meshlist])
        outn = vstack([x[1] for x in meshlist])
        outplst = []
        n = 0
        for x in meshlist:
            outplst.append(x[2] + n)
            n += len(x[0])
        outp = vstack(outplst)
        return (outv, outn, outp)

def writeMesh(vertex, normal, polygon):
	vv = aims.vector_POINT3DF()
	vn = aims.vector_POINT3DF()
	vp = aims.vector_AimsVector_U32_3()
	for x in vertex:
		vv.append(x)
	for x in normal:
		vn.append(x)
	for x in polygon:
		vp.append(x)
	m = aims.AimsTimeSurface_3()
	m.vertex().assign( vv )
	m.normal().assign( vn )
	m.polygon().assign( vp )
	W = aims.Writer() 
	W.write(m, '/tmp/mesh0')
	

M = meshlistMerge([getFromMesh(v, x) for x in range(v.size())])
writeMesh(*M)
