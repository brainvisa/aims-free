#! /usr/bin/env python
############################################################################
# This software and supporting documentation are distributed by
# CEA/NeuroSpin, Batiment 145, 91191 Gif-sur-Yvette cedex, France.
# This software is governed by the CeCILL license version 2 under
# French law and abiding by the rules of distribution of free software.
# You can  use, modify and/or redistribute the software under the
# terms of the CeCILL license version 2 as circulated by CEA, CNRS
# and INRIA at the following URL "http://www.cecill.info".
############################################################################

# system import
import numpy

# Soma import
from soma import aims


def resample_mesh_to_sphere(mesh, sphere, longitude, latitude):    
    """Resample a mesh to the sphere.
    
    Parameters
    ----------
    mesh: (aimsTimeSurface_3)
        a spherical triangulation of cortical hemisphere of the subject
    sphere: (aimsTimeSurface_3)
        an icosphere
        For example, a spherical mesh of size 100 located in standard 
        BrainVISA directory can be used.
    longitude: (TimeTexture_FLOAT)
        a longitude texture from HipHop mapping that go with the white_mesh 
        of the subject. This texture indicates the sperical coordinates 
        at each point. 
    latitude: (TimeTexture_FLOAT)
        a latitude texture from HipHop mapping that go with the white_mesh 
        of the subject. This texture indicates the sperical coordinates 
        at each point.
        
    Return
    ------
    resampled: (aimsTimeSurface_3)
        
    """    
    # a vector of vertices where each vertex is a 3D point
    # with coordinates in millimeters
    vert = sphere.vertex()
    nvert = numpy.asarray(vert)
    
    #########################################################################
    #                         A latitude texture                            #
    #########################################################################
    ray = numpy.sqrt(numpy.square(nvert[:, 0]) + numpy.square(nvert[:, 1]))
    sphere_lat = numpy.arctan(nvert[:, 2] / ray)
    z = numpy.where(ray == 0)[0]
    sphere_lat[z] = -numpy.pi / 2
    sphere_lat[z[nvert[z, 2] >= 0]] = numpy.pi / 2
    sphere_lat += numpy.pi / 2
    sphere_lat *= 180. / numpy.pi
    slat_tex = aims.TimeTexture(sphere_lat.astype(numpy.float32))
    
    #########################################################################
    #                         A longitude texture                           #
    #########################################################################
    sphere_lon = numpy.arctan(nvert[:, 1] / nvert[:, 0])
    z = numpy.where(nvert[:, 0] == 0)[0]
    sphere_lon[z] = -numpy.pi / 2
    sphere_lon[z[nvert[z, 1] >= 0]] = numpy.pi / 2
    sphere_lon[nvert[:, 0] < 0] += numpy.pi
    sphere_lon[sphere_lon < 0] += numpy.pi * 2
    sphere_lon *= 180. / numpy.pi
    slon_tex = aims.TimeTexture(sphere_lon.astype(numpy.float32))
    
    #########################################################################
    #                         Mesh interpoler                               #
    #########################################################################
    
    #  ...
    interpoler = aims.CoordinatesFieldMeshInterpoler(
        mesh, sphere, latitude, longitude, slat_tex, slon_tex)

    # set interpoler discontinuity thresholds to handle 0/360 and 0/180 deg
    # gaps
    interpoler.setDiscontinuityThresholds( 200, 100, 0 )
    # the main operation is project(), which calculates the correspondances
    # between the source and destination mesh
    interpoler.project()
    
    # Resample the sourceshape mesh onto the topology of the interpoler
    # destination mesh, but staying in the native space of sourceshape.
    # sourceshape and the source mesh of the interpoler must have the same
    # structure and topology (same vertices number and order, same polygons).
    resampled = interpoler.resampleMesh(mesh)
    
    return resampled
    