/*
 *  Copyright (C) 2000-2005 CEA
 *
 *  This software and supporting documentation were developed by
 *  	CEA/DSV/SHFJ
 *  	4 place du General Leclerc
 *  	91401 Orsay cedex
 *  	France
 *
 */
 
#include <aims/getopt/getopt2.h>

#include <aims/data/data_g.h>
#include <aims/roi/roi_g.h>

#include <aims/roi/roistat.h>
#include <aims/roi/roigtm.h>
#include <aims/io/roiR.h>
#include <aims/io/roigtmR.h>
#include <aims/resampling/resampling_g.h>
#include <aims/roi/hie.h>
#include <aims/io/hieR.h>
#include <aims/io/roiselectorR.h>
#include <aims/io/finder.h>
#include <aims/io/process.h>
#include <aims/io/reader.h>
#include <aims/def/path.h>
#include <aims/graph/graphmanip.h>
#include <aims/io/labelImage.h>
#include <cartobase/stream/directory.h>

using namespace aims;
using namespace carto;
using namespace std;


static void Listsel()
{
  Directory	dir( Path::singleton().dependencies() + "/Rsrc" );
  if( !dir.isValid() )
    AimsError( string( "cannot open directory" ) \
               + Path::singleton().dependencies() + "/Rsrc" );

  string	fname;
  set<string>	fnames = dir.files();
  set<string>::iterator	is, es = fnames.end();

  cout << "List of selector available" << endl;
  cout << "             ==============================" << endl;
  cout << "             " 
       << "all"
       << endl;
  
  for( is=fnames.begin(); is!=es; ++is )
    {
      fname = *is;
      string::size_type pos = fname.find( ".sel" );
      if( pos != string::npos)
	cout << "             " 
	     << fname.substr( 0, pos )
	     << endl;
    }
  cout << "             ==============================" << endl;
}


template<class T>
static bool doit( Process &, const string &, Finder & );


class VoiStat : public Process
{
public:
  VoiStat( RoiStats & rs, AimsRoi & r, AffineTransformation3d & m,
           const string & fout, RoiGtm *rgtm, bool framed );

private:
  template<class T>
  friend bool doit( Process &, const string &, Finder & );

  RoiStats	& rsa;
  AimsRoi	& roi;
  AffineTransformation3d & motion;
  string	fileoutput;
  RoiGtm	*gtm;
  bool		byframe;
};


VoiStat::VoiStat( RoiStats & rs, AimsRoi & r, AffineTransformation3d & m,
                  const string & fout, RoiGtm *rgtm, bool framed )
  : Process(), rsa( rs ), roi( r ), motion( m ), fileoutput( fout ), 
    gtm( rgtm ), byframe( framed )
{
  registerProcessType( "Volume", "U8", &doit<uint8_t> );
  registerProcessType( "Volume", "S8", &doit<int8_t> );
  registerProcessType( "Volume", "U16", &doit<uint16_t> );
  registerProcessType( "Volume", "S16", &doit<int16_t> );
  registerProcessType( "Volume", "U32", &doit<uint32_t> );
  registerProcessType( "Volume", "S32", &doit<int32_t> );
  registerProcessType( "Volume", "FLOAT", &doit<float> );
  registerProcessType( "Volume", "DOUBLE", &doit<double> );
}


template<class T>
bool doit( Process & p, const string & fname, Finder & f )
{
  VolumeRef<T>		vol;
  Reader<Volume<T> >	r( fname );
  string		format = f.format();
  VoiStat	& vs = (VoiStat &) p;

  const PythonHeader *ph = 
     dynamic_cast<const PythonHeader *>( f.header() );
  if( vs.byframe )
    {
      int dimT = 1;
      if( !ph )
	{
	  cerr << "image series header does not seem correct (?)\n";
	  return( false );
	}
      vector<int>	dims;
      if( !ph->getProperty( "volume_dimension", dims ) )
	{
	  cerr << "could not determine image dimensions\n";
	  return( false );
	}
      if( dims.size() >= 4 )
	dimT = dims[3];
      else if( !ph->getProperty( "nb_t_pos", dimT ) )
	{
	  cerr << "could not determine image time dimension\n";
	  return( false );
	}
      if ( dimT )
	{
	  cout << "Gather stats through frame by frame reading" << endl;
	  for( int t = 0; t < dimT; ++t )
	    {
	      vol.reset( r.read( 0, &format, t ) );
	      vs.rsa.populate( vol, vs.roi, vs.motion );
	    }
	}
    }
  else
    {
      // try to use memory mapping
      r.setAllocatorContext( AllocatorContext( AllocatorStrategy::ReadOnly, 
                                               DataSource::none(), false, 
                                               0.01 ) );
      vol.reset( r.read( 0, &format ) );

      vs.rsa.populate( vol, vs.roi, vs.motion );
    }
  
  string unit ;
  if( !ph )
    cerr << "image series header does not seem correct (?)\n" ;
  else if( ph->getProperty( "image_unit", unit ) ){
    ofstream ofs(vs.fileoutput.c_str()) ;
    ofs << "Image unit : " << unit << endl ;
    ofs << "Volume unit : mm3" << endl ;
  }
  
  if( vs.gtm )
    {
      cout << "Processing of mean ac tremine" << endl; 
      vs.rsa.applyGtm( *vs.gtm );
    }
  vs.rsa.streamout( (char *) vs.fileoutput.c_str() );

  return( true );
}


void selectAll (const AimsRoi& roi, RoiSelector& sel )
{
  AimsRoi::const_iterator iter( roi.begin() ), last(roi.end() ) ;
  
  string selector = "selector" ;
  sel.setProperty( "graph_syntax", selector ) ;
  
  int nonameNumber = 1 ;
  int roiNumber = 1 ;
  string name ;
  char  temp[64] ;
  while( iter != last )
    {
      if( ! (*iter)->getProperty( "name", name ) )
	AimsError( "There should be a region name for each roi node") ;
      
      if ( name == "unknown" )
	{
	  sprintf( temp, "%d", nonameNumber );
	  name += string( temp ) ;
	  ++nonameNumber ;
	}
      else if ( name == "roi" )
	{
	  sprintf( temp, "%d", roiNumber );
	  name += string( temp ) ;
	  ++roiNumber ;
	}

      Tree * node = new RoiSelector() ;
      node->setProperty( "surname", name ) ;
      Tree * subNode = new RoiSelector() ;
      subNode->setProperty( "nomenName", name ) ;
    
    
      node->insert(subNode) ;
      sel.insert( node ) ;
      (*iter)->setProperty( "name", name ) ;
      ++iter ;
    }



  return ;
}


int main( int argc, const char **argv )
{
  int result = EXIT_SUCCESS;

  try
  {
    string	fileseries, filevoi;
    string	filemotion, filehierarchy, fileoutput, fileroiselector = "all";
    string	fileroiseluser;
    string	voitype = "arg";
    bool		byframe = false, listsels = false;
    
    AimsApplication	app( argc, argv, 
                         "Compute ordinary stat from a RoiArg and and Image (Warning:"
                         "--selection all and --voitype gtm are incompatibles) (Warning: "
                         "DOUBLE images are converted to FLOAT so there si a possible loss "
                         "of information)" );
    
    app.addOption( fileseries, "-i", "input dynamic data" );
    app.addOption( filevoi, "-v", "file containing roi information see "
                   "(--voitype)" );
    app.addOption( fileoutput, "-o", "output" );
    app.addOption( filemotion, "-d", "displacement file from functional image " 
                   "to ROI. default val : Identity", true );
    app.addOption( fileroiseluser, "-s", "user defined selector file name", 
                   true );
    app.addOption( listsels, "-l", "list of available template selectors", 
                   true );
    app.addOption( filehierarchy, "--hierarchy", "nomenclature file", true );
    app.addOption( fileroiselector, "--selection", "template selector name " 
                   "available selectors may be listed (switch -l) " 
                   "OR \"all\" to disable selection mode", true );
    app.addOption( voitype, "--voitype", "subselector for voi file type arg " 
                   "(for arg format), image (for image format), " 
                   "gtm (for composite gtm arg file and roi arg file)\n" 
                   "[Default: arg]", true );
    app.addOption( byframe, "--byframe", "set frame by frame computation mode", 
                   true );
    app.alias( "--input", "-i" );
    app.alias( "--voi", "-v" );
    app.alias( "--displ", "-d" );
    app.alias( "--selector", "-s" );
    app.alias( "--output", "-o" );
    app.alias( "--list", "-l" );
    
    app.initialize();
    
    cerr << endl << "Warning: This command is deprecated and will be removed in future releases. You should use the following command instead:" << endl;
    if ( ! filemotion.empty() ) {
      cerr << "  AimsInvertTransformation -i \"" << filemotion << "\" -o \"" << filemotion << ".inverted.trm\"" << endl;
    }
    cerr << "  AimsRoiFeatures -f csv -i \"" << filevoi << "\" -s \"" << fileseries << "\" -o \"" << fileoutput << "\"";
    if ( ! filemotion.empty() ) {
      cerr << " -t \"" << filemotion << ".inverted.trm\"";
    }
    cerr << endl << endl;
    
    if( listsels )
      {
        Listsel();
        return 0;
      }
    
    // BEWARE !!!!!! Restriction remove (I don't see any reason for this restriction excepted to avoid applying GTM
    //  				       on any region distribution)
    /*  if (!fileroiselector.empty() && !voitype.empty() )
        if( (string(fileroiselector) == string("all") || 
        string(fileroiselector) == string("all.sel") ) &&
        (string(voitype) == string("gtm")) )
        {
        cerr << "Gtm mode and selection mode(all) are incompatibles\n";
        return( 1 );
        }
    */  // Read Hierarchy info =====================================================
    Hierarchy hie;
    if( !filehierarchy.empty() )
      {
        HierarchyReader hrd( filehierarchy, hie.syntax());
        hrd >> hie;
      }
    else
      {
        HierarchyReader hrd( (Path::singleton()).hierarchy()+
                             "/neuronames.hie", hie.syntax());
        hrd >> hie;
      }
    
    // Read roi info (arg/image or gtm mode) ==================================
    AimsRoi roi(&hie);
    RoiGtm rgtm;
    if( voitype == "arg" )
      {
        Reader<Graph> rrd( filevoi );
        if(!rrd.read( roi ) )
	  cerr << "can't read ROI graph" << endl ;
        GraphManip::volume2Buckets(roi) ;
      }
    else if( voitype == "image" )
      {
        LabelReader lrd( filevoi);
        VolumeRef< short > label;
        lrd.read( *label );
        roi.setLabel( label );
        roi.data2bucket();
        VolumeRef< short > zero(1,1,1);
        roi.setLabel( zero );
      }
    else if( voitype == "gtm" )
      {
        RoiGtmReader rgtmR(filevoi, RoiGtmReader::read_gtmarg_and_roi);
        rgtmR >> rgtm;
        GraphManip::volume2Buckets(rgtm) ;
      }
    else 
      {
        cerr <<" --voitype " << voitype << " is an invalid voi type" <<endl;
        return 1;	
      }
    
    //Read Motion info =========================================================
    AffineTransformation3d motion;  // set with Identity
    if( !filemotion.empty() )
      {
        Reader<AffineTransformation3d> mrd( filemotion );
        mrd.read( motion );
      }
    
    // Initialisation
    bool	gtm = ( voitype == "gtm" );
    AimsRoi	& newroi = 
      gtm ? rgtm.getMaskRoi() : roi;
    RoiGtm	*newrgtm = 0;
    if( gtm )
      newrgtm = &rgtm;
    
    
    // Read RoiSelection Info =================================================
    RoiSelector roiSel;
    if( !fileroiseluser.empty() )
    {
      RoiSelectorReader roiSelRd( fileroiseluser, roiSel.syntax());
      roiSelRd >> roiSel;
    }
    else
      {
        if( !fileroiselector.empty() )
          {
            if( fileroiselector == "all" || 
                fileroiselector == "all.sel" )
              {
                selectAll( newroi, roiSel ) ;
              }
            else
              {
                RoiSelectorReader roiSelRd( Path::singleton().dependencies()+
                                            "/Rsrc/"+fileroiselector,
                                            roiSel.syntax());
                roiSelRd >> roiSel;
              }
          }
        else
          {
            RoiSelectorReader roiSelRd( Path::singleton().dependencies()+
                                        "/Rsrc/MedNuc.sel",
                                        roiSel.syntax());
            roiSelRd >> roiSel;
          }
      }



    // Get a Stat Actuator Object with chosen interpolator and perf. stats
    Resampler< float > *interpolator = new LinearResampler< float >();
    RoiStats rsa( interpolator, &roiSel );
    
    VoiStat	proc( rsa, newroi, motion, fileoutput, newrgtm, byframe );
    if( !proc.execute( fileseries ) )
      {
        cerr << "Could not process\n";
        return( 1 );
      }
  }
  catch ( user_interruption& e )
  {
  }
  catch ( std::exception& e )
  {

    cerr << argv[ 0 ] << ": " << e.what() << endl;
    result = EXIT_FAILURE;

  }

  return result;
}


