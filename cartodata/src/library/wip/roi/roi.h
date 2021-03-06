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
