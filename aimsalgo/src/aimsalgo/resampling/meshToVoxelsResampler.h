
#ifndef AIMS_RESAMPLING_MESHTOVOXELSRESAMPLER_H
#define AIMS_RESAMPLING_MESHTOVOXELSRESAMPLER_H

#include <cartodata/volume/volume.h>
#include <aims/vector/vector.h>
#include <aims/mesh/surface.h>
#include <cartobase/type/types.h>
#include <aims/bucket/bucket.h>

namespace aims
{

/** 3D rasterization (voxelization) of a mesh
    O : VolumeRef<unsigned int> or BucketMap<Void>
*/
template<typename O>
class MeshToVoxelsResampler
{
public:

	MeshToVoxelsResampler() {}
	virtual ~MeshToVoxelsResampler() {};

public:
	/**
          surface : input mesh
          spacing : voxel size (according to mesh metric)
          connexity : 6 or 26
        */
	O doit(const AimsSurfaceTriangle &surface, float spacing=1.,
					unsigned int connexity=26);

private:
	O init_data(const AimsVector<float,3> &offset, float spacing,
		unsigned int dimx, unsigned int dimy, unsigned int dimz) const;
	void set(O &output, uint x2, uint y2, uint z2, uint ind) const;
	void fill_header(carto::PropertySet &hdr, O &output,
		const AimsVector<float,3> & offset, float spacing) const;
    void setVoxelSize( O & output, float vx, float vy, float vz,
                       float vt ) const;
};


template<> inline
carto::rc_ptr<carto::Volume<unsigned int> >
MeshToVoxelsResampler<carto::rc_ptr<carto::Volume<unsigned int> > >::
init_data(const AimsVector<float,3> &offset, float spacing,
	unsigned int dimx, unsigned int dimy, unsigned int dimz) const
{
	carto::VolumeRef<unsigned int>	ima(dimx, dimy, dimz);
	ima = 0; //value of background
	carto::PropertySet &hdr = ima.header();
	fill_header( hdr, ima, offset, spacing );
	return ima;
}

template<> inline void  MeshToVoxelsResampler<carto::rc_ptr<carto::Volume<unsigned int> > >::
set( carto::rc_ptr<carto::Volume<unsigned int> > &output, uint x2, uint y2,
     uint z2, uint ind ) const
{
  output->at(x2, y2, z2) = ind;
}

template<> inline
aims::BucketMap<Void>  MeshToVoxelsResampler<aims::BucketMap<Void> >::
init_data(const AimsVector<float,3> &offset, float spacing,
		unsigned int, unsigned int, unsigned int) const
{
	aims::BucketMap<Void>	bucketmap;
	MeshToVoxelsResampler<aims::BucketMap<Void> >::fill_header(
      bucketmap.header().value<carto::PropertySet>(), bucketmap, offset,
      spacing);
	
	return bucketmap;
}

template<> inline
void MeshToVoxelsResampler<aims::BucketMap<Void> >::
set(aims::BucketMap<Void> &output, uint x2, uint y2, uint z2, uint) const
{
	output.insert(Point3d(x2, y2, z2), Void());
}

template<> inline
void
MeshToVoxelsResampler<carto::rc_ptr<carto::Volume<unsigned int> > >::
setVoxelSize( carto::rc_ptr<carto::Volume<unsigned int> > & output, float vx,
              float vy, float vz, float vt) const
{
  output->setVoxelSize( vx, vy, vz, vt );
}

template<> inline
void
MeshToVoxelsResampler<aims::BucketMap<Void> >::
setVoxelSize( aims::BucketMap<Void> & output, float vx,
              float vy, float vz, float vt) const
{
  output.setSizeXYZT( vx, vy, vz, vt );
}

}
 
#endif
