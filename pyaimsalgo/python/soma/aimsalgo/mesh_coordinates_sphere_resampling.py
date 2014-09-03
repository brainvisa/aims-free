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


def sphere_coordinates(sphere):
    """
    Compute spherical coordinates (longitude, latitude) on a sphere.

    Parameters
    ----------
    sphere: (aimsTimeSurface_3)
        a sphere mesh: vertices must be on a sphere with center 0.

    Return
    ------
    (longitude, latitude): tuple, each element being a TimeTexture_FLOAT
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

    return slon_tex, slat_tex


def resample_mesh_to_sphere(mesh, sphere, longitude, latitude):
    """Resample a mesh to the sphere.

    Parameters
    ----------
    mesh: (aimsTimeSurface_3)
        a spherical triangulation of cortical hemisphere of the subject
    sphere: (aimsTimeSurface_3)
        a sphere mesh with center 0.
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
    # get spherical coordinates textures on the sphere
    slon_tex, slat_tex = sphere_coordinates(sphere)

    #########################################################################
    #                         Mesh interpoler                               #
    #########################################################################

    #  ...
    interpoler = aims.CoordinatesFieldMeshInterpoler(
        mesh, sphere, latitude, longitude, slat_tex, slon_tex)

    # set interpoler discontinuity thresholds to handle 0/360 and 0/180 deg
    # gaps
    interpoler.setDiscontinuityThresholds(200, 100, 0)
    # the main operation is project(), which calculates the correspondances
    # between the source and destination mesh
    interpoler.project()

    # Resample the sourceshape mesh onto the topology of the interpoler
    # destination mesh, but staying in the native space of sourceshape.
    # sourceshape and the source mesh of the interpoler must have the same
    # structure and topology (same vertices number and order, same polygons).
    resampled = interpoler.resampleMesh(mesh)

    return resampled


def texture_by_polygon(mesh, texture):
    """
    """
    poly = numpy.asarray(mesh.polygon())
    tex = numpy.asarray(texture[0])
    poly_tex = tex[poly[:,0]] + tex[poly[:,1]] + tex[poly[:,2]]
    return poly_tex / 3


def polygon_average_sizes(mesh):
    """
    """
    poly = numpy.asarray(mesh.polygon())
    vert = numpy.asarray(mesh.vertex())
    p1 = vert[poly[:,0]]
    p2 = vert[poly[:,1]]
    p3 = vert[poly[:,2]]
    d1 = numpy.sqrt(numpy.sum(numpy.square(p2 - p1), axis=1))
    d2 = numpy.sqrt(numpy.sum(numpy.square(p3 - p2), axis=1))
    d3 = numpy.sqrt(numpy.sum(numpy.square(p1 - p3), axis=1))
    return (d1 + d2 + d3) / 3


def refine_sphere_mesh(init_sphere, avg_dist_texture, current_sphere,
    target_avg_dist, init_sphere_coords=None, current_sphere_coords=None):
    """
    """
    if init_sphere_coords is not None:
        init_lon, init_lat = init_sphere_coords
    else:
        init_lon, init_lat = sphere_coordinates(init_sphere)
    if current_sphere_coords is not None:
        current_lon, current_lat = current_sphere_coords
    else:
        current_lon, current_lat = sphere_coordinates(current_sphere)

    init_sphere_dist = (init_sphere.vertex()[init_sphere.polygon()[0][1]]
        - init_sphere.vertex()[init_sphere.polygon()[0][0]]).norm()
    interpoler = aims.CoordinatesFieldMeshInterpoler(
      init_sphere, current_sphere, init_lat, init_lon, current_lat, current_lon)
    interpoler.setDiscontinuityThresholds(200, 100, 0)
    interpoler.project()
    resampled_dist_tex = interpoler.resampleTexture(avg_dist_texture)
    polygon_dist = texture_by_polygon(current_sphere, resampled_dist_tex)
    polygon_sizes = polygon_average_sizes(current_sphere)
    polygon_dist /= polygon_sizes / init_sphere_dist
    next_sphere = aims.SurfaceManip.refineMeshTri4(
        current_sphere,
        numpy.where(numpy.asarray(polygon_dist) >= target_avg_dist)[0])

    # snap vertices to sphere
    vert2 = numpy.square(numpy.asarray(current_sphere.vertex()))
    dist = numpy.sqrt(numpy.sum(vert2, axis=1))
    radius = numpy.average(dist)
    next_vert = numpy.asarray(next_sphere.vertex())
    next_vert2 = numpy.square(next_vert)
    next_dist = numpy.sqrt(numpy.sum(next_vert2, axis=1))
    next_vert = next_vert * radius / next_dist.reshape((next_vert.shape[0], 1))
    next_sphere.vertex().assign([aims.Point3df(x) for x in next_vert])
    return next_sphere

