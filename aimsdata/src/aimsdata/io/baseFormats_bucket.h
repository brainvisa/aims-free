/*
 *  Data reader class
 */
#ifndef AIMS_IO_BASEFORMATS_BUCKET_H
#define AIMS_IO_BASEFORMATS_BUCKET_H

#include <aims/config/aimsdata_config.h>
#include <aims/io/fileFormat.h>
#include <aims/bucket/bucket.h>


namespace aims
{
  template<class T> class BucketMap;

  template<class T>
  class BckFormat : public FileFormat<AimsBucket<T> >
  {
  public:
    virtual bool read( const std::string & filename, AimsBucket<T> & obj, 
		       const carto::AllocatorContext & context, 
                       carto::Object options );
    virtual bool write( const std::string & filename, 
			const AimsBucket<T> & vol, 
                        carto::Object options = carto::none() );
  };

  template<class T>
  class BckMapFormat : public FileFormat<BucketMap<T> >
  {
  public:
    virtual bool read( const std::string & filename, BucketMap<T> & obj, 
		       const carto::AllocatorContext & context, 
                       carto::Object options );
    virtual bool write( const std::string & filename, 
			const BucketMap<T> & vol, 
                        carto::Object options = carto::none() );
  };

}


#endif
