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


#include <aims/distancemap/advection.h>
#include <aims/connectivity/component.h>
#include <aims/bucket/bucket.h>
#include <aims/io/writer.h>
#include <string>
#include <sstream>
#include <math.h>

using namespace aims;
using namespace carto;

GradientAdvection::GradientAdvection() { }

GradientAdvection::~GradientAdvection() { }

void GradientAdvection::doit(const carto::VolumeRef<int16_t>  &vol,
				const AimsData<float> &gradX,
				const AimsData<float> &gradY,
				const AimsData<float> &gradZ, int seed)
{
	std::set<int16_t> seeds;
	seeds.insert(seed);
	doit(vol, gradX, gradY, gradZ, seeds);
}


void GradientAdvection::doit(const carto::VolumeRef<int16_t>  & vol,
				const AimsData<float> &gradX,
				const AimsData<float> &gradY,
				const AimsData<float> &gradZ,
				const std::set<int16_t> & seeds)
{
	AimsData<int16_t>		img(vol);
	AimsData<float>			data1(img.dimX(), img.dimY(),
						img.dimZ(), img.dimT());
	AimsData<float>			data2(img.dimX(), img.dimY(),
						img.dimZ(), img.dimT());
	AimsData<float>			*src, *dst, *swap;
	int				x, y, z;
	std::set<int16_t>::const_iterator	is, es;
	src = &data1; dst = &data2;
	int	n = img.dimX() - 2;
	int	m = img.dimY() - 2;

	int i = 0;
	for (is = seeds.begin(), es = seeds.end(); is != es; ++is, ++i)
	{
		data1 = 0.;
		data2 = 0.;
		BucketMap<Void>				dots1;
		BucketMap<Void>				dots2;
		BucketMap<Void>				*cur = &dots1;
		BucketMap<Void>				*next = &dots2;
		BucketMap<Void>				*swap_bck;
		BucketMap<Void>::Bucket::const_iterator	ib, eb;
		//std::cout << "seed = " << *is << std::endl;
		Connectivity cd(0, 0, Connectivity::CONNECTIVITY_26_XYZ);
		int	cdi, cdn = cd.nbNeighbors();
		float	pre_sum = 0.;
	  	ForEach3d(img, x, y, z)
		{
			if ((x == 0) or (y == 0) or
				(x == img.dimX() - 1) or (y == img.dimY() - 1))
			{
				std::cout << x << ", " << y << ", " << z << std::endl;
				(*src)(x, y, z) = -1.;
				continue;
			}
			if (img(x, y, z) == 0) continue;
			Point3d		p(x, y, z);
			if (img(x, y, z) == *is)
				(*src)(x, y, z) = 1.;
			else {
				(*src)(x, y, z) = -1.;
				for (int cdi = 0; cdi < cdn; ++cdi)
				{
					Point3d	p2 = p + cd.xyzOffset(cdi);
					if ((p2[0] < 0) or (p2[0] >= img.dimX())) continue;
					if ((p2[1] < 0) or (p2[1] >= img.dimY())) continue;
					if ((p2[2] < 0) or (p2[2] >= img.dimZ())) continue;
					(*src)(p2[0], p2[1], p2[2]) = -1.;
				}
				continue;
			}
			pre_sum += 1.;
			std::pair<Point3d, Void>	pair;
			pair.first = p;
			(*cur)[0].insert(pair);
			for (int cdi = 0; cdi < cdn; ++cdi)
			{
				std::pair<Point3d, Void>	pair;
				pair.first = p + cd.xyzOffset(cdi);
				(*cur)[0].insert(pair);
			}
		}
		Writer<AimsData<float> >	writer("src.ima");
		writer.write(*src);

		float sw, se, nw, ne, xd, yd;
		int id, jd;
		float dtspas = 0.5;

		for (int t = 0; t < 200; ++t)
		{
		//begin
		/*
	  	ForEach3d(img, x, y, z)
		{
			if (img(x, y, z) != *is) continue;
			(*src)(x, y, z) = 1.;
			pre_sum += 1.;
			Point3d		p(x, y, z);
			std::pair<Point3d, Void>	pair;
			pair.first = p;
			(*cur)[0].insert(pair);
			for (int cdi = 0; cdi < cdn; ++cdi)
			{
				std::pair<Point3d, Void>	pair;
				pair.first = p + cd.xyzOffset(cdi);
				(*cur)[0].insert(pair);
			}
		} */ //end

        	BucketMap<Void>::Bucket	&b = (*((*cur).begin())).second;
		(*dst) = 0.;
		float	sum = 0.;
		float	c;
        	for( ib=b.begin(), eb=b.end(); ib!=eb; ++ib )
		{
			const Point3d	&p = (*ib).first;
			if ((p[0] < 0) or (p[0] >= img.dimX())) continue;
			if ((p[1] < 0) or (p[1] >= img.dimY())) continue;
			if ((p[2] < 0) or (p[2] >= img.dimZ())) continue;
			if ((*src)(p[0], p[1], p[2]) == -1) continue;
			float gX = -dtspas * gradX(p[0], p[1], p[2]);
			float gY = -dtspas * gradY(p[0], p[1], p[2]);
			float gN = sqrt(gX * gX + gY * gY);
			gX /= gN;
			gY /= gN;
			xd = p[0] - gX;
			yd = p[1] - gY;

			if (xd < 0.5) xd = 0.5;
			if (yd < 0.5) yd = 0.5;
			if (xd > n + 0.5) xd = n + 0.5;
			if (yd > m + 0.5) yd = m + 0.5;
			id = (int) xd;
			jd = (int) yd;
			sw = (1. + id - xd) * (1. + jd - yd);
			se = (xd - id) * (1. + jd - yd);
			nw = (1. + id - xd) * (yd - jd);
			ne = (xd - id) * (yd - jd);
			//std::cout << "sw = " << sw << ", " << se << ", " << nw << ", " << ne << std::endl;
			std::cout << (*src)(id, jd) << ", " << 
				(*src)(id + 1, jd) << ", " << 
				(*src)(id, jd + 1) << ", " <<
				(*src)(id + 1, jd + 1) << std::endl;
			/*std::cout << "p = " << p[0] << ", " << p[1] << ", " << p[2] << std::endl;
			std::cout << "xd/yd = " << xd << ", " << yd << std::endl;
			std::cout << "id/jd = " << id << ","  << jd << std::endl;*/

			float e, f, g, h;
			e = (*src)(id, jd, p[2]);
			f = (*src)(id + 1, jd, p[2]);
			g = (*src)(id, jd + 1, p[2]);
			h = (*src)(id + 1, jd + 1, p[2]);
			c = 0.;
			if (e != -1) c += sw * e;
			if (f != -1) c += se * f;
			if (g != -1) c += nw * g;
			if (h != -1) c += ne * h;
			std::cout << (*src)(p[0], p[1], p[2]) << "->" << (*dst)(p[0], p[1], p[2]) << std::endl;
			if (c > 0.3) c = 1.;
			else c = 0;

			(*dst)(p[0], p[1], p[2]) = c;
			sum += c;
			if (c != 0)
			{
				std::pair<Point3d, Void>	pair;
				pair.first = p;
				for (int cdi = 0; cdi < cdn; ++cdi)
				{
					std::pair<Point3d, Void>	pair;
					/* si deja traite on ne l'ajoute pas*/
					Point3d	p2 = p + cd.xyzOffset(cdi);
					pair.first = p2;
					if ((p2[0] < 0) or (p2[0] >= img.dimX())) continue;
					if ((p2[1] < 0) or (p2[1] >= img.dimY())) continue;
					if ((p2[2] < 0) or (p2[2] >= img.dimZ())) continue;

					if ((*src)(p2[0], p2[1], p2[2]) == 0)
						(*next)[0].insert(pair);
				}
			}
		}
	  	ForEach3d((*dst), x, y, z)
		{
			if ((*src)(x, y, z) != 0)
			{
				(*dst)(x, y, z) = (*src)(x, y, z);
				continue;
			}/*
			if ((*dst)(x, y, z) == 0) continue;
			(*dst)(x, y, z) = ((*dst)(x, y, z) * pre_sum) / sum;
			std::cout << "dst = " << (*dst)(x, y, z)  << std::endl;*/
		}
		/*std::cout << ">>>>>>>size = " << (*next)[0].size() << std::endl;*/
		if ((*next)[0].size() == 0) break;
		b.erase(b.begin(), b.end());
		swap_bck = cur;
		cur = next;
		next = swap_bck;
		std::ostringstream s;
		s << "plop_" << i << "_" << t << ".ima";
		Writer<AimsData<float> >	writer(s.str());
		writer.write(*dst);
		swap = src;
		src = dst;
		dst = swap;
		}
	std::ostringstream s;
	s << "final_" << i << ".ima";
	Writer<AimsData<float> >	writer2(s.str());
	writer2.write(*src);
	}
};
