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


#include <cartobase/stream/fileutil.h>
#include <aims/io/roigtmR.h>
#include <aims/io/aimsGraphR.h>
#include <aims/io/reader.h>
#include <graph/graph/greader.h>

using namespace aims;
using namespace std;
using namespace carto;

RoiGtmReader::RoiGtmReader( const string& name, ReadMode readRoi )
  : _name( name ),
    _readRoi( readRoi )
{
}


RoiGtmReader::~RoiGtmReader()
{
}


string RoiGtmReader::removeExtension( const string& name )
{
  string res = name;
  string ext="",ext2="";
  if ( res.length() > 4 )
    ext = res.substr( int(res.length() - 4), 4 );
  if (ext == string(".arg") )
    res = res.substr( 0, res.length() - 4 );
  return res;
}


void RoiGtmReader::read( RoiGtm& thing )
{ 
  // Read the graph
  GraphReader	gr( _name, thing.syntax() );
  if(thing.hasProperty("roi_basename") )
    thing.removeProperty("roi_basename") ;
  if(thing.hasProperty("selector_basename") )
    thing.removeProperty("selector_basename") ;
  
  cerr << "Reading" << endl ;
  gr.read( thing );
  cerr << "Read" << endl ;
  thing.editionSet();

  // If necessary read Roi file
  if( ReadRoi() )
    {
      string maskname; thing.getProperty("gtm_mask_basename", maskname);
      cout << "Mask name is " << maskname << endl ;
      cout << "File base name is " <<  FileUtil::dirname( _name ) << endl ;
      
      AimsRoi *roi = new AimsRoi;
      Reader<Graph> rrd( FileUtil::dirname( _name ) + "/" + maskname );
      rrd.read( *roi );
      thing.setMaskRoi( roi );
    }
}
