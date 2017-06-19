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

#include <aims/io/giftiheader.h>
#include <aims/io/giftiutil.h>
#include <aims/resampling/motion.h>
#include <cartobase/stream/fileutil.h>
#include <cartobase/stream/fdinhibitor.h>
#include <aims/resampling/standardreferentials.h>
#include <aims/mesh/texture.h>
#include <cartobase/thread/mutex.h>
#include <cstdio>

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


Mutex & GiftiHeader::giftiMutex()
{
  // Must be initialized (generally in main thread) before using concurrently
  static Mutex mutex( Mutex::Recursive );
  return mutex;
}


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
  //cout << "GiftiHeader::read\n";

  gifti_image   *gim = 0;
  string fname = _name;
  gifti_set_verb( 0 ); // not enough to make it fail silently...
  if( !FileUtil::fileStat( fname ).empty() )
  {
    // avoid printing anything from gitficlib
    fdinhibitor   fdi( stderr );
    fdi.close();
    giftiMutex().lock();
    gim = gifti_read_image( fname.c_str(), 0 );
    giftiMutex().unlock();
    fdi.open();
  }
  if( !gim && fname.substr( fname.length()-4, 4 ) != ".gii" )
  {
    fname += ".gii";
    if( FileUtil::fileStat( fname ).empty() )
      return false;
    // avoid printing anything from gitficlib
    fdinhibitor   fdi( stderr );
    fdi.close();
    giftiMutex().lock();
    gim = gifti_read_image( fname.c_str(), 0 );
    giftiMutex().unlock();
    fdi.open();
  }
  if( !gim )
    return false;
  _name = fname;

  int i;
  string mname, mval;

  // read meta-data GIFTI file

  if( gim->meta.length > 0 )
  {
    Object o = Object::value( Dictionary() );
    for( i=0; i<gim->meta.length; ++i )
    {
      mname = gim->meta.name[i];

//      std::cout << "lecture meta data" << std::endl;
//      std::cout << mname << std::endl;
//      std::cout << gim->meta.value[i] << std::endl;

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
  //gifti_disp_LabelTable("gim->labeltable", &gim->labeltable);

  if( gim->labeltable.length > 0 )
  {
    Object o = Object::value( IntDictionary() );
    for( i=0; i<gim->labeltable.length; ++i )
      {
      vector<float> rgba;
      rgba.push_back( (float)(gim->labeltable.rgba[4*i]));
      rgba.push_back( (float)(gim->labeltable.rgba[4*i+1]));
      rgba.push_back( (float)(gim->labeltable.rgba[4*i+2]));
      rgba.push_back( (float)(gim->labeltable.rgba[4*i+3]));

      Object LabelTable = Object::value( Dictionary() );
      LabelTable->setProperty( "RGB", rgba );
      LabelTable->setProperty( "Label", Object::value( string( gim->labeltable.label[i])) );
      o->setArrayItem( gim->labeltable.key[i], LabelTable );
      }

    setProperty( "GIFTI_labels_table", o );
  }
  // labels table

  giftiAddLabelTable( gim );

  setProperty( "GIFTI_version", string( gim->version ) );
  if( gim->compressed )
    setProperty( "GIFTI_compressed", int(1) );

  // read arrays information
  int nda = gim->numDA, j;
  int nmesh = 0, nnorm = 0, npoly = 0, ntex = 0, polydim = 0, vnum = 0,
    texlen = 0, texdim = 0, polynum = 0, nttex = 0;
  string dtype, otype;
  Object daattr = Object::value( IntDictionary() );
  bool mesh, tex;
  vector<string> texnames;
  int nts = 0;

  for( i=0; i<nda; ++i )
  {
	mesh = false;
    tex = false;
    int nt = 1;
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
    case NIFTI_INTENT_TIME_SERIES:

      if( nts == 0 )
        ++ntex;
      ++nts;
      tex = true;
      texlen = da->dims[0];
      texdim = da->num_dim;
      dtype = giftiTextureDataType( da->datatype, texdim, da->dims, da->intent,
                                    nt );
      if( nts > nttex )
        nttex = nts;

      break;
    default:
      ++ntex;
      nts = 0;
      tex = true;
      texlen = da->dims[0];
      texdim = da->num_dim;
      dtype = giftiTextureDataType( da->datatype, texdim, da->dims, da->intent,
                                    nt );
      if( nt > nttex )
        nttex = nt;
    }

	Object daattr2 = Object::value( Dictionary() );

     //read meta-data dataArray
	if( da->meta.length > 0 )
    {
        Object o = Object::value( Dictionary() );
		for( j=0; j<da->meta.length; ++j )
		{
		  mname = da->meta.name[j];
		  //std::cout << mname << ":" ;
          //value = gifti_get_meta_value(&da->meta, mname.c_str());

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
        string dataspace = da->coordsys[j]->dataspace;
        dataspace = niftiReferential( dataspace );
        //o->setProperty( "referential", dataspace );
        if( mesh && nmesh == 1 && j == 0
          && dataspace != "Arbitrary coordinates" )
          // share ref/transfo information in the main header
          setProperty( "referential", dataspace );
        string xformspace = niftiReferential( da->coordsys[j]->xformspace );
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

    daattr2->setProperty( "encoding",da->encoding);

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
    setProperty( "nb_t_pos", std::max(nmesh, nttex) );
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
    if( ntex > 0 )
      setProperty( "texture_number", ntex );
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

  readMinf( fname + ".minf" );

  gifti_free_image( gim );

  return true;
}


gifti_image* GiftiHeader::giftiImageBase()
{
  std::string fname = _name;
  if( fname.length() < 4
    || fname.substr( fname.length() - 4, 4 ) != ".gii" )
    fname += ".gii";
  _name = fname;
  gifti_image * gim = gifti_create_image( -1, -1, -1, -1, 0, 0 );

  // metadata
  if( hasProperty( "GIFTI_metadata" ) )
  {
    carto::Object md = getProperty( "GIFTI_metadata" );
    carto::Object it = md->objectIterator();
    for( ; it->isValid(); it->next() )
    {
      carto::Object val = it->currentValue();
      if( val.isNull() )
        gifti_add_to_meta( &gim->meta, it->key().c_str(), 0, 0 );
      else
        try
        {
          gifti_add_to_meta( &gim->meta, it->key().c_str(),
                             val->getString().c_str(), 0 );
        }
        catch( ... )
        {
        }
    }
  }
  // TODO: should we put all .minf properties in Gifti metadata ?

  return gim;
}


// ---

namespace
{

  template <typename T>
  class giftiAllocDAData // make it a functor to allow partial specialization
  {
  public:
    void operator() ( gifti_image *gim, giiDataArray * da,
                      size_t npadding, size_t nt )
    {
      da->dims[1] = 1;
      da->nbyper = sizeof( T );
      da->datatype = niftiIntDataType( carto::DataTypeCode<T>::name() );
      da->nvals = npadding * nt;
      int nda = gim->numDA - 1;
      gifti_alloc_DA_data( gim, &nda, 1 );
    }
  };


  template <typename T, int D>
  class giftiAllocDAData<AimsVector<T,D> >
  {
  public:
    void operator() ( gifti_image *gim, giiDataArray * da,
                      size_t npadding, size_t nt )
    {
      da->dims[1] = D;
      da->nbyper = sizeof( T );
      da->datatype = niftiIntDataType( carto::DataTypeCode<T>::name() );
      da->nvals = npadding * D * nt;
      int nda = gim->numDA - 1;
      gifti_alloc_DA_data( gim, &nda, 1 );
    }
  };


  template <typename T>
  class giftiFillTextureBuffer
  {
    public:
      void operator () ( giiDataArray * da, const std::vector<T> & tex,
                         size_t npadding, int t )
    {
      unsigned i, n = tex.size();
      T* buf = reinterpret_cast<T *>( da->data );
      buf += t * npadding;
      for( i=0; i<n; ++i )
        *buf++ = tex[i];
      T null(0);
      for( ; i<npadding; ++i )
        *buf++ = null;
    }
  };


  template <typename T, int D>
  class giftiFillTextureBuffer<AimsVector<T,D> >
  {
  public:
    void operator() ( giiDataArray * da,
                      const std::vector<AimsVector<T, D> > & tex,
                      size_t npadding, int t )
    {
      unsigned i, n = tex.size();
      int j;
      T* buf = reinterpret_cast<T *>( da->data );
      buf += t * npadding * D;
      for( i=0; i<n; ++i, ++buf )
        for( j=0; j<D; ++j )
          *(buf+j*npadding) = tex[i][j];
      T null(0);
      for( ; i<npadding; ++i, ++buf )
        for( j=0; j<D; ++j )
          *(buf+j*npadding) = null;
    }
  };

}



template <typename T>
void GiftiHeader::giftiAddTexture( gifti_image* gim,
                                   const TimeTexture<T> & tex )
{
  // std::cout << "AddTexture type TimeTexture<T>\n";
  typename TimeTexture<T>::const_iterator it, et = tex.end();
  size_t nmax = 0;
  int t = 0;

  for( it=tex.begin(); it!=et; ++it )
    if( it->second.nItem() > nmax )
      nmax = it->second.nItem();

  int hdrtexda = 0;

  for( it=tex.begin(); it!=et; ++it, ++t )
  {
    const vector<T> & tx = it->second.data();
    if( !tx.empty() )
    {
      int nda = gim->numDA;
      giiDataArray * da = 0;
      gifti_add_empty_darray( gim, 1 );
      da = gim->darray[nda];
      gifti_set_DA_defaults( da );
      if( tex.size() == 1 )
      {
        carto::Object da_info;
        try
        {
          int ida;
          da_info = getProperty( "GIFTI_dataarrays_info" );
          carto::Object it = da_info->objectIterator();
          std::string intentit;
          // find in header if there was a corresponding data array
          // specifying a given intent code
          for( ida=0; it->isValid() && ida<=nda; it->next(), ++ida )
          {}
          if( ida <= nda )
          {
            carto::Object el = it->currentValue();
            if( el->getProperty( "intent", intentit ) )
              da->intent = gifti_intent_from_string(intentit.c_str());
          }
          else // nothing in header
            da->intent = NIFTI_INTENT_SHAPE;
        }
        catch( ... )
        {
          //std::cout << "error GIFTI_dataarrays_info\n";
        }
      }
      else
        da->intent = NIFTI_INTENT_TIME_SERIES;
      da->num_dim = 1;
      da->dims[0] = nmax;
      da->dims[1] = 0;
      da->dims[2] = 0;
      da->dims[3] = 0;
      da->dims[4] = 0;
      da->dims[5] = 0;
      giftiAllocDAData<T>()( gim, da, nmax, 1 );
      giftiFillTextureBuffer<T>()( da, tx, nmax, 0 );

      da->encoding = GIFTI_ENCODING_B64GZ; // set it as default
      carto::Object da_info;
      try
      {
        int ida;
        da_info = getProperty( "GIFTI_dataarrays_info" );
        carto::Object it = da_info->objectIterator();
        for( ida=0; ida<=nda && it->isValid(); it->next(), ++ida )
        {}
        if( ida <= nda )
        {
          carto::Object el = it->currentValue();
          el->getProperty( "encoding", da->encoding );
        }

        if( !options().isNull() )
        {
          try
          {
            carto::Object a = options()->getProperty( "encoding" );
            if ((int) a->getScalar() != 0)
            {
              da->encoding = (int) a->getScalar();
            }
          }
          catch( ... )
          {
          }
        }
      }
      catch( ... )
      {
      }

      string mname,mval;
      // metadata dataArray
      carto::Object dainf
        = GiftiHeader::giftiFindHdrDA( hdrtexda, da_info, "" );
      if( !dainf.isNone() )
      {
        ++hdrtexda;
        GiftiHeader::giftiCopyMetaToGii( dainf, da );
      }

    }
  }
}

template <typename T>
void GiftiHeader::giftiAddTextureObject( gifti_image* gim, Object texture )
{
  TimeTexture<T> & tex
    = texture->carto::GenericObject::value<TimeTexture<T> >();
  giftiAddTexture( gim, tex );
}


void GiftiHeader::giftiAddExternalTextures( gifti_image *gim, int & hdrtexda,
                                            carto::Object da_info )
{
  try
  {
    carto::Object tex = getProperty( "textures" );
    carto::Object it = tex->objectIterator();
    for( ; it->isValid(); it->next() )
    {
      carto::Object texture = it->currentValue();
      std::string ttype = texture->type();
      if( ttype == carto::DataTypeCode<TimeTexture<float> >::name() )
        giftiAddTextureObject<float>( gim, texture );
      else if( ttype
        == carto::DataTypeCode<TimeTexture<double> >::name() )
        giftiAddTextureObject<double>( gim, texture );
      else if( ttype
        == carto::DataTypeCode<TimeTexture<Point2df> >::name() )
        giftiAddTextureObject<Point2df>( gim, texture );
      else if( ttype
        == carto::DataTypeCode<TimeTexture<int16_t> >::name() )
        giftiAddTextureObject<int16_t>( gim, texture );
      else if( ttype
        == carto::DataTypeCode<TimeTexture<int32_t> >::name() )
        giftiAddTextureObject<int32_t>( gim, texture );
      else if( ttype
        == carto::DataTypeCode<TimeTexture<uint32_t> >::name() )
        giftiAddTextureObject<uint32_t>( gim, texture );
      else if( ttype
        == carto::DataTypeCode<TimeTexture<Point2d> >::name() )
        giftiAddTextureObject<Point2d>( gim, texture );
      else
      {
        std::cerr << "GIFTI writer warning: cannot save texture of type "
        << ttype << std::endl;
        continue;
      }

      // metadata
      carto::Object dainf = giftiFindHdrDA( hdrtexda, da_info, "" );
      if( !dainf.isNone() )
      {
        ++hdrtexda;
        giftiCopyMetaToGii( dainf, gim->darray[gim->numDA-1] );
      }
    }
  }
  catch( ... )
  {
  }
}

void GiftiHeader::giftiAddLabelTable( gifti_image *gim )
{
  if( hasProperty( "GIFTI_labels_table" ) )
  {
    carto::IntDictionary lt;
    getProperty( "GIFTI_labels_table", lt );
    carto::IntDictionary::const_iterator it, et = lt.end();
    giiLabelTable & glt = gim->labeltable;
    glt.length = lt.size();
    glt.key = (int *) malloc( glt.length * sizeof( int ) );
    glt.label = (char **) malloc( glt.length * sizeof( char * ) );
    glt.rgba = (float *) malloc( 4 * glt.length * sizeof( float ) );
    int i = 0;

    Object LabelTable = Object::value( Dictionary() );

    for( it=lt.begin(); it!=et; ++it, ++i )
      try
      {
        glt.key[i] = it->first;
        //std::cout << glt.key[i] << "\n";

        LabelTable = it->second;

        if( it->second.isNone() )
          glt.label[i] = 0;
        else
          {
        	LabelTable = it->second;
        	std::string label;
        	LabelTable->getProperty( "Label", label );
        	glt.label[i] = strdup( label.c_str() );
        	//std::cout << glt.label[i] << "\n";
        	std::vector<float> rgba;
		    LabelTable->getProperty( "RGB", rgba );
		    vector<float>::iterator itrgb = rgba.begin();
		    for (int j = 0; itrgb != rgba.end();itrgb++,j++)
			  {
			  glt.rgba[4*i + j]=  *itrgb;
			  //std::cout << (float)*itrgb << " ";
			  }
          }
      }
      catch( ... )
      {
        glt.key[i] = 0;
        glt.label[i] = 0;
      }
    //removeProperty( "GIFTI_labels_table" );
  }
}

Object GiftiHeader::giftiFindHdrDA( int & nda, Object dainfo,
                                    const string & intent )
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
    //std::cout << intentit << std::endl;

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

      //std::cout << i << std::endl;
      break;
    }
  }

  return inf;
}

std::string GiftiHeader::niftiRefFromAimsString( const std::string & space )
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


void GiftiHeader::giftiSetTransformations( carto::Object cs, giiDataArray *da )
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


void GiftiHeader::giftiCopyMetaToGii( carto::Object dainf, giiDataArray *da )
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


template <typename T>
void GiftiHeader::giftiAddTexture( gifti_image* gim, const vector<T> & tex )
{
  //std::cout << "AddTexture type vector<T>\n";
  if( !tex.empty() )
  {
    int nda = gim->numDA;
    gifti_add_empty_darray( gim, 1 );
    giiDataArray * da = gim->darray[nda];
    gifti_set_DA_defaults( da );
    da->intent = NIFTI_INTENT_SHAPE;
    da->num_dim = 2;
    da->dims[0] = tex.size();
    da->dims[2] = 0;
    da->dims[3] = 0;
    da->dims[4] = 0;
    da->dims[5] = 0;

    da->encoding = GIFTI_ENCODING_B64GZ; // set it as default
    if( !options().isNull() )
    {
      try
      {
        carto::Object a = options()->getProperty( "encoding" );
        if ((int) a->getScalar() != 0)
        {
          da->encoding = (int) a->getScalar();
        }
      }
      catch( ... )
      {
      }
    }

    //std::cout << "encoding = "<< da->encoding << "\n";
    gifti_alloc_DA_data( gim, &nda, 1 );
    giftiFillTextureBuffer<T>()( da, tex, da->dims[0], 0 );
  }
}


namespace aims
{
  template <>
  void GiftiHeader::giftiAddTexture( gifti_image*, const vector<Void> & )
  {
  //std::cout << "AddTexture type vector<Void>\n";
  }

}

