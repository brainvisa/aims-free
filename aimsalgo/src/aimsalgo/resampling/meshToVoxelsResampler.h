/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
 *
 * This software is governed by the CeCILL-B license under
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the
 * terms of the CeCILL-B license as circulated by CEA, CNRS
 * and INRIA at the following URL "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL-B license and that you accept its terms.
 */


#ifndef AIMS_RESAMPLING_MESHTOVOXELSRESAMPLER_H
#define AIMS_RESAMPLING_MESHTOVOXELSRESAMPLER_H

#include <aims/data/data.h>
#include <aims/vector/vector.h>
#include <aims/mesh/surface.h>
#include <cartobase/type/types.h>
#include <aims/bucket/bucket.h>

namespace aims
{

/** 3D rasterization (voxelization) of a mesh
    O : AimsData<unsigned int> or BucketMap<Void>
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
	void fill_header(PythonHeader &hdr, O &output,
		const AimsVector<float,3> & offset, float spacing) const;
};


template<> inline
AimsData<unsigned int> MeshToVoxelsResampler<AimsData<unsigned int> >::
init_data(const AimsVector<float,3> &offset, float spacing,
	unsigned int dimx, unsigned int dimy, unsigned int dimz) const
{
	AimsData<unsigned int>	ima(dimx, dimy, dimz);
	ima = 0; //value of background
	PythonHeader *hdr = dynamic_cast<aims::PythonHeader *>(ima.header());
	MeshToVoxelsResampler<AimsData<unsigned int> >::fill_header(*hdr,
						ima, offset, spacing);
	return ima;
}

template<> inline void  MeshToVoxelsResampler<AimsData<unsigned int> >::
set(AimsData<unsigned int> &output, uint x2, uint y2, uint z2, uint ind) const
{
	output(x2, y2, z2) = ind;
}

template<> inline
aims::BucketMap<Void>  MeshToVoxelsResampler<aims::BucketMap<Void> >::
init_data(const AimsVector<float,3> &offset, float spacing,
		unsigned int, unsigned int, unsigned int) const
{
	aims::BucketMap<Void>	bucketmap;
	MeshToVoxelsResampler<aims::BucketMap<Void> >::fill_header(
			bucketmap.header(), bucketmap, offset, spacing);
	
	return bucketmap;
}

template<> inline
void MeshToVoxelsResampler<aims::BucketMap<Void> >::
set(aims::BucketMap<Void> &output, uint x2, uint y2, uint z2, uint) const
{
	output.insert(Point3d(x2, y2, z2), Void());
}

};
 
#endif
