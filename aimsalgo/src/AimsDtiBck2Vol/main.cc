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
#include <aims/def/def_g.h>
#include <aims/data/data_g.h>
#include <aims/io/io_g.h>
#include <aims/math/dtitensor.h>
#include <aims/getopt/getopt.h>
#include <aims/utility/utility_g.h>
#include <aims/bucket/bucket_g.h>
#include <cstdlib>

using namespace aims;
using namespace std;

BEGIN_USAGE(usage)
  "------------------------------------------------------------------",
  "AimsDtiBck2Vol -i[nput] <bucket>                                  ",
  "               [-o[utput] <dti>]                                  ",
  "               [-x <dimx> -y <dimy> -z <dimz>]                    ",
  "               [-h[elp]]                                          ",
  "------------------------------------------------------------------",
  "Convert a DTI bucket to a FLOAT tensor image                      ",
  "------------------------------------------------------------------",
  "     bucket     : input DTI bucket data                           ",
  "     dti        : destination FLOAT tensor data [default=bucket]  ",
  "     dimx,dimy,                                                   ",
  "     dimz       : dimensions of the destination volume            ",
  "------------------------------------------------------------------",
END_USAGE


void Usage( void )
{
  AimsUsage( usage );
}


int main( int argc, char **argv )
{
  char *fileBucket = NULL, *fileOut = NULL;
  int dimX = 0, dimY = 0, dimZ = 0;
  int maxX = 0, maxY = 0, maxZ = 0;

  AimsOption opt[] = {
  { 'h',"help"  ,AIMS_OPT_FLAG  ,( void* )Usage       ,AIMS_OPT_CALLFUNC,0,},
  { 'i',"input" ,AIMS_OPT_STRING,&fileBucket ,0                ,1},
  { 'o',"output",AIMS_OPT_STRING,&fileOut    ,0                ,0},
  { 'x',"x"     ,AIMS_OPT_INT   ,&dimX       ,0                ,0},
  { 'y',"y"     ,AIMS_OPT_INT   ,&dimY       ,0                ,0},
  { 'z',"z"     ,AIMS_OPT_INT   ,&dimZ       ,0                ,0},
  { 0  ,0       ,AIMS_OPT_END   ,0           ,0                ,0}};

  AimsParseOptions( &argc, argv, opt, usage );

  if ( fileOut == NULL )
    fileOut = fileBucket;

  AimsBucket< DtiTensor > bucket;
  Reader<AimsBucket<DtiTensor> >	bucketR( fileBucket );
  bucketR >> bucket;

  ASSERT( bucket.size() == 1 );

  list< AimsBucketItem<DtiTensor> >::iterator it;

  for ( it = bucket[0].begin(); it != bucket[0].end(); it++ )
  {
    if ( it->location().item( 0 ) > maxX )
      maxX = it->location().item( 0 );
    if ( it->location().item( 1 ) > maxY )
      maxY = it->location().item( 1 );
    if ( it->location().item( 2 ) > maxZ )
      maxZ = it->location().item( 2 );
  }

  dimX = max( dimX, maxX + 1 );
  dimY = max( dimY, maxY + 1 );
  dimZ = max( dimZ, maxZ + 1 );

  AimsData<float> dti( dimX, dimY, dimZ, 6 );
  dti.setSizeX( bucket.sizeX() );
  dti.setSizeY( bucket.sizeY() );
  dti.setSizeZ( bucket.sizeZ() );
  dti = 0.0;

  int i;
  for ( it = bucket[0].begin(); it != bucket[0].end(); it++ )
    for ( i = 0; i < 6; i++ )
      dti( it->location().item( 0 ),
           it->location().item( 1 ),
           it->location().item( 2 ), i ) = it->value().base().coef().item( i );
    
  Writer<AimsData<float> > dataW( fileOut );
  dataW << dti;

  return EXIT_SUCCESS;
}
