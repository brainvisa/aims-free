/* Copyright (c) 1995-2007 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
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


#include <cstdlib>
#include <iostream>
#include <aims/getopt/getopt2.h>
#include <aims/data/data.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/io/process.h>
#include <aims/io/finder.h>
#include <aims/texdata/texdata.h>
#include <aims/primalsketch/coupledDiffusion2DSmoother_d.h>

using namespace aims;
using namespace carto;
using namespace std;

int main(int argc, const char **argv)
{
  std::string fileIn1="", fileout1, fileCont1;
  std::string fileIn2="", fileout2, fileCont2;
  float dt=0.1, tmax, alpha=1.0, beta=1.0, gamma=1.0;

  AimsApplication     app( argc, argv, "Lissage couplé d'un couple d'images 2D avec des contraintes ");


  app.addOption( fileIn1, "-i1", "input Image1", "");
  app.alias( "--input1", "-i1" );
  app.addOption( fileIn2, "-i2", "input Image2", "");
  app.alias( "--input2", "-i2" );
  app.addOption( fileCont1, "-c1", "input constraint1");
  app.alias( "--cont1", "-c1" );
  app.addOption( fileCont2, "-c2", "input constraint2");
  app.alias( "--cont2", "-c2" );
  app.addOption( fileout1, "-o1", "output Image1");
  app.alias( "--output1", "-o1" );
  app.addOption( fileout2, "-o2", "output Image2");
  app.alias( "--output2", "-o2" );
  app.addOption( tmax, "-t", "scale");
  app.alias( "--scale", "-t" );
  app.addOption( dt, "-dt", "time step (diffusion only; default=0.1)", 0.1);
  app.alias( "--deltat", "-dt" );
  app.addOption( alpha, "-a", "diffusion weight",  1.0);
  app.addOption( beta, "-b", "constraints term", 1.0);
  app.addOption( gamma, "-g", "coupling term", 1.0);
  try
  {
    app.initialize();
  }
  catch( user_interruption & )
  {
    return( EXIT_FAILURE );
  }
  catch( exception & e )
  {
    cerr << e.what() << endl;
    return( EXIT_FAILURE );
  }


      AimsData<float> constraint1, constraint2, imageOut1, imageOut2;
      Reader<AimsData<float> > readerC1( fileCont1 );
      Reader<AimsData<float> > readerC2( fileCont2 );
      cout << "Reading constraint images " << fileCont1 << " and " << fileCont2 << endl;
      int sx, sy, x, y,
        nb1=0,
        nb2=0;
      float mean1=0.0,
        mean2=0.0;

      if( !readerC1.read( constraint1 ))
        return EXIT_FAILURE;
      if ( !readerC2.read( constraint2 ))
        return EXIT_FAILURE;
      if ( !hasSameDim(constraint1, constraint2))
        {
          cerr << "Contraints images do not have same size !!!" << endl;
          return EXIT_FAILURE;
        }
      sx=constraint1.dimX(); sy=constraint1.dimY();

      if ( constraint1.dimZ() >1)
        {
          std::cerr<<"Only 2D images please" << endl;
          return EXIT_FAILURE;
        }
      /*
        std::cout << "Building images" << endl;
        for (y=0; y<128; y++)
        for (x=0; x<128; x++)
        {
        if (x==0)
        constraint1(x,y)=1.0;
        else if (x==127)
        constraint1(x,y)=100.0;
        if (y==0)
        constraint2(x,y)=1.0;
        else if (y==127)
        constraint2(x,y)=100.0;
        if ((x==y) && (x>=43) && (x<86))
        constraint2(x,y)=0.0;
        if ( ((y+10)==x) && (x>=50) && (x<=70) )
        constraint2(x,y)=50.0;
        if ( ((y-10)==x) && (x>=60) && (x<=80) )
        constraint2(x,y)=50.0;

        }
        std::cout << "Writing them" << endl;

        Writer<AimsData<float> > writerC1("twoCont1.ima");
        Writer<AimsData<float> > writerC2("twoCont2.ima");
        writerC1.write(constraint1);
        writerC2.write(constraint2);*/


      cout << "Dimensions : " << sx << "x" << sy << endl;
      AimsData<float> imageIn1, imageIn2;

      if ((fileIn1=="") || (fileIn2==""))
        {
          imageIn1=AimsData<float>(sx,sy);
          imageIn2=AimsData<float>(sx,sy);
          std::cout << "Computing constraints mean for initial images" << endl;

          for (y=0; y<sy; y++)
            for (x=0; x<sx; x++)
              {
                if (constraint1(x,y) > 0)
                  {
                    nb1++; mean1=mean1+constraint1(x,y);
                  }
                if (constraint2(x,y) > 0)
                  {
                    nb2++; mean2=mean2+constraint2(x,y);
                  }
              }
          mean1=mean1/float(nb1);
          mean2=mean2/float(nb2);
          std::cout << "mean1=" << mean1 << ", mean2=" << mean2 << endl;
          std::cout << "Building initial images" << endl;
          for (y=0; y<sy; y++)
            for (x=0; x<sx; x++)
              {
                imageIn1(x,y)=mean1;
                imageIn2(x,y)=mean2;
              }
        }
      else
        {
          Reader<AimsData<float> > readerI1( fileIn1 );
          Reader<AimsData<float> > readerI2( fileIn2 );
          cout << "Reading initial images " << fileIn1 << " and " << fileIn2 << endl;
          if ( !readerI1.read( imageIn1 ))
            return EXIT_FAILURE;
          if ( !readerI2.read( imageIn2 ))
            return EXIT_FAILURE;
        }

      std::cout << "Building appropriate smoother" << endl;


      CoupledDiffusion2DSmoother<float> *smooth;
      smooth=new CoupledDiffusion2DSmoother<float>(dt, alpha, beta, gamma);


      cout << "Starting" << endl;

      std::pair<AimsData<float>, AimsData<float> > result;
      result=smooth->doSmoothing( std::pair<AimsData<float>, AimsData<float> >(imageIn1, imageIn2), std::pair<AimsData<float>, AimsData<float> >(constraint1, constraint2), tmax / dt);

      cout << "Writing smoothed image " << fileout1 << " and " << fileout2 << endl;

      Writer<AimsData<float> > writer1( fileout1 );
      Writer<AimsData<float> > writer2( fileout2 );
      writer1.write(result.first);
      writer2.write(result.second);

      cout << "FINISHED !" << endl;


  return(EXIT_SUCCESS);
}
