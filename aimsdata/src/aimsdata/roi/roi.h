/*
 *  Region Of Interest
 */
#ifndef AIMS_ROI_ROI_H
#define AIMS_ROI_ROI_H

#include <aims/config/aimsdata_config.h>
#include <map>

#include <cartodata/volume/volume.h>
#include <aims/bucket/bucket.h>
#include <graph/graph/graph.h>
#include <aims/roi/hie.h>


namespace aims {


class AIMSDATA_API AimsRoi : public Graph
{
public:

  AimsRoi( Hierarchy *hierarchy=NULL );
  virtual ~AimsRoi() { }

  void setLabel( const carto::VolumeRef< short> & label);

  void setHierarchy( Hierarchy *hie ) { hRoi = hie; }
  Hierarchy* getHierarchy() { return hRoi;};

  std::set<Vertex*> VertByNameAndDescendantName(std::string& name);

  void setBucketFilename( const std::string& );

  void data2bucket();
  void data2bucket( const std::map <int,std::string> &nameList );
  void bucket2data( int borderWidth=0 );
  AimsRoi* clipBucket( int x, int y, int z, int lx, int ly, int lz);



private:

  Hierarchy	*hRoi;
  Hierarchy	 hDefault;
};

}

#endif
