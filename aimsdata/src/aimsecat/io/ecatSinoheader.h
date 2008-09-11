/* Copyright (c) 1995-2005 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */

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

