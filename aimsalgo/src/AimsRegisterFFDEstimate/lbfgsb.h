/* Copyright (C) 2000-2013 CEA
 *
 * This software and supporting documentation were developed by
 *     bioPICSEL
 *     CEA/DSV/I²BM/MIRCen/LMN, Batiment 61,
 *     18, route du Panorama
 *     92265 Fontenay-aux-Roses
 *     France
 */


#ifndef _AIMSLGBFS_H_
#define _AIMSLGBFS_H_

#define  F77_STUB_REQUIRED
#include <vector> 
#include "fortran.h"


#define LBFGSB_START 0 
#define LBFGSB_FG    1
#define LBFGSB_FG_ST 11
#define LBFGSB_FG_LN 12
#define LBFGSB_NEW_X 2 
#define LBFGSB_CONV  3
#define LBFGSB_ABNO  4
#define LBFGSB_ERROR 5


SUBROUTINE_F77 lbfgsbwrapper_( INTEGER& N, INTEGER& M,
			       DOUBLE_PRECISION* X,
			       DOUBLE_PRECISION& F, DOUBLE_PRECISION* G,
			       DOUBLE_PRECISION* L, DOUBLE_PRECISION* U,
			       INTEGER* NBD,
			       DOUBLE_PRECISION* WA, INTEGER* IWA,
			       char* TASK,
			       INTEGER& IPRINT,
			       char* csave,
			       LOGICAL* lsave,
			       INTEGER* isave,
			       DOUBLE_PRECISION* dsave,
			       short& tlen, short& csavlen); 


/* extern "C" */
/* { */
/* int lbfgsbwrapper_(int N, int M, */
/* 		   double F, double* G, */
/* 		   double* L, double* U, */
/* 		   int* NBD, */
/* 		   double* WA, int* IWA, */
/* 		   int TASK); */
/* } */

/* const int nmax = 1024; */
const int nmax = 3001;    // 10*10*10 * 3
const int mmax = 17; 


class LbfgsbOptimizer
{
 public: 


               LbfgsbOptimizer();
     virtual  ~LbfgsbOptimizer();
     void      init();
     bool      cont();
     bool      updateAvailable();
     int       status();
     std::vector< float > getMinDir(std::vector<float>& x, 
				    float val, 
				    std::vector<float>& der, 
				    std::vector<float>& low, 
				    std::vector<float>& upp);

/*      vector< float > getMinDir(vector<float>& x, float val, vector<float>& der); */

 private:
     DOUBLE_PRECISION  *_wa;
     INTEGER           *_iwa;
     char            _task[60];
     INTEGER         _iprint;
     char            _csave[60];
     LOGICAL         _lsave[4];
     INTEGER         _isave[44];
     DOUBLE_PRECISION      _dsave[44];

};



#endif 
