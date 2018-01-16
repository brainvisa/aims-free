/*
 *  Copyright (C) 1997-2004 CEA
 *
 *  This software and supporting documentation were developed by
 *    CEA/DSV/SHFJ
 *    4 place du General Leclerc
 *    91401 Orsay cedex
 *    France
 *
 */
#include <aims/fibers/bundles.h>
#include <aims/getopt/getopt2.h>
#include <time.h>

#include <aims/fibers/fiberDistance.h>
#include <aims/getopt/getopt2.h>


using namespace std;
using namespace carto;
using namespace aims;


//------------------//
// SelectCenterFiber //
//------------------//
class SelectCenterFiber : public BundleProducer, public BundleListener
{
public:

  SelectCenterFiber(string &, int &, int &) ;
  virtual ~SelectCenterFiber();

protected:


  virtual void bundleStarted( const BundleProducer &, const BundleInfo & );
  virtual void bundleTerminated( const BundleProducer &, const BundleInfo & );
  virtual void fiberStarted( const BundleProducer &, const BundleInfo &,
           const FiberInfo & );
  virtual void fiberTerminated( const BundleProducer &, const BundleInfo &,
                                const FiberInfo &, FiberPoint *, int & );
  virtual void noMoreBundle( const BundleProducer & );
  Point3df interp(Point3df p1, Point3df p2, float fact);

private:

  int _bundleCount;
  vector<FiberPoint *> _faisceau;
  vector <int> _sizes;
  float _distMin;
  int _ind;
  vector< FiberPoint *> _selectedFaisceaux;
  vector<int> _selectedSizes;
  string _bunOut;
  vector<string> _bundleNames;
  vector<string> _selectedBundleNames;
  int _dist_type;
  int normFactor;
  rc_ptr< BundleWriter > _writer;
};


//-----------------------------------------------------------------------------
SelectCenterFiber::SelectCenterFiber( string &bunOut, int &dist_type, int &norm) 
{
  _bundleCount = 0;
  _distMin = 100000000.;
  _ind = -1;
  _bunOut = bunOut;
  _dist_type = dist_type;
  normFactor = norm;
  _writer.reset( new BundleWriter() );
  _writer->setFileString(_bunOut);
  srand(time(NULL));
}


//-----------------------------------------------------------------------------
SelectCenterFiber::~SelectCenterFiber()
{
}


//-----------------------------------------------------------------------------
void SelectCenterFiber::bundleStarted( const BundleProducer &, 
           const BundleInfo & )
{
  _faisceau.clear();
  _sizes.clear();
  _bundleNames.clear();
  _distMin = 100000000.;
  _ind = -1;
  _bundleCount++;
}


//-----------------------------------------------------------------------------
void SelectCenterFiber::bundleTerminated( const BundleProducer &producer,
              const BundleInfo & )
{
  int size =_faisceau.size();
  vector<int> fiberMat;
  int matrixSize = _faisceau.size();

  if (matrixSize > 110)
  {
    matrixSize = 100;

    for(int i=0; i<100; i++)
    {
      int num = rand()%(_faisceau.size());
      bool isUnique = true;
      for(int j= 0; j<i ; j++)
      {
        if( num == fiberMat[j])
        {
          isUnique = false;
        }
      }
      if (isUnique == true)
      {
        fiberMat.push_back(num);
      }
      else
      {
        i--;
      }
    }
  }
  else
  {
    for (int i = 0; i<_faisceau.size();i++)
    {
      fiberMat.push_back(i);
    }
  }

  vector< vector<float> > matrix(matrixSize);
  for (int i=0; i<matrixSize;i++)
  {
    matrix[i].resize(matrixSize);
  }

  Point3df p1;

  if ( _faisceau.size() > 0 )
  {
    float dist;

    // Calculate disctance Matrix
    //std::cout << "creating distance matrix... " << std::endl;

    // Write distance matrix
    //Aims format
    int f1 = 0;
    while( f1 < matrixSize ) // fibers
    {
      int f2 = f1;
      //float middle = (int)_faisceau.size() - f1;
      while ( f2 < matrixSize ) // fibers
      {
        if ( f1 != f2 )
        {
          if (_dist_type == 0) // mean of mean closest points distances
          {
            dist = fiberDistance( _faisceau[fiberMat[f1]],
                                  _faisceau[fiberMat[f2]],
                                  _sizes[fiberMat[f1]],
                                  _sizes[fiberMat[f2]] );
          }
          else if (_dist_type == 1) // max of mean closest points distances
          {
            dist = fiberDistanceMax( _faisceau[fiberMat[f1]],
                                     _faisceau[fiberMat[f2]],
                                     _sizes[fiberMat[f1]],
                                     _sizes[fiberMat[f2]]);
          }
          else if (_dist_type == 2) // max distance of corresponding points
          {
            dist = fiberDistanceMaxDistCorrespPoints(_faisceau[fiberMat[f1]],
                                                     _faisceau[fiberMat[f2]],
                                                     _sizes[fiberMat[f1]]);
          }
          else if (_dist_type == 3) // max distance of corresponding points normalized by fibers length
          {
            dist = fiberDistanceMaxDistCorrespPointsNormByMinLength(_faisceau[fiberMat[f1]],
                                                                    _faisceau[fiberMat[f2]],
                                                                    normFactor,
                                                                    _sizes[fiberMat[f1]],
                                                                    _sizes[fiberMat[f2]]);
          }
          matrix[f1][f2] = dist;
          matrix[f2][f1] = dist;
        }
        f2++;
      }
      f1++;
    }

    for (f1 = 0; f1 < matrixSize; f1++)
    {
      float distSum = 0;
      for (int f2 =0; f2 < matrixSize; f2++)
      {
        distSum += matrix[f1][f2];
      }
      if (distSum < _distMin)
      {
        _distMin = distSum;
        _ind = fiberMat[f1];
      }
    }

    if (_bundleCount%20 == 0)
    {
      std::cout << "#"<< std::flush;
    }

    BundleInfo bundle_tmp(_bundleNames[0]);
    _writer->bundleStarted(producer, bundle_tmp);
    FiberInfo fiber_tmp(1);
    _writer->addFiber( producer, bundle_tmp, fiber_tmp, _faisceau[_ind], _sizes[_ind]);
    _writer->bundleTerminated(producer,bundle_tmp);

    for( int i =0; i<_faisceau.size();i++)
    {
      if (i != _ind)
        delete []_faisceau[i];
    }
  }

}
//-----------------------------------------------------------------------------
Point3df SelectCenterFiber::interp(Point3df p1, Point3df p2, float fact)
{
  Point3df _p = (p2-p1)*fact + p1;
  return _p;
}
//-----------------------------------------------------------------------------
void SelectCenterFiber::fiberStarted( const BundleProducer &,
          const BundleInfo &,
          const FiberInfo & )
{

}


//-----------------------------------------------------------------------------
void SelectCenterFiber::fiberTerminated( const BundleProducer &, 
             const BundleInfo &bunInfo,
             const FiberInfo & , FiberPoint *fiber, int &fiberSize)
{
  _faisceau.push_back(fiber);
  _sizes.push_back(fiberSize);
  _bundleNames.push_back(bunInfo.name());
}



//-----------------------------------------------------------------------------
void SelectCenterFiber::noMoreBundle( const BundleProducer &producer )
{
  _writer->noMoreBundle(producer);
  cout<<endl<<"fin de l'écriture!!"<<endl<<flush;
}

int main( int argc, const char **argv )
{
  try
  {
    string bunOut;
    string bunIn;
    int dist_type=0;
    int normFactor=5.0;

    AimsApplication  app( argc, argv, "Selects the centroid fiber from distance matrix" );
    app.addOption(bunIn,        "-i", "bundle input file" );
    app.addOption(bunOut,        "-o", "bundle output file" );
    app.addOption(dist_type,        "-dist_type", "fiber distance, \n0: mean of two mean closest point distance (default), \n1: max of two mean closest point distance, \n2: maximum distance between corresponding points, \n3: maximum distance between corresponding points normalized by fibers length", true );
    app.addOption( normFactor, "-normFactor", "Length normalization factor (only for dist_type=3) (default: 5.0)", true );
    app.initialize();

    rc_ptr< SelectCenterFiber > selectCenterFiber;
    selectCenterFiber.reset( new SelectCenterFiber(bunOut, dist_type,normFactor) );
    BundleReader bundle(bunIn);
    bundle.addBundleListener(*selectCenterFiber);
    bundle.read();

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
