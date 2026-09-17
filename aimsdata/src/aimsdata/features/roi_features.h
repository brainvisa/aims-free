#ifndef AIMS_ROI_FEATURES_H
#define AIMS_ROI_FEATURES_H


#include <iostream>
#include <cartobase/config/verbose.h>
#include <cartobase/object/property.h>
#include <aims/roi/roiIterator.h>
#include <aims/resampling/linearInterpolator.h>
#include <aims/features/point_set_features.h>


namespace aims {

  //---------------//
 //  RoiFeatures  //
//---------------//

class RoiFeatures
{
public:

  void addImageStatistics( const std::string & label,
                           const std::string & filename
                         );
  void addImageStatistics( const std::string & label,
                           const std::string & filename,
                           const std::string & weights_filename
                         );
  void computeFeatures( const carto::rc_ptr< RoiIterator > & );
  void write( std::ostream &out, const std::string &format = "minf" ) const;

protected:

  void writeMinf( std::ostream &out ) const;
  void writeCSV( std::ostream &out ) const;


  typedef std::map< std::string,
                    std::pair< std::string,
                               std::string > > Images_t;

  ScalarSetFeatures _scalarSetFeatures;
  // Associate a label name to an image filename and a weight filename
  Images_t _images;

  carto::rc_ptr< RoiIterator > _roiIterator;
  carto::Object _result;

};


//-----------------------------------------------------------------------------
inline void RoiFeatures::
addImageStatistics( const std::string &label,
                    const std::string &filename )
{
  addImageStatistics( label, filename, "" );
}

} // namespace aims

#endif // ifndef AIMS_ROI_FEATURES_H
