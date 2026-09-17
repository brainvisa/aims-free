#include <aims/spam/spam.h>

using namespace aims;

SpamBase::SpamBase() {};
SpamBase::~SpamBase() {};
float SpamBase::huge_logval = -50;
static float small_val = std::exp(-50);


SpamFromLikelihood::SpamFromLikelihood() : SpamBase() {};
SpamFromLikelihood::~SpamFromLikelihood() {};

std::pair<double, double> SpamFromLikelihood::prodlikelihoods(
    const carto::Volume<double> &X, double shift) const
{
  double      logli = 0., li;
  unsigned int    voxels_n = X.getSizeX();
  int      x, y, z;
  float      val;

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
    val = _img_density->at(x, y, z, 0) + small_val;
    if( val > 0 )
      logli += std::log(val);
    else  logli += huge_logval;
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
  double      logli = 0., li;
  unsigned int    voxels_n = X.getSizeX();
  int      x, y, z;

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
