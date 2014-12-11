
namespace aims
{

  class FastMarching_Volume_S16 /PyName=FastMarching/
  {
%Docstring
Fast marching algorithm implementation, for images.

The fast marching is a propagation algorithm which is typically used to
perform distance maps. It may use a speed map to locally change the
propagation speed and distance. It may also be used to perform a Voronoi
diagam, and to get Voronoi regions boundaries.

It is used the following way:

* instantiate a FastMarching object, with specified connectivity
* if needed, set the speed map (or inverse speed map) using setSpeedMap()
  or setInvSpeedMap()
* propagate using one of the doit() methods. It will return the distanc
  map.
* Voronoi and boundaries can then be retreived using voronoiVol(),
  midInterface() or midInterfaceVol()
%End

%TypeHeaderCode
#include <aims/distancemap/fastmarching.h>
namespace aims
{
  typedef aims::FastMarching<carto::Volume<int16_t> > FastMarching_Volume_S16;
}
%End

  public:
    FastMarching_Volume_S16(const std::string &connectivity="26",
                            bool mid_interface=false);
%Docstring
FastMarching(connectivity="26", process_mid_interface=False)

FastMarching(process_mid_interface)

(2nd form: deprecated constructor, with connectivity 26.)

Parameters
----------
connectivity: string
    "26", "18", "6". The connectivity type is only used to build the interfaces
    between the work region and the seeds: the fast marching itself only uses
    6-connectivity.
process_mid_interface: bool
    If process_mid_interface is false (the default), voronoi boundaries will
    not be available after propagation.
%End

%MethodCode
sipCpp = new aims::FastMarching_Volume_S16(
    aims::Connectivity::type_from_string(*a0),
    a1);
%End

    FastMarching_Volume_S16(bool);
%MethodCode
sipIsErr = PyErr_Warn(PyExc_DeprecationWarning,
                      "the constructor aims.FastMarching(bool) is deprecated, "
                      "use aims.FastMarching(\"26\", bool) instead.");
if(!sipIsErr) {
  sipCpp = new aims::FastMarching_Volume_S16( aims::Connectivity::CONNECTIVITY_26_XYZ, a0 );
 }
%End

    ~FastMarching_Volume_S16();

    rc_ptr_Volume_FLOAT doit( rc_ptr_Volume_S16 vol, 
      short worklabel, short inlabel, short outlabel ) /ReleaseGIL/;
%Docstring
doit(vol, worklabel, inlabel, outlabel)

Perform fast marching propagation from a label image "vol".

This is a simplified interface to the other, more general, doit()
method.

Propagation will take place in the region "worklabel", from seeds
"inlabel" and "outlabel".

Parameters
----------
vol: rc_ptr_Volume_S16
    label volume to build distance map from. Contains seeds, propagation (work) region, and possibly forbidden regions
worklabel: int16
inlabel: int16
outlabel: int16

Returns
-------
distance_map: rc_ptr_Volume_FLOAT
    resulting distance map
%End

    rc_ptr_Volume_FLOAT doit( rc_ptr_Volume_S16 vol,
      const set_S16 & worklabels, const set_S16 & seedlabels ) /ReleaseGIL/;
%Docstring
doit(vol, worklabels, seedlabels)

Perform fast marching propagation from a label image "vol".

Propagation will take place in the regions listed in "worklabels", from
all seeds in the "seedlabels" list.

Parameters
----------
vol: rc_ptr_Volume_S16
    label volume to build distance map from. Contains seeds, propagation (work) region, and possibly forbidden regions
worklabels: set_S16
seedlabels: set_S16

Returns
-------
distance_map: rc_ptr_Volume_FLOAT
    resulting distance map
%End

    rc_ptr_Volume_FLOAT midInterfaceVol (short label1, short label2) const
      throw ( );
%Docstring
midInterfaceVol(label1, label2)

get the interface between Voronoi regions label1 and label2, as a volume.

the mid_interface option must have been used when instantiating the
FastMarching object, and propagation maust have taken place.
%End

    const BucketMap_FLOAT & midInterface( short label1, short label2 ) const;
%Docstring
midInterface(label1, label2)

get the interface between Voronoi regions label1 and label2, as a bucket.

The mid_interface option must have been used when instantiating the
FastMarching object, and propagation maust have taken place.
%End

    rc_ptr_Volume_S16 voronoiVol() const;
%Docstring
voronoiVol()

get the resulting Voronoi regions (after propagation)
%End

    void setVerbose( bool x );
    bool verbose() const;
    void setSpeedMap( rc_ptr_Volume_FLOAT speed );
%Docstring
setSpeedMap(speed)

Sets an initialized speed map. The inverse speed will be deduced from it.

Parameters
----------
speed: rc_ptr_Volume_FLOAT
%End

    void setInvSpeedMap( rc_ptr_Volume_FLOAT invspeed );
%Docstring
setInvSpeedMap(invspeed)

Sets an initialized inverse speed map (overrides any previous speed map).

Parameters
----------
invspeed: rc_ptr_Volume_FLOAT
%End

    void clearSpeedMap();

    SIP_PYTUPLE midInterfaceLabels() const;
%Docstring
midInterfaceLabels()

Voronoi interfaces labels.
Given as a vector of pairs of labels

Returns
-------
labels: tuple of tuples
    in the shape ((label1, label2), (label3, label4), ...)
%End

%MethodCode
  std::vector<std::pair<short, short> > mil = sipCpp->midInterfaceLabels();
  sipRes = PyTuple_New( mil.size() );
  int i, n = (int) mil.size();
  for( i=0; i<n; ++i )
  {
    PyObject *item = PyTuple_New( 2 );
    PyTuple_SetItem( item, 0, PyInt_FromLong( mil[i].first ) );
    PyTuple_SetItem( item, 1, PyInt_FromLong( mil[i].second ) );
    PyTuple_SetItem( sipRes, i, item );
  }
%End
  };


  class FastMarching_BucketMap_S16
  {
%Docstring
Fast marching algorithm implementation, for buckets.

The fast marching is a propagation algorithm which is typically used to
perform distance maps. It may use a speed map to locally change the
propagation speed and distance. It may also be used to perform a Voronoi
diagam, and to get Voronoi regions boundaries.

It is used the following way:

* instantiate a FastMarching object, with specified connectivity
* if needed, set the speed map (or inverse speed map) using setSpeedMap()
  or setInvSpeedMap()
* propagate using one of the doit() methods. It will return the distanc
  map.
* Voronoi and boundaries can then be retreived using voronoiVol(),
  midInterface() or midInterfaceVol()
%End

%TypeHeaderCode
#include <aims/distancemap/fastmarching.h>
namespace aims
{
  typedef aims::FastMarching<BucketMap<int16_t> > FastMarching_BucketMap_S16;
}
%End

  public:
    FastMarching_BucketMap_S16(const std::string &connectivity="26",
                               bool mid_interface=false);
%Docstring
FastMarching_BucketMap_S16(connectivity="26", process_mid_interface=False)

FastMarching_BucketMap_S16(process_mid_interface)

(2nd form: deprecated constructor, with connectivity 26.)


Parameters
----------
connectivity: string
    "26", "18", "6". The connectivity type is only used to build the interfaces
    between the work region and the seeds: the fast marching itself only uses
    6-connectivity.
process_mid_interface: bool
    If process_mid_interface is false (the default), voronoi boundaries will
    not be available after propagation.

%End

%MethodCode
sipCpp = new aims::FastMarching_BucketMap_S16(
    aims::Connectivity::type_from_string(*a0),
    a1);
%End

    FastMarching_BucketMap_S16(bool);
%MethodCode
sipCpp = new aims::FastMarching_BucketMap_S16( aims::Connectivity::CONNECTIVITY_26_XYZ, a0 );
%End

    ~FastMarching_BucketMap_S16();
    rc_ptr_BucketMap_FLOAT doit( rc_ptr_BucketMap_S16 vol,
      short worklabel, short inlabel, short outlabel ) /ReleaseGIL/;
%Docstring
doit(vol, worklabel, inlabel, outlabel)

Perform fast marching propagation from a label bucket "vol".

This is a simplified interface to the other, more general, doit()
method.

Propagation will take place in the region "worklabel", from seeds
"inlabel" and "outlabel".

Parameters
----------
vol: rc_ptr_BucketMap_S16
    label bucket to build distance map from. Contains seeds, propagation (work) region. Propagation outside the bucket will be forbidden.
worklabel: int16
inlabel: int16
outlabel: int16

Returns
-------
distance_map: rc_ptr_Volume_FLOAT
    resulting distance map
%End

    rc_ptr_BucketMap_FLOAT doit( rc_ptr_BucketMap_S16 vol,
      const set_S16 & worklabels, const set_S16 & seedlabels ) /ReleaseGIL/;
%Docstring
doit(vol, worklabels, seedlabels)

Perform fast marching propagation from a label bucket "vol".

Propagation will take place in the regions listed in "worklabels", from
all seeds in the "seedlabels" list.

Parameters
----------
vol: rc_ptr_BucketMap_S16
    label bucket to build distance map from. Contains seeds, propagation (work) region, and possibly forbidden regions
worklabels: set_S16
seedlabels: set_S16

Returns
-------
distance_map: rc_ptr_Volume_FLOAT
    resulting distance map
%End

    rc_ptr_Volume_FLOAT midInterfaceVol (short label1, short label2) const
      throw ( );
%Docstring
midInterfaceVol(label1, label2)

get the interface between Voronoi regions label1 and label2, as a volume.

the mid_interface option must have been used when instantiating the
FastMarching object, and propagation maust have taken place.
%End

    const BucketMap_FLOAT & midInterface( short label1, short label2 ) const;
%Docstring
midInterface(label1, label2)

get the interface between Voronoi regions label1 and label2, as a bucket.

The mid_interface option must have been used when instantiating the
FastMarching object, and propagation maust have taken place.
%End

    rc_ptr_BucketMap_S16 voronoiVol() const;
%Docstring
voronoiVol()

get the resulting Voronoi regions (after propagation)
%End

    void setVerbose( bool x );
    bool verbose() const;
    void setSpeedMap( rc_ptr_BucketMap_FLOAT speed );
%Docstring
setSpeedMap(speed)

Sets an initialized speed map. The inverse speed will be deduced from it.

Parameters
----------
speed: rc_ptr_BucketMap_FLOAT
%End

    void setInvSpeedMap( rc_ptr_BucketMap_FLOAT invspeed );
%Docstring
setInvSpeedMap(invspeed)

Sets an initialized inverse speed map (overrides any previous speed map).

Parameters
----------
invspeed: rc_ptr_BucketMap_FLOAT
%End

    void clearSpeedMap();

    SIP_PYTUPLE midInterfaceLabels() const;
%Docstring
midInterfaceLabels()

Voronoi interfaces labels.
Given as a vector of pairs of labels

Returns
-------
labels: tuple of tuples
    in the shape ((label1, label2), (label3, label4), ...)
%End

%MethodCode
  std::vector<std::pair<short, short> > mil = sipCpp->midInterfaceLabels();
  sipRes = PyTuple_New( mil.size() );
  int i, n = (int) mil.size();
  for( i=0; i<n; ++i )
  {
    PyObject *item = PyTuple_New( 2 );
    PyTuple_SetItem( item, 0, PyInt_FromLong( mil[i].first ) );
    PyTuple_SetItem( item, 1, PyInt_FromLong( mil[i].second ) );
    PyTuple_SetItem( sipRes, i, item );
  }
%End
};

};

