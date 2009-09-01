/* Copyright (c) 2009 CEA
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

#include <aims/io/giftiheader.h>
extern "C"
{
#include "../gifticlib/gifti_io.h"
}
#include <aims/io/giftiutil.h>
#include <aims/resampling/motion.h>
#include <cartobase/stream/fileutil.h>
#include <cartobase/stream/fdinhibitor.h>
#include <aims/resampling/standardreferentials.h>

using namespace aims;
using namespace carto;
using namespace std;

GiftiHeader::GiftiHeader( const string & name ) :
  PythonHeader(),
  _name( name )
{
}


GiftiHeader::~GiftiHeader()
{
}
#include <cartobase/stream/fdinhibitor.h>


string GiftiHeader::extension() const
{
  return ".gii";
}


const string& GiftiHeader::name() const
{
  return _name;
}

namespace
{

  string niftiReferential( const string & dataspace )
  {
    if( dataspace == "NIFTI_XFORM_MNI_152" )
      return StandardReferentials::mniTemplateReferential();
    else if( dataspace == "NIFTI_XFORM_SCANNER_ANAT" )
      return "Scanner-based anatomical coordinates";
    else if( dataspace == "NIFTI_XFORM_ALIGNED_ANAT" )
      return "Coordinates aligned to another file or to anatomical truth";
    else if( dataspace == "NIFTI_XFORM_TALAIRACH" )
      return StandardReferentials::talairachReferential();
    else if( dataspace == "NIFTI_XFORM_UNKNOWN" )
      return "Arbitrary coordinates";
    else
      return dataspace;
  }

}


bool GiftiHeader::read()
{
  // cout << "GiftiHeader::read\n";

  gifti_image   *gim = 0;
  string fname = _name;
  gifti_set_verb( 0 ); // not enough to make it fail silently...
  if( !FileUtil::fileStat( fname ).empty() )
  {
    // avoid printing anything from gitficlib
    fdinhibitor   fdi( STDERR_FILENO );
    fdi.close();
    gim = gifti_read_image( fname.c_str(), 0 );
    fdi.open();
  }
  if( !gim && fname.substr( fname.length()-4, 4 ) != ".gii" )
  {
    fname += ".gii";
    if( FileUtil::fileStat( fname ).empty() )
      return false;
    // avoid printing anything from gitficlib
    fdinhibitor   fdi( STDERR_FILENO );
    fdi.close();
    gim = gifti_read_image( fname.c_str(), 0 );
    fdi.open();
  }
  if( !gim )
    return false;
  _name = fname;

  int i;
  string mname, mval;

  // read meta-data
  if( gim->meta.length > 0 )
  {
    Object o = Object::value( Dictionary() );
    for( i=0; i<gim->meta.length; ++i )
    {
      mname = gim->meta.name[i];
      if( !gim->meta.value[i] )
        o->setProperty( mname, none() );
      else
      {
        mval = gim->meta.value[i];
        o->setProperty( mname, mval );
      }
    }
    setProperty( "GIFTI_metadata", o );
  }
  if( gim->ex_atrs.length > 0 )
  {
    Object o = Object::value( Dictionary() );
    for( i=0; i<gim->ex_atrs.length; ++i )
    {
      mname = gim->ex_atrs.name[i];
      if( !gim->ex_atrs.value[i] )
        o->setProperty( mname, none() );
      else
      {
        mval = gim->ex_atrs.value[i];
        o->setProperty( mname, mval );
      }
    }
    setProperty( "GIFTI_extra_attributes", o );
  }

  // read labels table
  if( gim->labeltable.length > 0 )
  {
    Object o = Object::value( IntDictionary() );
    for( i=0; i<gim->labeltable.length; ++i )
      o->setArrayItem( gim->labeltable.index[i],
                       Object::value( string( gim->labeltable.label[i] ) ) );
    setProperty( "GIFTI_labels_table", o );
  }

  setProperty( "GIFTI_version", string( gim->version ) );
  if( gim->compressed )
    setProperty( "GIFTI_compressed", int(1) );

  // read arrays information
  int nda = gim->numDA, j;
  int nmesh = 0, nnorm = 0, npoly = 0, ntex = 0, polydim = 0, vnum = 0,
    texlen = 0, texdim = 0, polynum = 0;
  string dtype, otype;
  Object daattr = Object::value( IntDictionary() );
  bool mesh, tex;
  vector<string> texnames;

  for( i=0; i<nda; ++i )
  {
    mesh = false;
    tex = false;
    giiDataArray *da = gim->darray[i];
    switch( da->intent )
    {
    case NIFTI_INTENT_POINTSET:
      ++nmesh;
      mesh = true;
      vnum = da->dims[0];
      break;
    case NIFTI_INTENT_VECTOR:
      ++nnorm;
      break;
    case NIFTI_INTENT_TRIANGLE:
      ++npoly;
      polydim = da->dims[1];
      polynum = da->dims[0];
      break;
    default:
      ++ntex;
      tex = true;
      dtype = niftiDataType( da->datatype );
      texlen = da->dims[0];
      texdim = da->num_dim;
      dtype = giftiTextureDataType( da->datatype, texdim, da->dims );
    }

    // read meta-data
    Object daattr2 = Object::value( Dictionary() );
    if( da->meta.length > 0 )
    {
      Object o = Object::value( Dictionary() );
      for( j=0; j<da->meta.length; ++j )
      {
        mname = da->meta.name[j];
        if( !da->meta.value[j] )
        {
          o->setProperty( mname, none() );
          if( tex && mname == "Name" )
            texnames.push_back( "<unnamed>" );
        }
        else
        {
          mval = da->meta.value[j];
          o->setProperty( mname, mval );
          if( tex && mname == "Name" )
            texnames.push_back( mval );
        }
      }
      daattr2->setProperty( "GIFTI_metadata", o );
    }
    if( da->ex_atrs.length > 0 )
    {
      Object o = Object::value( Dictionary() );
      for( j=0; j<da->ex_atrs.length; ++j )
      {
        mname = da->ex_atrs.name[j];
        if( !da->ex_atrs.value[j] )
          o->setProperty( mname, none() );
        else
        {
          mval = da->ex_atrs.value[j];
          o->setProperty( mname, mval );
        }
      }
      daattr2->setProperty( "GIFTI_extra_attributes", o );
    }

    // read coordinates system info
    if( da->numCS > 0 )
    {
      Object cs = Object::value( Dictionary() );
      Object datarefso = Object::value( vector<string>() );
      Object refso = Object::value( vector<string>() );
      Object trso = Object::value( vector<vector<float> >() );
      vector<string> & refs = refso->value<vector<string> >(),
        & datarefs = datarefso->value<vector<string> >();
      vector<vector<float> > & trs= trso->value<vector<vector<float> > >();
      refs.reserve( da->numCS );
      datarefs.reserve( da->numCS );
      trs.reserve( da->numCS );
      for( j=0; j<da->numCS; ++j )
      {
        string dataspace = da->coordsys[i]->dataspace;
        dataspace = niftiReferential( dataspace );
//         o->setProperty( "referential", dataspace );
        if( mesh && nmesh == 1 && j == 0
          && dataspace != "Arbitrary coordinates" )
          // share ref/transfo information in the main header
          setProperty( "referential", dataspace );
        string xformspace = niftiReferential( da->coordsys[i]->xformspace );
        vector<float> m( 16 );
        int k;
        for( k=0; k<4; ++k )
        {
          m[ k*4 ] = da->coordsys[j]->xform[k][0];
          m[ k*4+1 ] = da->coordsys[j]->xform[k][1];
          m[ k*4+2 ] = da->coordsys[j]->xform[k][2];
          m[ k*4+3 ] = da->coordsys[j]->xform[k][3];
        }
        if( !Motion( m ).isIdentity() || dataspace != xformspace )
        {
          trs.push_back( m );
          datarefs.push_back( dataspace );
          refs.push_back( xformspace );
        }
      }
      if( !trs.empty() )
      {
        cs->setProperty( "referentials", refso );
        cs->setProperty( "data_referentials", datarefso );
        cs->setProperty( "transformations", trso );

        if( mesh && nmesh == 1 )
        {
          // share ref/transfo information in the main header
          setProperty( "referentials", cs->getProperty( "referentials") );
          setProperty( "transformations",
                      cs->getProperty( "transformations") );
        }
        daattr2->setProperty( "GIFTI_coordinates_systems", cs );
      }
    }

    daattr2->setProperty( "intent",
                          string( gifti_intent_to_string( da->intent ) ) );
    if( tex )
      daattr2->setProperty( "data_type", niftiDataType( da->datatype ) );
    daattr2->setProperty( "ind_ord", da->ind_ord );
    vector<int> dims( da->num_dim );
    for( j=0; j<da->num_dim; ++j )
      dims[j] = da->dims[j];
    daattr2->setProperty( "dimensions", dims );
    if( daattr2->size() != 0 )
      daattr->setArrayItem( i, daattr2 );
  }
  if( daattr->size() != 0 )
    setProperty( "GIFTI_dataarrays_info", daattr );

  setProperty( "file_type", "GIFTI" );
  if( nmesh > 0 )
  {
    setProperty( "vertex_number", vnum );
    setProperty( "nb_t_pos", std::max(nmesh, ntex) );
    setProperty( "polygon_dimension", polydim );
    if( polydim == 2 )
      setProperty( "object_type", "Segments" );
    else if( polydim == 4 )
      setProperty( "object_type", "Mesh4" );
    else
      setProperty( "object_type", "Mesh" );
    setProperty( "polygon_number", polynum );
    if( !texnames.empty() )
      setProperty( "texture_names", texnames );
  }
  else if( ntex > 0 )
  {
    setProperty( "object_type", "Texture" );
  }
  if( ntex > 0 )
  {
    setProperty( "data_type", dtype );
    setProperty( "texture_dimension", texdim );
    if( nmesh == 0 )
      setProperty( "vertex_number", texlen );
    if( nmesh > 0 && dtype != "VOID" )
    {
      vector<string> pdt;
      pdt.reserve( 2 );
      pdt.push_back( dtype );
      pdt.push_back( "VOID" );
      setProperty( "possible_data_types", pdt );
    }
  }
  else
    setProperty( "data_type", "VOID" );

  gifti_free_image( gim );

  return true;
}


