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

#include <aims/resampling/harmonicCageResampler.h>
#include <aims/resampling/meshToVoxelsResampler.h>
#include <aims/signalfilter/diffusionSmoother.h>
#include <aims/utility/threshold.h>
#include <aims/utility/converter_volume.h>

using namespace aims;
using namespace std;

aims::HarmonicCageMeshResampler::HarmonicCageMeshResampler(
const AimsSurfaceTriangle &cage, float spacing)
{
	//const std::vector<AimsVector<float, 3> > &vertices = cage.vertex();
	MeshToVoxelsResampler<AimsData<unsigned int> >	sampler;
	_cage = sampler.doit(cage, spacing, 26);

	_controls = cage.vertex();
	Point3df offset(0., 0., 0.); //FIXME
	for (unsigned int i = 0; i < _controls.size(); ++i)
	{
		_controls[i] = (_controls[i] - offset) / spacing;
	}
}

aims::HarmonicCageMeshResampler::HarmonicCageMeshResampler(
	const AimsData<unsigned int> &cage, std::vector<Point3df> &controls,
	short background, short border, short inside) : _cage(cage),
	_controls(controls), _background(background),
	_border(border), _inside(inside), _keep_image_coords(false) {}


void aims::HarmonicCageMeshResampler::computeCoordinates(float /*threshold*/)
{
	float		dt = 0.1;
	int		maxiter = 100; //FIXME : use error on enery
	unsigned int	ind;
	std::vector<Point3df>::const_iterator	i, e;

	//only borders
	AimsThreshold<uint, short>	thr(AIMS_EQUAL_TO, _border);
	//only inside voxels
	AimsThreshold<uint, short>	thr2(AIMS_EQUAL_TO, _inside);
  	AimsData<short>	border_mask = thr.bin(_cage);
	for (i = _controls.begin(), e = _controls.end(); i != e; ++i)
	{
		const	Point3df &p = *i;
		std::cout << "p = " << p[0] << ", " << p[1] << ", " << p[2] << std::endl;
		border_mask(p[0], p[1], p[2]) = 1.;
	}
  	AimsData<short>	inside_mask = thr2.bin(_cage);

	//FIXME replace by multiscale solver
	MaskedDiffusionSmoother<float, AimsData<short> > border_smoother(dt);
	MaskedDiffusionSmoother<float>			 inside_smoother(dt);
	border_smoother.set_neumann_value(0);
	border_smoother.setMask(&border_mask, 1.);
	inside_smoother.setMask(&inside_mask);

	if (_keep_image_coords) _image_coords.reserve(_controls.size());

	for (ind = 0, i = _controls.begin(), e = _controls.end();
			i != e; ++i, ++ind)
	{
		AimsData<float> data(_cage.dimX(), _cage.dimY(),
					_cage.dimZ(), _cage.dimT());
		const	Point3df &p = *i;
		data = 0.;
		data(p[0], p[1], p[2]) = 1.;
		AimsData<float> r = border_smoother.doSmoothing(data, maxiter);
	//	AimsData<float> r2 = inside_smoother.doSmoothing(r, maxiter);
		//if (_keep_image_coords) _image_coords.push_back(r2);
		if (_keep_image_coords) _image_coords.push_back(r);
/*
		int x, y, z;
		ForEach3d(r2, x, y, z)
		{
			float	val = r2(x, y, z);
			if (val >= threshold)
				_weights[Point3df(x, y, z)][ind] = val;
		}*/
		break;//FIXME
	}
}


AimsSurfaceTriangle *
aims::HarmonicCageMeshResampler::coordinates(const AimsSurfaceTriangle &mesh)
const
{
	unsigned int i;
	AimsSurfaceTriangle	*mesh2 = new AimsSurfaceTriangle(mesh);
	const vector<Point3df>	& vert = mesh.vertex();
	vector<Point3df>	& vert2 = mesh2->vertex();

	for (i = 0; i < mesh.size(); ++i) vert2[i] = coordinate(vert[i]);
	return mesh2;
}

const Point3df &aims::HarmonicCageMeshResampler::getControl(int ind) const
{
	return _controls[ind];
}

void aims::HarmonicCageMeshResampler::moveControl(int ind, const Point3df &p)
{
	_controls[ind] = p;
}
