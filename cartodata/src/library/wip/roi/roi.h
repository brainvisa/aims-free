#ifndef CARTODATA_ROI_ROI_H
#define CARTODATA_ROI_ROI_H

#include <cartobase/smart/rcptr.h>
#include <cartobase/object/headered.h>
#include <cartodata/wip/roi/siteIterator.h>

namespace carto {

class Roi :  public Headered
{
public:

  virtual ~Roi();
}; //Roi


class DiscreteRoi: public Roi
{
public:

  virtual ~DiscreteRoi();

  virtual rc_ptr<SiteIterator> getSiteIterator() const = 0;
  bool holds(const Site  &site);
  virtual int count() const = 0;

}; //DiscreteRoi


class ContinuousRoi: public Roi
{
public:

  virtual ~ContinuousRoi();
  virtual bool contains(const Site &site) const = 0;
}; //ContinuousRoi





class RoiIterator : public virtual RCObject
{
public:

  virtual ~RoiIterator();
  virtual void next() = 0;
  virtual void restart() = 0;
  virtual bool isValid() const = 0;
  virtual rc_ptr<Roi> getRoi() const = 0;

}; //RoiIterator


class ContinuousRoiIterator : public RoiIterator
{
public:
  virtual ~ContinuousRoiIterator();
  virtual rc_ptr<ContinuousRoi> getContinuousRoi() = 0;

}; // ContinuousRoiIterator


class DiscreteRoiIterator : public RoiIterator
{
public:
  
  virtual ~DiscreteRoiIterator();
  virtual rc_ptr<DiscreteRoi> getDiscreteRoi() = 0;

}; // DiscreteRoiIterator




} //namespace carto


#endif // ifndef CARTODATA_ROI_ROI_H
