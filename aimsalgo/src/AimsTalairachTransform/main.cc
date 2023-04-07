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
#include <aims/io/process.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/io/finder.h>
#include <aims/getopt/getopt2.h> 
#include <iostream>
#include <aims/talairach/talBox.h>
#include <aims/talairach/talBoxBase.h>
#include <aims/talairach/talACPCNorm.h>
#include <aims/talairach/talBoundingBoxPoints.h>
#include <aims/io/apcreader.h>

using namespace aims;
using namespace carto;
using namespace std;

template<class T>
static bool tal( Process & p, const string & filename, Finder & f );

class TalTransform : public Process
{
public:
  TalTransform( const string & fout, const Point3df& ac, 
		const Point3df& pc, const Point3df& ihp );

  template<class T> 
  bool tal_m( VolumeRef<T> & data, const string & filename, Finder & f );

public:
  string       _fileout;
  Point3df      _ac ;
  Point3df      _pc ;
  Point3df      _ihp ;
  string	_doScale ;
};

TalTransform::TalTransform( const string & fout, const Point3df& ac, 
		const Point3df& pc, const Point3df& ihp )
  : Process(), _fileout( fout ), _ac( ac), _pc(pc), _ihp(ihp)
{
  registerProcessType( "Volume", "U8", &tal<unsigned char > );
  registerProcessType( "Volume", "S8", &tal<char > );
  registerProcessType( "Volume", "U16", &tal<unsigned short > );
  registerProcessType( "Volume", "S16", &tal<short > );
}

template<class T> 
bool tal( Process & p, const string & filename, Finder & f )
{
  TalTransform	& dp = (TalTransform &) p;
  
  VolumeRef<T>		data;
  return( dp.tal_m( data, filename, f ) );
}


template<class T> 
bool TalTransform::tal_m( VolumeRef<T> & data, const string & filename, Finder & f )
{
  cout << "With mask" << endl ;
  Reader< VolumeRef< T > >	reader( filename );
  string	format = f.format();
  if( !reader.read( data, 1, &format ) )
    return( false );
  
  TalairachPoints talPoints( _ac, _pc, _ihp ) ;
  
  TalairachBox<T> talBox ;
  AffineTransformation3d transf = talBox.computeTransformationAndBox(
    talPoints, data ) ;
  
  unique_ptr<AffineTransformation3d> invTransf = transf.inverse() ;
  cout << "AC transformed : " << transf.transform( _ac ) << "\tPC transformed : " << transf.transform( _pc ) 
       << "\tIPH transformed : " << transf.transform( _ihp ) << endl ;
  cout << "ACInv transformed : " << invTransf->transform( Point3df(0., 0., 0.) )
       << "\tPCInv transformed : " << invTransf->transform( Point3df(0., 1., 0.) )
       << "\tIPHInv transformed : " << invTransf->transform( Point3df(0., 0., 1.) )
       << "\tXInv transformed : " << invTransf->transform( Point3df(1., 0., 0.) ) << endl ;
  Writer<AffineTransformation3d> writer( _fileout );
  writer.write( transf ) ;
  return( 1 );
}


int main( int argc, const char **argv )
{
  try
    {
      string filein, fileout, apcfile, boundingBoxPointsfile;
      Point3df  acp(0.), pcp(0.), ihpp(0.) ;
      bool noScale(false) ;
      AimsApplication app (argc, argv, 
                           "Compute talairach transform from talairach base coordinates and a mask" ) ;
  
      //   app.addOptionSeries( ac, "-ac", "Anterior commissure point (mm)", 3, 3) ;
      //   app.addOptionSeries( pc, "-pc", "Posterior commissure point (mm)", 3, 3) ;
      //   app.addOptionSeries( ihp, "-ihp", "Inter-hemispheric point (mm)",3, 3) ;
  
      app.addOption( apcfile, "-apc", "apc file transformation", true ) ;
      app.addOption( fileout, "-o", "Output transformation file", true ) ;
      app.addOption( filein, "-m", "Mask", true ) ;
      app.addOption( noScale, "-ns", "No scale factor is applied, single rotation around AC, no talairach coordinates", true ) ;
      app.addOption( boundingBoxPointsfile, "-p", "Roi graph containing points placed on the extremities of the brain", true ) ;
      app.initialize() ;
  
      if( fileout == "" ){
        string::size_type pos = apcfile.find(".APC") ;
        if( pos > apcfile.size() || pos == string::npos )
          fileout = apcfile + string("_T0_Talairach.trm") ;
        else
          fileout = apcfile.substr(0, pos) + string("_T0_Talairach.trm")  ;
      }

      bool	hasmm = false;
      APCReader	apcr( apcfile );
      Object	apc = apcr.read();
      vector<float>	vac, vpc, vih;
      if( !apc->getProperty( "ACmm", vac ) 
          || !apc->getProperty( "PCmm", vpc ) 
          || !apc->getProperty( "IHmm", vih ) )
        {
          if( filein.empty() )
            throw runtime_error( string( "No millimetric information in " ) 
                                 + apcfile + ". Please provide a mask with " 
                                 "valid voxel sizes" );
          apc->getProperty( "AC", vac );
          apc->getProperty( "PC", vpc );
          apc->getProperty( "IH", vih );
        }
      else
        hasmm = true;
      acp[0] = vac[0];
      acp[1] = vac[1];
      acp[2] = vac[2];
      pcp[0] = vpc[0];
      pcp[1] = vpc[1];
      pcp[2] = vpc[2];
      ihpp[0] = vih[0];
      ihpp[1] = vih[1];
      ihpp[2] = vih[2];

      if( !hasmm )
        {
          Finder	f;
          if( !f.check( filein ) )
            f.launchException();
          const Header	*h = f.header();
          const PythonHeader	*ph = dynamic_cast<const PythonHeader *>( h );
          vector<float>		vs;
          if( !ph || !ph->getProperty( "voxel_size", vs ) )
            throw runtime_error( string( "missing voxel_size information " 
                                         "in mask " ) + filein );
          acp[0] *= vs[0];
          acp[1] *= vs[1];
          acp[2] *= vs[2];
          pcp[0] *= vs[0];
          pcp[1] *= vs[1];
          pcp[2] *= vs[2];
          ihpp[0] *= vs[0];
          ihpp[1] *= vs[1];
          ihpp[2] *= vs[2];
        }

      cout << acp << endl << pcp << endl << ihpp << endl ;
    
      TalairachPoints talPoints( acp, pcp, ihpp ) ;
      AffineTransformation3d		transf;
      if( noScale ){
        TalairachReferential talairach ;
        transf = talairach.computeTransformation(talPoints) ;
        AffineTransformation3d transl ;
        transl.setTranslation( acp );
        transf = transl * transf ;
        cout << "AC transformed : " << transf.transform( acp ) << "\tPC transformed : " << transf.transform( pcp ) 
             << "\tIPH transformed : " << transf.transform( ihpp ) << endl ;

        Writer<AffineTransformation3d> writer( fileout );
        writer.write( transf ) ;
        cout << "Rigid transformation saved"  << endl ;
        return( EXIT_SUCCESS );    
      } 
      else if( filein == "" ){
        if( boundingBoxPointsfile == "" ){
          TalairachACPCNorm talairach ;
          transf = talairach.computeTransformation(talPoints) ;      

          cout << "AC transformed : " << transf.transform( acp ) << "\tPC transformed : " << transf.transform( pcp ) 
               << "\tIPH transformed : " << transf.transform( ihpp ) << endl ;
    
          Writer<AffineTransformation3d>	writer( fileout );
          writer.write( transf ) ;
          cout << "Transformation with single scale factor saved"  << endl ;
          return( EXIT_SUCCESS );    
        } else{
	  TalairachBoundingBoxPoints talairach ;
          transf = talairach.computeTransformationAndBox(talPoints, getRoiIterator( boundingBoxPointsfile ) ) ;
          cout << "AC transformed : " << transf.transform( acp ) << "\tPC transformed : " << transf.transform( pcp ) 
               << "\tIPH transformed : " << transf.transform( ihpp ) << endl ;
    
          Writer<AffineTransformation3d>	writer( fileout );
          writer.write( transf ) ;
          cout << "Transformation with scale factors computed with bounding box points saved"  << endl ;
	  return( EXIT_SUCCESS ); 	  
	}
      }
      TalTransform	proc( fileout, acp, pcp, ihpp ) ;
      if( !proc.execute( filein ) )
        {
          cerr << "Couldn't process\n";
          return( 1 );
        }
    }
   catch( user_interruption & )
    {
    }
  catch( exception & e )
    {
      cerr << e.what() << endl;
      return EXIT_FAILURE;
    }

  return( EXIT_SUCCESS );
}
