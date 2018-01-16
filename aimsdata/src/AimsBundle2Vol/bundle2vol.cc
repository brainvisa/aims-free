/*
 *  Copyright (C) 1997-2004 CEA
 *
 *  This software and supporting documentation were developed by
 *  	CEA/DSV/SHFJ
 *  	4 place du General Leclerc
 *  	91401 Orsay cedex
 *  	France
 *
 *  Selection de bundles par rois avec un taux d'appartenance
 */

#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <cartodata/volume/volume.h>
#include <cartobase/config/verbose.h>
#include <aims/getopt/getopt2.h>
#include <fstream>
#include <vector>
#include <algorithm> 
#include <iostream>
#include <string.h>
#include <aims/fibers/bundles_features.h>
#include <aims/curve/curve.h>
#include <aims/mesh/surface.h>
#include <aims/vector/vector.h>

using namespace std;
using namespace carto;
using namespace aims;


  //------------------//
 //  CurveSelection  //
//------------------//

class Curve2image : public BundleListener
{
public:

  Curve2image( VolumeRef< int32_t > &, int & );
  virtual ~Curve2image();

protected:

  typedef std::vector< FiberPoint > FiberPoints;

  virtual void bundleStarted( const BundleProducer &, const BundleInfo & );
  virtual void bundleTerminated( const BundleProducer &, const BundleInfo & );
  virtual void fiberStarted( const BundleProducer &, const BundleInfo &,
                             const FiberInfo & );
  virtual void fiberTerminated( const BundleProducer &, const BundleInfo &,
                                const FiberInfo & ,FiberPoint *, int &);
  virtual void noMoreBundle( const BundleProducer & );

private:
  VolumeRef< int32_t > _image;
  int _mode;
  int lastIndX;
  int lastIndY;
  int lastIndZ;
};


//------------------//
//  Curve2image     //
//------------------//

//-----------------------------------------------------------------------------
Curve2image::Curve2image(  VolumeRef< int32_t > &image, int &mode) :
  _image(image), _mode(mode)
{
}


//-----------------------------------------------------------------------------
Curve2image::~Curve2image()
{
}


//-----------------------------------------------------------------------------
void Curve2image::bundleStarted( const BundleProducer &, 
				    const BundleInfo & )
{
}


//-----------------------------------------------------------------------------
void Curve2image::bundleTerminated( const BundleProducer &,
				       const BundleInfo & )
{
}


//-----------------------------------------------------------------------------
void Curve2image::fiberStarted( const BundleProducer &,
				   const BundleInfo &,
				   const FiberInfo & )
{
  lastIndX = -1;
  lastIndY = -1;
  lastIndZ = -1;
}


//-----------------------------------------------------------------------------
void Curve2image::fiberTerminated( const BundleProducer &, 
                                    const BundleInfo &,
                                    const FiberInfo & ,FiberPoint *fiber , int &fiberSize)
{
  vector<float> vs = _image->getVoxelSize();
  vector<int> dims = _image->getSize();

  for(int i=0; i<fiberSize; i++)
  {
    float x,y,z;
    FiberPoint point=fiber[i];

    x = point[0] / vs[0];
    y = point[1] / vs[1];
    z = point[2] / vs[2];
    const int indX = (int) rint(x);
    const int indY = (int) rint(y);
    const int indZ = (int) rint(z);

    if ( indX == lastIndX && indY == lastIndY && indZ == lastIndZ ) {
    // If several contiguous points of a fiber are in the same voxel,
    // only the first one is considered.

    }
    else
    {
      lastIndX = indX;
      lastIndY = indY;
      lastIndZ = indZ;

      if( indX >= 0 && indX < dims[0]
          && indY >= 0 && indY < dims[1]
          && indZ >= 0 && indZ < dims[2] )
      {
        if(_mode==2)
          ++_image->at( indX, indY, indZ );
        else if(_mode==3)
        {
          // ici il faut passer par une image de float qu'on instancie en mode 3
          float dx,dy,dz;
          dx = x-indX;
          dy = y-indY;
          dz = z-indZ;
          _image->at( indX, indY, indZ )
              += (int)(1000*(1-dx)*(1-dy)*(1-dz));
          _image->at( indX, indY, indZ+1)
            += (int)(1000*(1-dx)*(1-dy)*dz);
          _image->at( indX, indY+1, indZ )
            += (int)(1000*(1-dx)*dy*(1-dz));
          _image->at( indX, indY+1, indZ+1 )
            += (int)(1000*(1-dx)*dy*dz);
          _image->at( indX+1, indY, indZ)
            += (int)(1000*dx*(1-dy)*(1-dz));
          _image->at( indX+1, indY, indZ+1 )
            += (int)(1000*dx*(1-dy*dz));
          _image->at( indX+1, indY+1, indZ )
            += (int)(1000*dx*dy*(1-dz));
          _image->at( indX+1, indY+1, indZ+1 )
            += (int)(1000*dx*dy*dz);
        }
        else
        {
          _image->at(indX,indY,indZ) = 1;
        }
      }
    }
  }
  delete []fiber;
}



//-----------------------------------------------------------------------------
void Curve2image::noMoreBundle( const BundleProducer &)
{
  vector<int> dims = _image->getSize();
  if(_mode==3)
    for(int iz=0;iz<dims[2];++iz)
      for(int iy=0;iy<dims[1];++iy)
	for(int ix=0;ix<dims[0];++ix)
	  if(_image->at(ix,iy,iz)>0)
	    _image->at(ix,iy,iz)=_image->at(ix,iy,iz)/1000;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------MAIN PROGRAM------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


int main( int argc, const char **argv )
{
  try
    {
      AimsApplication	app( argc, argv, "Make an image of the bundles density\nAll bundles in the same volume" );
      Reader< Volume<int16_t> > filein;
      Writer< Volume<int32_t> > fileout;
      string  bunIn ;
      int     mode=2;

      app.addOption( bunIn,     "-i", "bundles input" );
      app.addOption( filein,    "-r", "input image template" );
      app.addOption( fileout,   "-o", "bundles ima output" );
      app.addOption( mode,      "-m",
      "mode=1->mask,\nmode=2->count(NN)\nmode=3->count(LinearInterpol)\n default=2\n mode 3 doesn't work!!!", true );

      app.initialize();
      VolumeRef<int16_t> image;
      image.reset( filein.read() );
      VolumeRef<int32_t> result( image->getSizeX(),
                                 image->getSizeY(),
                                 image->getSizeZ(),
                                 image->getSizeT());
      result->copyHeaderFrom( image->header() );
      vector<float> vs = image->getVoxelSize();
      result->header().setProperty( "voxel_size", vs );

      // Initialisatin de l'image de resultat
      vector<int> dims = result->getSize();
      for(int z=0;z<dims[2];++z)
	for(int y=0;y<dims[1];++y)
	  for(int x=0;x<dims[0];++x)
	    result->at(x,y,z)=0;
      
      //Cretion du reader de bundles
      if ( verbose ) cout << "Creating bundles reader: " << flush;
      BundleReader bundleReader(bunIn  );
      if ( verbose ) cout << "done" << endl;

      rc_ptr<Curve2image> curve2image;
      if ( verbose ) cout << "Creating motion filter: " << flush;
      curve2image.reset( new Curve2image(result,mode ) );
      bundleReader.addBundleListener( *curve2image);
      //curve2image.get();
      if ( verbose ) cout << "done" << endl;

      if ( verbose ) cout << "Analysing bundles: " << flush;
      bundleReader.read();
      if ( verbose ) cout << "done" << endl;


      fileout.write( *result );


      return EXIT_SUCCESS;
    }
  catch( user_interruption & )
    {
    }
  catch( exception & e )
    {
      cerr << e.what() << endl;
    }
  return EXIT_FAILURE;
}
