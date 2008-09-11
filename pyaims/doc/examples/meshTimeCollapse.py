# Note : this script is an example. It's suboptimal performance-wise.

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
