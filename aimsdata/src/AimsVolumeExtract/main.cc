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

//--- aims -------------------------------------------------------------------
#include <aims/io/finder.h>
#include <aims/io/process.h>
#include <aims/io/fileFormat.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/hsv/hsv.h>
//--- cartodata --------------------------------------------------------------
#include <cartodata/volume/volume.h>
//--- soma-io ----------------------------------------------------------------
#include <soma-io/config/soma_config.h>
#include <soma-io/getopt/getopt.h>
#include <soma-io/datasourceinfo/datasourceinfo.h>
#include <soma-io/datasourceinfo/datasourceinfoloader.h>
#include <soma-io/io/formatdictionary.h>
#include <soma-io/io/reader.h>
#include <soma-io/io/writer.h>
//--- cartobase --------------------------------------------------------------
#include <cartobase/object/object.h>
#include <cartobase/stream/fileutil.h>
//--- system -----------------------------------------------------------------
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
//----------------------------------------------------------------------------

using namespace aims;
using namespace soma;
using namespace carto;
using namespace std;

// ===========================================================================
//                              AimsVolumeExtract
// ---------------------------------------------------------------------------
// This command extends the previous AimsVolumeExtract cammnd by taking
// avantage of the capacities brought by the SomaIO system.
// What was previously possible should still be so (writing an input volume
// in a bigger output volume).
// When possible, this new command do so by not allocating the full output
// volume. It just books disk space for the ouput volume, and writes the
// input data at the chosen location (making use of partial writing
// capacities). When this is not possible, the old strategy is used.
// The commands also allows us to :
// - Create an empty output volume on disk
// - Use only part of the input data (partial reading)
// - Write data in an existing volume (not just an empty one)
// When using this command, one should keep in mind the partial i/o
// capacities of the formats used as input and output, since some actions
// may be restricted by these.
//
// Notes for developers : Since i/o systems are divided between aims and
// soma systems, the current procedure is quite messy (it needs to check
// at each step which system to use). If all readers/writers are
// ported to soma-io, a lot of cleaning should be possible.
// ===========================================================================

class ObjectType{
public:
    static const std::string                        VOLUME;
};

const std::string ObjectType::VOLUME = "Volume";

struct ReadInfo
{
  bool    exists;
  bool    somaReadable;
  bool    somaWritable;
  bool    aimsReadable;
  bool    aimsWritable;
  bool    canReadVolume;
  bool    canReadSlice;
  bool    canReadLine;
  bool    canReadVoxel;
  bool    canWriteVolume;
  bool    canWriteSlice;
  bool    canWriteLine;
  bool    canWriteVoxel;
  Object  header;
  string  objectType;
  string  dataType;
};

// ===========================================================================
// Return initialized ReadInfo
// ===========================================================================
ReadInfo readInfoCreator()
{
  ReadInfo out;
  out.exists          = false;
  out.somaReadable    = false;
  out.somaWritable    = false;
  out.aimsReadable    = false;
  out.aimsWritable    = false;
  out.canReadVolume   = false;
  out.canReadSlice    = false;
  out.canReadLine     = false;
  out.canReadVoxel    = false;
  out.canWriteVolume  = false;
  out.canWriteSlice   = false;
  out.canWriteLine    = false;
  out.canWriteVoxel   = false;
  out.header.reset(0);
  out.objectType      = ObjectType::VOLUME;
  out.dataType        = "";
  return out;
}


// ===========================================================================
// DataTypeCodeRegistry
// ===========================================================================

class DataTypeCodeRegistry {

public:
    //typedef std::string (*ObjectType)();
    //typedef std::string (*DataType)();

    DataTypeCodeRegistry();
    virtual ~DataTypeCodeRegistry();

    //std::string objectType(std::string object_type);
    std::string dataTypeName(const std::string & object_type,
                             const std::string & data_type);

protected:
    // Store information about object types and data types
    std::map<std::string, std::map<std::string, std::string> > _objectDataTypes;
};

DataTypeCodeRegistry::DataTypeCodeRegistry() {
    // Register info about usable volumes
    _objectDataTypes[ObjectType::VOLUME]["U8"]      = DataTypeCode<Volume<uint8_t> >::name();
    _objectDataTypes[ObjectType::VOLUME]["S8"]      = DataTypeCode<Volume<int8_t> >::name();
    _objectDataTypes[ObjectType::VOLUME]["U16"]     = DataTypeCode<Volume<uint16_t> >::name();
    _objectDataTypes[ObjectType::VOLUME]["S16"]     = DataTypeCode<Volume<int16_t> >::name();
    _objectDataTypes[ObjectType::VOLUME]["U32"]     = DataTypeCode<Volume<uint32_t> >::name();
    _objectDataTypes[ObjectType::VOLUME]["S32"]     = DataTypeCode<Volume<int32_t> >::name();
    _objectDataTypes[ObjectType::VOLUME]["U64"]     = DataTypeCode<Volume<uint64_t> >::name();
    _objectDataTypes[ObjectType::VOLUME]["S64"]     = DataTypeCode<Volume<int64_t> >::name();
    _objectDataTypes[ObjectType::VOLUME]["FLOAT"]   = DataTypeCode<Volume<float> >::name();
    _objectDataTypes[ObjectType::VOLUME]["DOUBLE"]  = DataTypeCode<Volume<double> >::name();
//     _objectDataTypes[ObjectType::VOLUME]["CFLOAT"]   = DataTypeCode<Volume<cfloat> >::name();
//     _objectDataTypes[ObjectType::VOLUME]["CDOUBLE"]  = DataTypeCode<Volume<cdouble> >::name();
    _objectDataTypes[ObjectType::VOLUME]["RGB"]     = DataTypeCode<Volume<AimsRGB> >::name();
    _objectDataTypes[ObjectType::VOLUME]["RGBA"]    = DataTypeCode<Volume<AimsRGBA> >::name();
//     _objectDataTypes[ObjectType::VOLUME]["HSV"]     = DataTypeCode<Volume<AimsHSV> >::name();
//     _objectDataTypes[ObjectType::VOLUME]["HSVA"]    = DataTypeCode<Volume<AimsHSVA> >::name();
}

DataTypeCodeRegistry::~DataTypeCodeRegistry() {
}

string DataTypeCodeRegistry::dataTypeName(const string & object_type,
                                          const string & data_type){

    map<string, string> data_types = _objectDataTypes[object_type];
    if (data_types.size() > 0) {
        if (!data_types[data_type].empty()) {
            return data_types[data_type];
        }
        else {
            throw runtime_error("Data type " + data_type + " is not"
                              + " registered for " + object_type + ".");
        }
    }
    else {
        throw runtime_error("Object type " + object_type + " is not"
                            + " a valid registered type.");
    }
}

// ===========================================================================
// Checks whether output file can be written using soma-io/aims systems.
// Since there is no system for "checking" non existant files, this function
// is an inperfect workaround. For now, I just check whether or not output
// extension is GIS.
//
// \param  fname  Filename to check
// \param  info  ReadInfo structure to fill
// ===========================================================================
void checkWritable( const string   & fname,
                          ReadInfo & info )
{
  string rname = FileUtil::uriFilename( fname );
  string ext = FileUtil::extension( rname );

  cout << "-> Enable writing trough soma: ";

  DataTypeCodeRegistry registry;
  string data_type_name;

  try {
    data_type_name = registry.dataTypeName(info.objectType,
                                           info.dataType);
  }
  catch(...){}

  const set<string> soma_write_formats = DataSourceInfoLoader::writeFormats(
                                                                   ext,
                                                                   data_type_name
                                                               );

  // Try to find associated formats
/*
  bool soma_writable = false;
  if (!data_type_name.empty()) {

    // Try to find an associated format for the given extension
    typename carto::IOObjectTypesDictionary::FormatInfo format_info
           = carto::IOObjectTypesDictionary::writeTypes()[data_type_name];

    if (format_info){
        const set<string> data_type_formats = format_info();
        vector<string> soma_formats(
            max(data_type_formats.size(), formats.size())
        );
        vector<string>::iterator it = set_intersection(
            data_type_formats.begin(), data_type_formats.end(),
            formats.begin(), formats.end(),
            soma_formats.begin()
        );
        soma_formats.resize(it - soma_formats.begin());

        // Soma formats were found in the intersection that matches object type,
        // data type and extension requirements
        if (soma_formats.size() > 0)
            soma_writable = true;
    }
  }*/

  // try soma writable
  if(soma_write_formats.size() > 0) {
    cout << "yes !" << endl;
    info.somaWritable = true;
    info.canWriteVolume = true;
    info.canWriteSlice = true;
    info.canWriteLine = true;
    info.canWriteVoxel = true;
    return;
  }

  cout << "no !" << endl;
  // try aims writable
  info.aimsWritable = true;
  info.canWriteVolume = false;
  info.canWriteSlice = false;
  info.canWriteLine = false;
  info.canWriteVoxel = false;
  cout << "-> Enable writing trough aims: yes !" << endl;
  return;
}

// ===========================================================================
// Checks whether output file can be read using soma-io/aims systems and
// process actuall header reading.
// If soma-io works, we don't try aims unless forceaims is TRUE.
//
// \param  fname  Filename to check
// \param  info  ReadInfo structure to fill
// \param  forceaims Force use of AIMS checker
// ===========================================================================
void checkReadable( const  string   & fname,
                           ReadInfo & info,
                    const  bool     & forceaims = false)
{
  // test soma readable
  if( !forceaims )
  {
    cout << "-> Enable reading trough soma: ";
    DataSourceInfo odsi( fname );
    DataSourceInfoLoader dsil;
    try {
      odsi = dsil.check( odsi );
      if( odsi.header().get() )
      {
        info.somaReadable = true;
        info.header = odsi.header();
        info.canReadVolume = odsi.capabilities().canSeekVolume();
        info.canReadSlice = odsi.capabilities().canSeekSlice();
        info.canReadLine = odsi.capabilities().canSeekLine();
        info.canReadVoxel = odsi.capabilities().canSeekVoxel();
        info.objectType = odsi.header()->getProperty("object_type")->getString();
        info.dataType = odsi.header()->getProperty("data_type")->getString();
        cout << "yes !" << endl;
        return;
      } else
        throw runtime_error( "soma cannot check" );
    } catch( ... ) {
      cout << "no !" << endl;
    }
  }

  // test aims readable
  cout << "-> Enable reading trough aims: ";
  Finder fnd;
  fnd.check( fname );
  if( fnd.headerObject() )
  {
    info.aimsReadable = true;
    info.header = fnd.headerObject();
    info.canReadVolume = false;
    info.canReadSlice= false;
    info.canReadLine = false;
    info.canReadVoxel = false;
    info.objectType = info.header->getProperty("object_type")->getString();
    info.dataType = info.header->getProperty("data_type")->getString();
    cout << "yes !" << endl;
    return;
  } else
    cout << "no !" << endl;
}

// ===========================================================================
// First try for existence of file. If exists, calls checkReadable. Then
// calls checkWritable anyway.
//
// \param  fname  Filename to check
// \param  info  ReadInfo structure to fill
// \param  forceaims  If true, do not try soma-io system.
// ===========================================================================
void checkInput( const  string   & fname,
                        ReadInfo & info,
                 const  bool     & forceaims = false)
{
  cout << "Checking file : " << fname << endl;

  string rname = FileUtil::uriFilename( fname );
  info.exists = FileUtil::fileStat( rname ).find( '+' ) != string::npos;
  if( info.exists )
  {
    checkReadable( fname, info, forceaims );
  } else
    cout << "-> File does not exist : nothing to check" << endl;
  checkWritable( fname, info );
}

// ===========================================================================
// Resize Volume
// ===========================================================================
template <typename T>
void resizeVolume( rc_ptr<Volume<T> > & vol,
                   int dimx, int dimy, int dimz, int dimt,
                   int posx, int posy, int posz, int post,
                   int regx, int regy, int regz, int regt,
                   int rpox, int rpoy, int rpoz, int rpot )
{
  VolumeRef<T> newvol( dimx, dimy, dimz, dimt );
  int ni=posx, nj=posy, nk=posz, nl=post;
  for( int l=rpot; l<( regt ? regt+rpot : vol->getSizeT()-rpot ); ++l ) {
    for( int k=rpoz; l<( regz ? regz+rpoz : vol->getSizeZ()-rpoz ); ++k ) {
      for( int j=rpoy; l<( regy ? regy+rpoy : vol->getSizeY()-rpoy ); ++j ) {
        for( int i=rpox; l<( regx ? regx+rpox : vol->getSizeX()-rpox ); ++i ) {
          newvol(ni,nj,nk,nl) = (*vol)(i,j,k,l);
          ++ni;
        }
        ++nj;
      }
      ++nk;
    }
    ++nl;
  }
  vol = newvol;
}

// ===========================================================================
// Write empty volume using soma-io system (this way, we don't need to
// allocate such a volume)
// ===========================================================================
template <typename T>
void somaWriteEmptyVolume_f( const vector<int>    & sizeout,
                             const vector<float>  & voxelout,
                             const string         & ofname )
{
  cout << "Writing empty volume (soma)"
       << endl;
  cout << "-> size : [ "
       << sizeout[0] << ", " << sizeout[1] << ", "
       << sizeout[2] << ", " << sizeout[3] << " ]"
       << endl;
  cout << "-> voxel_size : [ "
       << voxelout[0] << ", " << voxelout[1] << ", "
       << voxelout[2] << ", " << voxelout[3] << " ]"
       << endl;

  VolumeRef<T> voltowrite( new Volume<T>( sizeout[0], sizeout[1],
                                          sizeout[2], sizeout[3],
                                          AllocatorContext(), false )
                         );
  voltowrite.header().setProperty( "voxel_size", voxelout );
  soma::Writer<Volume<T> > writer( ofname );
  writer.write(*voltowrite);
}
typedef void (*somaWriteEmptyVolume_g)( const vector<int>    &,
                                        const vector<float>  &,
                                        const string         & );
map<string,somaWriteEmptyVolume_g> somaWriteEmptyVolume;

// ===========================================================================
// Write empty volume using aims system (this way, we need to
// first allocate such a volume)
// ===========================================================================
template <typename T>
void aimsWriteEmptyVolume_f( const vector<int>    & sizeout,
                             const vector<float>  & voxelout,
                             const string         & ofname )
{
  cout << "Writing empty volume (aims)"
       << endl;
  cout << "-> size : [ "
       << sizeout[0] << ", " << sizeout[1] << ", "
       << sizeout[2] << ", " << sizeout[3] << " ]"
       << endl;
  cout << "-> voxel_size : [ "
       << voxelout[0] << ", " << voxelout[1] << ", "
       << voxelout[2] << ", " << voxelout[3] << " ]"
       << endl;

  Volume<T> voltowrite( sizeout[0], sizeout[1], sizeout[2], sizeout[3] );
  voltowrite.header().setProperty( "voxel_size", voxelout );
  aims::Writer<Volume<T> > writer( ofname );
  writer << voltowrite;
}
typedef void (*aimsWriteEmptyVolume_g)( const vector<int>    &,
                                        const vector<float>  &,
                                        const string         & );
map<string,aimsWriteEmptyVolume_g> aimsWriteEmptyVolume;

// ===========================================================================
// Read input using soma-io.
// Write output using soma-io
// ===========================================================================
template <typename T>
void somaReadSomaWrite_f( const string       & ifname,
                          const string       & ofname,
                          const vector<int>  & regionin,
                          const vector<int>  & posin,
                          const vector<int>  & sizein,
                          const vector<int>  & sizeout,
                          const vector<int>  & posout,
                          const bool         & extract,
                          const ReadInfo     & inputInfo,
                          const ReadInfo     & outputInfo )
{
  cout << "Reading input volume (soma)"
       << endl;
  cout << "-> size : [ "
       << ( inputInfo.canReadVoxel  ? regionin[0] : sizein[0] ) << ", "
       << ( inputInfo.canReadLine   ? regionin[1] : sizein[1] ) << ", "
       << ( inputInfo.canReadSlice  ? regionin[2] : sizein[2] ) << ", "
       << ( inputInfo.canReadVolume ? regionin[3] : sizein[3] ) << " ]"
       << endl;
  cout << "-> position : [ "
       << ( inputInfo.canReadVoxel  ? posin[0] : 0 ) << ", "
       << ( inputInfo.canReadVoxel  ? posin[1] : 0 ) << ", "
       << ( inputInfo.canReadVoxel  ? posin[2] : 0 ) << ", "
       << ( inputInfo.canReadVoxel  ? posin[3] : 0 ) << " ]"
       << endl;

  // reader
  soma::Reader<Volume<T> > reader( ifname );
  Object readoptions = Object::value( PropertySet() );
  if( inputInfo.canReadVoxel ) {
    readoptions->setProperty( "ox", posin[0] );
    readoptions->setProperty( "sx", regionin[0] );
  }
  if( inputInfo.canReadLine ) {
    readoptions->setProperty( "oy", posin[1] );
    readoptions->setProperty( "sy", regionin[1] );
  }
  if( inputInfo.canReadSlice ) {
    readoptions->setProperty( "oz", posin[2] );
    readoptions->setProperty( "sz", regionin[2] );
  }
  if( inputInfo.canReadVolume ) {
    readoptions->setProperty( "ot", posin[3] );
    readoptions->setProperty( "st", regionin[3] );
  }
  reader.setOptions( readoptions);
  // read volume
  VolumeRef<T> readvol( reader.read() );

  if( !inputInfo.canReadVoxel  || !outputInfo.canWriteVoxel ||
      !inputInfo.canReadLine   || !outputInfo.canWriteLine  ||
      !inputInfo.canReadSlice  || !outputInfo.canWriteSlice ||
      !inputInfo.canReadVolume || !outputInfo.canWriteVolume )
  {
    cout << "Resizing volume"
         << endl;
    resizeVolume( readvol,
                  ( inputInfo.canWriteVoxel
                    ? ( outputInfo.canReadVoxel  ? readvol->getSizeX() : regionin[0] )
                    : sizeout[0] ) ,
                  ( inputInfo.canWriteLine
                    ? ( outputInfo.canReadLine   ? readvol->getSizeY() : regionin[1] )
                    : sizeout[1] ) ,
                  ( inputInfo.canWriteSlice
                    ? ( outputInfo.canReadSlice  ? readvol->getSizeZ() : regionin[2] )
                    : sizeout[2] ) ,
                  ( inputInfo.canWriteVolume
                    ? ( outputInfo.canReadVolume ? readvol->getSizeT() : regionin[3] )
                    : sizeout[3] ) ,
                  ( inputInfo.canWriteVoxel  ? 0 : posout[0] ) ,
                  ( inputInfo.canWriteLine   ? 0 : posout[1] ) ,
                  ( inputInfo.canWriteSlice  ? 0 : posout[2] ) ,
                  ( inputInfo.canWriteVolume ? 0 : posout[3] ) ,
                  ( inputInfo.canReadVoxel  ? readvol.getSizeX() : regionin[0] ),
                  ( inputInfo.canReadLine   ? readvol.getSizeY() : regionin[1] ),
                  ( inputInfo.canReadSlice  ? readvol.getSizeZ() : regionin[2] ),
                  ( inputInfo.canReadVolume ? readvol.getSizeT() : regionin[3] ),
                  ( inputInfo.canReadVoxel  ? readvol.getSizeX() : posin[0] ),
                  ( inputInfo.canReadLine   ? readvol.getSizeY() : posin[1] ),
                  ( inputInfo.canReadSlice  ? readvol.getSizeZ() : posin[2] ),
                  ( inputInfo.canReadVolume ? readvol.getSizeT() : posin[3] )
                );
}

  cout << "Writing output volume (soma)" << ( extract ? "" : " (partially)" )
       << endl;
  cout << "-> size : [ "
       << readvol->getSizeX() << ", " << readvol->getSizeY() << ", "
       << readvol->getSizeZ() << ", " << readvol->getSizeT() << " ]"
       << endl;
  if( !extract ) {
    cout << "-> position : [ "
         << ( outputInfo.canWriteVoxel ? posout[0] : 0 ) << ", "
         << ( outputInfo.canWriteVoxel ? posout[1] : 0 ) << ", "
         << ( outputInfo.canWriteVoxel ? posout[2] : 0 ) << ", "
         << ( outputInfo.canWriteVoxel ? posout[3] : 0 ) << " ]"
         << endl;
  }

  // writer
  soma::Writer<Volume<T> > writer( ofname );
  Object writeoptions = Object::value( PropertySet() );
  if( !extract && ( readvol->getSizeX() != sizeout[0] ||
                    readvol->getSizeY() != sizeout[1] ||
                    readvol->getSizeZ() != sizeout[2] ||
                    readvol->getSizeT() != sizeout[3] ) )
  {
    writeoptions->setProperty( "partial_writing", true );
    VolumeRef<T> fullvol( new Volume<T>( sizeout[0], sizeout[1],
                                         sizeout[2], sizeout[3],
                                         AllocatorContext(), false ) );
    readvol->setRefVolume(fullvol);
    readvol->setPosInRefVolume(
      typename carto::Volume<T>::Position4Di(
        outputInfo.canWriteVoxel  ? posout[0] : 0,
        outputInfo.canWriteLine   ? posout[1] : 0,
        outputInfo.canWriteSlice  ? posout[2] : 0,
        outputInfo.canWriteVolume ? posout[3] : 0 )
    );
  }
  writer.write( *readvol, writeoptions );
}
typedef void (*somaReadSomaWrite_g)( const string       & ,
                                     const string       & ,
                                     const vector<int>  & ,
                                     const vector<int>  & ,
                                     const vector<int>  & ,
                                     const vector<int>  & ,
                                     const vector<int>  & ,
                                     const bool         & ,
                                     const ReadInfo     & ,
                                     const ReadInfo     & );
map<string,somaReadSomaWrite_g> somaReadSomaWrite;

// ===========================================================================
// Read input using aims.
// Write output using soma-io
// ===========================================================================
template <typename T>
void aimsReadSomaWrite_f( const string       & /*ifname*/,
                          const string       & /*ofname*/,
                          const vector<int>  & /*regionin*/,
                          const vector<int>  & /*posin*/,
                          const vector<int>  & /*sizeout*/,
                          const vector<int>  & /*posout*/,
                          const bool         & /*extract*/ )
{
  cerr << "Not implemented yet." << endl;
}
typedef void (*aimsReadSomaWrite_g)( const string       & ,
                                     const string       & ,
                                     const vector<int>  & ,
                                     const vector<int>  & ,
                                     const vector<int>  & ,
                                     const vector<int>  & ,
                                     const bool         & );
map<string,aimsReadSomaWrite_g> aimsReadSomaWrite;

// ===========================================================================
// Read input using aims.
// Write output using aims
// ===========================================================================
template <typename T>
void aimsReadAimsWrite_f( const string       & /*ifname*/,
                          const string       & /*ofname*/,
                          const vector<int>  & /*regionin*/,
                          const vector<int>  & /*posin*/,
                          const vector<int>  & /*sizeout*/,
                          const vector<int>  & /*posout*/,
                          const bool         & /*extract*/ )
{
  cerr << "Not implemented yet." << endl;
}
typedef void (*aimsReadAimsWrite_g)( const string       & ,
                                     const string       & ,
                                     const vector<int>  & ,
                                     const vector<int>  & ,
                                     const vector<int>  & ,
                                     const vector<int>  & ,
                                     const bool         & );
map<string,aimsReadAimsWrite_g> aimsReadAimsWrite;

// ===========================================================================
// Read input using soma-io.
// Write output using aims
// ===========================================================================
template <typename T>
void somaReadAimsWrite_f( const string       & /*ifname*/,
                          const string       & /*ofname*/,
                          const vector<int>  & /*regionin*/,
                          const vector<int>  & /*posin*/,
                          const vector<int>  & /*sizeout*/,
                          const vector<int>  & /*posout*/,
                          const bool         & /*extract*/ )
{
  cerr << "Not implemented yet." << endl;
}
typedef void (*somaReadAimsWrite_g)( const string       & ,
                                     const string       & ,
                                     const vector<int>  & ,
                                     const vector<int>  & ,
                                     const vector<int>  & ,
                                     const vector<int>  & ,
                                     const bool         & );
map<string,somaReadAimsWrite_g> somaReadAimsWrite;

// ===========================================================================
// Register all generic functions
// ===========================================================================
void registerFunc()
{
  somaWriteEmptyVolume[ "S8" ] = somaWriteEmptyVolume_f<int8_t>;
  somaWriteEmptyVolume[ "S16" ] = somaWriteEmptyVolume_f<int16_t>;
  somaWriteEmptyVolume[ "S32" ] = somaWriteEmptyVolume_f<int32_t>;
  somaWriteEmptyVolume[ "U8" ] = somaWriteEmptyVolume_f<uint8_t>;
  somaWriteEmptyVolume[ "U16" ] = somaWriteEmptyVolume_f<uint16_t>;
  somaWriteEmptyVolume[ "U32" ] = somaWriteEmptyVolume_f<uint32_t>;
  somaWriteEmptyVolume[ "RGB" ] = somaWriteEmptyVolume_f<AimsRGB>;
  somaWriteEmptyVolume[ "RGBA" ] = somaWriteEmptyVolume_f<AimsRGBA>;
  somaWriteEmptyVolume[ "HSV" ] = somaWriteEmptyVolume_f<AimsHSV>;
  somaWriteEmptyVolume[ "FLOAT" ] = somaWriteEmptyVolume_f<float>;
  somaWriteEmptyVolume[ "DOUBLE" ] = somaWriteEmptyVolume_f<double>;

  aimsWriteEmptyVolume[ "S8" ] = aimsWriteEmptyVolume_f<int8_t>;
  aimsWriteEmptyVolume[ "S16" ] = aimsWriteEmptyVolume_f<int16_t>;
  aimsWriteEmptyVolume[ "S32" ] = aimsWriteEmptyVolume_f<int32_t>;
  aimsWriteEmptyVolume[ "U8" ] = aimsWriteEmptyVolume_f<uint8_t>;
  aimsWriteEmptyVolume[ "U16" ] = aimsWriteEmptyVolume_f<uint16_t>;
  aimsWriteEmptyVolume[ "U32" ] = aimsWriteEmptyVolume_f<uint32_t>;
  aimsWriteEmptyVolume[ "RGB" ] = aimsWriteEmptyVolume_f<AimsRGB>;
  aimsWriteEmptyVolume[ "RGBA" ] = aimsWriteEmptyVolume_f<AimsRGBA>;
  aimsWriteEmptyVolume[ "HSV" ] = aimsWriteEmptyVolume_f<AimsHSV>;
  aimsWriteEmptyVolume[ "FLOAT" ] = aimsWriteEmptyVolume_f<float>;
  aimsWriteEmptyVolume[ "DOUBLE" ] = aimsWriteEmptyVolume_f<double>;

  somaReadSomaWrite[ "S8" ] = somaReadSomaWrite_f<int8_t>;
  somaReadSomaWrite[ "S16" ] = somaReadSomaWrite_f<int16_t>;
  somaReadSomaWrite[ "S32" ] = somaReadSomaWrite_f<int32_t>;
  somaReadSomaWrite[ "U8" ] = somaReadSomaWrite_f<uint8_t>;
  somaReadSomaWrite[ "U16" ] = somaReadSomaWrite_f<uint16_t>;
  somaReadSomaWrite[ "U32" ] = somaReadSomaWrite_f<uint32_t>;
  somaReadSomaWrite[ "RGB" ] = somaReadSomaWrite_f<AimsRGB>;
  somaReadSomaWrite[ "RGBA" ] = somaReadSomaWrite_f<AimsRGBA>;
  somaReadSomaWrite[ "HSV" ] = somaReadSomaWrite_f<AimsHSV>;
  somaReadSomaWrite[ "FLOAT" ] = somaReadSomaWrite_f<float>;
  somaReadSomaWrite[ "DOUBLE" ] = somaReadSomaWrite_f<double>;

  aimsReadSomaWrite[ "S8" ] = aimsReadSomaWrite_f<int8_t>;
  aimsReadSomaWrite[ "S16" ] = aimsReadSomaWrite_f<int16_t>;
  aimsReadSomaWrite[ "S32" ] = aimsReadSomaWrite_f<int32_t>;
  aimsReadSomaWrite[ "U8" ] = aimsReadSomaWrite_f<uint8_t>;
  aimsReadSomaWrite[ "U16" ] = aimsReadSomaWrite_f<uint16_t>;
  aimsReadSomaWrite[ "U32" ] = aimsReadSomaWrite_f<uint32_t>;
  aimsReadSomaWrite[ "RGB" ] = aimsReadSomaWrite_f<AimsRGB>;
  aimsReadSomaWrite[ "RGBA" ] = aimsReadSomaWrite_f<AimsRGBA>;
  aimsReadSomaWrite[ "HSV" ] = aimsReadSomaWrite_f<AimsHSV>;
  aimsReadSomaWrite[ "FLOAT" ] = aimsReadSomaWrite_f<float>;
  aimsReadSomaWrite[ "DOUBLE" ] = aimsReadSomaWrite_f<double>;

  aimsReadAimsWrite[ "S8" ] = aimsReadAimsWrite_f<int8_t>;
  aimsReadAimsWrite[ "S16" ] = aimsReadAimsWrite_f<int16_t>;
  aimsReadAimsWrite[ "S32" ] = aimsReadAimsWrite_f<int32_t>;
  aimsReadAimsWrite[ "U8" ] = aimsReadAimsWrite_f<uint8_t>;
  aimsReadAimsWrite[ "U16" ] = aimsReadAimsWrite_f<uint16_t>;
  aimsReadAimsWrite[ "U32" ] = aimsReadAimsWrite_f<uint32_t>;
  aimsReadAimsWrite[ "RGB" ] = aimsReadAimsWrite_f<AimsRGB>;
  aimsReadAimsWrite[ "RGBA" ] = aimsReadAimsWrite_f<AimsRGBA>;
  aimsReadAimsWrite[ "HSV" ] = aimsReadAimsWrite_f<AimsHSV>;
  aimsReadAimsWrite[ "FLOAT" ] = aimsReadAimsWrite_f<float>;
  aimsReadAimsWrite[ "DOUBLE" ] = aimsReadAimsWrite_f<double>;

  somaReadAimsWrite[ "S8" ] = somaReadAimsWrite_f<int8_t>;
  somaReadAimsWrite[ "S16" ] = somaReadAimsWrite_f<int16_t>;
  somaReadAimsWrite[ "S32" ] = somaReadAimsWrite_f<int32_t>;
  somaReadAimsWrite[ "U8" ] = somaReadAimsWrite_f<uint8_t>;
  somaReadAimsWrite[ "U16" ] = somaReadAimsWrite_f<uint16_t>;
  somaReadAimsWrite[ "U32" ] = somaReadAimsWrite_f<uint32_t>;
  somaReadAimsWrite[ "RGB" ] = somaReadAimsWrite_f<AimsRGB>;
  somaReadAimsWrite[ "RGBA" ] = somaReadAimsWrite_f<AimsRGBA>;
  somaReadAimsWrite[ "HSV" ] = somaReadAimsWrite_f<AimsHSV>;
  somaReadAimsWrite[ "FLOAT" ] = somaReadAimsWrite_f<float>;
  somaReadAimsWrite[ "DOUBLE" ] = somaReadAimsWrite_f<double>;
}

// ===========================================================================
// Main processing.
// Checks inputs and outputs to correctly used soma-io or aims systems.
// Initialize needed parameters.
// Calls accordingly generic processes.
// ===========================================================================
void executeExtractVolume( string        ifname,
                           string        ofname,
                           string        rfname,
                           vector<int>   sizeout,
                           vector<float> voxelout,
                           vector<int>   posout,
                           vector<int>   regionin,
                           vector<int>   posin,
                           string        datatype,
                           bool          extract )
{
  //--- variable utils -------------------------------------------------------
  int32_t     i                 = 0;      // loop index
  bool        canWritePartial   = false;  // choose process
  bool        canReadPartial    = false;  // choose process
  bool        needWritePartial  = false;  // choose process
  bool        needReadPartial   = false;  // choose process
  bool        overwriteOutput   = false;  // writing new volume anyway
  vector<int> sizein            (4,1);    // full size of input volume

  //--- checking input -------------------------------------------------------
  ReadInfo inputInfo = readInfoCreator();

  if( !ifname.empty() )
    checkInput( ifname, inputInfo );

  if( datatype.empty() && !inputInfo.dataType.empty() )
    datatype = inputInfo.dataType;

  //--- checking reference ---------------------------------------------------
  ReadInfo referenceInfo = readInfoCreator();

  if( !rfname.empty() )
    checkInput( rfname, referenceInfo );

  if( datatype.empty() && !referenceInfo.dataType.empty() )
    datatype = referenceInfo.dataType;

  //--- checking output ------------------------------------------------------
  ReadInfo outputInfo = readInfoCreator();
  outputInfo.dataType = datatype;

  if( !ofname.empty() )
    checkInput( ofname, outputInfo );

  //--- checking parameters consitency ---------------------------------------
  if( ofname.empty() )
    throw runtime_error( "Output filename is needed" );
  if( !ifname.empty() && !inputInfo.header.get() )
    throw runtime_error( "Input checking failed : " + ifname );
  if( !rfname.empty() && !referenceInfo.header.get() )
    throw runtime_error( "Reference checking failed : " + rfname );
  if( !outputInfo.header.get()
      && !referenceInfo.header.get()
      && sizeout == std::vector<int>(4,0)
      && !extract )
    throw runtime_error( "Output checking failed : " + ofname );
  if( ifname.empty() && rfname.empty() && datatype.empty() )
    throw runtime_error( "Output creation needs either input volume (-i), "
                          "reference volume (-r) or data type (-t)" );
  if( ifname.empty()
      && rfname.empty()
      && sizeout == std::vector<int>(4,0) )
    throw runtime_error( "Output creation needs either input volume (-i), "
                         "reference volume (-r) or output dimensions (-od)" );
  if( inputInfo.header.get() && !inputInfo.header->hasProperty( "data_type" ) )
    throw runtime_error( "Input header lacks \"data_type\" : " + ifname );
  if( outputInfo.header.get() && !outputInfo.header->hasProperty( "data_type" ) )
    throw runtime_error( "Input header lacks \"data_type\" : " + ofname );
  if( !extract && inputInfo.header.get()
      && outputInfo.header.get() && sizeout == std::vector<int>(4,0)
      && ( inputInfo.header->getProperty( "data_type" )->getString() !=
           outputInfo.header->getProperty( "data_type" )->getString() ) )
    throw runtime_error( "Input and output data types must be consistent" );


  if( !rfname.empty() ) {
    //--- reading parameters from reference ------------------------------------
    for( int i = 0; i < 4; ++i ) {
        if( sizeout[i] == 0 )
        sizeout[i] = referenceInfo.header->getProperty( "volume_dimension" )
                                        ->getArrayItem( i )->getScalar();
        if( voxelout[i] == 0 )
        voxelout[i] = referenceInfo.header->getProperty( "voxel_size" )
                                            ->getArrayItem( i )->getScalar();
    }
  }

  //--- filling missing parameters -------------------------------------------
  for( i=0; i<4; ++i )
  {
    string champ = ( i==0 ? "sizeX" :
                     ( i==1 ? "sizeY" :
                       ( i==2 ? "sizeZ" :
                         ( i==3 ? "sizeT" : "" ))));
    if( inputInfo.header.get()
        && inputInfo.header->hasProperty( "volume_dimension") )
    {
      sizein[i] = (int) rint(
        inputInfo.header->getProperty( "volume_dimension" )
                        ->getArrayItem( i )->getScalar()
      );
    } else if( inputInfo.header.get() && inputInfo.header->hasProperty( champ ) ) {
      sizein[i] = (int) rint( inputInfo.header->getProperty( champ )->getScalar() );
    }
    if( regionin[i] == 0 )
      regionin[i] = sizein[i];
  }

  overwriteOutput = true;
  if( sizeout == std::vector<int>(4,0) && !extract )
  {
    overwriteOutput = false;
    for( i = 0; i < 4; ++i )
    {
      sizeout.push_back( (int) rint(
        outputInfo.header->getProperty("volume_dimension")
                         ->getArrayItem(i)->getScalar()
      ) );
    }
  }
  else if( sizeout == std::vector<int>(4,0) && extract )
    sizeout = regionin;

  if( sizeout != std::vector<int>(4,0) )
  {
    // need to create output volume
    for( i = 0; i < 4; ++i )
    {
      if( sizeout[i] == 0 )
        sizeout[i] = sizein[i];
      if( voxelout[i] == 0.0 )
      {
        if( inputInfo.header.get()
            && inputInfo.header->hasProperty( "voxel_size") )
            voxelout[i] = inputInfo.header->getProperty( "voxel_size" )
                                          ->getArrayItem( i )->getScalar();
        else
          voxelout[i] = 1.0;
      }
    }
  }

  cout << "Recap parameters : " << endl;
  if( !ifname.empty() ) {
    cout << "-> input volume size : [ "
         << sizein[0] << ", " << sizein[1] << ", "
         << sizein[2] << ", " << sizein[3] << " ]"
         << endl;
    if( regionin != sizein ) {
      cout << "-> input region size : [ "
           << regionin[0] << ", " << regionin[1] << ", "
           << regionin[2] << ", " << regionin[3] << " ]"
           << endl;
      cout << "-> input region position : [ "
           << posin[0] << ", " << posin[1] << ", "
           << posin[2] << ", " << posin[3] << " ]"
           << endl;
    }
  }
  cout << "-> output volume size : [ "
       << sizeout[0] << ", " << sizeout[1] << ", "
       << sizeout[2] << ", " << sizeout[3] << " ]"
       << endl;
  cout << "-> output voxel size : [ "
       << voxelout[0] << ", " << voxelout[1] << ", "
       << voxelout[2] << ", " << voxelout[3] << " ]"
       << endl;
  if( !ifname.empty() ) {
    cout << "-> output data position : [ "
       << posout[0] << ", " << posout[1] << ", "
       << posout[2] << ", " << posout[3] << " ]"
       << endl;
  }
  cout << "-> output data type : "
       << datatype
       << endl;
  cout << "-> overwrite output : "
       << ( overwriteOutput ? "yes" : "no" )
       << endl;


  //--- generic processing ---------------------------------------------------
  // Cases :
  // - NeedReadPartial (regionin/posin used)
  //   - CanReadPartial
  //   - CantReadPartial ~ NeedExtraction
  // - NeedPartialWriting (ifname && !extract)
  //   - CanWritePartial
  //   - CantWritePartial && CreateOutput ~ NeedExtraction
  //   - CantWritePartial && Output preexisted => CANNOT DO

  needReadPartial = regionin[0] != sizein[0] ||
                    regionin[1] != sizein[1] ||
                    regionin[2] != sizein[2] ||
                    regionin[3] != sizein[3] ;

  canReadPartial = inputInfo.somaReadable && (
                   ( regionin[3] != sizein[3] && inputInfo.canReadVolume ) ||
                   ( regionin[2] != sizein[2] && inputInfo.canReadSlice ) ||
                   ( regionin[1] != sizein[1] && inputInfo.canReadLine ) ||
                   ( regionin[0] != sizein[0] && inputInfo.canReadVoxel ) );

  needWritePartial = !ifname.empty() && !extract &&
                    ( regionin[0] != sizeout[0] ||
                      regionin[1] != sizeout[1] ||
                      regionin[2] != sizeout[2] ||
                      regionin[3] != sizeout[3] );

  canWritePartial = outputInfo.somaWritable && (
                    ( regionin[3] != sizeout[3] && outputInfo.canWriteVolume ) ||
                    ( regionin[2] != sizeout[2] && outputInfo.canWriteSlice ) ||
                    ( regionin[1] != sizeout[1] && outputInfo.canWriteLine ) ||
                    ( regionin[0] != sizeout[0] && outputInfo.canWriteVoxel ) );

  // empty volumes
  if( !overwriteOutput && needWritePartial && !canWritePartial )
    runtime_error( "Cannot write partially as asked. Check format capabilities." );
  if( overwriteOutput && outputInfo.somaWritable && ( ifname.empty() || needWritePartial ) )
    somaWriteEmptyVolume[ datatype ]( sizeout, voxelout, ofname );
  if( overwriteOutput && outputInfo.aimsWritable && ifname.empty() )
    aimsWriteEmptyVolume[ datatype ]( sizeout, voxelout, ofname );
  if( ifname.empty() )
    return;

  // input reading
  if( inputInfo.somaReadable && outputInfo.somaWritable )
  {
    if( needReadPartial && !canReadPartial )
      runtime_error( "Needs extraction at read : cannot do for now" );
    if( needWritePartial && !canWritePartial && !overwriteOutput )
      runtime_error( "Can't write this region in this format" );
    if( needWritePartial && !canWritePartial && overwriteOutput )
      runtime_error( "Needs extraction at read : cannot do for now" );
    somaReadSomaWrite[ datatype ]( ifname, ofname, regionin, posin,
                                   sizein, sizeout, posout, extract,
                                   inputInfo, outputInfo );
    return;
  }
  if( inputInfo.aimsReadable && outputInfo.somaWritable )
  {
    if( needReadPartial && !canReadPartial )
      runtime_error( "Needs extraction at read : cannot do for now" );
    if( needWritePartial && !canWritePartial && !overwriteOutput )
      runtime_error( "Can't write this region in this format" );
    if( needWritePartial && !canWritePartial && overwriteOutput )
      runtime_error( "Needs extraction at read : cannot do for now" );
    aimsReadSomaWrite[ datatype ]( ifname, ofname, regionin, posin,
                                   sizeout, posout, extract );
    return;
  }
  if( inputInfo.aimsReadable && outputInfo.aimsWritable )
  {
    if( needReadPartial && !canReadPartial )
      runtime_error( "Needs extraction at read : cannot do for now" );
    if( needWritePartial && !canWritePartial && !overwriteOutput )
      runtime_error( "Can't write this region in this format" );
    if( needWritePartial && !canWritePartial && overwriteOutput )
      runtime_error( "Needs extraction at read : cannot do for now" );
    aimsReadAimsWrite[ datatype ]( ifname, ofname, regionin, posin,
                                   sizeout, posout, extract );
    return;
  }
  if( inputInfo.somaReadable && outputInfo.aimsWritable )
  {
    if( needReadPartial && !canReadPartial )
      runtime_error( "Needs extraction at read : cannot do for now" );
    if( needWritePartial && !canWritePartial && !overwriteOutput )
      runtime_error( "Can't write this region in this format" );
    if( needWritePartial && !canWritePartial && overwriteOutput )
      runtime_error( "Needs extraction at read : cannot do for now" );
    somaReadAimsWrite[ datatype ]( ifname, ofname, regionin, posin,
                                   sizeout, posout, extract );
    return;
  }

}

// ===========================================================================
// Main executable
// ===========================================================================
int main( int argc, const char** argv )
{
  try
  {
    //=== APPLICATION ========================================================
    string        ifname;             // file to read
    string        rfname;             // reference file for output dim
    string        ofname;             // file to write
    vector<int>   sizeout(4, 0);      // dimensions of volume to write
    vector<float> voxelout(4, 0);     // voxel size of volume to write
    string        datatype;           // data type of volume to write
    vector<int>   posout(4,0);        // output position where to write data
    vector<int>   regionin(4,0);      // size of partial region to read
    vector<int>   posin(4,0);         // position of partial region to read
    bool          extract = false;    // extraction mode

    CartoApplication app( argc, argv,
      "#============================================================================#\n"
      "#                              WORK IN PROGRESS                              #\n"
      "#        Only Soma-IO formats (currently GIS) are supported for now          #\n"
      "#============================================================================#\n"
      "Writing of volumes with various sources.\n"
      "This command allows one to create a volume of given size, which may "
      "be filled with existing data, or be entirely blank. It also allows "
      "writing exisiting data in a region of an existing volume.\n"
      "It is thus possible to :\n"
      "\t- Write a blank volume\n"
      "\t- Extend the dimensions of a volume (same as AimsOverVolume)\n"
      "\t- Crop a volume (same as AimsSubVolume)\n"
      "At least one of -i and -s[x|y|z|t] option is required. See "
      "additional descriptions to understand the usage of parameters.\n"
    );
    app.addOption( ofname, "-o", "Output filename to be written.\n"
      "If volume dimensions are given, it is created on disk, else if the "
      "output file already exist, data are written in it\n"
    );
    app.addOption( rfname, "-r", "Reference volume\n"
      "Its dimension and voxel size will be used for the output volume in "
      "the case of empty volume creation, when no input is given. "
      "They will be overriden by the use of -od and -ov options.\n",
    true );
    app.addOptionSeries( sizeout, "-od", "Dimension of output volume.\n"
      "If one of the output size parameters is given, eventual existing "
      "volume is crushed. If it is not given and a new volume is created, "
      "default value is 1. If it is not given and we use an existing "
      "output volume, volume's size is used.\n",
    0, 4 );
    app.addOptionSeries( voxelout, "-ov", "Voxel size of output volume.\n"
      "Only used if an output volume is created (default value is 1). If "
      "an existing volume is used, we keep its voxel size.\n",
    0, 4 );
    app.addOption( ifname, "-i", "Source volume.\n"
      "It contains the data used to fill the output volume. If none is "
      "given, the output volume is entirely blank.\n"
      "Partial access options can be used on reading, see further.\n",
    true );
    app.addOptionSeries( regionin, "-ir", "Size of input region to read.\n"
      "This is a parameter relative to partial reading. It specifies the "
      "region of input volume we want to extract and write in output.\n"
      "If one of the partial reading parameters is given, default value "
      "is the size of the full volume.\n",
    0, 4 );
    app.addOptionSeries( posin, "-ip", "Position of input region to read.\n"
      "This is a parameter relative to partial reading. It specifies the "
      "region of input volume we want to extract and write in output.\n"
      "If one of the partial reading parameters is given, default value "
      "is the size of the full volume.\n",
    0, 4 );
    app.addOptionSeries( posout, "-op", "Position of data in output volume.\n"
      "This is a parameter relative to writing of data. It specifies the "
      "position of input data in the output volume (which can be larger).\n"
      "Default value is 0 (no translation).\n",
    0, 4 );
    app.addOption( datatype, "-t", "Data type of output volume\n"
      "Only use this when no input file is given. Data Type is described "
      "by a string : U8, S16, FLOAT, RGB...\n"
      "If both a data type and an input file are given, -t option is not "
      "used\n"
      "If both a data type and an existing output file are given, -t option "
      "is not used.\n",
    true );
    app.addOption( extract, "-extract", "UnderVolume mode.\n"
      "If this option is used, the chosen input region is extracted and "
      "written in output. No extension is done.\n"
      "Needs an input file. Do not use with output options (-od,-op,...)\n",
    true );
    app.alias( "-v", "--verbose" );
    app.alias( "-d", "--debugLevel" );

    app.initialize();

    //=== ALGORITHM ==========================================================
    registerFunc();
    executeExtractVolume( ifname, ofname, rfname,
                          sizeout, voxelout, posout,
                          regionin, posin,
                          datatype, extract );
  }
  catch( user_interruption & )
  {
  }
  catch( exception & e )
  {
    cerr << e.what() << endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
