// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/resampling/harmonicCageResampler.h>
#include <aims/resampling/meshToVoxelsResampler.h>
#include <aims/signalfilter/diffusionSmoother.h>
#include <aims/utility/threshold.h>
#include <aims/utility/converter_volume.h>

using namespace aims;
using namespace carto;
using namespace std;

aims::HarmonicCageMeshResampler::HarmonicCageMeshResampler(
const AimsSurfaceTriangle &cage, float spacing)
{
	//const std::vector<AimsVector<float, 3> > &vertices = cage.vertex();
	MeshToVoxelsResampler<rc_ptr<Volume<unsigned int> > >	sampler;
	_cage = sampler.doit(cage, spacing, 26);

	_controls = cage.vertex();
	Point3df offset(0., 0., 0.); //FIXME
	for (unsigned int i = 0; i < _controls.size(); ++i)
	{
		_controls[i] = (_controls[i] - offset) / spacing;
	}
}

aims::HarmonicCageMeshResampler::HarmonicCageMeshResampler(
	const rc_ptr<Volume<unsigned int> > &cage, std::vector<Point3df> &controls,
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
  	VolumeRef<short>	border_mask = thr.bin(_cage);
	for (i = _controls.begin(), e = _controls.end(); i != e; ++i)
	{
		const	Point3df &p = *i;
		std::cout << "p = " << p[0] << ", " << p[1] << ", " << p[2] << std::endl;
		border_mask(p[0], p[1], p[2]) = 1.;
	}
  	VolumeRef<short>	inside_mask = thr2.bin(_cage);

	//FIXME replace by multiscale solver
	MaskedDiffusionSmoother<float, VolumeRef<short> > border_smoother(dt);
	MaskedDiffusionSmoother<float>			 inside_smoother(dt);
	border_smoother.set_neumann_value(0);
	border_smoother.setMask(border_mask, 1.);
	inside_smoother.setMask(inside_mask);

	if (_keep_image_coords) _image_coords.reserve(_controls.size());

	for (ind = 0, i = _controls.begin(), e = _controls.end();
			i != e; ++i, ++ind)
	{
		VolumeRef<float> data( _cage->getSize() );
		const	Point3df &p = *i;
		data = 0.;
		data(p[0], p[1], p[2]) = 1.;
		VolumeRef<float> r = border_smoother.doSmoothing(data, maxiter);
	//	VolumeRef<float> r2 = inside_smoother.doSmoothing(r, maxiter);
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
