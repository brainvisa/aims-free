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


def draw_sphere(mesh, longitude, latitude):
    """Draw a sphere

    Parameters
    ----------
    mesh: (AimsTimeSurface_3_VOID)
        a spherical triangulation of cortical hemisphere of the subject
    longitude: (TimeTexture_FLOAT)
        a longitude texture from HipHop mapping that go with the white_mesh
        of the subject. This texture indicates the spherical coordinates
        at each point.
    latitude: (TimeTexture_FLOAT)
        a latitude texture from HipHop mapping that go with the white_mesh
        of the subject. This texture indicates the spherical coordinates
        at each point.

    Return
    ------
    sphere_mesh: (AimsTimeSurface_3_VOID)
        a spherical triangulation of the subject of its cortical hemisphere,
        projected on a sphere
    """
    lon = longitude[0].arraydata()
    lat = latitude[0].arraydata()

    nmesh = numpy.asarray(mesh.vertex())

    # generate all x, y, z for each vertex (list)
    list_coord = []
    for index, coord in enumerate(nmesh):
        x, y, z = sphere((lon[index]*numpy.pi / 180) + numpy.pi,
                         (lat[index]*numpy.pi / 180) - numpy.pi / 2)
        list_coord.append([x, y, z])

    # replace the polar coordinates with the news coordinates
    mesh.vertex().assign([aims.Point3df(x) for x in list_coord])

    return mesh


def sphere(v, u):
    """Generate a sphere from polar coordinates to spheric coordinates.

    Parameters
    ----------
    radius: (float)
        around of 100
    u: (float)
        angle phi (latitude) WARNING: radian
    v: (float)
        angle theta (longitude) WARNING: radian
    """
    x = 100 * numpy.cos(u) * numpy.cos(v)
    y = 100 * numpy.cos(u) * numpy.sin(v)
    z = 100 * -numpy.sin(u)

    return x, y, z


def sphere_coordinates(sphere, inversion=False):
    """
    Compute spherical coordinates (longitude, latitude) on a sphere.

    Parameters
    ----------
    sphere: (AimsTimeSurface_3_VOID)
        a sphere mesh: vertices must be on a sphere with center 0.
    inversion: bool
        if True, the longitude coord is inverted (useful for right hemisphere)

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
    radius = numpy.sqrt(numpy.square(nvert[:, 0]) + numpy.square(nvert[:, 1]))
    sphere_lat = numpy.arctan2(radius, nvert[:, 2])
    sphere_lat = -sphere_lat * 180. / numpy.pi + 180.
    slat_tex = aims.TimeTexture(sphere_lat.astype(numpy.float32))

    #########################################################################
    #                         A longitude texture                           #
    #########################################################################
    sphere_lon = numpy.arctan2(nvert[:, 1], nvert[:, 0])
    sphere_lon *= 180. / numpy.pi
    sphere_lon += 180
    print 'inversion: ', inversion
    if inversion == "True":
        print "il y a inversion", inversion
        sphere_lon = 360 - sphere_lon
    slon_tex = aims.TimeTexture(sphere_lon.astype(numpy.float32))
    return slon_tex, slat_tex


def resample_mesh_to_sphere(
    mesh, sphere, longitude, latitude, inversion=False):
    """Resample a mesh to the sphere.

    Parameters
    ----------
    mesh: (AimsTimeSurface_3_VOID)
        a spherical triangulation of cortical hemisphere of the subject
    sphere: (AimsTimeSurface_3_VOID)
        a sphere mesh with center 0.
        For example, a spherical mesh of size 100 located in standard
        BrainVISA directory can be used.
    longitude: (TimeTexture_FLOAT)
        a longitude texture from HipHop mapping that go with the white_mesh
        of the subject. This texture indicates the spherical coordinates
        at each point.
    latitude: (TimeTexture_FLOAT)
        a latitude texture from HipHop mapping that go with the white_mesh
        of the subject. This texture indicates the spherical coordinates
        at each point.
    inversion: bool
        if True, the longitude coord is inverted (useful for right hemisphere)

    Return
    ------
    resampled: (AimsTimeSurface_3_VOID)

    """
    # get spherical coordinates textures on the sphere
    slon_tex, slat_tex = sphere_coordinates(sphere, inversion)

    #########################################################################
    #                         Mesh interpoler                               #
    #########################################################################

    #  ...
    # multiply latitudes by 2 to get same amplitude on both coords
    latitude = aims.TimeTexture(latitude[0].arraydata() * 2)
    slat_tex = aims.TimeTexture(slat_tex[0].arraydata() * 2)
    interpoler = aims.CoordinatesFieldMeshInterpoler(
        mesh, sphere, latitude, longitude, slat_tex, slon_tex)

    # set interpoler discontinuity thresholds to handle 0/360 and 0/180 deg
    # gaps
    interpoler.setDiscontinuityThresholds(200, 200, 0)
    # the main operation is project(), which calculates the correspondances
    # between the source and destination mesh
    interpoler.project()

    # Resample the sourceshape mesh onto the topology of the interpoler
    # destination mesh, but staying in the native space of sourceshape.
    # sourceshape and the source mesh of the interpoler must have the same
    # structure and topology (same vertices number and order, same polygons).
    resampled = interpoler.resampleMesh(mesh)

    return resampled


def resample_texture_to_sphere(
    mesh, sphere, longitude, latitude, texture, interpolation='linear',
    inversion=False):
    """Resample a mesh to the sphere.

    Parameters
    ----------
    mesh: (AimsTimeSurface_3_VOID)
        a spherical triangulation of cortical hemisphere of the subject
    sphere: (AimsTimeSurface_3_VOID)
        a sphere mesh with center 0.
        For example, a spherical mesh of size 100 located in standard
        BrainVISA directory can be used.
    longitude: (TimeTexture_FLOAT)
        a longitude texture from HipHop mapping that go with the white_mesh
        of the subject. This texture indicates the spherical coordinates
        at each point.
    latitude: (TimeTexture_FLOAT)
        a latitude texture from HipHop mapping that go with the white_mesh
        of the subject. This texture indicates the spherical coordinates
        at each point.
    interpolation: string or MeshInterpoler.InterpolationType enum
        resampling interpolation type: "linear" or "nearest_neighbour"
    inversion: bool
        if True, the longitude coord is inverted (useful for right hemisphere)

    Return
    ------
    resampled: (same type as input texture)

    """
    # get spherical coordinates textures on the sphere
    slon_tex, slat_tex = sphere_coordinates(sphere, inversion)

    #########################################################################
    #                         Mesh interpoler                               #
    #########################################################################

    #  ...
    # multiply latitudes by 2 to get same amplitude on both coords
    latitude = aims.TimeTexture(latitude[0].arraydata() * 2)
    slat_tex = aims.TimeTexture(slat_tex[0].arraydata() * 2)
    interpoler = aims.CoordinatesFieldMeshInterpoler(
        mesh, sphere, latitude, longitude, slat_tex, slon_tex)

    # set interpoler discontinuity thresholds to handle 0/360 and 0/180 deg
    # gaps
    interpoler.setDiscontinuityThresholds(200, 200, 0)
    # the main operation is project(), which calculates the correspondances
    # between the source and destination mesh
    interpoler.project()

    # Resample the sourceshape texture onto the topology of the interpoler
    # destination mesh, but staying in the native space of sourceshape.
    # sourceshape and the source mesh of the interpoler must have the same
    # structure and topology (same vertices number and order, same polygons).
    if isinstance(interpolation, interpoler.InterpolationType):
        interp_type = interpolation
    elif interpolation == 'linear':
        interp_type = interpoler.Linear
    elif interpolation == 'nearest_neighbour':
        interp_type = interpoler.NearestNeighbour
    else:
        raise ValueError('unknown interpolation type: %s'
            % repr(interpolation))
    resampled = interpoler.resampleTexture(texture, interp_type)

    return resampled


def texture_by_polygon(mesh, texture):
    """Averages a texture (classically, by vertex) on polygons.

    Used by refine_sphere_mesh() and sphere_mesh_from_distance_map()

    Parameters
    ----------
    mesh: (AimsTimeSurface_3_VOID)
        a mesh providing trianglar struture
    texture: (TimeTexture_FLOAT)
        texture data

    Return
    ------
    poly_tex: (nupy array)
        texture averaged on polygons

    """
    poly = numpy.asarray(mesh.polygon())
    tex = numpy.asarray(texture[0])
    poly_tex = numpy.max(
        (tex[poly[:, 0]], tex[poly[:, 1]], tex[poly[:, 2]]), axis=0)
    return poly_tex


def polygon_average_sizes(mesh):
    """Return the average edge length for each triangle of a mesh

    Used by refine_sphere_mesh() and sphere_mesh_from_distance_map()

    Parameters
    ----------
    mesh: (AimsTimeSurface_3_VOID)
        a mesh providing trianglar struture

    Return:
    lengths: (numpy array)
        average size for each polygon
    """
    poly = numpy.asarray(mesh.polygon())
    vert = numpy.asarray(mesh.vertex())
    p1 = vert[poly[:, 0]]
    p2 = vert[poly[:, 1]]
    p3 = vert[poly[:, 2]]
    d1 = numpy.sqrt(numpy.sum(numpy.square(p2 - p1), axis=1))
    d2 = numpy.sqrt(numpy.sum(numpy.square(p3 - p2), axis=1))
    d3 = numpy.sqrt(numpy.sum(numpy.square(p1 - p3), axis=1))
    return (d1 + d2 + d3) / 3


def polygon_max_sizes(mesh):
    """Return the max edge length for each triangle of a mesh

    Used by refine_sphere_mesh() and sphere_mesh_from_distance_map()

    Parameters
    ----------
    mesh: (AimsTimeSurface_3_VOID)
        a mesh providing trianglar struture

    Return:
    lengths: (numpy array)
        average size for each polygon
    """
    poly = numpy.asarray(mesh.polygon())
    vert = numpy.asarray(mesh.vertex())
    p1 = vert[poly[:, 0]]
    p2 = vert[poly[:, 1]]
    p3 = vert[poly[:, 2]]
    d1 = numpy.sqrt(numpy.sum(numpy.square(p2 - p1), axis=1))
    d2 = numpy.sqrt(numpy.sum(numpy.square(p3 - p2), axis=1))
    d3 = numpy.sqrt(numpy.sum(numpy.square(p1 - p3), axis=1))
    return numpy.max((d1, d2, d3), axis=0)


def refine_sphere_mesh(init_sphere, avg_dist_texture, current_sphere,
                       target_avg_dist, inversion=False,
                       init_sphere_coords=None, current_sphere_coords=None,
                       dist_texture_is_scaled=True):
    """Adaptively refine polygons of a sphere mesh according to an average
    distance map (genrally calculated in a different space), and a target
    length.

    This is one single step if the iterative sphere_mesh_from_distance_map().

    Polygons where the average distance map value is "too high" are oversampled
    (divided in 4).

    Parameters
    ----------
    init_sphere: (AimsTimeSurface_3_VOID)
    avg_dist_texture: (TimeTexture_FLOAT)
    current_sphere: (AimsTimeSurface_3_VOID)
    target_avg_dist: (float)
    init_sphere_coords: (tuple of 2 textures) (optional)
    current_sphere_coords: (tuple of 2 textures) (optional)

    Returns
    -------
    refined_sphere: (AimsTimeSurface_3_VOID)
    """
    if init_sphere_coords is not None:
        init_lon, init_lat = init_sphere_coords
    else:
        init_lon, init_lat = sphere_coordinates(init_sphere, inversion)
    if current_sphere_coords is not None:
        current_lon, current_lat = current_sphere_coords
    else:
        current_lon, current_lat = sphere_coordinates(current_sphere, inversion)

    if dist_texture_is_scaled:
        init_sphere_dist = 1
    else:
        init_sphere_dist = (init_sphere.vertex()[init_sphere.polygon()[0][1]]
                            - init_sphere.vertex()[
                            init_sphere.polygon()[0][0]]).norm()

    init_lat = aims.TimeTexture(init_lat[0].arraydata() * 2)
    current_lat = aims.TimeTexture(current_lat[0].arraydata() * 2)
    interpoler = aims.CoordinatesFieldMeshInterpoler(init_sphere,
                                                     current_sphere,
                                                     init_lat,
                                                     init_lon,
                                                     current_lat,
                                                     current_lon)
    interpoler.setDiscontinuityThresholds(200, 200, 0)
    interpoler.project()
    resampled_dist_tex = interpoler.resampleTexture(avg_dist_texture)
    polygon_dist = texture_by_polygon(current_sphere, resampled_dist_tex)
    polygon_sizes = polygon_max_sizes(current_sphere)
    polygon_dist *= polygon_sizes / init_sphere_dist
    refined_poly = numpy.where(numpy.asarray(polygon_dist)
                               >= target_avg_dist)[0]
    if refined_poly.shape[0] != 0:
        refined_sphere = aims.SurfaceManip.refineMeshTri4(
            current_sphere, refined_poly)
    else:
        return current_sphere

    # snap vertices to sphere
    vert2 = numpy.square(numpy.asarray(current_sphere.vertex()))
    dist = numpy.sqrt(numpy.sum(vert2, axis=1))
    radius = numpy.average(dist)
    next_vert = numpy.asarray(refined_sphere.vertex())
    next_vert2 = numpy.square(next_vert)
    next_dist = numpy.sqrt(numpy.sum(next_vert2, axis=1))
    next_vert = next_vert * radius / next_dist.reshape((next_vert.shape[0], 1))
    refined_sphere.vertex().assign([aims.Point3df(x) for x in next_vert])
    return refined_sphere


def sphere_mesh_from_distance_map(init_sphere, avg_dist_texture,
                                  target_avg_dist,
                                  inversion=False,
                                  dist_texture_is_scaled=True):
    """Builds a sphere mesh with vertices density driven by an average distance
    map, coming with another initial sphere mesh, (genrally calculated in a
    different space), and a target length.

    Starting from an icosahedron, this procedure iterates calls to
    refine_sphere_mesh() until the target_avg_dist criterion is reached
    everywhere on the mesh.

    The initial avg_dist_texture can be (and, has better be) scaled according
    to the edges length of the init_sphere mesh polygons. In this case it is
    the ratio of post / pre deformation edges lengths.

    Use case:
    - get an initial sphere mesh (typically an icosphere)
    - get subjects mesh (typically grey/white brain interfaces), which have
      also coordinates maps (output of the Hip Hop toolbox for BrainVISA)
    - resample subjects mesh to the initial sphere. Obtained meshes will be
      very inhomogen
    - build a edges legth map from these subjects resampled meshes
    - use sphere_mesh_from_distance_map to build an adapted template sphere

    Parameters
    ----------
    init_sphere: (AimsTimeSurface_3_VOID)
    avg_dist_texture: (TimeTexture_FLOAT)
    target_avg_dist: (float)
    dist_texture_is_scaled: (bool) (optional)
        If True, the avg_dist_texture is considered to be scaled according to
        the inital sphere triangles edges
        (see aims.SurfaceManip.meshEdgeLengthRatioTexture).
        Default: True

    Return
    ------
    refined_sphere: (AimsTimeSurface_3_VOID)
    """
    vert2 = numpy.square(numpy.asarray(init_sphere.vertex()))
    dist = numpy.sqrt(numpy.sum(vert2, axis=1))
    radius = numpy.average(dist)
    init_sphere_coords = sphere_coordinates(init_sphere, inversion)

    current_sphere = aims.SurfaceGenerator.icosahedron((0, 0, 0), radius)

    next_sphere = None
    step = 0
    while next_sphere is not current_sphere:
        if next_sphere is not None:
            current_sphere = next_sphere
        print 'step:', step
        next_sphere = refine_sphere_mesh(
            init_sphere, avg_dist_texture, current_sphere, target_avg_dist,
            inversion, init_sphere_coords,
            dist_texture_is_scaled=dist_texture_is_scaled)
        step += 1

    return next_sphere