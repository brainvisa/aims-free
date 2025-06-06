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

#include <aims/sparsematrix/ciftitools.h>
#include <aims/sparsematrix/sparseordensematrix.h>
#include <aims/mesh/texture.h>
#include <aims/mesh/surface.h>

using namespace aims;
using namespace carto;
using namespace std;


struct CiftiTools::Private
{
  void clear();
  // cache for vertex / matrix line/col mapping, on each dimension
  mutable map<int32_t, vector<map<uint32_t, uint32_t> > > dimVertexMatrixMap;
  mutable map<int32_t, vector<size_t> > surfaceSize;
};


void CiftiTools::Private::clear()
{
  dimVertexMatrixMap.clear();
  surfaceSize.clear();
}


CiftiTools::CiftiTools( rc_ptr<SparseOrDenseMatrix> matrix,
                        const BrainStuctureToMeshMap & smap )
  : d( new Private )
{
  _matrix = matrix;
  if( smap.empty() )
    _smap = defaultBrainStructureToMeshMap();
  else
    _smap = smap;
}


CiftiTools::~CiftiTools()
{
  delete d;
}


void CiftiTools::setMatrix( carto::rc_ptr<SparseOrDenseMatrix> matrix )
{
  _matrix = matrix;
  // clear caches
  d->clear();
}


Object CiftiTools::getDimensionObject( int dim ) const
{
  Object hdr = _matrix->header();
  if( !hdr )
    return none();
  Object cifti_dims;
  try
  {
    cifti_dims = hdr->getProperty( "cifti_dimensions" );
  }
  catch( exception & )
  {
    return none();
  }
  if( dim >= (int)cifti_dims->size() )
    return none();
  Object cdim;
  try
  {
    cdim = cifti_dims->getArrayItem( dim );
  }
  catch( exception & )
  {
    return none();
  }

  return cdim;
}


CiftiTools::RoiTextureList *CiftiTools::roiTextureFromDimension(
  int dim ) const
{
  RoiTextureList *roilist = new RoiTextureList;
  Object cdim = getDimensionObject( dim );
  if( !cdim )
  {
    int dsize = _matrix->getSize()[ dim ];
    rc_ptr<TimeTexture<int> > tex( new TimeTexture<int> );
    roilist->push_back( tex );
    vector<int> & data = (*tex)[0].data();
    data.resize( dsize );
    for( int i=0; i<dsize; ++i )
      data[i] = 1;
    return roilist;
  }

  BrainStuctureToMeshMap dsmap;

  if( cdim->hasProperty( "labels" ) )
  {
    getLabelsInfo( cdim, *roilist );
  }
  else if( cdim->hasProperty( "scalars" ) )
  {
    // nothing more to do ?
  }
  else if( cdim->hasProperty( "series" ) )
  {
    getSeriesInfo( cdim, *roilist );
  }
  else if( cdim->hasProperty( "parcels" ) )
  {
    getParcelsInfo( cdim, *roilist );
  }
  else if( cdim->hasProperty( "brain_models" ) )
  {
    getBrainModelsInfo( cdim, *roilist );
  }

  return roilist;
}


void CiftiTools::getLabelsInfo( Object cifti_info,
                                RoiTextureList & roilist )
{
  Object labels = cifti_info->getProperty( "labels" );
  // read colormaps
  // store them each in a texture header, interpreted by Anatomist.

  Object iter = labels->objectIterator();
  for( ; iter->isValid(); iter->next() )
  {
    map<int, string> label_map;
    map<int, vector<int> > colors;
    Object item = iter->currentValue();
    string label_map_name;
    try
    {
      label_map_name = item->getProperty( "name" )->getString();
    }
    catch( ... )
    {
    }
    unsigned i, maxlabel = 0;
    try
    {
      map<int, Object> ctable;
      item->getProperty( "label_table", ctable );
      map<int, Object>::iterator citer, cend = ctable.end();
      for( citer=ctable.begin(); citer!=cend; ++citer )
      {
        Object cobj = citer->second;
        label_map[ citer->first ] = cobj->getProperty( "label" )->getString();
        Object color = cobj->getProperty( "color" );
        if( color->size() >= 4 )
        {
          vector<int> col( 4 );
          col[0] = int( rint(
            color->getArrayItem( 0 )->getScalar() * 255.9 ) );
          col[1] = int( rint(
            color->getArrayItem( 1 )->getScalar() * 255.9 ) );
          col[2] = int( rint(
            color->getArrayItem( 2 )->getScalar() * 255.9 ) );
          col[3] = int( rint(
            color->getArrayItem( 3 )->getScalar() * 255.9 ) );
          colors[ citer->first ] = col;
        }
        maxlabel = citer->first;
      }
    }
    catch( ... )
    {
    }

    // transform maps to vectors (color table)
    vector<string> labels( maxlabel + 1 );
    vector<int> vcolors( (maxlabel + 1) * 4 );
    map<int, string>::const_iterator il, nolabel = label_map.end();
    map<int, vector<int> >::const_iterator ic, nocol = colors.end();
    for( i=0; i<=maxlabel; ++i )
    {
      il = label_map.find( i );
      if( il != nolabel )
        labels[i] = il->second;
      ic = colors.find( i );
      if( ic == nocol )
      {
        vcolors[i*4] = 255;
        vcolors[i*4+1] = 255;
        vcolors[i*4+2] = 255;
        vcolors[i*4+3] = 255;
      }
      else
      {
        vcolors[i*4] = ic->second[0];
        vcolors[i*4+1] = ic->second[1];
        vcolors[i*4+2] = ic->second[2];
        vcolors[i*4+3] = ic->second[3];
      }
    }
    Object palettedict = Object::value( Dictionary() );
    roilist.push_back( rc_ptr<TimeTexture<int> >( new TimeTexture<int> ) );
    rc_ptr<TimeTexture<int> > rtex = *roilist.rbegin();
    (*rtex)[0]; // force creating 1 texture
    GenericObject & hdr = rtex->header();
    hdr.setProperty( "palette", palettedict );

    palettedict->setProperty( "colors", vcolors );
    palettedict->setProperty( "color_mode", "rgba" );
    palettedict->setProperty( "labels", labels );
    palettedict->setProperty( "palette", label_map_name );
    palettedict->setProperty( "min", 0. );
    palettedict->setProperty( "max", 1. );

    Object texlist = Object::value( ObjectVector() );
    hdr.setProperty( "texture_properties", texlist );
    Object texdict = Object::value( Dictionary() );
    texlist->insertArrayItem( -1, texdict );
    texdict->setProperty( "interpolation", "rgb" );
  }
}


void CiftiTools::getSeriesInfo( Object cifti_info, RoiTextureList & roilist )
{
  size_t len;
  try
  {
    len = (size_t) cifti_info->getProperty( "dimension_length" )->getScalar();
  }
  catch( exception & )
  {
    return;
  }
  rc_ptr<TimeTexture<int> > roi( new TimeTexture<int> );
  roilist.push_back( roi );
  (*roi)[0].data().resize( len );
  try
  {
    string unit = cifti_info->getProperty( "unit" )->getString();
    roi->header().setProperty( "unit", unit );
  }
  catch( ... )
  {
  }
}


void CiftiTools::getParcelsInfo( Object cifti_info,
                                 RoiTextureList & roilist ) const
{
  Object parcels_o = cifti_info->getProperty( "parcels" );
  Object parcels = parcels_o->getProperty( "parcels" );
  Object surfaces;
  int background = -1;
  try
  {
    surfaces = parcels_o->getProperty( "surface_structures" );
  }
  catch( ... )
  {
  }
  Object piter = parcels->objectIterator();
  size_t label = 0;
  for( ; piter->isValid(); piter->next(), ++label )
  {
    Object parcel = piter->currentValue();
    string roi_name = parcel->getProperty( "name" )->getString();
    Object par_def = parcel->getProperty( "surface_nodes" );
    Object pditer = par_def->objectIterator();
    for( ; pditer->isValid(); pditer->next() )
    {
      string struct_name = pditer->key();
      Object elem = pditer->currentValue();
      int surfnum = _smap.find( struct_name )->second;
      while( (int)roilist.size() <= surfnum )
        roilist.push_back( rc_ptr<TimeTexture<int> >() );
      rc_ptr<TimeTexture<int> > & troi
        = roilist[ _smap.find( struct_name )->second ];
      if( troi.isNull() )
      {
        troi.reset( new TimeTexture<int> );
        Object texplist = Object::value( ObjectVector() );
        troi->header().setProperty( "texture_properties", texplist );
        Object texp = Object::value( Dictionary() );
        texplist->insertArrayItem( -1, texp );
        texp->setProperty( "interpolation", "rgb" );
        Object labels_list = Object::value( ObjectVector() );
        troi->header().setProperty( "labels", labels_list );
      }
      troi->header().getProperty( "labels" )->insertArrayItem(
        -1, Object::value( roi_name ) );
      Texture<int> & roi = (*troi)[0];
      if( surfaces )
      {
        Object surf_str = surfaces->getProperty( struct_name );
        size_t nnodes
          = size_t( surf_str->getProperty( "number_of_nodes" )->getScalar() );
        if( roi.nItem() == 0 )
        {
          roi.data().resize( nnodes );
          for( size_t i=0; i<nnodes; ++i )
            roi[i] = background;
        }
        /* else if( roi.nItem() != nnodes )
        {
          cerr << "Warning: size mismatch in surfaces\n";
          cerr << "roi: " << roi.nItem() << ", nodes: " << nnodes << endl;
        } */
      }
      Object viter = elem->objectIterator();
      int64_t v;
      for( ; viter->isValid(); viter->next() )
      {
        v = int( rint( viter->currentValue()->getScalar() ) );
        if( (int64_t)roi.nItem() <= v )
          roi.data().resize( v + 1 );
        roi[v] = label;
      }
    }
  }
}


namespace
{

  void getSurfaceModelInfo( Object surf, CiftiTools::RoiTextureList & roilist,
                            const CiftiTools::BrainStuctureToMeshMap & smap )
  {
    Object siter = surf->objectIterator();
    for( ; siter->isValid(); siter->next() )
    {
      string struct_name = siter->key();
      Object surf = siter->currentValue();
      int surfnum = smap.find( struct_name )->second;
      while( (int)roilist.size() <= surfnum )
        roilist.push_back( rc_ptr<TimeTexture<int> >() );
      rc_ptr<TimeTexture<int> > & troi
        = roilist[ smap.find( struct_name )->second ];
      if( troi.isNull() )
        troi.reset( new TimeTexture<int> );
      Texture<int> & roi = (*troi)[0];
      size_t nnodes
        = size_t( rint(
          surf->getProperty( "number_of_nodes" )->getScalar() ) );
      int label =  int( rint( surf->getProperty(
        "label_number" )->getScalar() ) );
      Object labels_map;
      if( troi->header().hasProperty( "labels_map" ) )
        labels_map = troi->header().getProperty( "labels_map" );
      else
        labels_map = Object::value( IntDictionary() );
      labels_map->setArrayItem( label, Object::value( struct_name ) );
      troi->header().setProperty( "labels_map", labels_map );
      if( roi.data().empty() )
        roi.data().resize( nnodes, 0 );
      if( roi.nItem() < nnodes )
      {
        cout << "warning: number_of_nodes (" << nnodes
          << ") differ from allocated texture size (" << roi.nItem() << ")"
          << endl;
        roi.data().resize( nnodes, 0 );
      }
      Object indices = surf->getProperty( "vertices_map" );
      Object iiter = indices->objectIterator();
      for( ; iiter->isValid(); iiter->next() )
      {
        roi[ int( rint(
          iiter->currentValue()->getArrayItem(1)->getScalar() ) ) ]
            = label;
      }
    }
  }


  void getModelIndices( Object surf, const string & model_name,
                        vector<int> & indices_vec )
  {
    Object siter = surf->objectIterator();
    for( ; siter->isValid(); siter->next() )
    {
      string struct_name = siter->key();
      if( struct_name != model_name )
        continue;

      Object surf = siter->currentValue();
      Object indices = surf->getProperty( "vertices_map" );
      size_t i = indices_vec.size();
      indices_vec.resize( indices_vec.size() + indices->size() );
      Object iiter = indices->objectIterator();
      for( ; iiter->isValid(); iiter->next(), ++i )
        indices_vec[i] = int( rint(
          iiter->currentValue()->getArrayItem(0)->getScalar() ) );
    }
  }


  void getSurfaceModelIndicesForRoiIndices(
    Object surf, int surface_num, const set<int> & roi_indices,
    vector<int> & indices_vec,
    const CiftiTools::BrainStuctureToMeshMap & smap )
  {
    Object siter = surf->objectIterator();
    for( ; siter->isValid(); siter->next() )
    {
      string struct_name = siter->key();
      int surfnum = smap.find( struct_name )->second;
      if( surfnum == surface_num )
      {
        Object surf = siter->currentValue();
        Object indices = surf->getProperty( "vertices_map" );
        Object iiter = indices->objectIterator();
        for( ; iiter->isValid(); iiter->next() )
        {
          int surf_ind = int( rint(
            iiter->currentValue()->getArrayItem(1)->getScalar() ) );
          if( roi_indices.find( surf_ind ) != roi_indices.end() )
            indices_vec.push_back( int( rint(
              iiter->currentValue()->getArrayItem(0)->getScalar() ) ) );
        }
      }
    }
  }

}


void CiftiTools::getBrainModelsInfo( Object cifti_info,
                                     RoiTextureList & roilist ) const
{
  Object models = cifti_info->getProperty( "brain_models" );
  Object iter = models->objectIterator();
  int struct_index = 0;
  for( ; iter->isValid(); iter->next(), ++struct_index )
  {
    try
    {
      Object surf = iter->currentValue()->getProperty( "surface" );
      getSurfaceModelInfo( surf, roilist, _smap );
    }
    catch( exception & )
    {
      try
      {
        Object vox = iter->currentValue()->getProperty( "voxels" );
        // TODO
        // getVoxelsModelInfo( surf, roilist, _smap /*, struct_index*/ );
      }
      catch( exception & )
      {
      }
    }
  }
}

namespace
{

  bool load_row_or_col( SparseOrDenseMatrix & mat, int dim,
                        const vector<int> & pos )
  {
    if( pos.size() > 2 )
      mat.lazyReader()->selectDimension(
        vector<int32_t>( pos.begin() + 2, pos.end() ) );

    bool free_row = false;
    if( dim == 0 )
    {
      free_row = !mat.lazyReader()->hasColumn( pos[1] );
      mat.readColumn( pos[1] );
    }
    else if( dim == 1 )
    {
      free_row = !mat.lazyReader()->hasRow( pos[0] );
      mat.readRow( pos[0] );
    }
    return free_row;
  }


  vector<double> *load_row_or_col2( SparseOrDenseMatrix & mat, int dim,
                                    const vector<int> & pos )
  {
    if( pos.size() > 2 )
      mat.lazyReader()->selectDimension(
        vector<int32_t>( pos.begin() + 2, pos.end() ) );

    if( dim == 0 )
    {
      return mat.getReadColumn( pos[1] );
    }
    else if( dim == 1 )
    {
      return mat.getReadRow( pos[0] );
    }
    return 0;
  }


  void unload_row_or_col( SparseOrDenseMatrix & mat, int dim,
                          const vector<int> & pos, bool free_it )
  {
    if( free_it )
    {
      if( dim == 0 )
        mat.freeColumn( pos[1] );
      else
        mat.freeRow( pos[0] );
    }
  }

}

CiftiTools::TextureList *CiftiTools::expandedValueTextureFromDimension(
  int dim,
  const vector<int> & dim_indices_pos,
  TextureList* existing_tex_list ) const
{
  TextureList *texlist;
  if( existing_tex_list )
    texlist = existing_tex_list;
  else
    texlist = new TextureList;

  map<int32_t, vector<map<uint32_t, uint32_t> > >::iterator
    itm = d->dimVertexMatrixMap.find( dim );
  if( itm != d->dimVertexMatrixMap.end() )
  {
    // vertex/matrix map in cache
    size_t index, nsurf = itm->second.size();
    while( texlist->size() < nsurf )
      texlist->push_back( rc_ptr<TimeTexture<float> >(
        new TimeTexture<float> ) );
    vector<double> *row = load_row_or_col2( *_matrix, dim, dim_indices_pos );
    if( row )
    {
      for( index=0; index<nsurf; ++index )
      {
        vector<float> & data = (*(*texlist)[index])[0].data(); // limited to time 0
        if( data.size() != d->surfaceSize[dim][index] )
          data.resize( d->surfaceSize[dim][index], 0. );
        const map<uint32_t, uint32_t> & cache = itm->second[index];
        map<uint32_t, uint32_t>::const_iterator ic, ec = cache.end();
        for( ic=cache.begin(); ic!=ec; ++ic )
          data[ ic->first ] = (*row)[ ic->second ];
      }
      delete row;
      return texlist;
    }
  }

  Object cdim = getDimensionObject( dim );

  if( !cdim || cdim->hasProperty( "brain_models" ) )
  {
    getBrainModelsTexture( cdim, *texlist, dim, dim_indices_pos );
  }
  else if( cdim->hasProperty( "labels" ) )
  {
//     getLabelsTexture( cdim, *texlist, dim, dim_indices_pos );
  }
  else if( cdim->hasProperty( "scalars" ) )
  {
//     getScalarsTexture( cdim, *texlist, dim, dim_indices_pos );
  }
  else if( cdim->hasProperty( "series" ) )
  {
//     getSeriesTexture( cdim, *texlist, dim, dim_indices_pos );
  }
  else if( cdim->hasProperty( "parcels" ) )
  {
    getParcelsTexture( cdim, *texlist, dim, dim_indices_pos );
  }

  return texlist;
}


namespace
{

  void getScalarsPointData( SparseOrDenseMatrix & matrix, int value_dim,
    vector<int32_t> & pos, CiftiTools::TextureList & texlist,
    int tex_index, size_t pos_in_tex, float background, bool single )
  {
//     cout << "getScalarsPointData " << pos[0] << ", " << pos[1] << endl;
//     cout << "tex_index: " << tex_index << ", texlist: " << texlist.size() << endl;
//     cout << "value_dim: " << value_dim << endl;
//     cout << "mat size: " << matrix.getSize().size() << endl;

    size_t value_dim_size = CiftiTools::valuesDimSize( matrix, value_dim );
//     cout << "value_dim_size: " << value_dim_size << endl;

    if( single || value_dim_size == 1 )
    {
//       cout << "single\n";
      // select needed dimension when more than 2 are available
      if( pos.size() > 2 )
        matrix.lazyReader()->selectDimension( vector<int32_t>( pos.begin() + 2,
                                                               pos.end() ) );
      bool free_row = false;
      // if data are not in memory, need to load them
      if( !matrix.lazyReader()->hasRow( pos[0] )
          && !matrix.lazyReader()->hasColumn( pos[1] ) )
      {
        if( value_dim == 0 )
        {
          free_row = !matrix.lazyReader()->hasColumn( pos[1] );
          matrix.readColumn( pos[1] );
        }
        else
        {
          free_row = !matrix.lazyReader()->hasRow( pos[0] );
          matrix.readRow( pos[0] );
        }
      }

      (*texlist[tex_index])[0][ pos_in_tex ] = float( matrix( pos[0],
                                                              pos[1] ) );
      if( free_row )
      {
        /* if reading repetitively in the same row / column, freeing here
           may be very inefficient.
           In the other hand, not freeing, may eat memory if reading
           repetitively from different rows / columns. */
        if( value_dim == 0 )
          matrix.freeColumn( pos[1] );
        else
          matrix.freeRow( pos[0] );
      }
    }
    else
    {
//       cout << "multi\n";
      rc_ptr<TimeTexture<float> > tex = texlist[ tex_index ];
      size_t i, len = (*tex)[0].nItem();
      while( tex->size() < value_dim_size )
      {
        Texture<float> & tx = (*tex)[ tex->size() ];
        tx.data().resize( len );
        for( i=0; i<len; ++i )
          tx[i] = background;
      }

      pos[value_dim] = 0;
      // select needed dimension when more than 2 are available
      if( pos.size() > 2 )
        matrix.lazyReader()->selectDimension(
          vector<int32_t>( pos.begin() + 2, pos.end() ) );

      bool free_same_row = false, free_row = false;
      if( value_dim == 0 )
      {
        free_same_row = !matrix.lazyReader()->hasColumn( pos[1] );
        matrix.readColumn( pos[1] );
      }
      else if( value_dim == 1 )
      {
        free_same_row = !matrix.lazyReader()->hasRow( pos[0] );
        matrix.readRow( pos[0] );
      }
      else
        free_row = true;
      for( i=0; i<value_dim_size; ++i )
      {
        pos[value_dim] = i;
        // select needed dimension when more than 2 are available
        if( pos.size() > 2 )
          matrix.lazyReader()->selectDimension(
            vector<int32_t>( pos.begin() + 2, pos.end() ) );

        if( free_row )
          matrix.readRow( pos[0] );
        (*tex)[i][pos_in_tex] = float( matrix( pos[0], pos[1] ) );
        if( free_row )
          matrix.freeRow( pos[0] );
      }
      if( free_same_row )
      {
        if( value_dim == 0 )
          matrix.freeColumn( pos[1] );
        else
          matrix.freeRow( pos[0] );
      }
    }
  }


  void getLabelsPointData( SparseOrDenseMatrix & matrix, int value_dim,
    vector<int32_t> & pos, CiftiTools::TextureList & texlist,
    int tex_index, size_t pos_in_tex, float /* background */, bool single )
  {
    if( single )
    {
      // select needed dimension when more than 2 are available
      if( pos.size() > 2 )
        matrix.lazyReader()->selectDimension(
          vector<int32_t>( pos.begin() + 2, pos.end() ) );
      matrix.readRow( pos[1] );
      (*texlist[tex_index])[0][ pos_in_tex ] = float( matrix( pos[0],
                                                              pos[1] ) );
    }
    else
    {
      size_t i, n = matrix.getSize()[ value_dim ];
      int nsurfaces = texlist.size() / n;
      for( i=0; i<n; ++i )
      {
        pos[value_dim] = i;
        // select needed dimension when more than 2 are available
        if( pos.size() > 2 )
          matrix.lazyReader()->selectDimension(
            vector<int32_t>( pos.begin() + 2, pos.end() ) );
        matrix.readRow( pos[1] );
        (*texlist[ tex_index + i * nsurfaces ])[0][ pos_in_tex ]
          = float( matrix( pos[0], pos[1] ) );
      }
    }
  }


  void getPointData( SparseOrDenseMatrix & matrix, int value_dim,
    vector<int32_t> & pos, CiftiTools::TextureList & texlist,
    int tex_index, size_t pos_in_tex, const string & value_type,
    float background, bool single )
  {
    CiftiTools::TextureList::iterator itex, etex = texlist.end();
    if( value_type == "labels" )
      getLabelsPointData( matrix, value_dim, pos, texlist, tex_index,
                          pos_in_tex, background, single );
    else
      getScalarsPointData( matrix, value_dim, pos, texlist, tex_index,
                           pos_in_tex, background, single );
  }


  unsigned getSurfacesNumberInModel(
    Object surf, const CiftiTools::BrainStuctureToMeshMap & smap )
  {
    unsigned nsurfaces = 0;
    Object siter = surf->objectIterator();
    for( ; siter->isValid(); siter->next() )
    {
      string struct_name = siter->key();
      Object isurf = siter->currentValue();
      unsigned surfnum = smap.find( struct_name )->second;
      if( surfnum >= nsurfaces )
        nsurfaces = surfnum + 1;
    }
    return nsurfaces;
  }

}


void CiftiTools::getSurfaceModelTexture(
  Object surf, CiftiTools::TextureList & texlist,
  const CiftiTools::BrainStuctureToMeshMap & smap,
  SparseOrDenseMatrix & mat, int dim,
  const vector<int> & dim_indices_pos,
  unsigned  nsurfaces ) const
{
  Object siter = surf->objectIterator();
  unsigned i, n = dim_indices_pos.size();
  vector<int32_t> pos( n, 0 );
  for( i=0; i<n; ++i )
    pos[i] = dim_indices_pos[i];
  int value_dim = valuesDimNum();
  string value_type = CiftiTools::dimensionType(
    mat.header()->getProperty( "cifti_dimensions" ), value_dim );
  float background = -1;
  unsigned repet, nrepet = 1;

  if( value_type == "labels" )
    nrepet = CiftiTools::valuesDimSize( mat, value_dim );

  rc_ptr<CiftiTools::RoiTextureList> dim_info;
  map<string, Object> structures;

  for( ; siter->isValid(); siter->next() )
  {
    string struct_name = siter->key();
    Object isurf = siter->currentValue();
    structures[ struct_name ] = isurf;
  }

  unsigned csurf;

  for( repet=0; repet<nrepet; ++repet )
    for( csurf=0; csurf<nsurfaces; ++csurf )
      if( texlist.size() <= repet * nsurfaces + csurf )
      {
        rc_ptr<TimeTexture<float> > tex( new TimeTexture<float> );
        texlist.push_back( tex );
        if( value_type == "labels" )
        {
          if( !dim_info )
            dim_info.reset(
              roiTextureFromDimension( value_dim ) );
          tex->header().copyProperties(
            Object::reference( (*dim_info)[ repet ]->header() ) );
        }
      }

  bool free_row = load_row_or_col( mat, dim, pos );

  map<string, Object>::iterator is, es=structures.end();

  for( is=structures.begin(); is!=es; ++is )
  {
    string struct_name = is->first;
    Object isurf = is->second;
    int tex_index = smap.find( struct_name )->second;
    size_t nnodes
      = size_t( isurf->getProperty( "number_of_nodes" )->getScalar() );
    // fill cache
    if( (int)d->surfaceSize[dim].size() <= tex_index )
      d->surfaceSize[dim].resize( tex_index + 1, 0 );
    d->surfaceSize[dim][tex_index] = nnodes;

    Object indices = isurf->getProperty( "vertices_map" );

    for( repet=0; repet<nrepet; ++repet )
    {
      rc_ptr<TimeTexture<float> > & troi
        = texlist[ tex_index + repet * nsurfaces ];
      troi->header().setProperty( "structure", struct_name );

      Texture<float> & roi = (*troi)[0];
      if( roi.data().empty() )
        roi.data().resize( nnodes );
      if( roi.nItem() < nnodes )
      {
        cout << "warning: number_of_nodes (" << nnodes
          << ") differ from allocated texture size (" << roi.nItem() << ")"
          << endl;
        roi.data().resize( nnodes );
      }

      if( indices->size() != nnodes )
      {
        // fill with background value
        for( i=0; i<nnodes; ++i )
          roi[i] = background;
      }
    }
    Object iiter = indices->objectIterator();

    // build cache
    vector<map<uint32_t, uint32_t> > & vcache = d->dimVertexMatrixMap[ dim ];
    while( (int)vcache.size() <= tex_index )
      vcache.push_back( map<uint32_t, uint32_t>() );
    map<uint32_t, uint32_t> & cache = vcache[ tex_index ];
    uint32_t vertex;

    for( ; iiter->isValid(); iiter->next() )
    {
      pos[dim] = int( rint(
        iiter->currentValue()->getArrayItem(0)->getScalar() ) );
      vertex = uint32_t( rint(
        iiter->currentValue()->getArrayItem(1)->getScalar() ) );
      cache[ vertex ] = uint32_t( pos[dim] );
      getPointData( mat, value_dim, pos, texlist, tex_index, vertex,
        value_type, background, true );
    }
  }

  unload_row_or_col( mat, dim, pos, free_row );
}


void CiftiTools::getBrainModelsTexture(
  Object cifti_info, TextureList & texlist,
  int dim, const vector<int> & dim_indices_pos ) const
{
  bool free_it = load_row_or_col( *_matrix, dim, dim_indices_pos );

  if( !cifti_info )
  {
    // not a Cifti matrix
    rc_ptr<TimeTexture<float> > tex;
    if( texlist.size() < 1 )
    {
      tex.reset( new TimeTexture<float> );
      texlist.push_back( tex );
    }
    tex = texlist[0];
    vector<float> & data = (*tex)[0].data();
    data.clear();
    if( dim == 1 )
    {
      vector<double> ddata = _matrix->getRow( dim_indices_pos[0] );
      data.insert( data.end(), ddata.begin(), ddata.end() );
    }
    else if( dim == 0 )
    {
      vector<double> ddata = _matrix->getColumn( dim_indices_pos[1] );
      data.insert( data.end(), ddata.begin(), ddata.end() );
    }
    unload_row_or_col( *_matrix, dim, dim_indices_pos, free_it );
    return;
  }

  Object models = cifti_info->getProperty( "brain_models" );
  Object iter = models->objectIterator();
  int struct_index = 0;
  unsigned nsurfaces = 0;

  for( ; iter->isValid(); iter->next() )
  {
    try
    {
      Object surf = iter->currentValue()->getProperty( "surface" );
      nsurfaces = std::max( nsurfaces,
                            getSurfacesNumberInModel( surf, _smap ) );
    }
    catch( exception & )
    {
    }
  }

  for( iter = models->objectIterator(); iter->isValid();
      iter->next(), ++struct_index )
  {
    try
    {
      Object surf = iter->currentValue()->getProperty( "surface" );
      getSurfaceModelTexture( surf, texlist, _smap, *_matrix, dim,
                              dim_indices_pos, nsurfaces );
    }
    catch( exception & )
    {
      try
      {
        Object vox = iter->currentValue()->getProperty( "voxels" );
        // TODO
        // getVoxelsModelInfo( surf, texlist, _smap, struct_index );
      }
      catch( exception & )
      {
      }
    }
  }
  unload_row_or_col( *_matrix, dim, dim_indices_pos, free_it );
}


size_t CiftiTools::getBrainStructureMeshNumberOfNodes(
  int dim, const string & struct_name ) const
{
  Object cifti_info = getDimensionObject( dim );
  if( cifti_info.isNull() )
  {
    if( struct_name == "CORTEX" )
      return _matrix->getSize()[ dim ];
    return 0;
  }

  Object models = cifti_info->getProperty( "brain_models" );
  Object iter = models->objectIterator();
  size_t nnodes = 0;

  for( ; iter->isValid(); iter->next() )
  {
    try
    {
      Object surf = iter->currentValue()->getProperty( "surface" );
      Object bstruct = surf->getProperty( struct_name );
      nnodes = size_t( rint( bstruct->getProperty( "number_of_nodes" )
        ->getScalar() ) );
    }
    catch( exception & )
    {
    }
  }

  return nnodes;
}


vector<int> CiftiTools::getIndicesForBrainStructure(
  int dim, const string & struct_name ) const
{
  Object cifti_info = getDimensionObject( dim );
  vector<int> indices;

  if( cifti_info.isNull() )
  {
    if( struct_name == "CORTEX" )
    {
      // assume all cols
      size_t ncols = _matrix->getSize()[ dim ];
      indices.resize( ncols );
      for( size_t i=0; i<ncols; ++i )
        indices[i] = i;
    }
    return indices;
  }

  Object models = cifti_info->getProperty( "brain_models" );
  Object iter = models->objectIterator();

  for( ; iter->isValid(); iter->next() )
  {
    try
    {
      Object surf = iter->currentValue()->getProperty( "surface" );
      getModelIndices( surf, struct_name, indices );
    }
    catch( exception & )
    {
      try
      {
        Object vox = iter->currentValue()->getProperty( "voxels" );
        getModelIndices( vox, struct_name, indices );
      }
      catch( exception & )
      {
      }
    }
  }

  return indices;
}


vector<int> CiftiTools::getIndicesForSurfaceIndices(
  int dim, int surface_num, const vector<int> & roi_indices ) const
{
  Object cifti_info = getDimensionObject( dim );

  if( !cifti_info
      || !cifti_info->hasProperty( "brain_models" ) )
  {
    size_t i, n = std::min( (size_t) roi_indices.size(),
                            (size_t) _matrix->getSize()[ dim ] );
    vector<int> indices( n );
    if( dim == 0 )
      for( i=0; i<n; ++i )
        indices[i] = i;
    else if( dim == 1 )
    {
      int nv = _matrix->getSize()[ dim ];
      for( i=0; i<n; ++i )
        if( roi_indices[i] >= nv )
          indices[i] = 0;
        else
          indices[i] = roi_indices[i];
    }
    return indices;
  }

  Object models = cifti_info->getProperty( "brain_models" );
  Object iter = models->objectIterator();
  vector<int> indices;
  set<int> roi_set( roi_indices.begin(), roi_indices.end() );
  indices.reserve( roi_indices.size() );

  for( ; iter->isValid(); iter->next() )
  {
    try
    {
      Object surf = iter->currentValue()->getProperty( "surface" );
      getSurfaceModelIndicesForRoiIndices( surf, surface_num, roi_set,
                                           indices, _smap );
    }
    catch( exception & )
    {
    }
  }

  return indices;
}



list<string> CiftiTools::getBrainStructures( int dim, bool keepSurfaces,
                                             bool keepVoxels ) const
{
  Object cifti_info = getDimensionObject( dim );
  list<string> structures;

  if( cifti_info.isNull() )
  {
    structures.push_back( "CORTEX" ); // assume single whole cortex mesh
    return structures;
  }

  Object models;
  try
  {
    models = cifti_info->getProperty( "brain_models" );
  }
  catch( ... )
  {
    // not a "brain_model" dimension
    structures.push_back( "CORTEX" ); // assume single whole cortex mesh
    return structures;
  }

  Object iter = models->objectIterator();

  for( ; iter->isValid(); iter->next() )
  {
    Object surf_or_vox = none();
    if( keepSurfaces )
      try
      {
        surf_or_vox = iter->currentValue()->getProperty( "surface" );
      }
      catch( exception & )
      {
      }
    if( keepVoxels && surf_or_vox.isNull() )
      try
      {
        surf_or_vox = iter->currentValue()->getProperty( "voxels" );
      }
      catch( exception & )
      {
      }

    if( !surf_or_vox.isNull() )
    {
      Object siter = surf_or_vox->objectIterator();
      for( ; siter->isValid(); siter->next() )
      {
        string struct_name = siter->key();
        structures.push_back( struct_name );
      }
    }
  }

  return structures;
}


void CiftiTools::getParcelsTexture(
  Object cifti_info, TextureList & texlist,
  int dim, const vector<int> & dim_indices_pos ) const
{
  Object parcels_o = cifti_info->getProperty( "parcels" );
  Object parcels = parcels_o->getProperty( "parcels" );
  Object surfaces;
//   SparseOrDenseMatrix::DenseMatrixType matrix = _matrix->asDense();
  unsigned i, n = dim_indices_pos.size();
  vector<int32_t> pos( n, 0 );
  for( i=0; i<n; ++i )
    pos[i] = dim_indices_pos[i];
  int value_dim = CiftiTools::valuesDimNum();
  string value_type = CiftiTools::dimensionType(
    _matrix->header()->getProperty( "cifti_dimensions" ), value_dim );
  float background = -1;
  try
  {
    surfaces = parcels_o->getProperty( "surface_structures" );
  }
  catch( ... )
  {
  }
  Object piter = parcels->objectIterator();
  size_t label = 0;
  for( ; piter->isValid(); piter->next(), ++label )
  {
    Object parcel = piter->currentValue();
    string roi_name = parcel->getProperty( "name" )->getString();
    Object par_def = parcel->getProperty( "surface_nodes" );
    Object pditer = par_def->objectIterator();
    for( ; pditer->isValid(); pditer->next() )
    {
      string struct_name = pditer->key();
      Object elem = pditer->currentValue();
      int surfnum = _smap.find( struct_name )->second;
      while( (int)texlist.size() <= surfnum )
        texlist.push_back( rc_ptr<TimeTexture<float> >() );
      int tex_index = _smap.find( struct_name )->second;
      rc_ptr<TimeTexture<float> > & troi = texlist[ tex_index ];
      if( troi.isNull() )
      {
        troi.reset( new TimeTexture<float> );
        /* set palette in RGB interpolation mode: values are continuous float
           values, not labels, but they are assigned to "flat" regions, and
           we expect discontinuities at regions boundaries: neighbouring
           vertices with different values should not interpolate values.
        */
        Object texplist = Object::value( ObjectVector() );
        troi->header().setProperty( "texture_properties", texplist );
        Object texp = Object::value( Dictionary() );
        texplist->insertArrayItem( -1, texp );
        texp->setProperty( "interpolation", "rgb" );
      }
      Texture<float> & roi = (*troi)[0];
      if( surfaces )
      {
        Object surf_str = surfaces->getProperty( struct_name );
        size_t nnodes
          = size_t( surf_str->getProperty( "number_of_nodes" )->getScalar() );
        if( roi.nItem() == 0 )
        {
          roi.data().resize( nnodes );
          for( size_t i=0; i<nnodes; ++i )
            roi[i] = background;
        }
        /* else if( roi.nItem() != nnodes )
        {
          cerr << "Warning: size mismatch in surfaces\n";
          cerr << "roi: " << roi.nItem() << ", nodes: " << nnodes << endl;
        } */
      }
      Object viter = elem->objectIterator();
      int64_t v;
      for( ; viter->isValid(); viter->next() )
      {
        v = int( rint( viter->currentValue()->getScalar() ) );
        if( (int)roi.nItem() <= v )
          roi.data().resize( v + 1 );
        pos[dim] = label;
        // select needed dimension when more than 2 are available
        if( pos.size() > 2 )
          _matrix->lazyReader()->selectDimension(
            vector<int32_t>( pos.begin() + 2, pos.end() ) );
        _matrix->readRow( pos[1] );
        roi[v] = float( (*_matrix)( pos[0], pos[1] ) );

        getPointData( *_matrix, value_dim, pos, texlist, tex_index, v,
          value_type, background, false );

      }
    }
  }
}


CiftiTools::BrainStuctureToMeshMap CiftiTools::defaultBrainStructureToMeshMap()
{
  static BrainStuctureToMeshMap smap;
  if( smap.empty() )
  {
    smap[ "CORTEX_LEFT" ] = 0;
    smap[ "CortexLeft" ] = 0;
    smap[ "CORTEX_RIGHT" ] = 1;
    smap[ "CortexRight" ] = 1;
    smap[ "CEREBELLUM" ] = 0;
    smap[ "Cerebellum" ] = 0;
    smap[ "ACCUMBENS_LEFT" ] = 0;
    smap[ "AccumbensLeft" ] = 0;
    smap[ "ACCUMBENS_RIGHT" ] = 1;
    smap[ "AccumbensRight" ] = 1;
    smap[ "ALL" ] = 0;
    smap[ "All" ] = 0;
    smap[ "ALL_GREY_MATTER" ] = 0;
    smap[ "AllGreyMatter" ] = 0;
    smap[ "ALL_WHITE_MATTER" ] = 0;
    smap[ "AllWhiteMatter" ] = 0;
    smap[ "AMYGDALA_LEFT" ] = 0;
    smap[ "AmygdalaLeft" ] = 0;
    smap[ "AMYGDALA_RIGHT" ] = 1;
    smap[ "AmygdalaRight" ] = 1;
    smap[ "BRAIN_STEM" ] = 0;
    smap[ "BrainStem" ] = 0;
    smap[ "CAUDATE_LEFT" ] = 0;
    smap[ "CaudateLeft" ] = 0;
    smap[ "CAUDATE_RIGHT" ] = 1;
    smap[ "CaudateRight" ] = 1;
    smap[ "CEREBELLAR_WHITE_MATTER_LEFT" ] = 0;
    smap[ "CerebellarWhiteMatterLeft" ] = 0;
    smap[ "CEREBELLAR_WHITE_MATTER_RIGHT" ] = 1;
    smap[ "CerebellarWhiteMatterRight" ] = 1;
    smap[ "CEREBELLUM_LEFT" ] = 0;
    smap[ "CerebellumLeft" ] = 0;
    smap[ "CEREBELLUM_RIGHT" ] = 1;
    smap[ "CerebellumRight" ] = 1;
    smap[ "CEREBRAL_WHITE_MATTER_LEFT" ] = 0;
    smap[ "CerebralWhiteMatterLeft" ] = 0;
    smap[ "CEREBRAL_WHITE_MATTER_RIGHT" ] = 1;
    smap[ "CerebralWhiteMatterRight" ] = 1;
    smap[ "CORTEX" ] = 0;
    smap[ "Cortex" ] = 0;
    smap[ "DIENCEPHALON_VENTRAL_LEFT" ] = 0;
    smap[ "DiencephalonVentralLeft" ] = 0;
    smap[ "DIENCEPHALON_VENTRAL_RIGHT" ] = 1;
    smap[ "DiencephalonVentralRight" ] = 1;
    smap[ "HIPPOCAMPUS_LEFT" ] = 0;
    smap[ "HippocampusLeft" ] = 0;
    smap[ "HIPPOCAMPUS_RIGHT" ] = 1;
    smap[ "HippocampusRight" ] = 1;
    smap[ "INVALID" ] = 0;
    smap[ "Invalid" ] = 0;
    smap[ "OTHER" ] = 0;
    smap[ "Other" ] = 0;
    smap[ "OTHER_GREY_MATTER" ] = 0;
    smap[ "OtherGreyMatter" ] = 0;
    smap[ "OTHER_WHITE_MATTER" ] = 0;
    smap[ "OtherWhiteMatter" ] = 0;
    smap[ "PALLIDUM_LEFT" ] = 0;
    smap[ "PallidumLeft" ] = 0;
    smap[ "PALLIDUM_RIGHT" ] = 1;
    smap[ "PallidumRight" ] = 1;
    smap[ "PUTAMEN_LEFT" ] = 0;
    smap[ "PutamenLeft" ] = 0;
    smap[ "PUTAMEN_RIGHT" ] = 1;
    smap[ "PutamenRight" ] = 1;
    smap[ "THALAMUS_LEFT" ] = 0;
    smap[ "ThalamusLeft" ] = 0;
    smap[ "THALAMUS_RIGHT" ] = 1;
    smap[ "ThalamusRight" ] = 1;
  }
  return smap;
}


string CiftiTools::dimensionType( Object cifti_info, int dim )
{
  if( dim < 0 )
    return "scalars";
  Object cdim = cifti_info->getArrayItem( dim );
  if( cdim->hasProperty( "labels" ) )
    return "labels";
  else if( cdim->hasProperty( "scalars" ) )
    return "scalars";
  else if( cdim->hasProperty( "series" ) )
    return "series";
  else if( cdim->hasProperty( "parcels" ) )
    return "parcels";
  else if( cdim->hasProperty( "brain_models" ) )
    return "brain_models";

  return "";
}


string CiftiTools::dimensionType( int dim ) const
{
  return dimensionType( _matrix->header()->getProperty( "cifti_dimensions" ),
                        dim );
}


int CiftiTools::valuesDimNum( Object cifti_info )
{
  unsigned dim, ndim = cifti_info->size();
  set<string> values_dims;
  values_dims.insert( "labels" );
  values_dims.insert( "scalars" );
  values_dims.insert( "series" );
  string dim_type;

  for( dim=0; dim!=ndim; ++dim )
  {
    dim_type = dimensionType( cifti_info, dim );
    if( values_dims.find( dim_type ) != values_dims.end() )
      return dim;
  }

  return - 1;
}


size_t CiftiTools::valuesDimSize( const SparseOrDenseMatrix & mat,
                                  int value_dim )
{
  if( value_dim < 0 )
    return 1;
  size_t len = unsigned( rint(
        mat.header()->getProperty( "cifti_dimensions" )
          ->getArrayItem( value_dim )->getProperty( "dimension_length" )
          ->getScalar() ) );
  return len;
}


int CiftiTools::valuesDimNum() const
{
  Object hdr = _matrix->header();
  if( !hdr )
    return 0;
  Object cifti_dims;
  try
  {
    cifti_dims = hdr->getProperty( "cifti_dimensions" );
  }
  catch( exception & )
  {
    return 0;
  }
  return valuesDimNum( cifti_dims );
}


namespace
{
  template <typename T>
  class MeshOrTexInfo
  {
  public:
    static size_t nvertices( const T & mesh_or_tex );
  };

  template <>
  size_t MeshOrTexInfo<AimsSurfaceTriangle>::nvertices(
    const AimsSurfaceTriangle & mesh )
  {
    return mesh.vertex().size();
  }

  template <typename T>
  class MeshOrTexInfo<TimeTexture<T> >
  {
  public:
    static size_t nvertices( const TimeTexture<T> & tex )
    {
      return tex.begin()->second.nItem();
    }
  };


  string upper( const string & text )
  {
    string s = text;
    size_t i, n = text.size();
    for( i=0; i<n; ++i )
      if( s[i] >= 'a' && s[i] <= 'z' )
        s[i] -= 'a' - 'A';
    return s;
  }


  string upper_with_underscores( const string & text )
  {
    string s;
    size_t i, n = text.size();
    for( i=0; i<n; ++i )
    {
      char c = s[i];
      if( c >= 'a' && c <= 'z' )
        c -= 'a' - 'A';
      else if( c >= 'A' && c <= 'Z' && !s.empty() && s[s.length() - 1] != '_' )
        s += '_';
      s += c;
    }
    return s;
  }


  string getBrainStructure( const CiftiTools & c, const Object & hdr )
  {
    string bstruct = "CORTEX";
    string structure;

    if( hdr->hasProperty( "GIFTI_dataarrays_info" ) )
    {
      Object dai = hdr->getProperty( "GIFTI_dataarrays_info" );
      try
      {
        Object da0 = dai->getArrayItem( 0 );
        Object meta = da0->getProperty( "GIFTI_metadata" );
        structure = meta->getProperty(
          "AnatomicalStructurePrimary" )->getString();
      }
      catch( ... )
      {
        try
        {
          Object meta = hdr->getProperty( "GIFTI_metadata" );
          structure = meta->getProperty(
            "AnatomicalStructurePrimary" )->getString();
        }
        catch( ... )
        {
        }
      }
    }
    if( !structure.empty() )
    {
      const CiftiTools::BrainStuctureToMeshMap
        & bsmap = c.brainStructureMap();
      if( bsmap.find( structure ) != bsmap.end() )
        bstruct = structure;
      else
      {
        string s2 = upper( structure );
        if( bsmap.find( s2 ) != bsmap.end() )
          bstruct = structure;
        else
        {
          s2 = upper_with_underscores( structure );
          if( bsmap.find( s2 ) != bsmap.end() )
            bstruct = s2;
        }
      }
    }
    return bstruct;
  }

}


template <typename T>
bool CiftiTools::isMatchingSurfaceOrTexture( int dim, const T & mesh,
                                             list<string> & brainmodels,
                                             int & surf_index,
                                             int surf_hint ) const
{
  MeshOrTexInfo<T> info;
  return isMatchingSurfaceOrTexture( dim, info.nvertices( mesh ),
                                     Object::reference( mesh.header() ),
                                     brainmodels, surf_index, surf_hint );
}


bool CiftiTools::isMatchingSurfaceOrTexture( int dim, size_t nvert,
                                             const Object header,
                                             list<string> & brainmodels,
                                             int & surf_index,
                                             int surf_hint ) const
{
  Object dimobj = getDimensionObject( dim );
  if( dimobj.isNull() )
    return ( nvert == (size_t)_matrix->getSize()[ dim ] );

  if( dimensionType( dim ) != "brain_models" )
    return false;

  RoiTextureList *texlist = roiTextureFromDimension( dim );
  int i, j, ntex = (int) texlist->size();
  vector<int> indices( ntex );
  bool result = false;

  if( surf_hint >= 0 && surf_hint < ntex )
    indices[0] = surf_hint;
  for( i=0, j=1; i<ntex; ++i )
  {
    if( i != surf_hint )
      indices[j++] = i;
  }

  for( i=0; !result && i<ntex; ++i )
  {
    TimeTexture<int> & tex = *(*texlist)[indices[i]];
    if( tex[0].nItem() != nvert )
      continue;
    // mesh is of matching size. Check brain structures
    string meshstruct = getBrainStructure( *this, header );
    const BrainStuctureToMeshMap bsmap = brainStructureMap();

    if( bsmap.find( meshstruct )->second != indices[i] )
      continue;

    brainmodels.clear();
    Object lmap = tex.header().getProperty( "labels_map" );
    Object it = lmap->objectIterator();
    for( ; it->isValid(); it->next() )
      brainmodels.push_back( it->currentValue()->getString() );

    surf_index = indices[i];
    result = true;
  }

  delete texlist;
  return result;
}


template <typename T>
void CiftiTools::buildDimensionObjectFromLabelsTexture(
  int dim, const TimeTexture<T> & tex )
{
  Object hdr = _matrix->header();
  if( !hdr )
  {
    hdr = Object::value( PropertySet() );
    _matrix->setHeader( hdr );
  }

  Object cifti_dims;
  try
  {
    cifti_dims = hdr->getProperty( "cifti_dimensions" );
  }
  catch( exception & )
  {
    cifti_dims = Object::value( vector<Object>() );
    hdr->setProperty( "cifti_dimensions", cifti_dims );
  }

  vector<Object> & dims_vec = cifti_dims->value< vector<Object> >();

  while( dim >= dims_vec.size() )
    dims_vec.push_back( Object::value( Dictionary() ) );

  Object cifti_info = dims_vec[ dim ];
  cifti_info->setProperty( "dimension_length", _matrix->getSize()[ dim ] );
  Object parcels_o = Object::value( Dictionary() );
  cifti_info->setProperty( "parcels", parcels_o );
  Object parcels = Object::value( map<int, Object>() );
  parcels_o->setProperty( "parcels", parcels );
  Object surfaces = Object::value( Dictionary() );
  parcels_o->setProperty( "surface_structures", surfaces );

  const Texture<T> & texture = tex.begin()->second;
  map<int, Object> & parc = parcels->value< map<int, Object> >();

  _smap[ "COREXT" ] = 0;

  size_t i, n = texture.nItem();
  Object par_def;
  for( i=0; i<n; ++i )
  {
    int region = texture[i];
    Object & plabel = parc[ region ];

    if( !plabel )
    {
      plabel.reset( new ValueObject<Dictionary> );
      stringstream name;
      name << region;
      plabel->setProperty( "name", name.str() );
      par_def = Object::value( Dictionary() );
      plabel->setProperty( "surface_nodes", par_def );
      par_def->setProperty( "CORTEX", vector<int>() );
    }
    else
      par_def = plabel->getProperty( "surface_nodes" );

    vector<int> & roi
      = par_def->getProperty( "CORTEX" )->value< vector<int> >();
    roi.push_back( i );
  }

  n = _matrix->getSize()[ dim ];
  if( parc.size() < n )
  {
    /* the size of the matrix doesn't match the number of actually used
       regions: perhaps the matrix is lager (uses emty lines / cols) to
       match the regions numbers.
       fill in missing (unused) regions to match matrix definition */
    map<int, Object>::iterator im;
    for( i=0; i<n; ++i )
    {
      if( parc.find( i ) == parc.end() )
      {
        Object plabel = Object::value( Dictionary() );
        stringstream name;
        name << i;
        plabel->setProperty( "name", name.str() );
        par_def = Object::value( Dictionary() );
        plabel->setProperty( "surface_nodes", par_def );
        par_def->setProperty( "CORTEX", vector<int>() );
        parc[i] = plabel;
      }
    }
  }

  Object surf_str = Object::value( Dictionary() );
  surfaces->setProperty( "CORTEX", surf_str );
  surf_str->setProperty( "number_of_nodes", n );
}



template bool
CiftiTools::isMatchingSurfaceOrTexture<AimsSurfaceTriangle>(
  int, const AimsSurfaceTriangle &, list<string> &, int &, int ) const;
template bool
CiftiTools::isMatchingSurfaceOrTexture<TimeTexture<int16_t> >(
  int, const TimeTexture<int16_t> &, list<string> &, int &, int ) const;
template bool
CiftiTools::isMatchingSurfaceOrTexture<TimeTexture<int32_t> >(
  int, const TimeTexture<int32_t> &, list<string> &, int &, int ) const;
template bool
CiftiTools::isMatchingSurfaceOrTexture<TimeTexture<float> >(
  int, const TimeTexture<float> &, list<string> &, int &, int ) const;

template void CiftiTools::buildDimensionObjectFromLabelsTexture(
  int dim, const TimeTexture<float> & tex );
template void CiftiTools::buildDimensionObjectFromLabelsTexture(
  int dim, const TimeTexture<int> & tex );
template void CiftiTools::buildDimensionObjectFromLabelsTexture(
  int dim, const TimeTexture<int16_t> & tex );

