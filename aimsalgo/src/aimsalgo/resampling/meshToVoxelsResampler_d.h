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

#include <aims/resampling/meshToVoxelsResampler.h>
#include <aims/distancemap/stlsort.h> //for Point3dfCompare
#include <cartobase/object/object.h>
#include <aims/resampling/motion.h>
#include <map>
#include <float.h>

namespace aims
{

template<typename O> void MeshToVoxelsResampler<O>::fill_header(
PythonHeader &hdr, O &output, const AimsVector<float,3> & offset, float spacing)
const
{
	std::vector<float>		voxel_size(3, spacing);
	std::vector<std::string>	refs(1, "to_mesh");
	std::vector<std::vector<float> >trans;
	Motion				mot;
	mot.setToIdentity();
	mot.translation() = offset;
	trans.push_back( mot.toVector() );

	hdr.setProperty("voxel_size", voxel_size);
	hdr.setProperty("referentials", refs);
	hdr.setProperty("transformations", trans);
	output.setSizeXYZT(spacing, spacing, spacing, 1.);
}

/** Ad-hoc covering of voxels, then test if there own to the resulting mesh
    voxelization by the way proposed in :
    "An Accurate Method for Voxelizing Polygon Meshes", J. Huang, R. Yagel,
    V. Filippov, Y. Kurzion
*/
template<typename O>
O MeshToVoxelsResampler<O>::doit(const AimsSurfaceTriangle &m, float spacing,
						unsigned int connexity)
{
	const std::vector<AimsVector<float, 3> >	&vertices = m.vertex();
	const std::vector<AimsVector<uint, 3> >		&polygons = m.polygon();
	float	xmin, ymin, zmin, xmax, ymax, zmax;
	unsigned int 	vsize = vertices.size();
	unsigned int	psize = polygons.size();
	float		r2;
	float		tc;

	if (connexity == 26)
	{
		r2 = 0.75;
		tc = 0.5;
	}
	else if (connexity == 6)
	{
		r2 = 0.25;
		tc = sqrt(2.) / 4.;
	}

	xmin = ymin = zmin = FLT_MAX;
	xmax = ymax = zmax = -FLT_MAX;

	// bounding box
	for (unsigned int i = 0; i < vsize; ++i)
	{
		const AimsVector<float, 3> &v = vertices[i];
		if (v[0] < xmin) xmin = v[0];
		if (v[0] > xmax) xmax = v[0];
		if (v[1] < ymin) ymin = v[1];
		if (v[1] > ymax) ymax = v[1];
		if (v[2] < zmin) zmin = v[2];
		if (v[2] > zmax) zmax = v[2];
	}
	double	invspacing = 1./spacing;
	// borders min/max are not optimals
	xmin -= 1;
	ymin -= 1;
	zmin -= 1;
	AimsVector<float,3> offset(xmin, ymin, zmin);
	unsigned int dimx = (unsigned int) round((xmax - xmin) * invspacing) +2;
	unsigned int dimy = (unsigned int) round((ymax - ymin) * invspacing) +2;
	unsigned int dimz = (unsigned int) round((zmax - zmin) * invspacing) +2;

	O output = this->init_data(offset, spacing, dimx, dimy, dimz);
	
	AimsVector<float, 3>	t[3];

	for (unsigned int ind = 1; ind < psize + 1; ++ind)
	{
		const AimsVector<uint,3> &p = polygons[ind - 1];

		t[0] = (vertices[p[0]] - offset) * invspacing;
		t[1] = (vertices[p[1]] - offset) * invspacing;
		t[2] = (vertices[p[2]] - offset) * invspacing;

		AimsVector<float, 3> t1;
		// vertices (sphere with a radius of sqrt(r2))
		for (unsigned int j = 0; j < 3; ++j)
		{
			t1[0] = ceil(t[j][0] - 0.5);
			t1[1] = ceil(t[j][1] - 0.5);
			t1[2] = ceil(t[j][2] - 0.5);
			for (float dx = -1; dx <= 1; dx += 1)
			for (float dy = -1; dy <= 1; dy += 1)
			for (float dz = -1; dz <= 1; dz += 1)
			{
				uint x2 = (uint) ceil(t1[0] + dx);
				uint y2 = (uint) ceil(t1[1] + dy);
				uint z2 = (uint) ceil(t1[2] + dz);
				float dix = x2 - t1[0];
				float diy = y2 - t1[1];
				float diz = z2 - t1[2];
				if (dix * dix + diy * diy + diz * diz <= r2)
					this->set(output, x2, y2, z2, ind);
			}
		}

		// edges (cylinder with a radius of sqrt(r2))
		float	diff[3][3];
		uint	usize[3];
		AimsVector<float, 3> t3, t4, t5;
		for (unsigned int j = 0; j < 3; ++j)
		{
			const AimsVector<float, 3>	&t2 = t[(j + 1) % 3];
			float dix = (t[j][0] - t2[0] - 0.5);
			float diy = (t[j][1] - t2[1] - 0.5);
			float diz = (t[j][2] - t2[2] - 0.5);
			diff[j][0] = dix;
			diff[j][1] = diy;
			diff[j][2] = diz;
			float size2 = dix * dix + diy * diy + diz * diz;
			if (!size2) break;
			//usizej : not optimal value (for speed considerations)
			uint usizej = (uint) round(sqrt(size2) * 2.);
			usize[j] = usizej;
			std::map<Point3df, bool, Point3dfCompare> map;
			for (uint l = 0; l <= usizej; ++l)
			{
				float lambda = ((float) l) / usizej;
				t3 = t[j] * lambda + t2 * (1 - lambda);
				t3[0] = ceil(t3[0]);
				t3[1] = ceil(t3[1]);
				t3[2] = ceil(t3[2]);
				for (float dx = -1; dx <= 1; dx += 1)
				for (float dy = -1; dy <= 1; dy += 1)
				for (float dz = -1; dz <= 1; dz += 1)
				{
					Point3df	p;
					p[0] = t3[0] + dx;
					p[1] = t3[1] + dy;
					p[2] = t3[2] + dz;
					map[p] = true;
				}
			}
			std::map<Point3df,bool>::const_iterator	i, e;
			for (i = map.begin(), e = map.end(); i != e; ++i)
			{
				const Point3df &p0 = (*i).first;
				Point3df p = t[j] - p0;
				float vx = diy * p[2] - diz * p[1]; 
				float vy = diz * p[0] - dix * p[2]; 
				float vz = dix * p[1] - diy * p[0];
				if ((vx * vx + vy * vy + vz * vz) / size2 <= r2)
					this->set(output, (uint) p0[0],
						(uint) p0[1],
						(uint) p0[2], ind);
			}
		}

		// fill triangles (between 2 planes at a distance of dlnk)
		std::map<Point3df, bool, Point3dfCompare> map;
		for (uint l = 0; l <= usize[0]; ++l)
		{
			float lambda = ((float) l) / usize[0];
			float msize = usize[1] * lambda;
			t3 = t[0] * (1 - lambda) + t[1] * lambda;
			t4 = t[0] * (1 - lambda) + t[2] * lambda;
			for (uint m = 0; m < msize; ++m)
			{
				float mu = ((float) m) / msize;
				t5 = t3 * (1 - mu) + t4 * mu;
				t5[0] = ceil(t5[0] - 0.5);
				t5[1] = ceil(t5[1] - 0.5);
				t5[2] = ceil(t5[2] - 0.5);
				for (float dx = -1; dx <= 1; dx += 1)
				for (float dy = -1; dy <= 1; dy += 1)
				for (float dz = -1; dz <= 1; dz += 1)
				{
					Point3df	p;
					p[0] = t5[0] + dx;
					p[1] = t5[1] + dy;
					p[2] = t5[2] + dz;
					map[p] = true;
				}
			}
		}
		float nx = -diff[0][1] * diff[1][2] + diff[0][2] * diff[1][1];
		float ny = -diff[0][2] * diff[1][0] + diff[0][0] * diff[1][2];
		float nz = -diff[0][0] * diff[1][1] + diff[0][1] * diff[1][0];
		float norm = sqrt(nx * nx + ny * ny + nz * nz);
		nx /= norm;
		ny /= norm;
		nz /= norm;
		float nx2 = nx * (nx > 0 ? 1 : -1);
		float ny2 = ny * (ny > 0 ? 1 : -1);
		float nz2 = nz * (nz > 0 ? 1 : -1);
		float dlnk;
		float nsum = (nx2 + ny2 + nz2);
		if (connexity == 26)
			// dlnk = sqrt(3) * cos(alpha) / 2.
			//      = sqrt(3) * <N, K > / (||K|| * 2.)
			//      = <N, [1, 1, 1] > / 2
			dlnk = tc * nsum;
		else if (connexity == 6)
		{
			// dlkn = cos(beta) / 2 = cos(pi/4. - alpha) / 2
			//      = sqrt(2) * (cos(alpha)+sin(alpha))/4
			//      = sqrt(2) * (cos(alpha)+sqrt(1-cos(alpha)^2))/4
			float cosalpha = nsum / sqrt(3.);
			dlnk = tc * (cosalpha + sqrt(1 - cosalpha * cosalpha));
		}
		std::map<Point3df,bool>::const_iterator	i, e;
		for (i = map.begin(), e = map.end(); i != e; ++i)
		{
			const Point3df	&p0 = (*i).first;
			Point3df	p = p0 - t[0];
			float d = nx * p[0] + ny * p[1] + nz * p[2];
			if (d < 0) d = -d;
			if (d <= dlnk)
				this->set(output, (uint) p0[0],
					(uint) p0[1],
					(uint) p0[2], ind);
		}
	}
	return output;
}

};
