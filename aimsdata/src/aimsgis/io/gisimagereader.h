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

#ifndef PLUGINGIS_GISIMAGEREADER_H
#define PLUGINGIS_GISIMAGEREADER_H
//--- soma-io ------------------------------------------------------------------
#include <soma-io/config/soma_config.h>
#include <soma-io/image/imagereader.h>                               // heritage
#include <soma-io/datasource/chaindatasource.h>                      // heritage
#include <soma-io/reader/itemreader.h>                        // read + byteswap
//#include <soma-io/writer/itemwriter.h>                       // write + byteswap
//--- cartobase ----------------------------------------------------------------
#include <cartobase/object/object.h>                          // header, options
//--- system -------------------------------------------------------------------
#include <memory>
#include <vector>
//------------------------------------------------------------------------------

namespace soma
{
  class DataSourceInfo;
  
  /**
   * \todo doc ?
   */
  template<typename T>
  class GisImageReader : public ImageReader<T>, protected ChainDataSource
  {
    public:
      //========================================================================
      //   C O N S T R U C T O R S
      //========================================================================
      GisImageReader();
      //GisImageReader( DataSourceInfo & dsi );
      //GisImageReader( const GisImageReader<T> & );
      virtual ~GisImageReader();
      
      //========================================================================
      //   I M A G E R E A D E R
      //========================================================================
      virtual void read( T * dest, DataSourceInfo & dsi,
                         std::vector<int> & pos,  /* taille 4 : x,y,z,t */
                         std::vector<int> & size, /* taille 4 : x,y,z,t */
                         std::vector<int> stride = std::vector<int>(),
                         int level = 0, 
                         carto::Object options = carto::none() );
      
      //virtual void resetParams();
      virtual void updateParams( DataSourceInfo & dsi );
      
    protected:
      //========================================================================
      //   D A T A S O U R C E
      //========================================================================
      virtual DataSource* clone() const;
      virtual int iterateMode() const;
      virtual offset_t size() const;
      virtual offset_t at() const;
      virtual bool at( offset_t pos );
      virtual long readBlock( char * data, unsigned long maxlen );
      virtual long writeBlock( const char * data, unsigned long len );
      virtual int getch();
      virtual int putch( int ch );
      virtual bool ungetch( int ch );
      virtual bool allowsMemoryMapping() const;
      virtual bool setpos( int x, int y = 0, int z = 0, int t = 0 );
      
    protected:
      std::auto_ptr<ItemReader<T> > _itemr;
      //std::auto_ptr<ItemWriter<T> > _itemw;
  };
  
}

#endif