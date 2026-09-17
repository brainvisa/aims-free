
#ifndef AIMS_RESAMPLING_HARMONICCAGERESAMPLER_H
#define AIMS_RESAMPLING_HARMONICCAGERESAMPLER_H

#include <cartodata/volume/volume.h>
#include <aims/mesh/surface.h>
#include <aims/distancemap/stlsort.h> //for Point3dfCompare

namespace aims
{

class HarmonicCageMeshResampler
{
public:
	HarmonicCageMeshResampler(const AimsSurfaceTriangle &cage,
						float spacing);
	HarmonicCageMeshResampler(
      const carto::rc_ptr<carto::Volume<unsigned int> > &cage,
      std::vector<Point3df> &controls,
      short background=0, short border=1, short inside=2);
	~HarmonicCageMeshResampler() { }

public:
	const Point3df &getControl(int ind) const;
	const carto::VolumeRef<float> &get_image_coords(int ind) const;
	void set_keep_image_coords(bool state)
		{ _keep_image_coords = state; }

public:
	void moveControl(int ind, const Point3df &p);
	///from native coordinate to resample coordinates (point version)
	Point3df coordinate(const Point3df &p) const;
	///from native coordinate to resample coordinates (mesh version)
	AimsSurfaceTriangle *coordinates(const AimsSurfaceTriangle &mesh) const;
	void computeCoordinates(float threshold=10e-4);

protected:
	carto::VolumeRef<unsigned int>		_cage;
	std::vector<Point3df>		_controls;
	short				_background;
	short				_border;
	short				_inside;
	bool				_keep_image_coords;
	std::vector<carto::VolumeRef<float> >	_image_coords;
	std::map<Point3df, std::map<int, float>, Point3dfCompare > _weights;
};


inline
Point3df HarmonicCageMeshResampler::coordinate(const Point3df &p) const
{
	int		ind;
	Point3df	p2(rint(p[0]), rint(p[1]), rint(p[2]));
	const std::map<int, float>	&w = (*(_weights.find(p2))).second;
	std::map<int, float>::const_iterator	i, e;

	Point3df res(0, 0, 0);
	for (i = w.begin(), e = w.end(); i != e; ++i)
	{
		ind = (*i).first;
		res += (*i).second * _controls[ind];
	}
	return res;
}


inline const carto::VolumeRef<float> &
HarmonicCageMeshResampler::get_image_coords(int ind) const
{
	return _image_coords[ind];
}

};
 
#endif

