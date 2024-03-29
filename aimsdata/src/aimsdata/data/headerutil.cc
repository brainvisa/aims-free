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

#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/data/headerutil.h>
#include <aims/transformation/affinetransformation3d.h>

using namespace aims;
using namespace carto;
using namespace std;

void carto::setOrientationInformation( Object hdr, const string & orient_ )
{
  if( hdr.isNull() )
    return;

  // first get volume dimensions
  vector<int> vdims(3, 1);
  Object p;
  try
  {
    // new carto style
    p = hdr->getProperty( "sizeX" );
    vdims[0] = int( rint( p->getScalar() ) );
    p = hdr->getProperty( "sizeY" );
    vdims[1] = int( rint( p->getScalar() ) );
    p = hdr->getProperty( "sizeZ" );
    vdims[2] = int( rint( p->getScalar() ) );
  }
  catch( ... )
  {
    // old aims style
    p = hdr->getProperty( "volume_dimension" );
    if( p->size() >= 1 )
    {
      vdims[0] = int( rint( p->getArrayItem( 0 )->getScalar() ) );
      if( p->size() >= 2 )
      {
        vdims[1] = int( rint( p->getArrayItem( 1 )->getScalar() ) );
        if( p->size() >= 3 )
        {
          vdims[2] = int( rint( p->getArrayItem( 2 )->getScalar() ) );
        }
      }
    }
  }

  // orientation names aliases
  map<string, string> aliases;
  aliases["RAS"] = "abs: -1 -1 -1";
  aliases["LAS"] = "abs: 1 -1 -1";
  aliases["RPS"] = "abs: -1 1 -1";
  aliases["LPS"] = "abs: 1 1 -1";
  aliases["RAI"] = "abs: -1 -1 1";
  aliases["LAI"] = "abs: 1 -1 1";
  aliases["RPI"] = "abs: -1 1 1";
  aliases["LPI"] = "abs: 1 1 1";

  aliases["ARS"] = "abs: 0 -1 0  -1 0 0  0 0 -1";
  aliases["PRS"] = "abs: 0 -1 0  1 0 0  0 0 -1";
  aliases["ALS"] = "abs: 0 1 0  -1 0 0  0 0 -1";
  aliases["PLS"] = "abs: 0 1 0  1 0 0  0 0 -1";
  aliases["ARI"] = "abs: 0 -1 0  -1 0 0  0 0 1";
  aliases["PRI"] = "abs: 0 -1 0  1 0 0  0 0 1";
  aliases["ALI"] = "abs: 0 1 0  -1 0 0  0 0 1";
  aliases["PLI"] = "abs: 0 1 0  1 0 0  0 0 1";

  aliases["RSA"] = "abs: -1 0 0  0 0 -1  0 -1 0";
  aliases["RIA"] = "abs: -1 0 0  0 0 1  0 -1 0";
  aliases["RSP"] = "abs: -1 0 0  0 0 -1  0 1 0";
  aliases["RIP"] = "abs: -1 0 0  0 0 1  0 1 0";
  aliases["LSA"] = "abs: 1 0 0  0 0 -1  0 -1 0";
  aliases["LIA"] = "abs: 1 0 0  0 0 1  0 -1 0";
  aliases["LSP"] = "abs: 1 0 0  0 0 -1  0 1 0";
  aliases["LIP"] = "abs: 1 0 0  0 0 1  0 1 0";

  aliases["SAR"] = "abs: 0 0 -1  0 -1 0  -1 0 0";
  aliases["IAR"] = "abs: 0 0 1  0 -1 0  -1 0 0";
  aliases["SAL"] = "abs: 0 0 -1  0 -1 0  1 0 0";
  aliases["IAL"] = "abs: 0 0 1  0 -1 0  1 0 0";
  aliases["SPR"] = "abs: 0 0 -1  0 1 0  -1 0 0";
  aliases["IPR"] = "abs: 0 0 1  0 1 0  -1 0 0";
  aliases["SPL"] = "abs: 0 0 -1  0 1 0  1 0 0";
  aliases["IPL"] = "abs: 0 0 1  0 1 0  1 0 0";

  aliases["ASR"] = "abs: 0 0 -1  -1 0 0  0 -1 0";
  aliases["PSR"] = "abs: 0 0 -1  1 0 0  0 -1 0";
  aliases["AIR"] = "abs: 0 0 -1  -1 0 0  0 1 0";
  aliases["PIR"] = "abs: 0 0 -1  1 0 0  0 1 0";
  aliases["ASL"] = "abs: 0 0 1  -1 0 0  0 -1 0";
  aliases["PSL"] = "abs: 0 0 1  1 0 0  0 -1 0";
  aliases["AIL"] = "abs: 0 0 1  -1 0 0  0 1 0";
  aliases["PIL"] = "abs: 0 0 1  1 0 0  0 1 0";

  aliases["SRA"] = "abs: 0 -1 0  0 0 -1  -1 0 0";
  aliases["IRA"] = "abs: 0 -1 0  0 0 -1  1 0 0";
  aliases["SLA"] = "abs: 0 1 0  0 0 -1  -1 0 0";
  aliases["ILA"] = "abs: 0 1 0  0 0 -1  1 0 0";
  aliases["SRP"] = "abs: 0 -1 0  0 0 1  -1 0 0";
  aliases["IRP"] = "abs: 0 -1 0  0 0 1  1 0 0";
  aliases["SLP"] = "abs: 0 1 0  0 0 1  -1 0 0";
  aliases["ILP"] = "abs: 0 1 0  0 0 1  1 0 0";

  string orient = orient_;

  map<string, string>::const_iterator im = aliases.find( orient_ );
  if( im != aliases.end() )
    orient = im->second;

  // retreive existing storage_to_memory matrix, or setup a more or less
  // default one
  AffineTransformation3d stom;
  try
  {
    p = hdr->getProperty( "storage_to_memory" );
    stom = p;
  }
  catch( ... )
  {
    // default orientation: Nifti/MNI-like, and ratio convention
    stom.setTranslation( Point3df( 0., vdims[1] - 1., vdims[2] - 1. ) );
    stom.rotation()(1, 1) = -1.;
    stom.rotation()(2, 2) = -1.;
  }

  // parse the orientation string: split into "words"
  vector<string> sorient;
  string::size_type pos, np = orient.length(), pos0 = 0;
  set<char> spaces;
  spaces.insert( ' ' );
  spaces.insert( '\t' );
  spaces.insert( '\n' );
  spaces.insert( '\r' );
  spaces.insert( ',' );
  spaces.insert( ';' );
  set<char>::const_iterator nospace = spaces.end();
  bool absolute = false;

  for( pos=0; pos<np; ++pos )
  {
    if( pos0 != pos && spaces.find( orient[ pos0 ] ) != nospace )
      pos0 = pos;
    if( pos0 != pos && spaces.find( orient[pos] ) != nospace )
    {
      sorient.push_back( orient.substr( pos0, pos-pos0 ) );
      pos0 = pos;
      if( sorient.size() == 1 && sorient[0].length() >= 4
        && sorient[0].substr( 0, 4 ) == "abs:" )
      {
        // absolute mode
        absolute = true;
        if( sorient[0].length() == 4 )
          sorient.clear();
        else
          sorient[0].erase( 0, 4 );
      }
    }
  }
  if( pos0 != pos && spaces.find( orient[ pos0 ] ) == nospace )
  {
    sorient.push_back( orient.substr( pos0, pos-pos0 ) );
    if( sorient.size() == 1 && sorient[0].length() >= 4
      && sorient[0].substr( 0, 4 ) == "abs:" )
    {
      // absolute mode
      absolute = true;
      if( sorient[0].length() == 4 )
        sorient.clear();
      else
        sorient[0].erase( 0, 4 );
    }
  }

  if( absolute )
  {
    // absolute mode: start with ID matrix
    stom.setToIdentity();
  }

  // output matrix to convolve
  AffineTransformation3d omatrix;
  vector<string>::const_iterator is, es = sorient.end();
  vector<double> omat;
  omat.reserve( 16 );
  for( is=sorient.begin(); is!=es; ++is )
  {
    try
    {
      double w;
      stringTo<double>( *is, w );
      omat.push_back( w );
    }
    catch( ... )
    {
      break;
    }
  }
  if( omat.size() == 3 )
  {
    omatrix.rotation()(0,0) = omat[0];
    omatrix.rotation()(1,1) = omat[1];
    omatrix.rotation()(2,2) = omat[2];
  }
  else if( omat.size() == 9 )
  {
    for( int i=0; i<3; ++i )
      for( int j=0; j<3; ++j )
        omatrix.rotation()(i, j) = omat[j+i*3];
  }
  else if( omat.size() == 16 )
  {
    for( int i=0; i<3; ++i )
      for( int j=0; j<3; ++j )
        omatrix.rotation()(i, j) = omat[j+i*4];
    omatrix.matrix()(0, 3) = omat[3];
    omatrix.matrix()(1, 3) = omat[7];
    omatrix.matrix()(2, 3) = omat[11];
  }
  else if( omat.size() == 0 )
  {
    // symbolic orientation
    for( is=sorient.begin(); is!=es; ++is )
    {
      string o = stringLower( *is );
      pos = o.find( "flip" );
      if( pos != string::npos )
      {
        // flip axis
        o.replace( pos, 4, "" );
        if( o == "x" )
          omatrix.rotation()(0,0) = -1.;
        else if( o == "y" )
          omatrix.rotation()(1,1) = -1.;
        else if( o == "z" )
          omatrix.rotation()(2,2) = -1.;
        else
        {
          cout << "could not understand the orientation specifications: "
              << orient << endl;
          return;
        }
      }
      else
      {
        // other orientation info
        if( sorient.size() != 1 )
        {
          cout << "could not understand the orientation specifications: "
              << orient << endl;
          return;
        }
        if( o == "neuro" || o == "neurological" || o == "left-to-right" )
        {
          if( stom.rotation()(0,0) == 1. )
            omatrix.rotation()(0,0) = -1.;
        }
        else if( o == "radio" || o == "radiological" || o == "right-to-left" )
        {
          if( stom.rotation()(0,0) == -1. )
            omatrix.rotation()(0,0) = -1.;
        }
        else
        {
          cout << "could not understand the orientation specifications: "
              << orient << endl;
          return;
        }
      }
    }
  }
  else
  {
    cout << "could not understand the orientation specifications: " << orient
      << endl;
    return;
  }

  // fix translation if needed
  for( int i=0; i<3; ++i )
    if( omatrix.rotation()(i,i) == -1. )
      omatrix.matrix()(i, 3) = vdims[i]-1.;

  // compose to existing one
  stom *= omatrix;
  hdr->setProperty( "storage_to_memory", stom.toVector() );
}

