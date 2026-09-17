// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

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

