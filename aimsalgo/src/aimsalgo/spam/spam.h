
#ifndef AIMS_SPAM_SPAM_H
#define AIMS_SPAM_SPAM_H

#include <cartodata/volume/volume.h>
#include <aims/vector/vector.h>
#include <string>

namespace aims
{
  class SpamBase
  {
    /**	Spam : Statistical parametric anatomy map
    */
  public:
    SpamBase();
    virtual ~SpamBase();

    inline void	set_img_density(carto::Volume<float> *img_density)
    {
      _img_density = img_density;
    }
    inline void set_bb_talairach_offset(const Point3dd &bb_talairach_offset)
    {
      _bb_talairach_offset = bb_talairach_offset;
    }
    inline void set_bb_talairach_size(const Point3dd &bb_talairach_size)
    {
      _bb_talairach_size = bb_talairach_size;
    }

    virtual std::pair<double, double>
    prodlikelihoods(const carto::Volume<double> &X, double shift=0.) const = 0;

  protected:
    carto::Volume<float>	*_img_density;
    Point3dd			_bb_talairach_offset;
    Point3dd			_bb_talairach_size;
    static float		huge_logval;
  };

  class SpamFromLikelihood : public SpamBase
  {

  public : 
    SpamFromLikelihood();
    virtual ~SpamFromLikelihood();

    virtual std::pair<double, double>
    prodlikelihoods(const carto::Volume<double> &X, double shift=0.) const;
  };

  class Spam : public SpamBase
  {
  public:
    Spam();
    virtual ~Spam();
    
    virtual std::pair<double, double>
    prodlikelihoods(const carto::Volume<double> &X, double shift=0.) const;
  };
};


#endif
