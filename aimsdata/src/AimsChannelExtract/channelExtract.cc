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

#include <string>
#include <aims/io/process.h>
#include <aims/io/finder.h>
#include <aims/getopt/getopt2.h>
#include <aims/data/data.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/utility/channel.h>
#include <cartobase/stream/fileutil.h>
#include <cartobase/config/verbose.h>


using namespace aims;
using namespace carto;
using namespace std;

template<typename INP>
static bool select( Process & p, const string & filename, Finder & f );

template <typename INP, typename OUTP>
static bool doit( Process & p, const string & filename, Finder & f );


class ChannelExtractInput : public Process
{
public:
  ChannelExtractInput();
  
  template<typename T>
  friend bool select( Process &, const string &, Finder & );
  
  string filein;
  vector<string> filesout;
  vector<int> channels;
  string datatype;
  bool appendsuffix;
};

ChannelExtractInput::ChannelExtractInput()
  : Process(), appendsuffix(true)
{
  // Register input types
  registerProcessType( "Volume", "U8", &select<uint8_t> );
  registerProcessType( "Volume", "S8", &select<int8_t> );
  registerProcessType( "Volume", "U16", &select<uint16_t> );
  registerProcessType( "Volume", "S16", &select<int16_t> );
  registerProcessType( "Volume", "U32", &select<uint32_t> );
  registerProcessType( "Volume", "S32", &select<int32_t> );
  registerProcessType( "Volume", "FLOAT", &select<float> );
  registerProcessType( "Volume", "DOUBLE", &select<double>  );
  registerProcessType( "Volume", "RGB", &select<AimsRGB> );
  registerProcessType( "Volume", "RGBA", &select<AimsRGBA> );
  registerProcessType( "Volume", "HSV", &select<AimsHSV> );
};

template <typename I>
class ChannelExtract : public Process
{
public:
  ChannelExtract(ChannelExtractInput& ct);
  
  template<typename INP, typename OUTP>
  friend bool doit( Process &, const string &, Finder & );
  
  ChannelExtractInput& t;
};

template <typename I>
ChannelExtract<I>::ChannelExtract( ChannelExtractInput& ct)
  : Process(), t(ct)
{
  // Register output channel types
  registerProcessType( "Volume", "U8", &doit<I, uint8_t> );
  registerProcessType( "Volume", "S8", &doit<I, int8_t> );
  registerProcessType( "Volume", "U16", &doit<I, uint16_t> );
  registerProcessType( "Volume", "S16", &doit<I, int16_t> );
  registerProcessType( "Volume", "U32", &doit<I, uint32_t> );
  registerProcessType( "Volume", "S32", &doit<I, int32_t> );
  registerProcessType( "Volume", "FLOAT", &doit<I, float> );
  registerProcessType( "Volume", "DOUBLE", &doit<I, double> );
}

template<typename INP>
bool select( Process & p, const string & filename, Finder & f )
{
  ChannelExtractInput   &t = (ChannelExtractInput &) p;
  ChannelExtract<INP> c(t);

  Finder    f2;
  f2.setObjectType( "Volume" );
  if( t.datatype.empty() ) {
    f2.setDataType( DataTypeInfo<INP>::channelType() );
  }
  else
    f2.setDataType( t.datatype );

  return ( c.execute( f2, "" ) );
}


template <typename INP, typename OUTP>
bool doit( Process & p, const string & filename, Finder & f )
{
  string base, ext, dcode, cname;
  ChannelExtract<INP> &ce = (ChannelExtract<INP> &) p;
  ChannelExtractInput &t = ce.t;
  
  Reader<AimsData<INP> > reader(t.filein);
  ChannelSelector<AimsData<INP>, AimsData<OUTP> > selector;
  AimsData<INP> in;
  AimsData<OUTP> out;
  Writer<AimsData< OUTP > > writer;

  if ( t.filesout.size() > 0) // Should always be true
  {
    // Set default base and extension
    dcode = stringLower(DataTypeCode<INP>::name());
  }
  else 
  {
    cerr << "An error occured during channel extraction : "
         << "at least one output file must be specified" << endl;
    return false;
  }

  if (t.channels.size() == 0)
  {
    // Defaultly extract all channels
    for (int c = 0; c < DataTypeInfo<INP>::samples(); ++c)
    {
      t.channels.push_back(c);
    }
  }
  
  // Read input data
  cout << "reading " << stringUpper(dcode) << " input" << endl << flush;

  if (!reader.read(in)) {
    cerr << "An error occured during channel extraction : "
         << "unable to read input file " << t.filein << endl;
    return false;
  }
  
  string ofilename;
  for (int i = 0; i < t.channels.size(); ++i)
  {
    if (i < t.filesout.size()) {
      // Get specified name
      ofilename = t.filesout[i];
    }
    else {
      // Get default name using the first output specified file
      ofilename = t.filesout[0];
    }
    
    // Get channel name
    if (t.channels[i] < dcode.size()) {
      cname = dcode[ t.channels[i] ];
    }
    else {
      cname = toString(t.channels[i]);
    }
    
    ext = FileUtil::extension( ofilename );
    if( ext.empty() )
      ext = "ima";
      
    if (t.appendsuffix) {
      base = FileUtil::removeExtension( ofilename );
      // Append suffix to output file name      
      ofilename = base + "_" + cname + "." + ext;
    }
    
//     if (verbose)
//       cout << "Extracting channel " << toString(t.channels[i]) 
//            << " to " << ofilename << endl << flush;

    // Select channel
    cout << "getting " << stringUpper(cname) << " channel" << endl << flush;
    out = selector.select( in, t.channels[i] );
    
    cout << "saving " << stringUpper(cname) << " channel" << endl << flush;
    writer.setFileName( ofilename );
    if (!writer.write(out)){
      cerr << "An error occured during channel extraction : "
           << "unable to write output file " << ofilename << endl;
      return false;
    }
  }

  return true;
}


int main( int argc, const char **argv )
{
  try
  {
    ChannelExtractInput c;

    AimsApplication app( argc, argv,
                        "Extract channel(s) of multichannel files" );
    app.addOption( c.filein, "-i", "input data" );
    app.addOptionSeries( c.filesout, "-o", "output file(s)", 1 );
    app.addOptionSeries( c.channels, "-c", "channel(s) to extract", 0 );
    app.addOption( c.datatype, "-t", "output data type [default: U8]", true );
    app.addOption( c.appendsuffix, "-a", "append channel suffix [default: true]", true );
    app.alias( "-v", "--verbose" );
    app.initialize();

    if( ! c.execute( c.filein ))
    {
      return EXIT_FAILURE;
    }
  }
  catch( user_interruption &e )
  {
  }
  catch( std::exception &e )
  {

    cerr << argv[ 0 ] << ": " << e.what() << endl;
    return EXIT_FAILURE;

  }

  return EXIT_SUCCESS;

}


