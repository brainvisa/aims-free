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

#include <aims/io/spmattribconverter.h>
#include <aims/resampling/standardreferentials.h>
#include <aims/vector/vector.h>
#include <aims/resampling/motion.h>
#include <stdexcept>

using namespace aims;
using namespace carto;
using namespace std;

void aims::spmAttributesConverter( GenericObject & hdr )
{
  if( hdr.hasProperty( "transformations" ) )
  {
    // clear obsolete props
    if( hdr.hasProperty( "origin" ) )
      hdr.removeProperty( "origin" );
    if( hdr.hasProperty( "spm_origin" ) )
      hdr.removeProperty( "spm_origin" );
    if( hdr.hasProperty( "spm_radio_convention" ) )
      hdr.removeProperty( "spm_radio_convention" );
    if( hdr.hasProperty( "spm_normalized" ) )
      hdr.removeProperty( "spm_normalized" );
    if( hdr.hasProperty( "spm_spm2_normalization" ) )
      hdr.removeProperty( "spm_spm2_normalization" );
    return; // already done
  }

  Point3df  vs( 1, 1, 1);
  vector<float> vsf;
  if( hdr.getProperty( "voxel_size", vsf ) )
  {
    if( vsf.size() >= 1 )
    {
      vs[0] = vsf[0];
      if( vsf.size() >= 2 )
      {
        vs[1] = vsf[1];
        if( vsf.size() >= 3 )
          vs[2] = vsf[2];
      }
    }
  }

  bool  nrm = false;
  try
  {
    Object norm;
    norm = hdr.getProperty( "spm_normalized" );
    if( norm )
    {
      hdr.removeProperty( "spm_normalized" );
      nrm = (bool) norm->getScalar();
    }
  }
  catch( exception & )
  {
  }
  vector<float>     origin;
  bool  hasorg = hdr.getProperty( "origin", origin );
  // *origin fields should be removed in a later version
  if( hasorg )
    hdr.removeProperty( "origin" );
  if( hdr.hasProperty( "spm_origin" ) )
    hdr.removeProperty( "spm_origin" );
  if( hdr.hasProperty( "spm_radio_convention" ) )
    hdr.removeProperty( "spm_radio_convention" );
  if( hdr.hasProperty( "spm_spm2_normalization" ) )
    hdr.removeProperty( "spm_spm2_normalization" );

  Motion  mot;
  mot.setToIdentity();
  vector<string>    refs;

  if( nrm )
  {
    refs.push_back( StandardReferentials::mniTemplateReferential() );

    if( !hasorg )
    {
      mot.matrix()(0, 3) = 78.;
      mot.matrix()(1, 3) = 76.;
      mot.matrix()(2, 3) = 85.;
      /* (78,76,85) is the standard origin which is already taken
      into account in the ACPC->MNI transformation, so we must not
      count it again here */
      mot.rotation()( 0, 0 ) = -1.;
      mot.rotation()( 1, 1 ) = -1.;
      mot.rotation()( 2, 2 ) = -1.;
    }
    else
    {
      // SPM-like origin
      mot.matrix()(0, 3) = origin[0] * vs[0];
      mot.matrix()(1, 3) = origin[1] * vs[1];
      mot.matrix()(2, 3) = origin[2] * vs[2];
      mot.rotation()( 0, 0 ) = -1.;
      mot.rotation()( 1, 1 ) = -1.;
      mot.rotation()( 2, 2 ) = -1.;
    }
  }
  else
  {
    if( !hasorg )
      return;
    // SPM-like origin
    mot.rotation()( 0, 0 ) = -1.;
    mot.rotation()( 1, 1 ) = -1.;
    mot.rotation()( 2, 2 ) = -1.;
    vector<int> dims;
    hdr.getProperty( "volume_dimension", dims );
    while( dims.size() < 3 )
      dims.push_back( 1 );
    mot.matrix()(0, 3) = ( dims[0] - origin[0] ) * vs[0];
    mot.matrix()(1, 3) = ( dims[1] - origin[1] ) * vs[1];
    mot.matrix()(2, 3) = ( dims[2] - origin[2] ) * vs[2];
    refs.push_back( "other_referential" );
  }
  hdr.setProperty( "referentials", refs );
  vector< vector < float > > trans;
  trans.push_back( mot.toVector() );
  hdr.setProperty( "transformations", trans );
}

