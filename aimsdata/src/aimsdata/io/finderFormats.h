/*
 *  Finder class
 */

#ifndef AIMS_IO_FINDERFORMATS_H
#define AIMS_IO_FINDERFORMATS_H


#include <aims/config/aimsdata_config.h>
#include <aims/io/fileFormat.h>


namespace aims
{

  class FinderSpmFormat : public FinderFormat
  {
  public:
    virtual ~FinderSpmFormat() {}
    virtual bool check( const std::string & filename, Finder & f ) const;
  };

#ifdef VIDA_LIB
  class FinderVidaFormat : public FinderFormat
  {
  public:
    virtual ~FinderVidaFormat() {}
    virtual bool check( const std::string & filename, Finder & f ) const;
  };
#endif

#ifdef DICOM_LIB
  class FinderDicomFormat : public FinderFormat
  {
  public:
    virtual ~FinderDicomFormat() {}
    virtual bool check( const std::string & filename, Finder & f ) const;
  };
#endif

#ifdef ECAT_LIB
  class FinderEcatFormat : public FinderFormat
  {
  public:
    virtual ~FinderEcatFormat() {}
    virtual bool check( const std::string & filename, Finder & f ) const;
  };
#endif

#ifdef AIMS_JPEG
  class FinderJpegFormat : public FinderFormat
  {
  public:
    virtual ~FinderJpegFormat() {}
    virtual bool check( const std::string & filename, Finder & f ) const;
  };
#endif

  class FinderGenesisFormat : public FinderFormat
  {
  public:
    virtual ~FinderGenesisFormat() {}
    virtual bool check( const std::string & filename, Finder & f ) const;
  };

  class FinderMeshFormat : public FinderFormat
  {
  public:
    virtual ~FinderMeshFormat() {}
    virtual bool check( const std::string & filename, Finder & f ) const;
  };

  class FinderTriFormat : public FinderFormat
  {
  public:
    virtual ~FinderTriFormat() {}
    virtual bool check( const std::string & filename, Finder & f ) const;
  };

  class FinderMniObjFormat : public FinderFormat
  {
  public:
    virtual ~FinderMniObjFormat() {}
    virtual bool check( const std::string & filename, Finder & f ) const;
  };

  class FinderWavefrontFormat : public FinderFormat
  {
  public:
    virtual ~FinderWavefrontFormat() {}
    virtual bool check( const std::string & filename, Finder & f ) const;
  };

  class FinderSTLFormat : public FinderFormat
  {
  public:
    virtual ~FinderSTLFormat() {}
    virtual bool check( const std::string & filename, Finder & f ) const;
  };

  class FinderTexFormat : public FinderFormat
  {
  public:
    virtual ~FinderTexFormat() {}
    virtual bool check( const std::string & filename, Finder & f ) const;
  };

  class FinderBckFormat : public FinderFormat
  {
  public:
    virtual ~FinderBckFormat() {}
    virtual bool check( const std::string & filename, Finder & f ) const;
  };

  class FinderGraphFormat : public FinderFormat
  {
  public:
    virtual ~FinderGraphFormat() {}
    virtual bool check( const std::string & filename, Finder & f ) const;
  };

  class FinderHieFormat : public FinderFormat
  {
    public:
      virtual ~FinderHieFormat() {}
      virtual bool check( const std::string & filename, Finder & f ) const;
  };

  class FinderImasFormat : public FinderFormat
  {
    public:
      virtual ~FinderImasFormat() {}
      virtual bool check( const std::string & filename, Finder & f ) const;
  };

}


#endif


