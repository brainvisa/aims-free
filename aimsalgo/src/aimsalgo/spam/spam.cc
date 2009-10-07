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

#include <aims/spam/spam.h>

using namespace aims;

SpamBase::SpamBase() {};
SpamBase::~SpamBase() {};
float SpamBase::huge_logval = -50;


SpamFromLikelihood::SpamFromLikelihood() : SpamBase() {};
SpamFromLikelihood::~SpamFromLikelihood() {};

std::pair<double, double> SpamFromLikelihood::prodlikelihoods(
		const carto::Volume<double> &X, double shift) const
{
	double			logli = 0., li;
	unsigned int		voxels_n = X.getSizeX();
	int			x, y, z;
	float			val;
	
	for (unsigned int i = 0; i < voxels_n; ++i)
	{
		x = (int) floorf(X.at(i, 0, 0, 0) - _bb_talairach_offset[0]);
		y = (int) floorf(X.at(i, 1, 0, 0) - _bb_talairach_offset[1]);
		z = (int) floorf(X.at(i, 2, 0, 0) - _bb_talairach_offset[2]);

		if ((x < 0) or (y < 0) or (z < 0) or
			(x >= _bb_talairach_size[0]) or
			(y >= _bb_talairach_size[1]) or
			(z >= _bb_talairach_size[2]))
		{
			logli += huge_logval;
			continue;
		}
		val = _img_density->at(x, y, z, 0);
		if (val)
			logli += std::log(val);
		else	logli += huge_logval;
	}
	logli /= voxels_n;
	li = std::exp(logli + shift);
	return std::pair<double, double>(logli, li);
}


Spam::Spam() : SpamBase() {};
Spam::~Spam() {};

std::pair<double, double>
Spam::prodlikelihoods(const carto::Volume<double> &X, double shift) const
{
	double			logli = 0., li;
	unsigned int		voxels_n = X.getSizeX();
	int			x, y, z;
	
	for (unsigned int i = 0; i < voxels_n; ++i)
	{
		x = (int) floorf(X.at(i, 0, 0, 0) - _bb_talairach_offset[0]);
		y = (int) floorf(X.at(i, 1, 0, 0) - _bb_talairach_offset[1]);
		z = (int) floorf(X.at(i, 2, 0, 0) - _bb_talairach_offset[2]);

		if ((x < 0) or (y < 0) or (z < 0) or
			(x >= _bb_talairach_size[0]) or
			(y >= _bb_talairach_size[1]) or
			(z >= _bb_talairach_size[2]))
		{
			logli += huge_logval;
			continue;
		}
		logli += _img_density->at(x, y, z, 0);
	}
	logli /= voxels_n;
	li = std::exp(logli + shift);
	return std::pair<double, double>(logli, li);
}
