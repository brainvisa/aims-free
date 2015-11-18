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
#include <aims/getopt/getopt.h>
#include <aims/data/data_g.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/io/motionR.h>
#include <aims/io/motionW.h>
#include <aims/resampling/resampling_g.h>

using namespace aims;
using namespace std;


BEGIN_USAGE(usage)
  "----------------------------------------------------------------",
  "AimsNormDiff                                                    ",
  "            -r[eference image]   < ref filename >               ",
  "            -c[ompared image]    < compared filename >          ",
  "            -a[ref to mri dep]   <filename >                    ",
  "            -b[com to mri dep]   <filename >                    ",
  "            -o[utput]            < norm image >                 ",
  "           [-h[elp]]                                            ",
  "----------------------------------------------------------------",
  " Compute ration (R - C) / R                                     ",
  "----------------------------------------------------------------",
END_USAGE



void Usage( void )
{
  AimsUsage( usage );
}



int main(int argc,char **argv)
{
  char *ref=NULL, *comp=NULL; // Mandatory file name input
  char  *fileoutput=NULL;

  char *fileref2mri=NULL, *filecomp2mri=NULL;

  AimsOption opt[] = {
  { 'h',"help"     ,AIMS_OPT_FLAG  ,( void* )Usage      ,AIMS_OPT_CALLFUNC,0},
  { 'r',"reference",AIMS_OPT_STRING,&ref                    ,0            ,1},
  { 'c',"compared" ,AIMS_OPT_STRING,&comp                ,0               ,1},
  { 'a',"compared" ,AIMS_OPT_STRING,&fileref2mri          ,0              ,1},
  { 'b',"compared" ,AIMS_OPT_STRING,&filecomp2mri         ,0              ,0}, 
  { 'o',"output"   ,AIMS_OPT_STRING,&fileoutput          ,0               ,0},
  { 0  ,0          ,AIMS_OPT_END   ,0                    ,0               ,0}
};

  AimsParseOptions(&argc,argv,opt,usage);


  //Read Motion info =========================================================
  Motion ref2mri;  // set with Identity
  if (fileref2mri)
    {
      MotionReader mrd1( fileref2mri );
      mrd1 >> ref2mri;
    }

  Motion comp2mri;  // set with Identity
  if (fileref2mri)
    {
      MotionReader mrd2( filecomp2mri );
      mrd2 >> comp2mri;
    }



  AimsData< float > reference;
  Reader< AimsData< float > > rdr( ref );
  try {
    rdr >> reference;
  }
  catch(exception &e)
    {
      cerr << e.what() << endl;
      throw;
    }

  AimsData< float > compared;
  Reader< AimsData< float > > rdc( comp );
  try {
    rdc >> compared;
  }
  catch(exception &e)
    {
      cerr << e.what() << endl;
      throw;
    }

  if (
      (compared.dimX() != reference.dimX()) ||
      (compared.dimY() != reference.dimY()) ||
      (compared.dimZ() != reference.dimZ()) ||
      (compared.dimT() != reference.dimT()))
    {
      cerr << ref << " and " << comp <<" do not have the same dimensions." 
	   << endl;
    }
  
  // Code doit être étendu au 4D.
  if (compared.dimT() != 1)
    {
      cerr << "This command is limited to 3D data sets for now on..." << endl;
      exit(EXIT_FAILURE);
    }
  if (!fileref2mri && !filecomp2mri )
    {
      int x, y, z, t;

      
      ForEach4d(reference, x, y, z, t)
	{
	  if ( (reference(x, y, z) >=1)  && (compared(x, y, z) >=1))
	    reference(x, y, z, t) = (reference(x, y, z, t) - compared(x, y, z, t))/
	      reference(x, y, z, t);
	  else
	    reference(x, y, z, t) = 0.0;
	}
      
      
      Writer< AimsData< float > > wt( fileoutput );
      wt << reference;
    }
  else
    {
      Point3df from = (reference.sizeX(), reference.sizeY(), reference.sizeZ());

      // A = ref2mri   B = comp2mri : Wanted comp2ref
      // Pixel size are assumed to be the same
      Motion a, b, c, comp2ref;
      a = ref2mri; b = comp2mri;
      c = a.inverse();
      AimsData< float > aa(4,4), bb(4,4), cc(4,4), dd(4,4);
      

      int i, j , k;
      for (i=0;i<3;++i)
	for (j=0;j<3;++j)
	  {
	    bb(j,i) = b.rotation()(i,j);
	    cc(j,i) = c.rotation()(i,j);
	  }
      for (i=0;i<3;++i)
	{
	  bb(3,i) = b.translation().item(i);
	  cc(3,i) = c.translation().item(i);
	}
      bb(3,3) = cc(3,3) = 1.0;
      
      ForEach2d(dd, i, j)
	for (k=0;k<4;++k)
	  dd(i,j) += bb(i,k)*cc(k,j);
      

      for (i=0;i<3;++i)
	{
	  comp2ref.translation().item(i) = dd(3,i);
	}
     for (i=0;i<3;++i)
	for (j=0;j<3;++j)
	{
	  comp2ref.rotation()(j,i) = dd(i,j);
	}



      MotionWriter w("comp_TO_ref.trm");
      w.write( comp2ref );

      LinearResampler<float> interpolator;
      interpolator.setRef( compared );
      AimsData< float > res_compared = interpolator.doit( comp2ref,
        reference.dimX(),reference.dimY(),reference.dimZ(),
        Point3df(reference.sizeX(), reference.sizeY(),reference.sizeZ()) );

//       Writer< AimsData<float> > yy("groumf");
//       yy << res_compared;
      int x, y, z;
      ForEach3d(reference, x, y, z )
	{
	  if ( (reference(x, y, z) >=1)  && (res_compared(x, y, z) >=1))
	    reference(x, y, z) = 100 * (reference(x, y, z) - res_compared(x, y, z)) /
	      reference(x, y, z);
	  else
	    reference(x, y, z) = 0.0;
	}

      
      
      
      Writer< AimsData< float > > wt( fileoutput );
      wt << reference;
    }

  return EXIT_SUCCESS;
}
