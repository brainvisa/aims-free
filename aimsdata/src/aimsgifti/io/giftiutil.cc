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

#include <aims/io/giftiutil.h>
#include <aims/resampling/standardreferentials.h>

using namespace carto;
using namespace std;

namespace aims
{

  string niftiDataType( int dt )
  {
    switch( dt )
    {
      case NIFTI_TYPE_UINT8:
        return "U8";
      case NIFTI_TYPE_INT16:
        return "S16";
      case NIFTI_TYPE_INT32:
        return "S32";
      case NIFTI_TYPE_FLOAT32:
        return "FLOAT";
      case NIFTI_TYPE_COMPLEX64:
        return "CFLOAT";
      case NIFTI_TYPE_FLOAT64:
        return "DOUBLE";
      case NIFTI_TYPE_RGB24:
        return "RGB";
      case NIFTI_TYPE_INT8:
        return "S8";
      case NIFTI_TYPE_UINT16:
        return "U16";
      case NIFTI_TYPE_UINT32:
        return "U32";
      case NIFTI_TYPE_INT64:
        return "S64";
      case NIFTI_TYPE_UINT64:
        return "U64";
      case NIFTI_TYPE_FLOAT128:
        return "FLOAT128";
      case NIFTI_TYPE_COMPLEX128:
        return "CDOUBLE";
      case NIFTI_TYPE_COMPLEX256:
        return "CFLOAT128";
      case NIFTI_TYPE_RGBA32:
        return "RGBA";
      default:
        return "VOID";
    }
  }


  int niftiIntDataType( const string & typecode )
  {
    if( typecode == "U8" )
      return NIFTI_TYPE_UINT8;
    else if( typecode == "S16" )
      return NIFTI_TYPE_INT16;
    else if( typecode == "S32" )
      return NIFTI_TYPE_INT32;
    else if( typecode == "FLOAT" )
      return NIFTI_TYPE_FLOAT32;
    else if( typecode == "CFLOAT" )
      return NIFTI_TYPE_COMPLEX64;
    else if( typecode == "DOUBLE" )
      return NIFTI_TYPE_FLOAT64;
    else if( typecode == "RGB" )
      return NIFTI_TYPE_RGB24;
    else if( typecode == "S8" )
      return NIFTI_TYPE_INT8;
    else if( typecode == "U16" )
      return NIFTI_TYPE_UINT16;
    else if( typecode == "U32" )
      return NIFTI_TYPE_UINT32;
    else if( typecode == "S64" )
      return NIFTI_TYPE_INT64;
    else if( typecode == "U64" )
      return NIFTI_TYPE_UINT64;
    else if( typecode == "FLOAT128" )
      return NIFTI_TYPE_FLOAT128;
    else if( typecode == "CDOUBLE" )
      return NIFTI_TYPE_COMPLEX128;
    else if( typecode == "CFLOAT128" )
      return NIFTI_TYPE_COMPLEX256;
    else if( typecode == "RGBA" )
      return NIFTI_TYPE_RGBA32;
    else
      return DT_NONE;
  }


  string giftiTextureDataType( int dtype, int & ndim, int* dims )
  {
    string elemtype = niftiDataType( dtype );
    string gdtype = elemtype;

    if( ndim < 2 )
      ndim = 1;
    else if( ndim == 2 )
      ndim = dims[1];
    else if( ndim >= 3 )
    {
      gdtype = "volume of " + elemtype;
      ndim = 1;
    }
    switch( ndim )
    {
      case 1:
        break;
      case 2:
        if( elemtype == "FLOAT" )
          gdtype = "POINT2DF";
        else
          gdtype = "VECTOR_OF_2_" + elemtype;
        break;
      case 3:
        if( elemtype == "FLOAT" )
          gdtype = "POINT3DF";
        else
          gdtype = "VECTOR_OF_3_" + elemtype;
        break;
      default:
      {
        std::ostringstream os;
        os << "VECTOR_OF_" << ndim << "_" << elemtype;
        gdtype = os.str();
      }
    }

    return gdtype;
  }


  namespace internal
  {

    Object giftiFindHdrDA( int & nda, Object dainfo, const string & intent )
    {
      if( dainfo.isNone() )
        return carto::none();
      Object inf;
      Object it = dainfo->objectIterator();
      int i;
      string intentit;
      for( i=0; i<nda && it->isValid(); ++i, it->next() ) {}
      for( ; it->isValid(); ++i, it->next() )
      {
        Object el = it->currentValue();
        if( el->getProperty( "intent", intentit ) )
        {
          if( intent.empty() )
          {
            if( intentit == "NIFTI_INTENT_POINTSET"
              || intentit == "NIFTI_INTENT_VECTOR"
              || intentit == "NIFTI_INTENT_TRIANGLE" )
              continue;
          }
          else
            if( intentit != intent )
              continue;
          inf = el;
          nda = i;
          break;
        }
      }

      return inf;
    }


    std::string niftiRefFromAimsString( const std::string & space )
    {
      if( space == "Arbitrary coordinates" )
        return "NIFTI_XFORM_UNKNOWN";
      if( space == StandardReferentials::talairachReferential() )
        return "NIFTI_XFORM_TALAIRACH";
      if( space == StandardReferentials::mniTemplateReferential()
        || space == StandardReferentials::mniTemplateReferentialID() )
        return "NIFTI_XFORM_MNI_152";
      if( space == "Scanner-based anatomical coordinates" )
        return "NIFTI_XFORM_SCANNER_ANAT";
      if( space
        == "Coordinates aligned to another file or to anatomical truth" )
        return "NIFTI_XFORM_ALIGNED_ANAT";
      if( space == StandardReferentials::acPcReferentialID() )
        return StandardReferentials::acPcReferential();
      // else left it as is
      return space;
    }


    void giftiSetTransformations( carto::Object cs, giiDataArray *da )
    {
      std::vector<std::vector<float> > trs;
      std::vector<std::string> refs;
      std::vector<std::string> datarefs;
      if( cs->getProperty( "transformations", trs )
        && cs->getProperty( "referentials", refs ) )
      {
        if( !cs->getProperty( "data_referentials", datarefs ) )
        {
          std::string dataref;
          if( cs->getProperty( "referential", dataref ) )
            datarefs.push_back( dataref );
        }
        unsigned i, n = trs.size(), m = datarefs.size();
        if( refs.size() < n )
          n = refs.size();
        if( da->numCS != 0 )
          // erase older CS
          gifti_free_CS_list( da );
        for( i=0; i<n; ++i )
        {
          gifti_add_empty_CS( da );
          giiCoordSystem *c = da->coordsys[i];
          if( i < m )
            c->dataspace
              = strdup( niftiRefFromAimsString( datarefs[i] ).c_str() );
          else
            c->dataspace = strdup( "NIFTI_XFORM_UNKNOWN" );
          c->xformspace = strdup( niftiRefFromAimsString( refs[i] ).c_str() );
          for( unsigned j=0; j<4; ++j )
          {
            c->xform[j][0] = trs[i][j*4];
            c->xform[j][1] = trs[i][j*4+1];
            c->xform[j][2] = trs[i][j*4+2];
            c->xform[j][3] = trs[i][j*4+3];
          }
        }
      }
    }


    void giftiCopyMetaToGii( carto::Object dainf, giiDataArray *da )
    {
      if( dainf->hasProperty( "GIFTI_coordinates_systems" ) )
      {
        carto::Object cs
          = dainf->getProperty( "GIFTI_coordinates_systems" );
        giftiSetTransformations( cs, da );
      }
      if( dainf->hasProperty( "GIFTI_metadata" ) )
      {
        carto::Object md = dainf->getProperty( "GIFTI_metadata" );
        carto::Object it = md->objectIterator();
        for( ; it->isValid(); it->next() )
        {
          carto::Object val = it->currentValue();
          if( val.isNull() )
            gifti_add_to_meta( &da->meta, it->key().c_str(), 0, 1 );
          else
            try
            {
              gifti_add_to_meta( &da->meta, it->key().c_str(),
                                 val->getString().c_str(), 1 );
            }
            catch( ... )
            {
            }
        }
      }
      if( dainf->hasProperty( "GIFTI_extra_attributes" ) )
      {
        carto::Object md = dainf->getProperty( "GIFTI_extra_attributes" );
        carto::Object it = md->objectIterator();
        for( ; it->isValid(); it->next() )
        {
          carto::Object val = it->currentValue();
          if( val.isNull() )
            gifti_add_to_nvpairs( &da->ex_atrs, it->key().c_str(), 0 );
          else
            try
            {
              gifti_add_to_nvpairs( &da->ex_atrs, it->key().c_str(),
                                    val->getString().c_str() );
            }
            catch( ... )
            {
            }
        }
      }
    }

  } // namespace internal
} // namespace aims

