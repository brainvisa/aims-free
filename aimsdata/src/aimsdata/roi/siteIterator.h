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
 
#ifndef AIMSDATA_ROI_SITEITERATOR_H
#define AIMSDATA_ROI_SITEITERATOR_H

#include <cartobase/object/object.h>
#include <cartodata/roi/siteIterator.h>
#include <aims/bucket/bucketMap.h>
#include <aims/mesh/surface.h>
#include <aims/mesh/texture.h>
#include <aims/vector/vector.h>
#include <string>
#include <vector>


namespace carto {

class ISite : public Site
{
  std::vector<int> _coord;

public:

  ISite( const Point3d &p3d );
  ISite( int x, int y, int z);
  inline virtual  ~ISite() {};
  virtual int size() const;
  virtual double at( int index ) const;
  virtual int intAt( int index ) const;
  virtual void set( int index, double value ); 
  virtual void set( int index, int value);  
  
}; // ISite 


class DSite : public Site
{

  std::vector<double> _coord;

public:

  DSite( const Point3df &p3d);
  DSite( float x, float y, float z);
  inline virtual ~DSite() {};
  virtual int size() const;
  virtual double at( int index ) const;
  virtual void set( int index, double value );   
  
}; // DSite





template <>
class SiteIteratorOf< aims::BucketMap<Void> > :  public SiteIterator
{
  
  rc_ptr< aims::BucketMap<Void> > _bucketMap;
  aims::BucketMap<Void>::iterator _itBck;
  aims::BucketMap<Void>::Bucket *  _bucket;
  aims::BucketMap<Void>::Bucket::iterator _itPoints;
  bool _useLabel;
  int _label;
  
 public:
  
  SiteIteratorOf( aims::BucketMap<Void> &bckMap );
  SiteIteratorOf( std::string filename );
  SiteIteratorOf( aims::BucketMap<Void> &bckMap, int label );
  SiteIteratorOf( std::string filename, int label );
  
  virtual inline ~SiteIteratorOf() {};

  virtual void next();
  virtual void restart();
  virtual bool isValid() const;
  virtual rc_ptr<Site> getSite();
  virtual void writeSite();
};


template <>
class SiteIteratorOf< AimsSurfaceTriangle > :  public SiteIterator
{

 protected:

  rc_ptr< AimsSurfaceTriangle > _mesh;
  std::vector< Point3df >::iterator _itVertex;
  
 public:
  
  SiteIteratorOf( AimsSurfaceTriangle  &mesh );

  SiteIteratorOf( std::string filename );
  
  virtual inline ~SiteIteratorOf(){};

  virtual void next();
  virtual void restart();
  virtual bool isValid() const;
  virtual rc_ptr<Site> getSite();
  virtual void writeSite();
};

template<>
class SiteIteratorOf< Texture1d > : public SiteIteratorOf< AimsSurfaceTriangle >
{ 

  Texture1d _tex;
  int _currentVertexNum;
  bool _useLabel;
  float _label;
  
 public:

  SiteIteratorOf( std::string meshFilename, std::string texFilename ); 
  SiteIteratorOf( std::string meshFilename, std::string texFilename, float label);

  virtual inline ~SiteIteratorOf(){};

  virtual void restart();
  virtual void next();
};

} //namespace carto


#endif // ifndef AIMSDATA_ROI_SITEITERATOR_H
