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

using namespace aims;
using namespace cifti;
using namespace carto;
using namespace std;


Object CiftiToObjectConverter::ciftiToObject( CiftiFile & inputFile ) const
{
  Object hdr = Object::value( Dictionary() );

  // parse xml

  string key;
  Object ciftidims = Object::value( vector<Object>() );
  hdr->setProperty( "cifti_dimensions", ciftidims );

  const CiftiXML& myXML = inputFile.getCiftiXML();
  for (int whichDim = 0; whichDim < myXML.getNumberOfDimensions(); ++whichDim)
  {
    cout << "Dimension " << whichDim << ": ";
    Object dict = Object::value( Dictionary() );
    ciftidims->insertArrayItem( -1, dict );

    switch (myXML.getMappingType(whichDim))
    {
        case CiftiMappingType::BRAIN_MODELS:
        {
            key = "brain_models";
            Object mvec = Object::value( vector<Object>() );
            dict->setProperty( key, mvec );

            const CiftiBrainModelsMap& myMap = myXML.getBrainModelsMap(whichDim);
            cout << "Brain Models, length " << myMap.getLength() << endl;
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
                        string structure =
                          AString_to_std_string( StructureEnum::toName(
                            myInfo[i].m_structure ) );
                        Object sdict = Object::value( Dictionary() );
                        content = Object::value( Dictionary() );
                        mdict->setProperty( "surface", content );
                        content->setProperty( structure, sdict );
                        sdict->setProperty(
                          "number_of_nodes",
                          myMap.getSurfaceNumberOfNodes(
                            myInfo[i].m_structure ) );
                        sdict->setProperty(
                          "index_count", myInfo[i].m_indexCount );
                        break;
                    }
                    case CiftiBrainModelsMap::VOXELS:
                    {
                        content = Object::value( Dictionary() );
                        if( !mdict->getProperty( "voxels",
                            content ) )
                          mdict->setProperty( "voxels", content );
                        string structure =
                          AString_to_std_string( StructureEnum::toName(
                            myInfo[i].m_structure ) );
                        Object sdict = Object::value( Dictionary() );
                        content->setProperty( structure, sdict );
                        sdict->setProperty( "index_count",
                                            myInfo[i].m_indexCount );
                        break;
                    }
                }
            }
            break;
        }
        case CiftiMappingType::LABELS:
        {
            key = "labels";
            Object lvec = Object::value( vector<Object>() );
            dict->setProperty( key, lvec );

            const CiftiLabelsMap& myMap = myXML.getLabelsMap(whichDim);

            for (int i = 0; i < myMap.getLength(); ++i)
            {
                Object ldict = Object::value( Dictionary() );
                lvec->insertArrayItem( -1, ldict );
                ldict->setProperty(
                  "label", AString_to_std_string( myMap.getMapName( i ) ) );
            }
            break;
        }
        case CiftiMappingType::PARCELS:
        {
            key = "parcels";
            Object lvec = Object::value( vector<Object>() );
            dict->setProperty( key, lvec );

            const CiftiParcelsMap& myMap = myXML.getParcelsMap(whichDim);
            cout << "Parcels, length " << myMap.getLength() << endl;
            const vector<CiftiParcelsMap::Parcel>& myParcels = myMap.getParcels();
            for (int i = 0; i < (int)myParcels.size(); ++i)
            {
                Object pdict = Object::value( Dictionary() );
                lvec->insertArrayItem( -1, pdict );
                pdict->setProperty(
                  "name", AString_to_std_string( myParcels[i].m_name ) );

                cout << "   Index " << i << ", name '" << AString_to_std_string(myParcels[i].m_name) << "': ";
                int numVerts = 0;
                for (map<StructureEnum::Enum, set<int64_t> >::const_iterator iter = myParcels[i].m_surfaceNodes.begin(); iter != myParcels[i].m_surfaceNodes.end(); ++iter)
                {
                    numVerts += iter->second.size();
                }
                cout << numVerts << " vertices, " << myParcels[i].m_voxelIndices.size() << " voxels" << endl;
            }
            break;
        }
        case CiftiMappingType::SCALARS:
        {
            const CiftiScalarsMap& myMap = myXML.getScalarsMap(whichDim);
            cout << "Scalars, length " << myMap.getLength() << endl;
            for (int i = 0; i < myMap.getLength(); ++i)
            {
                cout << "   Index " << i << ": " << AString_to_std_string(myMap.getMapName(i)) << endl;
            }
            break;
        }
        case CiftiMappingType::SERIES:
        {
            const CiftiSeriesMap& myMap = myXML.getSeriesMap(whichDim);
            cout << "Series, length " << myMap.getLength() << endl;
            cout << "   Start: " << myMap.getStart() << endl;
            cout << "   Step: " << myMap.getStep() << endl;
            cout << "   Unit: " << AString_to_std_string(CiftiSeriesMap::unitToString(myMap.getUnit())) << endl;
            break;
        }
    }
  }

  return hdr;
}


void CiftiToObjectConverter::objectToCifti( Object header,
                                            CiftiFile & out_file ) const
{
}


