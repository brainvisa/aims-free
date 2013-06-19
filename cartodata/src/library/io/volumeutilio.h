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
#ifndef CARTODATA_IO_VOLUMEUTILIO_H
#define CARTODATA_IO_VOLUMEUTILIO_H

//--- cartodata ----------------------------------------------------------------
#include <cartodata/volume/volume.h>
//--- soma-io ------------------------------------------------------------------
#include <soma-io/config/soma_config.h>
#include <soma-io/writer/pythonwriter.h>
//--- cartobase ----------------------------------------------------------------
#include <cartobase/config/verbose.h>
#include <cartobase/object/object.h>
//--- system -------------------------------------------------------------------
#include <iostream>
#include <vector>
//------------------------------------------------------------------------------

namespace soma {

  template <typename T>
  class VolumeUtilIO
  {
    public:
      //=== READ ===============================================================
      /// list of properties triggering partial reading and/or borders
      static std::set<std::string> listReadProperties();

      /// Manages all the volumes necessary and returns the final VolumeView
      /// 
      /// This method is called from VolumeFormatReader::createAndRead(). 
      /// It reads all the parameters from options' properties and 
      /// removes them afterwards. It then performs necessary allocation
      /// and reading and returns a fully usable VolumeView.
      static carto::Volume<T>*
      read( carto::rc_ptr<DataSourceInfo> dsi, carto::Object options );

      /// Worker for partial reading case
      static carto::Volume<T>* 
      readPartial( carto::rc_ptr<DataSourceInfo> dsi, 
                  std::vector<int> position, std::vector<int> frame,
                  std::vector<int> borders, carto::Object options );

      /// Worker for full reading case
      static carto::Volume<T>* 
      readFull( carto::rc_ptr<DataSourceInfo> dsi, 
                std::vector<int> borders, carto::Object options );
  };

  //============================================================================
  //   U T I L I T I E S
  //============================================================================
  // to be moved/deleted

  inline void
  printheader( carto::Object hdr )
  {
    soma::PythonWriter  pw;
    pw.attach( std::cout );
    pw.write( hdr );
  }

  inline void
  printTitle( const std::string & title, int verbosity = 0, char symb = '=' )
  {
    if( carto::debugMessageLevel >= verbosity ) {
      int title_size = title.size();
      int col_size = 80;
      int symb_1_count = ( col_size - title_size - 2 ) / 2;
      int symb_2_count = col_size - symb_1_count - title_size - 2;
      std::string out( symb_1_count, symb );
      out += ( " " + title + " " );
      if( symb_2_count > 0 )
        out += std::string( symb_2_count, symb );
      std::cout << std::endl << out << std::endl;
    }
  }

  inline void
  printMessage( const std::string & message, int verbosity = 0 )
  {
    if( carto::debugMessageLevel >= verbosity ) {
      std::cout << message << std::endl;
    }
  }

  inline void
  printEnd( int verbosity = 0, char symb = '=' )
  {
    if( carto::debugMessageLevel >= verbosity ) {
      int col_size = 80;
      std::cout << std::string( col_size, symb ) << std::endl;
    }
  }
}

#endif