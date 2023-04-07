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


#include <cstdlib>
#include <aims/data/data_g.h>
#include <aims/getopt/getopt2.h>
#include <aims/getopt/getoptProcess.h>
#include <aims/io/reader.h>
#include <aims/io/finder.h>
#include <aims/io/process.h>
#include <aims/io/iooptions.h>
#include <aims/io/motionW.h>
#include <aims/io/fileFormat.h>
#include <cartobase/stream/fileutil.h>
#include <iomanip>

using namespace aims;
using namespace carto;
using namespace std;

template<typename T> 
static bool subvolume( Process &, const string &, Finder & );

class SubVolume : public Process
{
public:
  SubVolume( vector<int> &sx, vector<int> &sy, vector<int> &sz, 
	     vector<int> &st, vector<int> &ex, vector<int> &ey, 
	     vector<int> &ez, vector<int> &et,
	     bool lap, vector<string> &fileout, 
	     bool nominf, bool writemotion,
	     string motiondirect, string motioninverse, bool split4d );
  virtual ~SubVolume();

  vector<int> sx, sy, sz, st, ex, ey, ez, et;
  bool lap; 
  vector<string> fileout;
  bool nominf;
  bool writemotion;
  string motiondirect, motioninverse;
  bool split4d;
};


SubVolume::SubVolume( vector<int> &sx, vector<int> &sy, vector<int> &sz, 
		      vector<int> &st, vector<int> &ex, vector<int> &ey, 
		      vector<int> &ez, vector<int> &et,
		      bool lap, vector<string> &fileout, 
		      bool nominf, bool writemotion,
		      string motiondirect, string motioninverse, bool split4d )
  : sx(sx),sy(sy),sz(sz),st(st),ex(ex),ey(ey),ez(ez),et(et),
    lap(lap), fileout(fileout), nominf(nominf), writemotion(writemotion),
    motiondirect(motiondirect), motioninverse(motioninverse),
    split4d( split4d )
{
  registerProcessType( "Volume", "S8",    &subvolume<int8_t> );
  registerProcessType( "Volume", "U8",    &subvolume<uint8_t> );
  registerProcessType( "Volume", "U16",   &subvolume<uint16_t> );
  registerProcessType( "Volume", "S16",   &subvolume<int16_t> );
  registerProcessType( "Volume", "S32",   &subvolume<int32_t> );
  registerProcessType( "Volume", "U32",   &subvolume<uint32_t> );
  registerProcessType( "Volume", "FLOAT", &subvolume<float> );
  registerProcessType( "Volume", "DOUBLE", &subvolume<double> );
  registerProcessType( "Volume", "RGB", &subvolume<AimsRGB> );
  registerProcessType( "Volume", "RGBA", &subvolume<AimsRGBA> );
}


SubVolume::~SubVolume()
{
}


template<typename T>
bool subvolume( Process & p, const string & filein, Finder & f )
{
  SubVolume &sv = (SubVolume &) p;

  AimsData<T>		data;
  Reader< AimsData<T> >	r( filein );
  string		format = f.format();
  r.read( data, 0, &format );
  bool	first = true;

  if( sv.split4d )
  {
    size_t i, nt = data.dimT();
    sv.st.clear();
    sv.et.clear();
    sv.st.reserve( nt );
    sv.et.reserve( nt );
    string outfilename = sv.fileout[0];
    string extsuf;
    const map<string, list<string> > & ext
      = FileFormatDictionary<AimsData<T> >::extensions();
    map<string, list<string> >::const_iterator ile, ele = ext.end();
    for( ile=ext.begin(); ile!=ele; ++ile )
    {
      if( outfilename.substr( outfilename.length() - ile->first.length() - 1,
        ile->first.length() + 1 ) == "." + ile->first )
      {
        outfilename = outfilename.substr( 0, outfilename.length()
          - ile->first.length() - 1 );
        extsuf = "." + ile->first;
        break;
      }
    }
    bool addout = false;
    if( sv.fileout.size() != nt )
    {
      addout = true;
      sv.fileout.clear();
      sv.fileout.reserve( nt );
    }
    for( i=0; i<nt; ++i )
    {
      sv.st.push_back( i );
      sv.et.push_back( i );
      if( addout )
      {
        stringstream ss;
        ss << setfill( '0' ) << setw( 4 ) << i;
        sv.fileout.push_back( outfilename + "_" + ss.str() + extsuf );
      }
    }
  }

  bool lap  = ( (SubVolume &) p).lap;
  vector<int>::iterator isx = sv.sx.begin();
  vector<int>::iterator isy = sv.sy.begin();
  vector<int>::iterator isz = sv.sz.begin();
  vector<int>::iterator ist = sv.st.begin();
  vector<int>::iterator iex = sv.ex.begin();
  vector<int>::iterator iey = sv.ey.begin();
  vector<int>::iterator iez = sv.ez.begin();
  vector<int>::iterator iet = sv.et.begin();

  for( vector<string>::iterator ifileout = sv.fileout.begin();
       ifileout != sv.fileout.end(); ++ifileout )
  {
    int sx, sy, sz, st, ex, ey, ez, et;
    if ( isx == sv.sx.end() ) {
      sx  = 0;
    } else {
      sx = *isx;
      ++isx;
    }
    if ( isy == sv.sy.end() ) {
      sy  = 0;
    } else {
      sy = *isy;
      ++isy;
    }
    if ( isz == sv.sz.end() ) {
      sz  = 0;
    } else {
      sz = *isz;
      ++isz;
    }
    if ( ist == sv.st.end() ) {
      st = 0;
    } else {
      st = *ist;
      ++ist;
    }
    if ( iex == sv.ex.end() ) {
      ex  = data.dimX()-1;
    } else {
      ex = *iex;
      ++iex;
    }
    if ( iey == sv.ey.end() ) {
      ey  = data.dimY()-1;
    } else {
      ey = *iey;
      ++iey;
    }
    if ( iez == sv.ez.end() ) {
      ez  = data.dimZ()-1;
    } else {
      ez = *iez;
      ++iez;
    }
    if ( iet == sv.et.end() ) {
      et  = data.dimT()-1;
    } else {
      et = *iet;
      ++iet;
    }
    string fileout = *ifileout;

    AimsData<T> outimage;
    if (lap) {
      //cout << "dim: " << ex << ", " << ey << ", " << ez << ", " << et 
			//<< endl;
      outimage = AimsData<T>(ex, ey, ez, et);
      outimage.setSizeXYZT( data.sizeX(), data.sizeY(), data.sizeZ(),
                            data.sizeT() );
    } else {
      //cout << "dim: " << ex-sx+1 << ", " << ey-sy+1 << ", " << ez-sz+1 << ", "
			//		 << et-st+1 << endl;
      outimage = AimsData<T>( ex-sx+1, ey-sy+1, ez-sz+1, et-st+1 );
      outimage.setSizeXYZT( data.sizeX(), data.sizeY(), data.sizeZ(),
                            data.sizeT() );
    }

    cout << "Input volume dimensions : " << data.dimX() << " "
        << data.dimY() << " "
        << data.dimZ() << " "
        << data.dimT() << endl;
    cout << "Output volume dimensions : " << outimage.dimX() << " "
        << outimage.dimY() << " "
        << outimage.dimZ() << " "
        << outimage.dimT() << endl;
    int i, j, k, l;
    ForEach4d(outimage, i, j, k, l)
      outimage(i, j, k, l) = data(i+sx, j+sy, k+sz, l+st);

    // keep track of transformations
    Motion motion, im;
    im.setToIdentity();
    im.setTranslation( Point3df( sx * data.sizeX(), sy * data.sizeY(),
                       sz * data.sizeZ()) );
    motion = *im.inverse();

    // setup output header
    if( ( first || !sv.nominf ) && data.header() )
    {
      const PythonHeader
          *pheader = dynamic_cast<const PythonHeader *>( data.header() );
      if ( pheader && !pheader->hasProperty("zero_start_time") )
        outimage.setHeader( data.header()->cloneHeader() );
      else
      {
        PythonHeader
            *outHeader = dynamic_cast<PythonHeader *>
            (data.header()->cloneHeader() ) ;
        if( outHeader )
        {
          if ( !outHeader->hasProperty("zero_start_time") )
            outimage.setHeader( data.header()->cloneHeader() );
          else
          {
            vector<int> tmp;
            if( pheader->getProperty( "start_time", tmp ) ){
              vector<int> out(et - st + 1) ;
              for( int i = std::max( st, 0 );
                   i <= std::min( et, data.dimT()-1 ); ++i )
              {
                out[i - st] = tmp[i] ;
              }
              outHeader->setProperty("start_time", out);
            }
            if( pheader->getProperty( "duration_time", tmp ) ){
              vector<int> out(et - st + 1) ;
              for( int i = std::max( st, 0 );
                   i <= std::min( et, data.dimT()-1 ); ++i )
              {
                out[i - st] = tmp[i] ;
              }
              outHeader->setProperty("duration_time", out) ;
            }
            outimage.setHeader( outHeader );
          }
        }
      }

      PythonHeader
          *outHeader = dynamic_cast<PythonHeader *>(outimage.header() );
      // set transformation info
      if( outHeader && !motion.isIdentity() )
      {
        try
        {
          // remove any referential ID since we are no longer in the
          // same ref
          outHeader->removeProperty( "referential" );
        }
        catch( ... )
        {
        }
        try
        {
          carto::Object
              trs = outHeader->getProperty( "transformations" );
          carto::Object tit = trs->objectIterator();
          std::vector<std::vector<float> > trout;
          trout.reserve( trs->size() );
          for( ; tit->isValid(); tit->next() )
          {
            Motion m( tit->currentValue() );
            m *= im;
            trout.push_back( m.toVector() );
          }
          outHeader->setProperty( "transformations", trout );
        }
        catch( ... )
        {
          // setup a new transformations list
          std::vector<std::vector<float> > trout;
          std::vector<std::string> refsout;
          if( pheader )
            try
            {
              carto::Object iref = pheader->getProperty( "referential" );
              std::string refid = iref->getString();
              refsout.push_back( refid );
            }
            catch( ... )
            {
            }
          if( refsout.empty() )
            refsout.push_back( "Coordinates aligned to another file or to "
                "anatomical truth" );

          trout.push_back( im.toVector() );
          outHeader->setProperty( "transformations", trout );
          outHeader->setProperty( "referentials", refsout );
        }
      }
    }

    //
    Writer< AimsData<T> > w(fileout);
    w << outimage;

    if( sv.writemotion == true )
      {
	Motion m, im;

	if( sv.motiondirect.empty() )
	  {
	    sv.motiondirect = FileUtil::removeExtension( FileUtil::basename( filein )) + "_whole_TO_subvolume.trm";
	  }

	if( sv.motioninverse.empty() ) 
	  {
	    sv.motioninverse = FileUtil::removeExtension( FileUtil::basename( filein )) + "_subvolume_TO_whole.trm";
	  }

	im.setToIdentity();
	im.setTranslation( Point3df( sx * data.sizeX(), 
				     sy * data.sizeY(),
				     sz * data.sizeZ()) );
	
	m = *im.inverse();

	MotionWriter mw( sv.motiondirect );
	mw.write( m );
	MotionWriter imw( sv.motioninverse );
	imw.write( im );
      }

    if( first )
      {
        first = false;
        if( sv.nominf )
          IOOptions::ioOptions()->setProperty( "writeMinf", bool( false ) );
      }
  }
  return true;
}


int main( int argc, const char **argv )
{
  string filein, motiondirect, motioninverse;
  vector<string> fileout;
  vector<int> sx, sy, sz, st, ex, ey, ez, et;
  bool lap = false, nominf = false;
  bool writemotion = false;
  bool split4d = false;

  AimsApplication	app( argc, argv, 
			     "Carve a subvolume in the input volume ");
  app.addOption( filein, "-i", "input data");
  app.alias( "--input", "-i" );
  app.addOptionSeries( sx, "-x", "first x coordinate (default 0)" );
  app.alias( "--firstx", "-x");
  app.addOptionSeries( sy, "-y", "first y coordinate (default 0)" );
  app.alias( "--firsty", "-y");
  app.addOptionSeries( sz, "-z", "first z coordinate (default 0)" );
  app.alias( "--firstz", "-z");
  app.addOptionSeries( st, "-t", "first t coordinate (default 0)" );
  app.alias( "--firstt", "-t");
  app.addOptionSeries( ex, "-X", "last x coordinate (default SizeX-1)" );
  app.alias( "--lastx", "-X");
  app.addOptionSeries( ey, "-Y", "last y coordinate (default SizeY-1)" );
  app.alias( "--lasty", "-Y");
  app.addOptionSeries( ez, "-Z", "last z coordinate (default SizeZ-1)" );
  app.alias( "--lastz", "-Z");
  app.addOptionSeries( et, "-T", "last t coordinate (default SizeT-1)" );
  app.alias( "--lastt", "-T");
  app.addOption( split4d, "-s", "split a 4D volume into a series of 3D "
    "volumes. Output filenames will be appended with a timestep number.",
    true );
  app.alias( "--split", "-s");
  app.addOption( lap, "-l", " take X,Y,Z and T as dimensions instead of coordinates)", true);
  app.alias( "--lap", "-l");
  app.addOptionSeries( fileout, "-o", "output data", 1 );
  app.alias( "--output", "-o" );
  app.addOption( writemotion, "-motion", "Write motions whole_TO_subvolume and inverse (default=no)", true);
  app.addOption( motiondirect, "-mdir", "Motion name : whole_TO_subvolume", true );
  app.addOption( motioninverse, "-minv", "Motion name : subvolume_TO_whole", true );
  app.addOption( nominf, "--singleminf", "Only write .minf meta-header on " 
                 "the first sub-volume (useful when writing a series)", true );


  try
  {
    app.initialize();
    SubVolume proc( sx, sy, sz, st, ex, ey, ez, et, lap, fileout,
                    nominf, writemotion, motiondirect, motioninverse,
                    split4d );
    if( !proc.execute( filein ) )
            cout << "Couldn't process file - aborted\n";
  }
  catch( user_interruption &e ) {}
  catch( exception & e ) {
		cerr << e.what() << endl;
		return( EXIT_FAILURE );
  }

  return EXIT_SUCCESS;
}
