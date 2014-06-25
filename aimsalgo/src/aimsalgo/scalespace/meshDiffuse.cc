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
#include <aims/scalespace/meshDiffuse.h>
#include <aims/mesh/texture.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/io/io_g.h>
#include <aims/io/writer.h>
#include <set>
#include <stdio.h>
#include <math.h>

using namespace aims;
using namespace carto;
using namespace std;


void TextureSmoothing::surfaceBlockSmooth( const string & latt_filename,
                               const string *val_filename,
                               const string & path_name, unsigned nscans,
                               float dt, unsigned iter, unsigned scans_block,
                               unsigned nodes_block, unsigned nnodes,
                               unsigned ncolPINV, const string *outfiles , const float HMAX,const float Umax)
{
  unsigned                scans_block_curr, nodes_block_curr;
  FILE                        *fp_pinv, *fp_neighs;
  string                current_val_name, name_to_save, complete_val_name;
  static const unsigned        NVARS = 4;

  Texture1d        *tex = new Texture1d[ scans_block ];
  float                *scans = new float[ scans_block * nnodes ];
  float                *temp = new float[ scans_block * nnodes ];
  float                *PINV_chunk = new float[ nodes_block * NVARS * ncolPINV ];
  int                *IND_NEIGHS_chunk = new int[ nodes_block * ncolPINV ];
  float                *output = new float[ nodes_block ];

  /* opening files .pinv and .neighs for reading  */

  if( ! ( fp_pinv = fopen( (latt_filename + ".pinv").c_str(), "r" ) ) )
    {
      printf("%s\n","ERROR cannot open .pinv file");
      throw file_not_found_error( latt_filename + ".pinv" );
    }

  if( ! ( fp_neighs = fopen( (latt_filename + ".neighs").c_str(), "r" ) ) )
    {
      printf("%s\n","ERROR cannot open .neighs file");
      fclose( fp_pinv );
      throw file_not_found_error( latt_filename + ".neighs" );
    }

#if defined(__GNUC__) && defined (__sun__)
  int imax = (int)::ceil((double)nscans/scans_block);
#else
  int imax = (int)ceil((double)nscans/scans_block);
#endif
  for( int i=0; i<imax; ++i )
    {
      /* how many scans to be treated in current block? */

      scans_block_curr=scans_block;

#if defined(__GNUC__) && defined (__sun__)
      int foobar = (int)::ceil( (double) nscans / scans_block ) - 1;
#else
      int foobar = (int)ceil( (double) nscans / scans_block ) - 1;
#endif
      if( i == foobar && (int) fmod((double) nscans,scans_block) != 0 )
        {
          scans_block_curr = (int) fmod((double) nscans, scans_block );
        }

      /* reads block of scans_block_curr scans   (.val)*/
      /* WARNING: for the time being, default type "float" */

      for( unsigned z=0; z<scans_block_curr; ++z )
        {
          current_val_name = val_filename[ i * scans_block + z ];
          complete_val_name = path_name + current_val_name;
          cout << complete_val_name << endl;

          Reader<Texture1d>        reader( complete_val_name );
          if( !reader.read( tex[z] ) )
            {
              cerr << "Could not read texture " << complete_val_name << endl;
              fclose( fp_pinv );
              fclose( fp_neighs );
              throw corrupt_stream_error( complete_val_name );
            }
          if( tex[z].nItem() != nnodes )
            {
              cerr << "Texture size doesn't correspond to mesh\n";
              fclose( fp_pinv );
              fclose( fp_neighs );
              throw corrupt_stream_error( complete_val_name );
            }
          memcpy( scans + z*nnodes, &tex[z].item(0),
                  tex[z].nItem() * sizeof( float ) );
          float        m = 1e38, M = -1e38, x;
          for( unsigned i=0; i<tex[z].nItem(); ++i )
            {
              x = * ( scans + z*nnodes + i ); //tex[z].item(i);
              if( x < m )
                m = x;
              if( x > M )
                M = x;
            }
          cout << "min : " << m << ", max : " << M <<endl;
        }

      for( unsigned j=0; j<iter; ++j )
        {
          cout << j << "/" << iter << "\r" << flush;

#if defined(__GNUC__) && defined (__sun__)
          int kmax = (int)::ceil( (double) nnodes/nodes_block );
#else
          int kmax = (int)ceil( (double) nnodes/nodes_block );
#endif
          for( int k=0; k<kmax; ++k )
            {
              /* how many nodes to treat in current block ? */

              nodes_block_curr = nodes_block;
              /*printf("%d\n",nodes_block_curr);*/

              if( k == (int) ::ceil( (double) nnodes / nodes_block ) - 1
                  && (int) fmod((double) nnodes, nodes_block ) != 0 )
                {
                  nodes_block_curr
                    = (int) fmod((double) nnodes, nodes_block );
                }

              /* read chunk of PINV (current block) */
              /* WARNING only default type "float" */

              for( unsigned l=0; l<NVARS; ++l )
                {
                  fseek( fp_pinv,
                         4 * l * ncolPINV*nnodes+4*k*ncolPINV*nodes_block,
                         0 );
                  size_t len = fread( PINV_chunk + l*ncolPINV*nodes_block_curr,
                         sizeof(float), nodes_block_curr*ncolPINV, fp_pinv );
                  if( len != nodes_block_curr*ncolPINV )
                  {
                    fclose( fp_pinv );
                    fclose( fp_neighs );
                    throw corrupt_stream_error( latt_filename + ".pinv" );
                  }
                }

              /* read chunk of IND_NEIGHS (current block) */
              /* WARNING only default type "int" */

              fseek( fp_neighs, 4 * k * nodes_block * ncolPINV, 0 );
              int len = fread( IND_NEIGHS_chunk, sizeof(int),
                     nodes_block_curr * ncolPINV, fp_neighs );
              if( len != nodes_block_curr*ncolPINV )
              {
                fclose( fp_pinv );
                fclose( fp_neighs );
                throw corrupt_stream_error( latt_filename + ".neighs" );
              }

              /* cycle over number of scans in current block; call smoothing
                 routine  (1 iteration) */
              for( unsigned l=0; l<scans_block_curr; ++l )
                {
                  tayd_core( IND_NEIGHS_chunk, PINV_chunk, scans+l*nnodes,
                             dt, output, ncolPINV, nodes_block_curr,
                             nodes_block*k + 1,HMAX,Umax);

                  /* update temp */

                  for( unsigned m=0; m<nodes_block_curr; ++m )
                    {
                      *(temp + l*nnodes + k*nodes_block + m) = *(output+m);
                    }
                }
            }

          /* update scans */

          for( unsigned w=0; w<nnodes*scans_block_curr; ++w )
            {
              *(scans + w) = *(temp + w);
            }

        }

      cout << endl;

      /* save smoothed scans to file */

      cout << "saving smoothed scans to file\n";

      for( unsigned z=0; z<scans_block_curr; ++z )
        {
          name_to_save = outfiles[ i * scans_block + z ];
          cout << name_to_save << endl;

          memcpy( &tex[z].item(0), scans + z*nnodes,
                  tex[z].nItem() * sizeof( float ) );
          Writer<Texture1d>        writer( name_to_save );
          if( !writer.write( tex[z] ) )
            {
              cerr << "Could not write " << name_to_save << endl;
            }
          cout << "tex nitem : " << tex[z].nItem() << endl;
          float        m = 1e38, M = -1e38, x;
          for( unsigned i=0; i<tex[z].nItem(); ++i )
            {
              x = * ( scans + z*nnodes + i );
              if( x < m )
                m = x;
              if( x > M )
                M = x;
            }
          cout << "min : " << m << ", max : " << M <<endl;

        }
    }

  fclose( fp_pinv );
  fclose( fp_neighs );

  delete[] output;
  delete[] IND_NEIGHS_chunk;
  delete[] PINV_chunk;
  delete[] temp;
  delete[] scans;
  delete[] tex;
}

void TextureSmoothing::surfaceBlockSmooth( const string & latt_filename,
                               const Texture1d & tex, float dt, unsigned iter,
                               unsigned ncolPINV, Texture1d & outtex,
                               unsigned nodes_block, const float HMAX,const float Umax)
{

  unsigned                nodes_block_curr;
  unsigned                j, k, l, m, w;
  FILE                        *fp_pinv, *fp_neighs;
  static const unsigned        NVARS = 4;
  unsigned                nnodes = tex.nItem();

  float        *scans = new float[  nnodes ];
  float        *temp = new float[  nnodes ];
  float        *PINV_chunk = new float[ nodes_block * NVARS * ncolPINV ];
  int        *IND_NEIGHS_chunk = new int[ nodes_block * ncolPINV ];
  float        *output = new float[ nodes_block ];

  /* opening files .pinv and .neighs for reading  */

  if( ! ( fp_pinv = fopen( (latt_filename + ".pinv").c_str(), "r" ) ) )
    {
      printf("%s\n","ERROR cannot open .pinv file");
      throw file_not_found_error( latt_filename + ".pinv" );
    }

  if( ! ( fp_neighs = fopen( (latt_filename + ".neighs").c_str(), "r" ) ) )
    {
      printf("%s\n","ERROR cannot open .neighs file");
      fclose( fp_pinv );
      throw file_not_found_error( latt_filename + ".neighs" );
    }


  //"surfaceBlockSmooth, nnodes : " << nnodes << endl;

  memcpy( scans , &tex.item(0),
          tex.nItem() * sizeof( float ) );


  for( j=0; j<iter; ++j )
    {
      //if ( fmod(j,10) == 0)
        cout << (int) (j*100/iter) << "%\r" << flush;

      for( k=0; k<(unsigned) ::ceil( (double) nnodes/nodes_block ); ++k )
        {
          /* how many nodes to treat in current block ? */

          nodes_block_curr = nodes_block;
          /*printf("%d\n",nodes_block_curr);*/

          if( k == (unsigned) ::ceil( (double) nnodes / nodes_block ) - 1
              && (int) fmod((double) nnodes, nodes_block ) != 0 )
            {
              nodes_block_curr = (int) fmod((double) nnodes, nodes_block );
            }

          /* read chunk of PINV (current block) */
           /* WARNING only default type "float" */

          for( l=0; l<NVARS; ++l )
            {
              fseek( fp_pinv,
                     4 * l * ncolPINV*nnodes+4*k*ncolPINV*nodes_block,
                     0 );
              size_t len = fread( PINV_chunk + l*ncolPINV*nodes_block_curr,
                     sizeof(float), nodes_block_curr*ncolPINV, fp_pinv );
              if( len != nodes_block_curr*ncolPINV )
              {
                fclose( fp_pinv );
                fclose( fp_neighs );
                throw corrupt_stream_error( latt_filename + ".pinv" );
              }
            }

          /* read chunk of IND_NEIGHS (current block) */
          /* WARNING only default type "int" */

          fseek( fp_neighs, 4 * k * nodes_block * ncolPINV, 0 );
          size_t len = fread( IND_NEIGHS_chunk, sizeof(int),
                 nodes_block_curr * ncolPINV, fp_neighs );
          if( len != nodes_block_curr*ncolPINV )
          {
            fclose( fp_pinv );
            fclose( fp_neighs );
            throw corrupt_stream_error( latt_filename + ".neighs" );
          }


          /* call smoothing routine  (1 iteration) */


          tayd_core( IND_NEIGHS_chunk, PINV_chunk, scans,
                     dt, output, ncolPINV, nodes_block_curr,
                     nodes_block*k + 1,HMAX,Umax);



          /* update temp */


          for( m=0; m<nodes_block_curr; ++m )
            {
              *(temp + k*nodes_block + m) = *(output+m);
            }

        }
      /* update scans */

      for( w=0; w<nnodes; ++w )
        {
          *(scans + w) = *(temp + w);
        }


    }

  /* save smoothed scans to file */

  outtex[0].data().clear();
  outtex[0].data().insert( outtex[0].data().begin(), nnodes, 0 );
  memcpy( &outtex.item(0), scans,
          nnodes * sizeof( float ) );

  fclose( fp_pinv );
  fclose( fp_neighs );

  delete[] output;
  delete[] IND_NEIGHS_chunk;
  delete[] PINV_chunk;
  delete[] temp;
  delete[] scans;
}

void TextureSmoothing::tayd_core( const int *IND_NEIGHS, const float *PINV,
                      const float *values, float dt, float *output,
                      unsigned ncolPINV, unsigned nodes_iter,
                      unsigned first_node, const float HMAX,const float Umax)
{
  static const unsigned        NVARS = 4;
  unsigned                j, l, m, position;
  float                        *Df = new float[ nodes_iter * NVARS ];

  for( l=0; l<nodes_iter; ++l )
    {
      *(output + l) = *( values + first_node - 1 + l );
    }

  //        clear Df array
  for( l=0; l<nodes_iter*NVARS; ++l )
    Df[l] = 0;

  /* multiply PINV by (neigh value - node value) (resolution of the linear set
     of eqs.)
     the structure of Df is as follows:
     [(dI/dx)(1) (dI/dx)(2) ... (dI/dx)(nodes_iter) (dI/dy)(1)...
     ...(d2I/dx2)(1) ... (d2I/dy2)(1) ... (d2I/dy2)(nodes_iter)] */
  for( j=0; j<nodes_iter; ++j )
    {
      position=j*ncolPINV;
      for( m=0; m<NVARS; ++m )
        {
          for( l=0; l<ncolPINV && *(IND_NEIGHS + position + l)!=0; ++l )
            {

              *(Df + m*nodes_iter+j) +=
                (*(values + *(IND_NEIGHS + position +l)-1) -
                 *(values + first_node-1+j)) *
                (*(PINV+m*ncolPINV*nodes_iter+position + l));

            }
        }
    }

  float                       Ux, Uxx, Uy, Uyy /*, Uxy, anis*/;

  for( j=0; j<nodes_iter; ++j )
    {
      Ux = *(Df+0*nodes_iter+j);
      Uy = *(Df+1*nodes_iter+j);
      Uxx = *(Df+2*nodes_iter+j);
      Uyy = *(Df+3*nodes_iter+j);
      if (Uxx + Uyy > Umax)
        Uxx = Uyy = Umax/2;
      else
        if (Uxx + Uyy < -Umax)
          Uxx = Uyy = -Umax/2;
      //Uxy = *(Df+4*nodes_iter+j);
      //anis = ( Uxx*Uy*Uy - 2*Ux*Uy*Uxy + Uyy*Ux*Ux ) / ( Ux*Ux + Uy*Uy );
      *( output + j ) += 0.5 * dt * ( Uxx + Uyy ); // Def isotropic heat equation
      //*( output + j ) += 0.5 * dt * anis ; // Def anisotropic heat equation

      ASSERT (fabs  ( *( output + j ) ) < HMAX );

    }

  delete[] Df;
}

TimeTexture<float> TextureSmoothing::derivative_tayd_core( const int *IND_NEIGHS, const float *PINV,
                                               const float *values,
                                               unsigned ncolPINV, unsigned nnodes )
{
  static const unsigned        NVARS = 4;
  unsigned                i, j, l, m, position;
  float                        *Df = new float[ nnodes * NVARS ];
  TimeTexture<float>    derivTex;


 //        clear Df array
  for( l=0; l<nnodes*NVARS; ++l )
    Df[l] = 0;

  /* multiply PINV by (neigh value - node value) (resolution of the linear set
     of eqs.)
     the structure of Df is as follows:
     [(dI/dx)(1) (dI/dx)(2) ... (dI/dx)(nodes_iter) (dI/dy)(1)...
     ...(d2I/dx2)(1) ... (d2I/dy2)(1) ... (d2I/dy2)(nodes_iter)] */

  for( j=0; j<nnodes; ++j )
    {
      position=j*ncolPINV;
      for( m=0; m<NVARS; ++m )
        {
          for( l=0; l<ncolPINV && *(IND_NEIGHS + position + l)!=0; ++l )
            {
              *(Df + m*nnodes+j) +=
              (*(values + *(IND_NEIGHS + position +l)-1) -
               *(values + j)) *
              (*(PINV+m*ncolPINV*nnodes+position + l));
              //cout <<(*(values + *(IND_NEIGHS + position +l)-1) - *(values + j)) ;
              //cout << *(IND_NEIGHS + position +l)-1 << endl  ;

            }
        }
    }
  for( j=0; j<nnodes; ++j )
    for (i=0; i<NVARS; ++i)
      derivTex[i].push_back( *(Df + i*nnodes + j ) )  ;
  delete[] Df;

  return derivTex;
}

TimeTexture<float> TextureSmoothing::derivative( const string & latt_filename,
                                     const Texture<float> & tex,
                                     unsigned ncolPINV )
{

  FILE                        *fp_pinv, *fp_neighs;
  unsigned                nnodes = tex.nItem();
  static const unsigned        NVARS = 4;

  float                        *scans = new float[  nnodes ];
  float                        *PINV = new float[ nnodes * NVARS * ncolPINV ];
  int                        *IND_NEIGHS = new int[ nnodes * ncolPINV ];
  TimeTexture<float>          deriveTex;

  /* opening files .pinv and .neighs for reading  */

  if( ! ( fp_pinv = fopen( (latt_filename + ".pinv").c_str(), "r" ) ) )
    {
      printf("%s\n","ERROR cannot open .pinv file");
      return deriveTex;
    }

  if( ! ( fp_neighs = fopen( (latt_filename + ".neighs").c_str(), "r" ) ) )
    {
      printf("%s\n","ERROR cannot open .neighs file");
      fclose( fp_pinv );
      return deriveTex;
    }

  memcpy( scans , &tex.item(0),
           tex.nItem() * sizeof( float ) );


  size_t len = fread( PINV,sizeof(float), nnodes * NVARS * ncolPINV, fp_pinv );
  if( len != nnodes * NVARS * ncolPINV )
  {
    fclose( fp_pinv );
    fclose( fp_neighs );
    throw corrupt_stream_error( latt_filename + ".pinv" );
  }

  len = fread( IND_NEIGHS, sizeof(int),nnodes * ncolPINV, fp_neighs );
  if( len != nnodes * ncolPINV )
  {
    fclose( fp_pinv );
    fclose( fp_neighs );
    throw corrupt_stream_error( latt_filename + ".neighs" );
  }

  fclose( fp_pinv );
  fclose( fp_neighs );

  //for (unsigned i=0; i<nnodes * ncolPINV; ++i)
  //cout << IND_NEIGHS[i] << " " ;

  deriveTex = derivative_tayd_core( IND_NEIGHS, PINV, scans,ncolPINV, nnodes );

  delete[] IND_NEIGHS;
  delete[] PINV;

  return ( deriveTex );
}



Texture<float> TextureSmoothing::FiniteElementSmoothing( const Texture<float> &inittex,
                                                         float dt, float dur,
                                                         float Smax,
                                                         const map<unsigned, set< pair<unsigned,float> > > & weightLapl)
{

  unsigned                      t;
  float                         s;
  Texture<float>                smooth = inittex ,lapl;
  unsigned                      i,n=smooth.nItem();

  // TimeTexture<float> temp;
  // unsigned inc = 0;

  if (dur != -1)
  {
// #define use_matpow
// use_matpow: for test only: for a single texture it is far slower than the
// normal version.
#ifdef use_matpow
    LaplacianWeights *weightLaplPow
      = makeLaplacianSmoothingCoefficients( weightLapl, rint(dur/dt), dt,
                                            0.0001 );
    cout << "apply smoothing coefs...\n";
    applyLaplacianMatrix( inittex.data(), smooth.data(), *weightLaplPow );
    delete weightLaplPow;

#else

    for (t=0; t< rint(dur/dt); ++t)
      {
        //if (t%10 == 0)
        {
          cout << "                ";
          cout << "\r" << rint(100*t*dt/dur) << "%" << flush;
        }
        lapl =  AimsMeshLaplacian(smooth,weightLapl);
        for ( i=0; i<n; ++i)
          {
            s = smooth.item(i) + dt * lapl.item(i);
            if (s <= -Smax)
              smooth.item(i) = -Smax;
            else
              if (s>= Smax)
                smooth.item(i) = Smax;
              else
                smooth.item(i) = s;
          }

          /*        if (t%2 == 0)
          {
          for ( i=0; i<n; ++i)
            temp[inc].push_back( smooth.item(i) );
          ++inc;
          }
*/
      }
#endif
  }
  else
    {
      cout << "Estimation of the Laplacian of the texture\n";
      smooth =  AimsMeshLaplacian(smooth,weightLapl);
    }

/*
  cout << "size: " << temp.size() << endl;
  cout << "writing temp texture : " << endl;
  Writer<TimeTexture<float> >        texW( "/tmp/curv_ss.tex" );
  texW << temp ;
*/
  cout << "done " << endl;

  return smooth;
}

Texture<float> TextureSmoothing::FiniteElementSmoothingWithSource( const Texture<float> &inittex, const Texture<float> &sourcetex,
                                                                   float dt, float dur,
                                                                   const map<unsigned, set< pair<unsigned,float> > > & weightLapl)
{

  unsigned                      t;
  float                         s,max;
  Texture<float>            smooth = inittex ,lapl;
  unsigned                      i,n=smooth.nItem();



  for (t=0; t< rint(dur/dt); ++t)
      {

        cerr << "                ";
        cerr << "\r" << rint(100*t*dt/dur) << "%" << flush;

        if ( t%100 == 0 && t!= 0 )
          {
            max = -1;
            for ( i=0; i<n; ++i)
                if (fabs(lapl.item(i)) > max)
                  max = fabs(lapl.item(i));
            cout << max << endl;
          }

        lapl =  AimsMeshLaplacian(smooth,weightLapl);

        for ( i=0; i<n; ++i)
          if (sourcetex.item(i) == 0)
            {
              s = smooth.item(i) + dt * lapl.item(i);
              smooth.item(i) = s;

            }

      }

  return smooth;
}
