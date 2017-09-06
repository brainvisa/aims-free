/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
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

#include <aims/io/ciftiobject.h>
#include <CiftiException.h>
#include <Label.h>

using namespace aims;
using namespace cifti;
using namespace carto;
using namespace std;


namespace
{

  Object metadataObject( const MetaData& metadata ); // done
  Object labelTableOject( const LabelTable & ); // done
  Object brainModelObject( const CiftiXML& myXML, int whichDim );
  Object labelsObject( const CiftiXML& myXML, int whichDim ); // done
  Object parcelsObject( const CiftiXML& myXML, int whichDim ); // ?
  Object scalarsObject( const CiftiXML& myXML, int whichDim ); // done
  Object volumeSpaceObject( const VolumeSpace & ); // done
  Object surfaceObject( const CiftiBrainModelsMap& myMap, int i );
  Object voxelsObject( const CiftiBrainModelsMap& myMap, int i );


  Object metadataObject( const MetaData& metadata )
  {
    map<AString, AString> mmap = metadata.getAsMap();
    if( mmap.empty() )
      return none();
    Object mdict = Object::value( Dictionary() );
    map<AString, AString>::const_iterator im, em = mmap.end();
    for( im=mmap.begin(); im!=em; ++im )
      mdict->setProperty( AString_to_std_string( im->first ),
                          AString_to_std_string( im->second ) );
    return mdict;
  }


  Object labelTableOject( const LabelTable & ltable )
  {
    set<int32_t> keys = ltable.getKeys();
    if( keys.empty() )
      return none();
    Object labels = Object::value( IntDictionary() );
    set<int32_t>::const_iterator ik, ek = keys.end();
    vector<float> rgba(4);
    for( ik=keys.begin(); ik!=ek; ++ik )
    {
      Object lelem = Object::value( Dictionary() );
      labels->insertArrayItem( *ik, lelem );
      const Label* clabel = ltable.getLabel( *ik );
      lelem->setProperty( "label",
                          AString_to_std_string(
                            ltable.getLabelName( *ik ) ) );
      ltable.getLabelColor( *ik, &rgba[0] );
      lelem->setProperty( "color", rgba );
    }
    return labels;
  }


  Object volumeSpaceObject( const VolumeSpace & volspace )
  {
    Object vspace = Object::value( Dictionary() );

    const int64_t* dims = volspace.getDims();
    vector<int64_t> vdims(3);
    vdims[0] = dims[0];
    vdims[1] = dims[1];
    vdims[2] = dims[2];
    vspace->setProperty( "dims", vdims );

    const std::vector<std::vector<float> >& sform = volspace.getSform();
    vector<float> vsform;
    vsform.reserve( 16 );
    for( int r=0; r<4; ++r )
      for( int c=0; c<4; ++c )
        vsform.push_back( sform[r][c] );
    vspace->setProperty( "sform", vsform );

    // the following is deduced from dims/sform and can be omitted
    // moreover
    VolumeSpace::OrientTypes orient[3];

    volspace.getOrientation( orient );
    vector<string> orient_s(3);
    string orient_names[] = {"left_to_right", "posterior_to_anterior",
      "inferior_to_superior", "", "right_to_left", "anterior_to_superior",
      "superior_to_inferior",
    };
    for( int i=0; i<3; ++i )
      orient_s[i] = orient_names[ orient[i] ];
    vspace->setProperty( "orientation", orient_s );

    return vspace;
  }


  Object surfaceObject( const CiftiBrainModelsMap& myMap, int i )
  {
    Object content = Object::value( Dictionary() );
    vector<CiftiBrainModelsMap::ModelInfo> myInfo = myMap.getModelInfo();
    Object sdict = Object::value( Dictionary() );
    StructureEnum::Enum snum = myInfo[i].m_structure;
    string structure =
      AString_to_std_string( StructureEnum::toName( snum ) );
    content->setProperty( structure, sdict );
    sdict->setProperty(
      "number_of_nodes", myMap.getSurfaceNumberOfNodes( snum ) );
    sdict->setProperty( "index_count", myInfo[i].m_indexCount );
    sdict->setProperty( "label_number", int( snum ) );

    vector<CiftiBrainModelsMap::SurfaceMap>
      smap = myMap.getSurfaceMap( snum );
//     Object indiceso = Object::value( map<int, int>() );
//     // WARNING should use int64 here
//     map<int, int> & indices = indiceso->value< map<int, int> >();
    Object indiceso = Object::value( vector<vector<int64_t> >() );
    vector<vector<int64_t> > & indices
      = indiceso->value< vector<vector<int64_t> > >();
    indices.reserve( smap.size() );
    vector<CiftiBrainModelsMap::SurfaceMap>::const_iterator
      im, em = smap.end();
    vector<int64_t> item;
    for( im=smap.begin(); im!=em; ++im )
    {
      item = vector<int64_t>( 2 );
      item[0] = im->m_ciftiIndex;
      item[1] = im->m_surfaceNode;
      indices.push_back( item );
//       indices[ (int) im->m_ciftiIndex ] = (int) im->m_surfaceNode;
    }
    sdict->setProperty( "vertices_map", indiceso );

    return content;
  }


  Object voxelsObject( const CiftiBrainModelsMap& myMap, int i )
  {
    Object content = Object::value( Dictionary() );
    vector<CiftiBrainModelsMap::ModelInfo> myInfo = myMap.getModelInfo();
    StructureEnum::Enum snum = myInfo[i].m_structure;
    string structure =
      AString_to_std_string( StructureEnum::toName(
        myInfo[i].m_structure ) );
    Object sdict = Object::value( Dictionary() );
    content->setProperty( structure, sdict );
    sdict->setProperty( "index_count",
                        myInfo[i].m_indexCount );
    sdict->setProperty( "label_number", int( snum ) );
    vector<CiftiBrainModelsMap::VolumeMap>
      vmap = myMap.getVolumeStructureMap( snum );
    Object indiceso = Object::value( IntDictionary() );
    vector<CiftiBrainModelsMap::VolumeMap>::const_iterator
      im, em = vmap.end();
    vector<int64_t> item;
    for( im=vmap.begin(); im!=em; ++im )
    {
      item = vector<int64_t>( 3 );
      item[0] = im->m_ijk[0];
      item[1] = im->m_ijk[1];
      item[2] = im->m_ijk[2];
      indiceso->insertArrayItem( (int) im->m_ciftiIndex,
                                 Object::value( item ) );
    }
    sdict->setProperty( "vertices_map", indiceso );

    return content;
  }


  Object brainModelObject( const CiftiXML& myXML, int whichDim )
  {
    Object mvec = Object::value( vector<Object>() );

    const CiftiBrainModelsMap& myMap = myXML.getBrainModelsMap(whichDim);
    vector<CiftiBrainModelsMap::ModelInfo> myInfo = myMap.getModelInfo();//this is only summary info, same order as the models are in the cifti indices
    Object content;
    for (int i = 0; i < (int)myInfo.size(); ++i)//to get the lists of vertices/voxels for a model, see getSurfaceMap, getVolumeStructureMap, and getFullVolumeMap
    {
      Object mdict = Object::value( Dictionary() );
      mvec->insertArrayItem( -1, mdict );

      switch (myInfo[i].m_type)
      {
        case CiftiBrainModelsMap::SURFACE:
        {
          content = surfaceObject( myMap, i );
          mdict->setProperty( "surface", content );
          break;
        }
        case CiftiBrainModelsMap::VOXELS:
        {
          content = voxelsObject( myMap, i );
          mdict->setProperty( "voxels", content );
          break;
        }
      }
    }

    return mvec;
  }


  Object labelsObject( const CiftiXML& myXML, int whichDim )
  {
    Object lvec = Object::value( vector<Object>() );

    const CiftiLabelsMap& myMap = myXML.getLabelsMap(whichDim);
    Object meta, value;

    for (int i = 0; i < myMap.getLength(); ++i)
    {
      value = Object::value( Dictionary() );
      lvec->insertArrayItem( -1, value );
      meta = metadataObject( myMap.getMapMetadata( i ) );
      if( !meta.isNull() )
        value->setProperty( "metadata", meta );
      value->setProperty(
        "name", AString_to_std_string( myMap.getMapName( i ) ) );
      const LabelTable & ltable = myMap.getMapLabelTable( i );
      Object lto = labelTableOject( ltable );
      if( !lto.isNull() )
        value->setProperty( "label_table", lto );
    }

    return lvec;
  }


  Object parcelsObject( const CiftiXML& myXML, int whichDim )
  {
    Object parcels = Object::value( Dictionary() );
    Object lvec = Object::value( vector<Object>() );
    parcels->setProperty( "parcels", lvec );

    const CiftiParcelsMap& myMap = myXML.getParcelsMap(whichDim);
    const vector<CiftiParcelsMap::Parcel>& myParcels = myMap.getParcels();
//     set<StructureEnum::Enum> structures;

    for (int i = 0; i < (int)myParcels.size(); ++i)
    {
      Object pdict = Object::value( Dictionary() );
      lvec->insertArrayItem( -1, pdict );
      pdict->setProperty(
        "name", AString_to_std_string( myParcels[i].m_name ) );

      int numVerts = 0;
      if( !myParcels[i].m_surfaceNodes.empty() )
      {
        Object surf = Object::value( Dictionary() );
        pdict->setProperty( "surface_nodes", surf );

        for (map<StructureEnum::Enum, set<int64_t> >::const_iterator
            iter = myParcels[i].m_surfaceNodes.begin();
            iter != myParcels[i].m_surfaceNodes.end();
            ++iter)
        {
          Object value = Object::value( Dictionary() );
          string key = AString_to_std_string( StructureEnum::toName(
            iter->first ) );
          surf->setProperty( key, iter->second );
          numVerts += iter->second.size();
//           structures.insert( iter->first );
        }
      }
      if( !myParcels[i].m_voxelIndices.empty() )
      {
        Object voxels = Object::value( vector<vector<int64_t> >() );
        vector<vector<int64_t> > voxval
          = voxels->value< vector<vector<int64_t> > >();
        voxval.reserve( myParcels[i].m_voxelIndices.size() );
        pdict->setProperty( "voxel_indices", voxels );
        std::set<VoxelIJK>::const_iterator
          iv, ev = myParcels[i].m_voxelIndices.end();
        for( iv=myParcels[i].m_voxelIndices.begin(); iv!=ev; ++iv )
        {
          vector<int64_t> voxel( 3 );
          voxel[0] = iv->m_ijk[0];
          voxel[1] = iv->m_ijk[1];
          voxel[2] = iv->m_ijk[2];
          voxval.push_back( voxel );
        }
      }
    }

    if( myMap.hasVolumeData() )
    {
      const VolumeSpace& volspace = myMap.getVolumeSpace();
      Object vspace = volumeSpaceObject( volspace );
      if( !vspace.isNull() )
        parcels->setProperty( "volume_space", vspace );
    }

    // surfaces elements definitions
    vector<StructureEnum::Enum>
      structures = myMap.getParcelSurfaceStructures();
    if( !structures.empty() )
    {
      Object surfaces = Object::value( Dictionary() );
      parcels->setProperty( "surface_structures", surfaces );
      vector<StructureEnum::Enum>::const_iterator is, es = structures.end();
      for( is=structures.begin(); is!=es; ++is )
      {
        Object surfelem = Object::value( Dictionary() );
        int64_t nv = myMap.getSurfaceNumberOfNodes(*is);
        string key = AString_to_std_string( StructureEnum::toName( *is ) );
        surfaces->setProperty( key, surfelem );
        surfelem->setProperty( "number_of_nodes", nv );
      }
    }

    return parcels;
  }


  Object scalarsObject( const CiftiXML& myXML, int whichDim )
  {
    Object svec = Object::value( vector<Object>() );

    const CiftiScalarsMap& myMap = myXML.getScalarsMap(whichDim);
    Object meta, value;
    for (int i = 0; i < myMap.getLength(); ++i)
    {
      meta = metadataObject( myMap.getMapMetadata( i ) );
      value = Object::value( Dictionary() );
      if( !meta.isNull() )
        value->setProperty( "metadata", meta );
      value->setProperty( "name",
                          AString_to_std_string( myMap.getMapName( i ) ) );
      svec->insertArrayItem( -1, value );
    }

    return svec;
  }


  Object seriesObject( const CiftiXML& myXML, int whichDim )
  {
    Object series = Object::value( Dictionary() );

    const CiftiSeriesMap& myMap = myXML.getSeriesMap(whichDim);
    series->setProperty( "length", myMap.getLength() );
    series->setProperty( "start", myMap.getStart() );
    series->setProperty( "step", myMap.getStep() );
    series->setProperty(
      "unit",
      AString_to_std_string( CiftiSeriesMap::unitToString(
        myMap.getUnit() ) ) );

    return series;
  }

}


Object CiftiObjectConverter::ciftiToObject( CiftiFile & inputFile ) const
{
  Object hdr = Object::value( Dictionary() );

  // parse xml

  string key;
  Object ciftidims = Object::value( vector<Object>() );
  hdr->setProperty( "cifti_dimensions", ciftidims );

  const CiftiXML& myXML = inputFile.getCiftiXML();

  Object meta = metadataObject( myXML.getFileMetaData() );
  if( !meta.isNull() )
    hdr->setProperty( "metadata", meta );

  for (int whichDim = 0; whichDim < myXML.getNumberOfDimensions(); ++whichDim)
  {
    Object dict = Object::value( Dictionary() );
    ciftidims->insertArrayItem( -1, dict );
    dict->setProperty( "dimension_length",
                       myXML.getDimensionLength( whichDim ) );

    switch (myXML.getMappingType(whichDim))
    {
      case CiftiMappingType::BRAIN_MODELS:
      {
        Object mvec = brainModelObject( myXML, whichDim );
        dict->setProperty( "brain_models", mvec );
        break;
      }
      case CiftiMappingType::LABELS:
      {
        Object lvec = labelsObject( myXML, whichDim );
        dict->setProperty( "labels", lvec );
        break;
      }
      case CiftiMappingType::PARCELS:
      {
        Object pvec = parcelsObject( myXML, whichDim );
        dict->setProperty( "parcels", pvec );
        break;
      }
      case CiftiMappingType::SCALARS:
      {
        Object svec = scalarsObject( myXML, whichDim );
        dict->setProperty( "scalars", svec );
        break;
      }
      case CiftiMappingType::SERIES:
      {
        Object series = seriesObject( myXML, whichDim );
        dict->setProperty( "series", series );
        break;
      }
      default:
        cerr << "unrecoignized CIFTI mapping type: "
          << myXML.getMappingType(whichDim) << endl;
        break;
    }
  }

  return hdr;
}


void CiftiObjectConverter::objectToCifti( Object header,
                                          CiftiFile & out_file ) const
{
}


