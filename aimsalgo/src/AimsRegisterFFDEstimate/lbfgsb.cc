/* Copyright (C) 2000-2013 CEA
 *
 * This software and supporting documentation were developed by
 *     bioPICSEL
 *     CEA/DSV/I²BM/MIRCen/LMN, Batiment 61,
 *     18, route du Panorama
 *     92265 Fontenay-aux-Roses
 *     France
 */


#include <aims/data/data_g.h>
#include <cartobase/exception/assert.h>
#include "lbfgsb.h"

using namespace std;

LbfgsbOptimizer::LbfgsbOptimizer()
    : _wa(  new DOUBLE_PRECISION[2*nmax*mmax+4*nmax+12*mmax*mmax+12*mmax] ),
      _iwa( new INTEGER[3*nmax] )
{
}


LbfgsbOptimizer::~LbfgsbOptimizer()
{
}

void LbfgsbOptimizer::init()
{
  strcpy(_task, "START");
  _iprint = -1;
}

int  LbfgsbOptimizer::status()
{

  cout << "Status " <<  _task << endl;

  return( 1 );
}

bool LbfgsbOptimizer::updateAvailable()
{
  return( (strncmp(_task, "NEW_X",5 ) == 0) ||
	  (strncmp(_task, "FG_START",8 ) == 0)    );
}

bool LbfgsbOptimizer::cont()
{
  return( (strncmp(_task, "START",5) ==0) ||
          (strncmp(_task, "FG",2)    ==0) ||
          (strncmp(_task, "NEW_X",5)==0) );
}

vector< float>  LbfgsbOptimizer::getMinDir(vector<float>& p, float val, vector<float>& der, vector<float>& low, vector<float>& upp)
{
  ASSERT( p.size() == der.size() );
  ASSERT( p.size() == low.size() );
  ASSERT( p.size() == upp.size() );

  DOUBLE_PRECISION F, L[der.size()], U[der.size()], G[der.size()], X[der.size()];
  INTEGER NBD[der.size()+1];
  INTEGER N = der.size();
  INTEGER M = mmax;

  F = val;
  for(unsigned int i = 0; i <der.size(); i++)
    {
      L[i] = low[i];
      U[i] = upp[i];
//       L[i] = -1;
//       U[i] = 1;

      G[i] = der[i];
      X[i] = p[i];
      NBD[i] = 2;
    }

//   cout << "DBG> LBFGSB\nLower\t " << L[3] << "\t" << L[4] << "\t" << L[5] <<
//     "\nUpper\t" << U[3] << "\t" << U[4] << "\t" << U[5] <<
//     "\nParam\t" << X[3] << "\t" << X[4] << "\t" << X[5] <<  "vfvfvf " << G[3] << "VFVFVF"<< endl;

  {
    short l60 = 60;
    CHARACTER TASK(_task, 60);
    CHARACTER CSAVE(_csave, 60);
    lbfgsbwrapper_(N, M, X, F, G, L, U, NBD,
		 _wa, _iwa,
		 _task,
		 _iprint,
		 _csave,
		 _lsave,
		 _isave,
		 _dsave, l60,l60);
    // A la destruction de TASK ET CSAVE _task et _csave sont raffraichis
  }

  //  cout << "|";
  for(unsigned int i = 0; i < der.size(); i++)
    {
      p[i] = X[i];
      //  cout << p[i] << " ";
    }
  //cout << "|" << endl;

  return( p );
}
