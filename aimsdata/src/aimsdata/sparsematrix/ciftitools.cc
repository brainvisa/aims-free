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

#include <aims/sparsematrix/ciftitools.h>
#include <aims/sparsematrix/sparseordensematrix.h>
#include <aims/mesh/texture.h>

using namespace aims;
using namespace carto;
using namespace std;

CiftiTools::CiftiTools( rc_ptr<SparseOrDenseMatrix> matrix,
                        const BrainStuctureToMeshMap & smap )
{
  _matrix = matrix;
  if( smap.empty() )
    _smap = defaultBrainStructureToMeshMap();
  else
    _smap = smap;
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
  if( dim >= cifti_dims->size() )
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
  Object cdim = getDimensionObject( dim );
  if( !cdim )
    return 0;

  RoiTextureList *roilist = new RoiTextureList;
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
      while( roilist.size() <= surfnum )
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
        else if( roi.nItem() != nnodes )
        {
          cerr << "Warning: size mismatch in surfaces\n";
        }
      }
      Object viter = elem->objectIterator();
      int64_t v;
      for( ; viter->isValid(); viter->next() )
      {
        v = int( rint( viter->currentValue()->getScalar() ) );
        if( roi.nItem() <= v )
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
      while( roilist.size() <= surfnum )
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


CiftiTools::TextureList *CiftiTools::expandedValueTextureFromDimension(
  int dim,
  const vector<int> & dim_indices_pos,
  TextureList* existing_tex_list ) const
{
  Object cdim = getDimensionObject( dim );
  if( !cdim )
    return 0;

  TextureList *texlist;
  if( existing_tex_list )
    texlist = existing_tex_list;
  else
    texlist = new TextureList;

  BrainStuctureToMeshMap dsmap;

  if( cdim->hasProperty( "labels" ) )
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
  else if( cdim->hasProperty( "brain_models" ) )
  {
    getBrainModelsTexture( cdim, *texlist, dim, dim_indices_pos );
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
    int tex_index, size_t pos_in_tex, float background, bool single )
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
      int surfnum = smap.find( struct_name )->second;
      if( surfnum >= nsurfaces )
        nsurfaces = surfnum + 1;
    }
    return nsurfaces;
  }


  void getSurfaceModelTexture( const CiftiTools & cifti,
                               Object surf, CiftiTools::TextureList & texlist,
                               const CiftiTools::BrainStuctureToMeshMap & smap,
                               SparseOrDenseMatrix & mat, int dim,
                               const vector<int> & dim_indices_pos,
                               unsigned  nsurfaces )
  {
    cout << "getSurfaceModelTexture, dim: " << dim << endl;
    Object siter = surf->objectIterator();
    unsigned i, n = dim_indices_pos.size();
    vector<int32_t> pos( n, 0 );
    for( i=0; i<n; ++i )
      pos[i] = dim_indices_pos[i];
    int value_dim = cifti.valuesDimNum();
    cout << "value_dim: " << value_dim << endl;
    string value_type = CiftiTools::dimensionType(
      mat.header()->getProperty( "cifti_dimensions" ), value_dim );
    float background = -1;
    unsigned repet, nrepet = 1;

    cout << "value_type: " << value_type << endl;

    if( value_type == "labels" )
      nrepet = CiftiTools::valuesDimSize( mat, value_dim );
    cout << "nrepet: " << nrepet << endl;

    rc_ptr<CiftiTools::RoiTextureList> dim_info;
    map<string, Object> structures;

    for( ; siter->isValid(); siter->next() )
    {
      string struct_name = siter->key();
      Object isurf = siter->currentValue();
      structures[ struct_name ] = isurf;
    }
    cout << "structures: " << structures.size() << endl;
    cout << "nsurfaces: " << nsurfaces << endl;

    unsigned ntex = nsurfaces * nrepet;
    int csurf;

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
                cifti.roiTextureFromDimension( value_dim ) );
            tex->header().copyProperties(
              Object::reference( (*dim_info)[ repet ]->header() ) );
          }
        }

    map<string, Object>::iterator is, es=structures.end();

    for( is=structures.begin(); is!=es; ++is )
    {
      string struct_name = is->first;
      cout << "structure: " << struct_name << endl;
      Object isurf = is->second;
      int tex_index = smap.find( struct_name )->second;
      cout << "tex_index: " << tex_index << endl;
      size_t nnodes
        = size_t( isurf->getProperty( "number_of_nodes" )->getScalar() );
      cout << "nnodes: " << nnodes << endl;
      Object indices = isurf->getProperty( "vertices_map" );
      cout << "indices: " << indices->size() << endl;

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

      if( pos.size() > 2 )
        mat.lazyReader()->selectDimension( vector<int>( pos.begin() + 2,
                                                        pos.end() ) );
      if( dim == 0 )
        mat.readColumn( pos[1] );
      else if( dim == 1 )
        mat.readRow( pos[0] );

      for( ; iiter->isValid(); iiter->next() )
      {
        pos[dim] = int( rint(
          iiter->currentValue()->getArrayItem(0)->getScalar() ) );
        getPointData( mat, value_dim, pos, texlist, tex_index,
          int( rint( iiter->currentValue()->getArrayItem(1)->getScalar() ) ),
          value_type, background, true );
      }
    }
  }

}


void CiftiTools::getBrainModelsTexture(
  Object cifti_info, TextureList & texlist,
  int dim, const vector<int> & dim_indices_pos ) const
{
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
      getSurfaceModelTexture( *this, surf, texlist, _smap, *_matrix, dim,
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
}


vector<int> CiftiTools::getIndicesForBrainStructure(
  int dim, const string & struct_name ) const
{
  Object cifti_info = getDimensionObject( dim );

  Object models = cifti_info->getProperty( "brain_models" );
  Object iter = models->objectIterator();
  vector<int> indices;

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


list<string> CiftiTools::getBrainStructures( int dim ) const
{
  Object cifti_info = getDimensionObject( dim );

  Object models = cifti_info->getProperty( "brain_models" );
  Object iter = models->objectIterator();
  list<string> structures;

  for( ; iter->isValid(); iter->next() )
  {
    Object surf_or_vox;
    try
    {
      surf_or_vox = iter->currentValue()->getProperty( "surface" );
    }
    catch( exception & )
    {
      try
      {
        surf_or_vox = iter->currentValue()->getProperty( "voxels" );
      }
      catch( exception & )
      {
        continue;
      }
    }

    Object siter = surf_or_vox->objectIterator();
    for( ; siter->isValid(); siter->next() )
    {
      string struct_name = siter->key();
      structures.push_back( struct_name );
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
      while( texlist.size() <= surfnum )
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
        else if( roi.nItem() != nnodes )
        {
          cerr << "Warning: size mismatch in surfaces\n";
        }
      }
      Object viter = elem->objectIterator();
      int64_t v;
      for( ; viter->isValid(); viter->next() )
      {
        v = int( rint( viter->currentValue()->getScalar() ) );
        if( roi.nItem() <= v )
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


