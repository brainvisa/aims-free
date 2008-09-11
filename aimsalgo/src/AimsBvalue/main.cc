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


#include <aims/data/data_g.h>
#include <aims/vector/vector.h>
#include <aims/math/math_g.h>
#include <aims/io/io_g.h>
#include <aims/getopt/getopt2.h>
#include <iostream>
#include <iomanip>
#include <stdio.h>

using namespace aims;
using namespace carto;
using namespace std;


static int bipolar=0;

struct Trapeze
{
  float a;
  float pwa;
  float pw;
  float pwd;
};


struct Quadrilatere
{
  float aa;
  float ab;
  float pwa;
  float pw;
  float pwd;
};


struct MultiTrapeze1
{
  int rhhnover;
  float esp;
  float a;
  float pwad;
  float pw;
};


struct RadioFreq
{
  int numlobe;
  float a;
  float pwlobe;
  float pw_ss_rampz;
};


struct SequenceEPI1
{
  // X/read
  Trapeze Gxdl;
  Trapeze Gxdr;
  Trapeze Gx1;
  MultiTrapeze1 Gxw;
  
  // Y/phase
  Trapeze Gydl;
  Trapeze Gydr;
  Quadrilatere Gy1;
  Trapeze Gyb;

  // Z/slice
  RadioFreq Gzrf1;  
  Trapeze Gzdl;
  Trapeze Gzrf2l1;
  Trapeze Gzrf2;
  Trapeze Gzrf2r1;
  Trapeze Gzdr;

  float Pw_wgxdl, Pw_wgydl, Pw_wgzdl;
  float Pw_wgxdr, Pw_wgydr, Pw_wgzdr;

  float Te;
  int Etl;
  int NEcho;
  int NumTrial;
  AimsData<float>* Gradient;

  float TGzrf2, TGzrf2l1, TGxdl, TGydl, TGzdl;
  float TGzrf2r1, TGxdr, TGydr, TGzdr;
  AimsData<float>* TGzrf1;
  float TGx1, TGy1;
  AimsData<float> *TGxw, *TGyb;
};


SequenceEPI1 SeqEPI1;
float AGxdl, AGydl, AGzdl;
float AGxdr, AGydr, AGzdr;
float EPS = 1e-5;
int JMAX = 22;
int K = 10;


float Trap( float t, float amp, float start, float dt1, float dur, float dt2 )
{
  if ( t <= start || t >= start + dt1 + dur + dt2 ) 
    return 0.0;
  else if ( t > start && t < start + dt1 )           
    return amp * ( t - start ) / dt1;
  else if ( t >= start + dt1 && t < start + dt1 + dur )  
    return amp;
  else if ( t >= start + dt1 + dur && t < start + dt1 + dur + dt2 ) 
    return amp * ( start + dt1 + dur + dt2 - t ) / dt2;
  else
    return 0.0;
}


float TrapItg( float t, float amp, float start, float dt1, float dur, float dt2)
{
  if ( t <= start ) 
    return 0.0;
  else if ( t > start && t < start + dt1 )           
    return amp * sqr( t - start ) / ( 2 * dt1 );
  else if ( t >= start + dt1 && t < start + dt1 + dur )  
    return amp * ( t - start - dt1 / 2 );
  else if ( t >= start + dt1 + dur && t < start + dt1 + dur + dt2 ) 
    return amp * ( ( dt1 + dt2 ) / 2 + dur -
                   sqr( start + dt1 + dur + dt2 - t ) / ( 2 * dt2 ) );
  else if ( t >= start + dt1 + dur + dt2 )
    return amp * ( ( dt1 + dt2 ) / 2 + dur );
  else
    return 0.0;
}


float Quad( float t, float amp1, float amp2,
            float start, float dt1, float dur, float dt2)
{
  if ( t <= start || t >= start + dt1 + dur + dt2 ) 
    return 0.0;
  else if ( t > start && t < start + dt1 )           
    return amp1 * ( t - start ) / dt1;
  else if ( t >= start + dt1 && t < start + dt1 + dur )  
    return amp1 + ( amp2 - amp1 ) * ( t - start - dt1 ) / dur;
  else if ( t >= start + dt1 + dur && t < start + dt1 + dur + dt2 ) 
    return amp2 * ( start + dt1 + dur + dt2 - t ) / dt2;
  else
    return 0.0;
}


float QuadItg( float t, float amp1, float amp2,
               float start, float dt1, float dur, float dt2 )
{
  if ( t <= start ) 
    return 0.0;
  else if ( t > start && t < start + dt1 )           
    return amp1 * sqr( t - start ) / ( 2 * dt1 );
  else if ( t >= start + dt1 && t < start + dt1 + dur )  
    return amp1 * dt1 / 2 + amp1 * ( t - start - dt1 ) + 
           ( amp2 - amp1 ) * sqr( t - start - dt1 ) / ( 2 * dur );
  else if ( t >= start + dt1 + dur && t < start + dt1 + dur + dt2  ) 
    return amp1 * ( dt1 / 2 + dur / 2 ) + amp2 * ( dur / 2 + dt2 / 2 - 
                    sqr( start + dt1 + dur + dt2 - t ) / ( 2 * dt2 ) );
  else if ( t >= start + dt1 + dur + dt2 )
    return amp1 * ( dt1 / 2 + dur / 2 ) + amp2 * ( dur / 2 + dt2 / 2 );
  else
    return 0.0;
}


ostream& operator << ( ostream& out, const SequenceEPI1& seq )
{
  out << "##############" << endl;
  out << "# gradient X #" << endl;
  out << "##############" << endl << endl;

  out << "#trapeze Gxdl        " << endl;
  out << "-a_gxdl              " << seq.Gxdl.a       << endl;
  out << "-pw_gxdla            " << seq.Gxdl.pwa     << endl;
  out << "-pw_gxdl             " << seq.Gxdl.pw      << endl;
  out << "-pw_gxdld            " << seq.Gxdl.pwd     << endl << endl;

  out << "#trapeze Gxdr        " << endl;
  out << "-a_gxdr              " << seq.Gxdr.a       << endl;
  out << "-pw_gxdra            " << seq.Gxdr.pwa     << endl;
  out << "-pw_gxdr             " << seq.Gxdr.pw      << endl;
  out << "-pw_gxdrd            " << seq.Gxdr.pwd     << endl << endl;

  out << "#trapeze Gx1         " << endl;
  out << "-a_gx1               " << seq.Gx1.a        << endl;
  out << "-pw_gx1a             " << seq.Gx1.pwa      << endl;
  out << "-pw_gx1              " << seq.Gx1.pw       << endl;
  out << "-pw_gx1d             " << seq.Gx1.pwd      << endl << endl;

  out << "#multitrapeze1 Gxw   " << endl;
  out << "-rhhnover            " << seq.Gxw.rhhnover << endl;
  out << "-esp                 " << seq.Gxw.esp      << endl;
  out << "-a_gxw               " << seq.Gxw.a        << endl;
  out << "-pw_gxwad            " << seq.Gxw.pwad     << endl;
  out << "-pw_gxw              " << seq.Gxw.pw       << endl << endl;


  out << "##############" << endl;
  out << "# gradient Y #" << endl;
  out << "##############" << endl << endl;

  out << "#trapeze Gydl        " << endl;
  out << "-a_gydl              " << seq.Gydl.a       << endl;
  out << "-pw_gydla            " << seq.Gydl.pwa     << endl;
  out << "-pw_gydl             " << seq.Gydl.pw      << endl;
  out << "-pw_gydld            " << seq.Gydl.pwd     << endl << endl;

  out << "#trapeze Gydr        " << endl;
  out << "-a_gydr              " << seq.Gydr.a       << endl;
  out << "-pw_gydra            " << seq.Gydr.pwa     << endl;
  out << "-pw_gydr             " << seq.Gydr.pw      << endl;
  out << "-pw_gydrd            " << seq.Gydr.pwd     << endl << endl;

  out << "#quadrilatere Gy1    " << endl;
  out << "-a_gy1a              " << seq.Gy1.aa       << endl;
  out << "-a_gy1b              " << seq.Gy1.ab       << endl;
  out << "-pw_gy1a             " << seq.Gy1.pwa      << endl;
  out << "-pw_gy1              " << seq.Gy1.pw       << endl;
  out << "-pw_gy1d             " << seq.Gy1.pwd      << endl << endl;

  out << "#multitrapeze2 Gyb   " << endl;
  out << "-a_gyb               " << seq.Gyb.a        << endl;
  out << "-pw_gyba             " << seq.Gyb.pwa      << endl;
  out << "-pw_gyb              " << seq.Gyb.pw       << endl;
  out << "-pw_gybd             " << seq.Gyb.pwd      << endl << endl;


  out << "##############" << endl;
  out << "# gradient Z #" << endl;
  out << "##############" << endl << endl;

  out << "#multitrapeze1 Gzrf1 " << endl;
  out << "-num_rf1_lobe        " << seq.Gzrf1.numlobe     << endl;
  out << "-a_gzrf1             " << seq.Gzrf1.a           << endl;
  out << "-pw_gzrf1lobe        " << seq.Gzrf1.pwlobe      << endl;
  out << "-pw_ss_rampz         " << seq.Gzrf1.pw_ss_rampz << endl << endl;

  out << "#trapeze Gzdl        " << endl;
  out << "-a_gzdl              " << seq.Gzdl.a       << endl;
  out << "-pw_gzdla            " << seq.Gzdl.pwa     << endl;
  out << "-pw_gzdl             " << seq.Gzdl.pw      << endl;
  out << "-pw_gzdld            " << seq.Gzdl.pwd     << endl << endl;

  out << "#trapeze Gzrf2l1     " << endl;
  out << "-a_gzrf2l1           " << seq.Gzrf2l1.a    << endl;
  out << "-pw_gzrf2l1a         " << seq.Gzrf2l1.pwa  << endl;
  out << "-pw_gzrf2l1          " << seq.Gzrf2l1.pw   << endl;
  out << "-pw_gzrf2l1d         " << seq.Gzrf2l1.pwd  << endl << endl;

  out << "#trapeze Gzrf2       " << endl;
  out << "-a_gzrf2             " << seq.Gzrf2.a      << endl;
  out << "-pw_gzrf2a           " << seq.Gzrf2.pwa    << endl;
  out << "-pw_gzrf2            " << seq.Gzrf2.pw     << endl;
  out << "-pw_gzrf2d           " << seq.Gzrf2.pwd    << endl << endl;

  out << "#trapeze Gzrf2r1     " << endl;
  out << "-a_gzrf2r1           " << seq.Gzrf2r1.a    << endl;
  out << "-pw_gzrf2r1a         " << seq.Gzrf2r1.pwa  << endl;
  out << "-pw_gzrf2r1          " << seq.Gzrf2r1.pw   << endl;
  out << "-pw_gzrf2r1d         " << seq.Gzrf2r1.pwd  << endl << endl;

  out << "#trapeze Gzdr        " << endl;
  out << "-a_gzdr              " << seq.Gzdr.a       << endl;
  out << "-pw_gzdra            " << seq.Gzdr.pwa     << endl;
  out << "-pw_gzdr             " << seq.Gzdr.pw      << endl;
  out << "-pw_gzdrd            " << seq.Gzdr.pwd     << endl << endl;
 

  out << "##########" << endl;
  out << "#   Pw   #" << endl;
  out << "##########" << endl;
  out << "-pw_wgxdl            " << seq.Pw_wgxdl     << endl;
  out << "-pw_wgydl            " << seq.Pw_wgydl     << endl;
  out << "-pw_wgzdl            " << seq.Pw_wgzdl     << endl;
  out << "-pw_wgxdr            " << seq.Pw_wgxdr     << endl;
  out << "-pw_wgydr            " << seq.Pw_wgydr     << endl;
  out << "-pw_wgzdr            " << seq.Pw_wgzdr     << endl << endl;


  out << "##########" << endl;
  out << "#   Te   #" << endl;
  out << "##########" << endl;
  out << "-te                  " << seq.Te           << endl << endl;


  out << "##########" << endl;
  out << "#   Etl  #" << endl;
  out << "##########" << endl;
  out << "-etl                 " << seq.Etl          << endl << endl;


  out << "##########" << endl;
  out << "# Trials #" << endl;
  out << "##########" << endl;
  out << "-numtrial            " << seq.NumTrial     << endl << endl;
 
  out << *(seq.Gradient)                             << endl << endl;


  out << "##########" << endl;
  out << "# Times  #" << endl;
  out << "##########" << endl;
  out << "-TGzrf2              " << seq.TGzrf2       << endl;
  out << "-TGzrf2l1            " << seq.TGzrf2l1     << endl;
  out << "-TGxdl               " << seq.TGxdl        << endl;
  out << "-TGydl               " << seq.TGydl        << endl;
  out << "-TGzdl               " << seq.TGzdl        << endl;
  out << "-TGzrf2r1            " << seq.TGzrf2r1     << endl;
  out << "-TGxdr               " << seq.TGxdr        << endl;
  out << "-TGydr               " << seq.TGydr        << endl;
  out << "-TGzdr               " << seq.TGzdr        << endl;
  for ( int n = 0; n < seq.TGzrf1->dimX( ); n++ )
    out << "-TGzrf1(" << setw( 2 ) << n << ")          " << (*seq.TGzrf1)( n ) 
                                                         << endl;
  out << "-TGx1                " << seq.TGx1         << endl;
  out << "-TGy1                " << seq.TGy1         << endl;
  for ( int n = 0; n < seq.TGxw->dimX( ); n++ )
    out << "-TGxw(" << setw( 2 ) << n << ")            " << (*seq.TGxw)( n ) 
                                                         << endl;
  for ( int n = 0; n < seq.TGyb->dimX( ); n++ )
    out << "-TGyb(" << setw( 2 ) << n << ")            " << (*seq.TGyb)( n ) 
                                                         << endl;
  return out << endl;
}


void ReadEPI1( const string& filename, SequenceEPI1& seq )
{
  FILE *fp;
  char str[1024], strField[1024], strArg[1024], *strc;
  int index;

  seq.NumTrial = -1;
  fp = fopen( filename.c_str(), "rt" );
  if ( fp == NULL )
    AimsError("void AimsReadEPI1(const string, SequenceEPI1 &):\
               cannot open file" );

  while ( fgets( str, 1023, fp ) )
    if ( *str != '\n' && *str != '#' )
    {
      strc = str;
      do
      {
        AimsScanNflush( &strc, strField );
        if ( !strcmp( strField, "-a_gxdl" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gxdl.a );
          seq.Gxdl.a *= 1e-2;
        }
        else if ( !strcmp( strField, "-pw_gxdla" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gxdl.pwa );
          seq.Gxdl.pwa *= 1e-6;
        }
        else if ( !strcmp( strField, "-pw_gxdl" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gxdl.pw );
          seq.Gxdl.pw *= 1e-6;
        }
        else if ( !strcmp( strField, "-pw_gxdld" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gxdl.pwd );
          seq.Gxdl.pwd *= 1e-6;
        }
        else if ( !strcmp( strField, "-a_gxdr" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gxdr.a );
          seq.Gxdr.a *= 1e-2;
        }
        else if ( !strcmp( strField, "-pw_gxdra" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gxdr.pwa );
          seq.Gxdr.pwa *= 1e-6;
        }
        else if ( !strcmp( strField, "-pw_gxdr" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gxdr.pw );
          seq.Gxdr.pw *= 1e-6;
        }
        else if ( !strcmp( strField, "-pw_gxdrd" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gxdr.pwd );
          seq.Gxdr.pwd *= 1e-6;
        }
        else if ( !strcmp( strField, "-a_gx1" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gx1.a );
          seq.Gx1.a *= 1e-2;
        }
        else if (!strcmp( strField, "-pw_gx1a" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gx1.pwa );
          seq.Gx1.pwa *= 1e-6;
        }
        else if (!strcmp( strField, "-pw_gx1" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gx1.pw );
          seq.Gx1.pw *= 1e-6;
        }
        else if (!strcmp( strField, "-pw_gx1d" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gx1.pwd );
          seq.Gx1.pwd *= 1e-6;
        }
        else if (!strcmp( strField, "-rhhnover" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%d", &seq.Gxw.rhhnover );
        }
        else if (!strcmp( strField, "-esp" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gxw.esp );
          seq.Gxw.esp *= 1e-6;
        }
        else if (!strcmp( strField, "-a_gxw" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gxw.a );
          seq.Gxw.a *= 1e-2;
        }
        else if (!strcmp( strField, "-pw_gxwad" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gxw.pwad );
          seq.Gxw.pwad *= 1e-6;
        }
        else if (!strcmp( strField, "-pw_gxw" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gxw.pw );
          seq.Gxw.pw *= 1e-6;
        }
        else if (!strcmp( strField, "-a_gydl" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gydl.a );
          seq.Gydl.a *= 1e-2;
        }
        else if (!strcmp( strField, "-pw_gydla" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gydl.pwa );
          seq.Gydl.pwa *= 1e-6;
        }
        else if (!strcmp( strField, "-pw_gydl" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gydl.pw );
          seq.Gydl.pw *= 1e-6;
        }
        else if (!strcmp( strField, "-pw_gydld" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gydl.pwd );
          seq.Gydl.pwd *= 1e-6;
        }
        else if (!strcmp( strField, "-a_gydr" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gydr.a );
          seq.Gydr.a *= 1e-2;
        }
        else if (!strcmp( strField, "-pw_gydra" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gydr.pwa );
          seq.Gydr.pwa *= 1e-6;
        }
        else if (!strcmp( strField, "-pw_gydr" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gydr.pw );
          seq.Gydr.pw *= 1e-6;
        }
        else if (!strcmp( strField, "-pw_gydrd" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gydr.pwd );
          seq.Gydr.pwd *= 1e-6;
        }
        else if (!strcmp( strField, "-a_gy1a" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gy1.aa );
          seq.Gy1.aa *= 1e-2;
        }
        else if (!strcmp( strField, "-a_gy1b" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gy1.ab );
          seq.Gy1.ab *= 1e-2;
        }
        else if (!strcmp( strField, "-pw_gy1a" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gy1.pwa );
          seq.Gy1.pwa *= 1e-6;
        }
        else if (!strcmp( strField, "-pw_gy1" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gy1.pw );
          seq.Gy1.pw *= 1e-6;
        }
        else if (!strcmp( strField, "-pw_gy1d" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gy1.pwd );
          seq.Gy1.pwd *= 1e-6;
        }
        else if (!strcmp( strField, "-a_gyb" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gyb.a );
          seq.Gyb.a *= 1e-2;
        }
        else if (!strcmp( strField, "-pw_gyba" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gyb.pwa );
          seq.Gyb.pwa *= 1e-6;
        }
        else if (!strcmp( strField, "-pw_gyb" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gyb.pw );
          seq.Gyb.pw *= 1e-6;
        }
        else if (!strcmp( strField, "-pw_gybd" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gyb.pwd );
          seq.Gyb.pwd *= 1e-6;
        }
        else if (!strcmp( strField, "-num_rf1_lobe" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%d", &seq.Gzrf1.numlobe );
        }
        else if (!strcmp( strField, "-a_gzrf1" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gzrf1.a );
          seq.Gzrf1.a *= 1e-2;
        }
        else if (!strcmp( strField, "-pw_gzrf1lobe" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gzrf1.pwlobe );
          seq.Gzrf1.pwlobe *= 1e-6;
        }
        else if (!strcmp( strField, "-pw_ss_rampz" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gzrf1.pw_ss_rampz );
          seq.Gzrf1.pw_ss_rampz *= 1e-6;
        }
        else if (!strcmp( strField, "-a_gzdl" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gzdl.a );
          seq.Gzdl.a *= 1e-2;
        }
        else if (!strcmp( strField, "-pw_gzdla" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gzdl.pwa );
          seq.Gzdl.pwa *= 1e-6;
        }
        else if (!strcmp( strField, "-pw_gzdl" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gzdl.pw );
          seq.Gzdl.pw *= 1e-6;
        }
        else if (!strcmp( strField, "-pw_gzdld" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gzdl.pwd );
          seq.Gzdl.pwd *= 1e-6;
        }
        else if (!strcmp( strField, "-a_gzrf2l1" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gzrf2l1.a );
          seq.Gzrf2l1.a *= 1e-2;
        }
        else if (!strcmp( strField, "-pw_gzrf2l1a" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gzrf2l1.pwa );
          seq.Gzrf2l1.pwa *= 1e-6;
        }
        else if (!strcmp( strField, "-pw_gzrf2l1" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gzrf2l1.pw );
          seq.Gzrf2l1.pw *= 1e-6;
        }
        else if (!strcmp( strField, "-pw_gzrf2l1d" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gzrf2l1.pwd );
          seq.Gzrf2l1.pwd *= 1e-6;
        }
        else if (!strcmp( strField, "-a_gzrf2" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gzrf2.a );
          seq.Gzrf2.a *= 1e-2;
        }
        else if (!strcmp( strField, "-pw_gzrf2a" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gzrf2.pwa );
          seq.Gzrf2.pwa *= 1e-6;
        }
        else if (!strcmp( strField, "-pw_gzrf2" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gzrf2.pw );
          seq.Gzrf2.pw *= 1e-6;
        }
        else if (!strcmp( strField, "-pw_gzrf2d" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gzrf2.pwd );
          seq.Gzrf2.pwd *= 1e-6;
        }
        else if (!strcmp( strField, "-a_gzrf2r1" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gzrf2r1.a );
          seq.Gzrf2r1.a *= 1e-2;
        }
        else if (!strcmp( strField, "-pw_gzrf2r1a" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gzrf2r1.pwa );
          seq.Gzrf2r1.pwa *= 1e-6;
        }
        else if (!strcmp( strField, "-pw_gzrf2r1" ) )
        { AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gzrf2r1.pw );
          seq.Gzrf2r1.pw *= 1e-6;
        }
        else if (!strcmp( strField, "-pw_gzrf2r1d" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gzrf2r1.pwd );
          seq.Gzrf2r1.pwd *= 1e-6;
        }
        else if (!strcmp( strField, "-a_gzdr" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gzdr.a );
          seq.Gzdr.a *= 1e-2;
        }
        else if (!strcmp( strField, "-pw_gzdra" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gzdr.pwa );
          seq.Gzdr.pwa *= 1e-6;
        }
        else if (!strcmp( strField, "-pw_gzdr" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gzdr.pw );
          seq.Gzdr.pw *= 1e-6;
        }
        else if (!strcmp( strField, "-pw_gzdrd" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Gzdr.pwd );
          seq.Gzdr.pwd *= 1e-6;
        }
        else if (!strcmp( strField, "-pw_wgxdl" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Pw_wgxdl );
          seq.Pw_wgxdl *= 1e-6;
        }
        else if (!strcmp( strField, "-pw_wgydl" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Pw_wgydl );
          seq.Pw_wgydl *= 1e-6;
        }
        else if (!strcmp( strField, "-pw_wgzdl" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Pw_wgzdl );
          seq.Pw_wgzdl *= 1e-6;
        }
        else if (!strcmp( strField, "-pw_wgxdr" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Pw_wgxdr );
          seq.Pw_wgxdr *= 1e-6;
        }
        else if (!strcmp( strField, "-pw_wgydr" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Pw_wgydr );
          seq.Pw_wgydr *= 1e-6;
        }
        else if (!strcmp( strField, "-pw_wgzdr" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Pw_wgzdr );
          seq.Pw_wgzdr *= 1e-6;
        }
        else if (!strcmp( strField, "-te" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &seq.Te );
          seq.Te *= 1e-6;
        }
        else if (!strcmp( strField, "-etl" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%d", &seq.Etl );
        }
        else if (!strcmp( strField, "-numtrial" ) )
        {
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%d", &seq.NumTrial );
          seq.Gradient = new AimsData<float>( 3, seq.NumTrial );
        }
        else if (!strcmp( strField, "-trial" ) )
        {
          ASSERT( seq.NumTrial != -1 );
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%d", &index );
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &((*seq.Gradient)( 0, index ) ) );
          (*seq.Gradient)( 0, index ) *= 1e-2;
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &((*seq.Gradient)( 1, index ) ) );
          (*seq.Gradient)( 1, index ) *= 1e-2;
          AimsScanNflush( &strc, strArg );
          sscanf( strArg, "%f", &((*seq.Gradient)( 2, index ) ) );
          (*seq.Gradient)( 2, index ) *= 1e-2;
        }
      }
      while ( strc < str + strlen( str ) );
    }

  fclose( fp ); 

  seq.NEcho = ( seq.Gxw.rhhnover ) ? seq.Gxw.rhhnover : seq.Etl / 2;

  seq.TGzrf2   = seq.Te / 2 - ( seq.Gzrf2.pw / 2 + seq.Gzrf2.pwa );
  seq.TGzrf2l1 = seq.TGzrf2 /*- crud */ -
                 ( seq.Gzrf2l1.pwa + seq.Gzrf2l1.pw + seq.Gzrf2l1.pwd );
  seq.TGxdl    = seq.TGzrf2l1 - seq.Pw_wgxdl - 
                 ( seq.Gxdl.pwa + seq.Gxdl.pw + seq.Gxdl.pwd );
  seq.TGydl    = seq.TGzrf2l1 - seq.Pw_wgydl - 
                 ( seq.Gydl.pwa + seq.Gydl.pw + seq.Gydl.pwd );
  seq.TGzdl    = seq.TGzrf2l1 - seq.Pw_wgzdl - 
                 ( seq.Gzdl.pwa + seq.Gzdl.pw + seq.Gzdl.pwd );
  seq.TGzrf2r1 = seq.Te / 2 + ( seq.Gzrf2.pw / 2 + seq.Gzrf2.pwd ) /*+ crud*/;
  seq.TGxdr    = seq.TGzrf2r1 + ( seq.Gzrf2r1.pwa + seq.Gzrf2r1.pw +
                 seq.Gzrf2r1.pwd ) + seq.Pw_wgxdr;
  seq.TGydr    = seq.TGzrf2r1 + ( seq.Gzrf2r1.pwa + seq.Gzrf2r1.pw +
                 seq.Gzrf2r1.pwd ) + seq.Pw_wgydr;
  seq.TGzdr    = seq.TGzrf2r1 + ( seq.Gzrf2r1.pwa + seq.Gzrf2r1.pw +
                 seq.Gzrf2r1.pwd ) + seq.Pw_wgzdr;
  seq.TGzrf1   = new AimsData<float>(seq.Gzrf1.numlobe/2 );
  for ( int n = 0; n < seq.TGzrf1->dimX( ); n++ )
    (*seq.TGzrf1)( n ) = (float)n * seq.Gzrf1.pwlobe;
  seq.TGx1     = seq.Te - (float)seq.NEcho * seq.Gxw.esp -
                 ( seq.Gx1.pwa + seq.Gx1.pw + seq.Gx1.pwd );
  seq.TGy1     = seq.Te - (float)seq.NEcho *
                 seq.Gxw.esp - ( seq.Gy1.pwa + seq.Gy1.pw + seq.Gy1.pwd );
  seq.TGxw     = new AimsData<float>( seq.NEcho );
  for ( int n = 0; n < seq.TGxw->dimX( ); n++ )
    (*seq.TGxw)( n ) = seq.Te - (float)( seq.NEcho - n ) * seq.Gxw.esp;
  seq.TGyb     = new AimsData<float>( seq.NEcho );
  for ( int n = 0; n < seq.TGyb->dimX( ); n++ )
    (*seq.TGyb)( n ) = seq.Te - (float)( seq.NEcho - n - 1 ) * seq.Gxw.esp -
                     ( seq.Gyb.pwa + seq.Gyb.pw + seq.Gyb.pwd ) / 2;
 
}


/*------------------------------- Gradients ----------------------------------*/


float Gx( float t )
{
  float res;
  float sgn;

  if ( bipolar == 0 )
  {
    res = Trap( t, AGxdl, SeqEPI1.TGxdl,
               SeqEPI1.Gxdl.pwa, SeqEPI1.Gxdl.pw, SeqEPI1.Gxdl.pwd ) + 
          Trap( t, AGxdr, SeqEPI1.TGxdr,
               SeqEPI1.Gxdr.pwa, SeqEPI1.Gxdr.pw, SeqEPI1.Gxdr.pwd ) + 
          Trap( t, SeqEPI1.Gx1.a, SeqEPI1.TGx1,
               SeqEPI1.Gx1.pwa, SeqEPI1.Gx1.pw, SeqEPI1.Gx1.pwd )  ;
  }
  else
  {
    float width_l = ( SeqEPI1.Gxdl.pw - SeqEPI1.Gxdl.pwa - SeqEPI1.Gxdl.pwd ) / 
                    2.0;
    float start2_l = SeqEPI1.TGxdl + SeqEPI1.Gxdl.pwa + width_l +
                                     SeqEPI1.Gxdl.pwd;

    float width_r = ( SeqEPI1.Gxdr.pw - SeqEPI1.Gxdr.pwa - SeqEPI1.Gxdr.pwd ) / 
                    2.0;
    float start2_r = SeqEPI1.TGxdr + SeqEPI1.Gxdr.pwa + width_r + 
                                     SeqEPI1.Gxdr.pwd;

    res = Trap( t,AGxdl, SeqEPI1.TGxdl,
                SeqEPI1.Gxdl.pwa,width_l, SeqEPI1.Gxdl.pwd ) +
          Trap( t, -AGxdl, start2_l,
                SeqEPI1.Gxdl.pwa, width_l, SeqEPI1.Gxdl.pwd ) +
          Trap( t, AGxdr, SeqEPI1.TGxdr,
                SeqEPI1.Gxdr.pwa,width_r, SeqEPI1.Gxdr.pwd ) +
          Trap( t, -AGxdr,start2_r,
                SeqEPI1.Gxdr.pwa, width_r, SeqEPI1.Gxdr.pwd ) +
          Trap( t, SeqEPI1.Gx1.a, SeqEPI1.TGx1,
                SeqEPI1.Gx1.pwa, SeqEPI1.Gx1.pw, SeqEPI1.Gx1.pwd )  ;
  }

  sgn = ( SeqEPI1.Gx1.a < 0 ) ? -1 : 1;

  for ( int n = 0; n < SeqEPI1.TGxw->dimX( ); n++ )
    res += Trap( t, ( n % 2 ) ? sgn * SeqEPI1.Gxw.a : 
                               -sgn * SeqEPI1.Gxw.a, (*SeqEPI1.TGxw)( n ),
                 SeqEPI1.Gxw.pwad, SeqEPI1.Gxw.pw, SeqEPI1.Gxw.pwad );

  return res;
}


float Gy( float t )
{
  float res;

  if ( bipolar == 0 )
  {
    res = Trap( t, AGydl, SeqEPI1.TGydl,
                SeqEPI1.Gydl.pwa, SeqEPI1.Gydl.pw, SeqEPI1.Gydl.pwd ) + 
          Trap( t, AGydr, SeqEPI1.TGydr,
                SeqEPI1.Gydr.pwa, SeqEPI1.Gydr.pw, SeqEPI1.Gydr.pwd ) + 
          Quad( t, SeqEPI1.Gy1.aa, SeqEPI1.Gy1.ab, SeqEPI1.TGy1,
                SeqEPI1.Gy1.pwa, SeqEPI1.Gy1.pw, SeqEPI1.Gy1.pwd );
  }
  else
  {
    float width_l = ( SeqEPI1.Gydl.pw - SeqEPI1.Gydl.pwa - SeqEPI1.Gydl.pwd ) / 
                    2.0;
    float start2_l = SeqEPI1.TGydl + SeqEPI1.Gydl.pwa + width_l +
                                     SeqEPI1.Gydl.pwd;

    float width_r = ( SeqEPI1.Gydr.pw - SeqEPI1.Gydr.pwa - SeqEPI1.Gydr.pwd ) / 
                    2.0;
    float start2_r = SeqEPI1.TGydr + SeqEPI1.Gydr.pwa + width_r + 
                                     SeqEPI1.Gydr.pwd;


    res = Trap( t, AGydl, SeqEPI1.TGydl,
                SeqEPI1.Gydl.pwa, width_l, SeqEPI1.Gydl.pwd ) +
          Trap( t, -AGydl, start2_l,
                SeqEPI1.Gydl.pwa, width_l, SeqEPI1.Gydl.pwd ) +
          Trap( t, AGydr, SeqEPI1.TGydr,
                SeqEPI1.Gydr.pwa, width_r, SeqEPI1.Gydr.pwd ) +
          Trap( t, -AGydr, start2_r,
                SeqEPI1.Gydr.pwa, width_r, SeqEPI1.Gydr.pwd ) +
          Quad( t, SeqEPI1.Gy1.aa, SeqEPI1.Gy1.ab, SeqEPI1.TGy1,
                SeqEPI1.Gy1.pwa, SeqEPI1.Gy1.pw, SeqEPI1.Gy1.pwd );
  }

  for ( int n = 0; n < SeqEPI1.TGyb->dimX( ); n++ )
    res += Trap( t, SeqEPI1.Gyb.a, (*SeqEPI1.TGyb)( n ),
                 SeqEPI1.Gyb.pwa, SeqEPI1.Gyb.pw, SeqEPI1.Gyb.pwd );

  return res;
}


float Gz( float t )
{
  float res;

  if ( bipolar == 0 )
  {
    res = Trap( t, AGzdl, SeqEPI1.TGzdl,
                SeqEPI1.Gzdl.pwa, SeqEPI1.Gzdl.pw, SeqEPI1.Gzdl.pwd ) + 
          Trap( t, AGzdr, SeqEPI1.TGzdr,
                SeqEPI1.Gzdr.pwa, SeqEPI1.Gzdr.pw, SeqEPI1.Gzdr.pwd ) + 
          Trap( t, SeqEPI1.Gzrf2l1.a, SeqEPI1.TGzrf2l1,
                SeqEPI1.Gzrf2l1.pwa, SeqEPI1.Gzrf2l1.pw, SeqEPI1.Gzrf2l1.pwd ) +
          Trap( t, SeqEPI1.Gzrf2r1.a, SeqEPI1.TGzrf2r1,
                SeqEPI1.Gzrf2r1.pwa, SeqEPI1.Gzrf2r1.pw, SeqEPI1.Gzrf2r1.pwd ) + 
          Trap( t, SeqEPI1.Gzrf2.a, SeqEPI1.TGzrf2,
                SeqEPI1.Gzrf2.pwa, SeqEPI1.Gzrf2.pw, SeqEPI1.Gzrf2.pwd ); 
  }
  else
  {
    float width_l = ( SeqEPI1.Gzdl.pw - SeqEPI1.Gzdl.pwa - SeqEPI1.Gzdl.pwd ) / 
                    2.0;
    float start2_l = SeqEPI1.TGzdl + SeqEPI1.Gzdl.pwa + width_l +
                                     SeqEPI1.Gzdl.pwd;

    float width_r = ( SeqEPI1.Gzdr.pw - SeqEPI1.Gzdr.pwa - SeqEPI1.Gzdr.pwd ) / 
                    2.0;
    float start2_r = SeqEPI1.TGzdr + SeqEPI1.Gzdr.pwa + width_r + 
                                     SeqEPI1.Gzdr.pwd;


    res = Trap( t, AGzdl, SeqEPI1.TGzdl,
                SeqEPI1.Gzdl.pwa, width_l, SeqEPI1.Gzdl.pwd ) +
          Trap( t, -AGzdl, start2_l,
                SeqEPI1.Gzdl.pwa, width_l, SeqEPI1.Gzdl.pwd ) +
          Trap( t, AGzdr, SeqEPI1.TGzdr,
                SeqEPI1.Gzdr.pwa, width_r, SeqEPI1.Gzdr.pwd ) +
          Trap( t, -AGzdr, start2_r,
                SeqEPI1.Gzdr.pwa, width_r, SeqEPI1.Gzdr.pwd ) +
          Trap( t, SeqEPI1.Gzrf2l1.a, SeqEPI1.TGzrf2l1,
                SeqEPI1.Gzrf2l1.pwa, SeqEPI1.Gzrf2l1.pw, SeqEPI1.Gzrf2l1.pwd ) +
          Trap( t, SeqEPI1.Gzrf2r1.a, SeqEPI1.TGzrf2r1,
                SeqEPI1.Gzrf2r1.pwa, SeqEPI1.Gzrf2r1.pw, SeqEPI1.Gzrf2r1.pwd ) +
          Trap( t, SeqEPI1.Gzrf2.a, SeqEPI1.TGzrf2,
                SeqEPI1.Gzrf2.pwa, SeqEPI1.Gzrf2.pw, SeqEPI1.Gzrf2.pwd ); 
  }

  for ( int n = 0; n < SeqEPI1.TGzrf1->dimX( ); n++ )
    res += Trap( t, ( n % 2 ) ? +SeqEPI1.Gzrf1.a : -SeqEPI1.Gzrf1.a,
                 (*SeqEPI1.TGzrf1)( n ), SeqEPI1.Gzrf1.pw_ss_rampz,
                 SeqEPI1.Gzrf1.pwlobe - 2 * SeqEPI1.Gzrf1.pw_ss_rampz,
                 SeqEPI1.Gzrf1.pw_ss_rampz );

  return res;
}




/*-------------------------- Fonctions integrales ----------------------------*/

float Fx( float t )
{
  float res;
  float sgn;

  if ( bipolar == 0 )
  {
    res = TrapItg( t, AGxdl, SeqEPI1.TGxdl,
                   SeqEPI1.Gxdl.pwa, SeqEPI1.Gxdl.pw, SeqEPI1.Gxdl.pwd ) + 
          TrapItg( t, AGxdr, SeqEPI1.TGxdr,
                   SeqEPI1.Gxdr.pwa, SeqEPI1.Gxdr.pw, SeqEPI1.Gxdr.pwd ) + 
          TrapItg( t, SeqEPI1.Gx1.a, SeqEPI1.TGx1,
                   SeqEPI1.Gx1.pwa, SeqEPI1.Gx1.pw, SeqEPI1.Gx1.pwd );
  }
  else
  {
    float width_l = ( SeqEPI1.Gxdl.pw - SeqEPI1.Gxdl.pwa - SeqEPI1.Gxdl.pwd ) / 
                    2.0;
    float start2_l = SeqEPI1.TGxdl + SeqEPI1.Gxdl.pwa + width_l +
                                     SeqEPI1.Gxdl.pwd;

    float width_r = ( SeqEPI1.Gxdr.pw - SeqEPI1.Gxdr.pwa - SeqEPI1.Gxdr.pwd ) / 
                    2.0;
    float start2_r = SeqEPI1.TGxdr + SeqEPI1.Gxdr.pwa + width_r + 
                                     SeqEPI1.Gxdr.pwd;

    res = TrapItg( t, AGxdl, SeqEPI1.TGxdl,
                   SeqEPI1.Gxdl.pwa, width_l, SeqEPI1.Gxdl.pwd ) +
          TrapItg( t, -AGxdl, start2_l,
                   SeqEPI1.Gxdl.pwa, width_l, SeqEPI1.Gxdl.pwd ) +
          TrapItg( t, AGxdr, SeqEPI1.TGxdr,
                   SeqEPI1.Gxdr.pwa, width_r, SeqEPI1.Gxdr.pwd ) +
          TrapItg( t, -AGxdr,start2_r,
                   SeqEPI1.Gxdr.pwa, width_r, SeqEPI1.Gxdr.pwd ) +
          TrapItg( t, SeqEPI1.Gx1.a, SeqEPI1.TGx1,
                   SeqEPI1.Gx1.pwa, SeqEPI1.Gx1.pw, SeqEPI1.Gx1.pwd );
  }

  sgn = ( SeqEPI1.Gx1.a < 0) ? -1 : 1;

  for ( int n = 0; n < SeqEPI1.TGxw->dimX( ); n++ )
    res += TrapItg( t, ( n % 2 ) ? sgn * SeqEPI1.Gxw.a : 
                                  -sgn * SeqEPI1.Gxw.a, (*SeqEPI1.TGxw)( n ),
                SeqEPI1.Gxw.pwad, SeqEPI1.Gxw.pw, SeqEPI1.Gxw.pwad );

  return res;
}


float Fy( float t )
{
  float res;

  if ( bipolar == 0 )
  {
    res = TrapItg( t, AGydl, SeqEPI1.TGydl,
                   SeqEPI1.Gydl.pwa, SeqEPI1.Gydl.pw, SeqEPI1.Gydl.pwd ) + 
          TrapItg( t, AGydr, SeqEPI1.TGydr,
                  SeqEPI1.Gydr.pwa, SeqEPI1.Gydr.pw, SeqEPI1.Gydr.pwd ) + 
          QuadItg( t, SeqEPI1.Gy1.aa,-SeqEPI1.Gy1.ab, SeqEPI1.TGy1,
                  SeqEPI1.Gy1.pwa, SeqEPI1.Gy1.pw, SeqEPI1.Gy1.pwd );
  }
  else
  { float width_l = ( SeqEPI1.Gydl.pw - SeqEPI1.Gydl.pwa - SeqEPI1.Gydl.pwd ) / 
                    2.0;
    float start2_l = SeqEPI1.TGydl + SeqEPI1.Gydl.pwa + width_l +
                                     SeqEPI1.Gydl.pwd;

    float width_r = ( SeqEPI1.Gydr.pw - SeqEPI1.Gydr.pwa - SeqEPI1.Gydr.pwd ) / 
                    2.0;
    float start2_r = SeqEPI1.TGydr + SeqEPI1.Gydr.pwa + width_r + 
                                     SeqEPI1.Gydr.pwd;


    res = TrapItg( t,AGydl, SeqEPI1.TGydl,
                   SeqEPI1.Gydl.pwa,width_l, SeqEPI1.Gydl.pwd ) +
          TrapItg( t,-AGydl,start2_l,
                   SeqEPI1.Gydl.pwa,width_l, SeqEPI1.Gydl.pwd ) +
          TrapItg( t,AGydr, SeqEPI1.TGydr,
                   SeqEPI1.Gydr.pwa,width_r, SeqEPI1.Gydr.pwd ) +
          TrapItg( t,-AGydr,start2_r,
                   SeqEPI1.Gydr.pwa,width_r, SeqEPI1.Gydr.pwd ) +
          QuadItg( t, SeqEPI1.Gy1.aa,-SeqEPI1.Gy1.ab, SeqEPI1.TGy1,
                   SeqEPI1.Gy1.pwa, SeqEPI1.Gy1.pw, SeqEPI1.Gy1.pwd );
  }

 
  for ( int n = 0; n < SeqEPI1.TGyb->dimX( ); n++ )
    res += TrapItg( t, SeqEPI1.Gyb.a,(*SeqEPI1.TGyb)( n ),
                    SeqEPI1.Gyb.pwa, SeqEPI1.Gyb.pw, SeqEPI1.Gyb.pwd );

  return res;
}


float Fz( float t )
{
  float res;

  if ( bipolar == 0 )
  {
    res = TrapItg( t,AGzdl, SeqEPI1.TGzdl,
                   SeqEPI1.Gzdl.pwa, SeqEPI1.Gzdl.pw, SeqEPI1.Gzdl.pwd ) + 
          TrapItg( t,AGzdr, SeqEPI1.TGzdr,
                   SeqEPI1.Gzdr.pwa, SeqEPI1.Gzdr.pw, SeqEPI1.Gzdr.pwd ) +
          TrapItg( t, SeqEPI1.Gzrf2l1.a, SeqEPI1.TGzrf2l1,
                   SeqEPI1.Gzrf2l1.pwa, SeqEPI1.Gzrf2l1.pw, 
                   SeqEPI1.Gzrf2l1.pwd ) + 
          TrapItg( t, SeqEPI1.Gzrf2r1.a, SeqEPI1.TGzrf2r1,
                   SeqEPI1.Gzrf2r1.pwa, SeqEPI1.Gzrf2r1.pw,
                   SeqEPI1.Gzrf2r1.pwd ) + 
          TrapItg( t, SeqEPI1.Gzrf2.a, SeqEPI1.TGzrf2,
                   SeqEPI1.Gzrf2.pwa, SeqEPI1.Gzrf2.pw, SeqEPI1.Gzrf2.pwd ) ; 
  }
  else
  {
    float width_l = ( SeqEPI1.Gzdl.pw - SeqEPI1.Gzdl.pwa - SeqEPI1.Gzdl.pwd ) / 
                    2.0;
    float start2_l = SeqEPI1.TGzdl + SeqEPI1.Gzdl.pwa + width_l +
                                     SeqEPI1.Gzdl.pwd;

    float width_r = ( SeqEPI1.Gzdr.pw - SeqEPI1.Gzdr.pwa - SeqEPI1.Gzdr.pwd ) / 
                    2.0;
    float start2_r = SeqEPI1.TGzdr + SeqEPI1.Gzdr.pwa + width_r + 
                                     SeqEPI1.Gzdr.pwd;

    res = TrapItg( t,AGzdl, SeqEPI1.TGzdl,
                   SeqEPI1.Gzdl.pwa,width_l, SeqEPI1.Gzdl.pwd ) +
          TrapItg( t,-AGzdl,start2_l,
                   SeqEPI1.Gzdl.pwa,width_l, SeqEPI1.Gzdl.pwd ) +
          TrapItg( t,AGzdr, SeqEPI1.TGzdr,
                   SeqEPI1.Gzdr.pwa,width_r, SeqEPI1.Gzdr.pwd ) +
          TrapItg( t,-AGzdr,start2_r,
                   SeqEPI1.Gzdr.pwa,width_r, SeqEPI1.Gzdr.pwd ) +
          TrapItg( t, SeqEPI1.Gzrf2l1.a, SeqEPI1.TGzrf2l1,
                   SeqEPI1.Gzrf2l1.pwa, SeqEPI1.Gzrf2l1.pw,
                   SeqEPI1.Gzrf2l1.pwd ) + 
          TrapItg( t, SeqEPI1.Gzrf2r1.a, SeqEPI1.TGzrf2r1,
                   SeqEPI1.Gzrf2r1.pwa, SeqEPI1.Gzrf2r1.pw,
                   SeqEPI1.Gzrf2r1.pwd ) + 
          TrapItg( t, SeqEPI1.Gzrf2.a, SeqEPI1.TGzrf2,
                   SeqEPI1.Gzrf2.pwa, SeqEPI1.Gzrf2.pw, SeqEPI1.Gzrf2.pwd ) ; 
  }

  for ( int n = 0; n < SeqEPI1.TGzrf1->dimX( ); n++ )
    res += TrapItg( t, ( n % 2 ) ? +SeqEPI1.Gzrf1.a : -SeqEPI1.Gzrf1.a,
                   (*SeqEPI1.TGzrf1)( n ), SeqEPI1.Gzrf1.pw_ss_rampz,
                   SeqEPI1.Gzrf1.pwlobe - 2 * SeqEPI1.Gzrf1.pw_ss_rampz,
                   SeqEPI1.Gzrf1.pw_ss_rampz );

  return res;
}


/*------------------ Produits de fonctions integrales ------------------------*/
float Product_xx( float t )
{ 
  return Fx( t ) * Fx( t );
}


float Product_yy( float t )
{
  return Fy( t ) * Fy( t );
}


float Product_zz( float t )
{
  return Fz( t ) * Fz( t );
}


float Product_xy( float t )
{
  return Fx( t ) * Fy( t );
}


float Product_xz( float t )
{
  return Fx( t ) * Fz( t );
}


float Product_yz( float t )
{
  return Fy( t ) * Fz( t );
}


/*--------------------- Calcul des champs croises ----------------------------*/
float CrossedMagneticField( float (*fi)( float ),
                            float (*fj)( float ),
                            float (*product_ij)( float ) )
{
  float result;

  result = AimsRombergIntegration( product_ij, 0, SeqEPI1.Te, EPS, JMAX, K )
           - 2 * (*fi)( SeqEPI1.Te / 2 ) * 
           AimsRombergIntegration( fj, SeqEPI1.Te/2, SeqEPI1.Te, EPS, JMAX, K )
           - 2 * (*fj)( SeqEPI1.Te / 2 ) * 
           AimsRombergIntegration( fi, SeqEPI1.Te/2, SeqEPI1.Te, EPS, JMAX, K )
           + 2 * (*fi)( SeqEPI1.Te / 2 ) * (*fj)( SeqEPI1.Te / 2 ) * SeqEPI1.Te;

  return result;
}


AimsData< AimsVector<float,6> > MagneticField( float gamma )
{
  AimsData< AimsVector<float,6> > B( SeqEPI1.NumTrial );

  cout << "\nTrial : " << setw(3) << 0 << flush;
  for (int n = 0; n < B.dimX( ); n++ )
  {
    cout << "\b\b\b" << setw(3) << n+1 << flush;
    AGxdl = AGxdr = (*SeqEPI1.Gradient)( 0, n );
    AGydl = AGydr = (*SeqEPI1.Gradient)( 1, n );
    AGzdl = AGzdr = (*SeqEPI1.Gradient)( 2, n );
    B( n ).item( 0 ) = sqr( gamma ) *
                       CrossedMagneticField( &Fx, &Fx, &Product_xx );
    B( n ).item( 1 ) = sqr( gamma ) *
                       CrossedMagneticField( &Fx, &Fy, &Product_xy );
    B( n ).item( 2 ) = sqr( gamma ) *
                       CrossedMagneticField( &Fx, &Fz, &Product_xz );
    B( n ).item( 3 ) = sqr( gamma ) *
                       CrossedMagneticField( &Fy, &Fy, &Product_yy );
    B( n ).item( 4 ) = sqr( gamma ) *
                       CrossedMagneticField( &Fy, &Fz, &Product_yz );
    B( n ).item( 5 ) = sqr( gamma ) *
                       CrossedMagneticField( &Fz, &Fz, &Product_zz );
  }
  cout << endl;

  return B;
}


int main(int argc, const char **argv)
{
  string	filein, fileout;
  int		grad=0;
  float		gamma = 267475199;

  AimsApplication	app( argc, argv, "B matrices for diffusion tensor " 
                             "sequence parameters" );
  app.addOption( filein, "-i", "source EPI sequence" );
  app.addOption( fileout, "-o", "output volume name" );
  app.addOption( EPS, "-e", "integration relative error [1e-5]", true );
  app.addOption( JMAX, "-j", "maximum iteration number during integration " 
                 "[22]", true );
  app.addOption( K, "-k", "interpolation order for Romberg integration [10]", 
                 true );
  app.addOption( grad, "-a", "save gradients gx,gy,gz and their integrals", 
                 true );
  app.addOption( gamma, "-g", "gamma diffusion coefficient for protons " 
                 "[default=42,57e6 . 2 Pi]", true );
  app.addOption( bipolar, "-b", "bipolar gradients [default=no]", true );
  app.alias( "--input", "-i" );
  app.alias( "--output", "-o" );
  app.alias( "--eps", "-e" );
  app.alias( "--jmax", "-j" );
  app.alias( "--agrad", "-a" );
  app.alias( "--gamma", "-g" );
  app.alias( "--bipolar", "-b" );

  try
    {
      app.initialize();

      cout << "bipolar=" << bipolar << endl;

      AimsData< AimsVector<float,6> > B;

      ReadEPI1( filein, SeqEPI1 );
      cout << "parameters read" << endl;

      if ( grad )
        {
          AGxdl = AGxdr = 0.022;
          AGydl = AGydr = 0.022;
          AGzdl = AGzdr = 0.022;

          ofstream ofstx( "seq.x" );
          for ( float t = 0; t <= SeqEPI1.Te; t += SeqEPI1.Te / 1000 ) 
            ofstx << t << "\t" << Gx( t ) << endl;
          ofstx.close( );

          ofstream ofsty( "seq.y" );
          for ( float t = 0; t <= SeqEPI1.Te; t += SeqEPI1.Te / 1000 ) 
            ofsty << t << "\t" << Gy( t ) << endl;
          ofsty.close( );

          ofstream ofstz( "seq.z" );
          for ( float t = 0; t <= SeqEPI1.Te; t += SeqEPI1.Te / 1000 ) 
            ofstz << t << "\t" << Gz( t ) << endl;
          ofstz.close( );

          ofstream ofstxyz( "seq.xyz" );
          for ( float t = 0; t <= SeqEPI1.Te; t += SeqEPI1.Te / 1000 ) 
            ofstxyz << t << "\t" << Gx( t ) << endl;
          ofstxyz << endl;
          for ( float t = 0; t <= SeqEPI1.Te; t += SeqEPI1.Te / 1000 ) 
            ofstxyz << t << "\t" << Gy( t ) << endl;
          ofstxyz << endl;
          for ( float t = 0; t <= SeqEPI1.Te; t += SeqEPI1.Te / 1000 ) 
            ofstxyz << t << "\t" << Gz( t ) << endl;
          ofstxyz << endl;
          ofstxyz.close( );

          cout << "processing Fx\n";
          ofstream ofstfx( "seq.fx" );
          for ( float t = 0; t <= SeqEPI1.Te; t += SeqEPI1.Te / 1000 ) 
            ofstfx << t << "\t" << Fx( t ) << endl;
          ofstfx.close( );

          cout << "processing Fy\n";
          ofstream ofstfy( "seq.fy" );
          for ( float t = 0; t <= SeqEPI1.Te; t += SeqEPI1.Te / 1000 ) 
            ofstfy << t << "\t" << Fy( t ) << endl;
          ofstfy.close( );

          cout << "processing Fz\n";
          ofstream ofstfz("seq.fz" );
          for (float t=0;t<=SeqEPI1.Te;t+=SeqEPI1.Te/1000) 
            ofstfz << t << "\t" << Fz( t ) << endl;
          ofstfz.close( );
        }

      B = MagneticField(gamma );

      string tmp = string( fileout ) + ".par";
      ofstream ofst( tmp.c_str() );
      ofst << SeqEPI1;
      for ( int k = 0; k < B.dimX(); k++ )
        ofst << B( k ) << endl;
      ofst.close( );

      Writer< AimsData< AimsVector<float,6> > >	writer( fileout );
      writer << B;

      delete SeqEPI1.Gradient;
      delete SeqEPI1.TGzrf1;
      delete SeqEPI1.TGxw;
      delete SeqEPI1.TGyb;

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
