#ifndef AIMS_IO_ECATSINOHEADER_H
#define AIMS_IO_ECATSINOHEADER_H

#include <aims/config/aimsdata_config.h>
#include <string>
#include <aims/data/pheader.h>

#define _typeINIT 1
#define ECAT_QUANT 100
#define ECAT_LABEL 200


namespace aims
{

  /** The descriptor class of the Ecat header.
      The programmer can only read an existing header. It cannot write a new 
      one.
      Ecat data are signed 16 bits 4D objects.
  */
  class AIMSDATA_API EcatSinoHeader : public aims::PythonHeader
  { 
  public:
    enum MultiType{
      MULTIBED,
      MULTIFRAME,
      MULTIGATE
    } ;

    EcatSinoHeader(const std::string& name );
    EcatSinoHeader(  int type, const std::string & name );
    virtual ~EcatSinoHeader();

    /// Get type of items (ECAT_QUANT or ECAT_LABEL)
    int itemType() const { return _type; }

    /// Get the file name of the header
    std::string name() const { return _name; }
  
    virtual std::string extension() const { return( ".S" ); }
    virtual std::set<std::string> extensions() const;

    /// Read the header
    void read( ) ;
    void write() const;
    void setHeader( const std::vector<PythonHeader>& subHeaders ) ;
    PythonHeader getSingleBedDataFrame( int bedDataOrFrame ) const ;

  private:
    /// name Data
    /// File name
    std::string _name;
    /// Type of items
    int  _type;
  };

}

#endif

