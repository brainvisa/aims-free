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

// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/resampling/standardreferentials.h>
#include <aims/io/reader.h>
#include <aims/data/pheader.h>
#include <cartobase/config/paths.h>
#include <cartobase/uuid/uuid.h>

using namespace aims;
using namespace carto;
using namespace std;

string StandardReferentials::mniTemplateReferential()
{
  return "Talairach-MNI template-SPM";
}


string StandardReferentials::acPcReferential()
{
  return "Talairach-AC/PC-Anatomist";
}


string StandardReferentials::talairachReferential()
{
  return "Talairach";
}


string StandardReferentials::commonScannerBasedReferential()
{
  return "Scanner-based anatomical coordinates";
}


string StandardReferentials::mniTemplateReferentialID()
{
  return "803552a6-ac4d-491d-99f5-b938392b674b";
}


string StandardReferentials::acPcReferentialID()
{
  return "a2a820ac-a686-461e-bcf8-856400740a6c";
}


string StandardReferentials::commonScannerBasedReferentialID()
{
  return "a397f441-a525-298a-c8f9-26011047eadf";
}


string StandardReferentials::icbm2009cTemplateReferentialID()
{
  return "84b1989b-eb68-8665-0049-8feaf3c22679";
}


namespace
{
  AffineTransformation3d *tal_to_icbm = 0;
  AffineTransformation3d *tal_to_icbm_template = 0;
  Object icbm2009c_header;
}

const AffineTransformation3d & StandardReferentials::talairachToICBM()
{
  if( !tal_to_icbm )
  {
    string tpath = Paths::findResourceFile(
      "transformation/talairach_TO_spm_template_novoxels.trm" );
    Reader<AffineTransformation3d> r( tpath );
    tal_to_icbm = r.read();
  }
  return *tal_to_icbm;
}


const carto::Object StandardReferentials::icbm2009cTemplateHeader()
{
  if( !icbm2009c_header.get() )
  {
    icbm2009c_header = Object::value( Dictionary() );

    AffineTransformation3d shift;
    // invert all axes
    shift.affine()( 0, 0 ) = -1;
    shift.affine()( 1, 1 ) = -1;
    shift.affine()( 2, 2 ) = -1;
    // shift FOV
    shift.affine()( 0, 3 ) = 96;
    shift.affine()( 1, 3 ) = 96;
    shift.affine()( 2, 3 ) = 114;

    // set shift in header
    vector<string> refs( 1, StandardReferentials::mniTemplateReferential() );
    vector<vector<float> > trans( 1 );
    trans[0] = shift.toVector();
    vector<int> dims( 4, 1 );
    dims[0] = 193;
    dims[1] = 229;
    dims[2] = 193;
    vector<float> vs( 4, 1. );

    icbm2009c_header->setProperty( "referentials", refs );
    icbm2009c_header->setProperty( "transformations", trans );
    icbm2009c_header->setProperty( "volume_dimension", dims );
    icbm2009c_header->setProperty( "voxel_size", vs );
    icbm2009c_header->setProperty( "referential",
                                   icbm2009cTemplateReferentialID() );
  }
  return icbm2009c_header;
}


const AffineTransformation3d &
  StandardReferentials::talairachToICBM2009cTemplate()
{
  if( !tal_to_icbm_template )
  {
    tal_to_icbm_template = new AffineTransformation3d;
    AffineTransformation3d shift;
    // invert all axes
    shift.affine()( 0, 0 ) = -1;
    shift.affine()( 1, 1 ) = -1;
    shift.affine()( 2, 2 ) = -1;
    // shift FOV
    shift.affine()( 0, 3 ) = 96;
    shift.affine()( 1, 3 ) = 96;
    shift.affine()( 2, 3 ) = 114;

    *tal_to_icbm_template = shift * talairachToICBM();

    tal_to_icbm_template->header()->setProperty(
      "source_referential", acPcReferentialID() );
    tal_to_icbm_template->header()->setProperty(
      "destination_referential", icbm2009cTemplateReferentialID() );
    Object dheader = Object::value( Dictionary() );
    dheader->copyProperties( icbm2009cTemplateHeader() );
    tal_to_icbm_template->header()->setProperty(
      "destination_header", dheader );
  }
  return *tal_to_icbm_template;
}


bool StandardReferentials::isUUID( const std::string & refName )
{
  if( refName.length() != 36 )
    return false;
  string::size_type i, n = refName.length();
  for( i=0; i<n; ++i )
  {
    if( i == 8 || i == 13 || i == 18 || i == 23 )
    {
      if( refName[i] != '-' )
        return false;
    }
    else
    {
      char c = refName[i];
      if( c < '0' || c > 'f' || (c > '9' && c < 'a' ) )
        return false;
    }
  }
  return true;
}


string StandardReferentials::referentialID( const string & refName,
                                            bool commonScannerBased,
                                            bool genNewIds,
                                            const string & commonSuffix )
{
  if( isUUID( refName ) )
    return refName;
  if( refName == mniTemplateReferential() )
    return mniTemplateReferentialID();
  if( refName == acPcReferential() )
    return acPcReferentialID();
  if( refName == talairachReferential() )
    return mniTemplateReferentialID();
  if( refName == commonScannerBasedReferential() )
  {
    if( commonScannerBased )
      return commonScannerBasedReferentialID();
    else if( !commonSuffix.empty() )
      return refName + commonSuffix;
  }
  if( !genNewIds )
    return refName;  // unchanged
  UUID uuid;
  uuid.generate();
  return uuid.toString();
}

