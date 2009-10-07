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


#ifndef AIMS_RESAMPLING_HARMONICCAGERESAMPLER_H
#define AIMS_RESAMPLING_HARMONICCAGERESAMPLER_H

#include <aims/data/data.h>
#include <aims/mesh/surface.h>
#include <aims/distancemap/stlsort.h> //for Point3dfCompare

namespace aims
{

class HarmonicCageMeshResampler
{
public:
	HarmonicCageMeshResampler(const AimsSurfaceTriangle &cage,
						float spacing);
	HarmonicCageMeshResampler(const AimsData<unsigned int> &cage,
			std::vector<Point3df> &controls,
			short background=0, short border=1, short inside=2);
	~HarmonicCageMeshResampler() { }

public:
	const Point3df &getControl(int ind) const;
	const AimsData<float> &get_image_coords(int ind) const;
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
	AimsData<unsigned int>		_cage;
	std::vector<Point3df>		_controls;
	short				_background;
	short				_border;
	short				_inside;
	bool				_keep_image_coords;
	std::vector<AimsData<float> >	_image_coords;
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


inline const AimsData<float> &
HarmonicCageMeshResampler::get_image_coords(int ind) const
{
	return _image_coords[ind];
}

};
 
#endif

