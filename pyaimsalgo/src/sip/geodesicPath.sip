
namespace aims
{

class GeodesicPath
{
%TypeHeaderCode
  #include <aims/geodesicpath/geodesicPath.h>
  #include <pyaims/vector/stdvector.h>
  #include <aims/vector/vector.h>
  #ifndef PYAIMSSIP_VECTOR_U32_DEFINED
  #define PYAIMSSIP_VECTOR_U32_DEFINED
  typedef std::vector<unsigned int> vector_U32;
  #endif
%End

%Docstring
    Geodesic paths or distance maps, using the Dijkstra algorithm

    Ex::

        from soma import aims, aimsalgo
        mesh = aims.read('mesh.gii')
        gp = GeodesicPath(mesh, 0, 0)
        dmap = TimeTexture('FLOAT')
        // get a distance map from vertex no 12
        dmax = gp.distanceMap_1_N_ind(12, dmap[0].data(), 0)
        aims.write(dmax, 'distance.gii')

    It is possible to get distance maps, or paths between two or more points.

%End

  public :

    GeodesicPath( AimsSurfaceTriangle surface , int method, int strain);
    GeodesicPath( AimsSurfaceTriangle surface , TimeTexture_FLOAT texCurv, int method, int strain);
%Docstring
    GeodesicPath(surface, texCurv, method, strain)
    GeodesicPath(surface, method, strain)

    Initializes the geodesic path structures for a given mesh geometry,
    using a constraint map (curvature, typically). Several methods are
    available.

    Parameters
    ----------
    surface: AimsTimeSurface_3_VOID
        mesh to compute geodesic paths or distances on
    texCurv: TimeTexture_FLOAT
        constraint map texture. If not specified, the curvature will be
        computed and used as constraint.
    method: int
        0:
            unconstrained,
        1:
            minimize the constraint map, suitable for sulci using the
            curvature as constraint,
        2:
            maximize constraint map, suitable for gyri using the curvature
            as constraint,
        3:
            unconstrained
    strain: int
        weight factor on the constraints map.
        In practice we use 3 for sulci and gyri.
%End

     ~GeodesicPath();

    vector_U32 shortestPath_1_1_ind(unsigned source, unsigned target);
%Docstring
    shortestPath_1_1_ind(source, target)

    Parameters
    ----------
    source: unsigned
    target: unsigned

    Returns
    -------
    path: vector_U32
%End

    vector_U32 shortestPath_1_1_ind(unsigned source, unsigned target, TimeTexture_S16 subset);
%Docstring
    shortestPath_1_1_ind(source, target, subset)

    Parameters
    ----------
    source: unsigned
    target: unsigned
    subset: TimeTexture_S16

    Returns
    -------
    path: vector_U32
%End

    vector_U32 shortestPath_1_1_1_ind(unsigned source, unsigned middle, unsigned target);
%Docstring
    shortestPath_1_1_1_ind(source, middle, target)

    Parameters
    ----------
    source: unsigned
    middle: unsigned
    target: unsigned

    Returns
    -------
    path: vector_U32
%End

    void shortestPath_1_N_ind(unsigned source, vector_U32 targets,
                              unsigned *target, double *length);
%Docstring
    shortestPath_1_N_ind(source, targets)

    Parameters
    ----------
    source: unsigned
    targets: vector_U32

    Returns
    -------
    target: unsigned
    length: float
%End

    void shortestPath_1_1_ind_xyz(unsigned source, unsigned target,
                                  vector_U32& indice,
                                  vector_POINT3DF& coord3D);
%Docstring
    shortestPath_1_1_ind_xyz(source, target, indice, coord3D)

    Parameters
    ----------
    source: unsigned
    target: unsigned
    indice: vector_U32 (output)
    coord3D: vector_POINT3DF (output)
%End

    void shortestPath_1_1_tex(unsigned source, unsigned target,
                              float texturevalue, TimeTexture_FLOAT &tex);
%Docstring
    shortestPath_1_1_tex(source, target, texturevalue, tex)

    Parameters
    ----------
    source: unsigned
    target: unsigned
    texturevalue: float
    tex: TimeTexture_S16 (output)
%End

    double shortestPath_1_1_len(unsigned source, unsigned target);
%Docstring
    shortestPath_1_1_len(source, target)

    Parameters
    ----------
    source: unsigned
    target: unsigned

    Returns
    -------
    length: float
%End

    void longestPath_1_N_ind(unsigned source, vector_U32 targets,
                             unsigned *target, double *length,
                             int type_distance);
%Docstring
    longestPath_1_N_ind(source, targets, type_distance)

    Parameters
    ----------
    source: unsigned
    targets: vector_U32
    type_distance: int

    Returns
    -------
    target: unsigned
    length: float
%End

    vector_U32 longestPath_N_N_ind(vector_U32 points, int* s, int *d,
                                   double *length, int type_distance);
%Docstring
    longestPath_N_N_ind(points, type_distance)

    Parameters
    ----------
    points: vector_U32
    type_distance: int

    Returns
    -------
    s: int
    d: int
    length: float
%End

    void distanceMap_1_N_ind(unsigned source, vector_FLOAT &distanceMap,
        double *length, int type_distance);
%Docstring
    Compute a distance map from a given point

    Parameters
    ----------
    source: int
        index of the starting point (vertex number)
    distanceMap: vector_FLOAT
        output distance map, the vector can be empty, it
        will be filled with as many values as the mesh vertices number.
    type_distance: int
        0:
            weighted distance,
        1:
            euclidean distance

    Returns
    -------
    length: float
        max distance
%End
};

};

