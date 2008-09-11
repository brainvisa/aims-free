/* Copyright (c) 1995-2005 CEA
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


#include <stdio.h>
#include <stdarg.h>
#include <vidaIO/kernel/vidaio.h>
#include <string.h>

static consigne VIDA_PRIV_CONS; 
/**********************************************/
/* Espace reserve en dur lors de la generation*/
/* de la librairie.  Le nom de cette variable */
/* est prive. La zone memoire est rendu a     */
/* l'appelant qui n'en garde que son adresse  */
/**********************************************/


/*----------------------------------------------------------------------------
static int TestSize() : Fonction privee ancillaire.
   Cette fonction verifie certaines tailles en cas d'ecriture. D'autres
   verrifications seraient souhaitables...


   ENTREES :
   consigne cons : la consigne
  
   SORTIES :
   valeur rendue par la fonction : 
       0 : si pas d'erreur
       1 : Sinon
      

----------------------------------------------------------------------------*/

static int TestSize(consigne cons)
{ 
  switch(cons.mode[0])
    { 
    case 'w': 
      if ( cons.size_x == 0 || cons.size_y == 0 ||
	   cons.size_z == 0 || cons.size_t == 0   )
	{ 
	  fprintf(stderr, "TestSize: parametre de dimension de l'image nul\n");
	  return(1);
	}
      break;
    default : break;
    }
  return(0);
}

/*----------------------------------------------------------------------------
static int TestType() : Fonction privee ancillaire.
   Cette fonction verifie certaines tailles en cas d'ecriture. D'autres
   verrifications seraient souhaitables...


   ENTREES :
   consigne cons : la consigne
  
   SORTIES :
   valeur rendue par la fonction : 
       0 : si pas d'erreur
       1 : Sinon
----------------------------------------------------------------------------*/

static int TestType(consigne cons)
{ 
  if (cons.type & MSK_INT_FX)
    { 
      if (cons.scale_shift == 0)
	{ 
	  fprintf(stderr, "TestType: decalage bit a bit nul\n");
	  return(1);
	}
    }
  return(0);
}

/*----------------------------------------------------------------------------
consigne *VidaConsigne(va_list) : fonction publique du package libvidaio
  
   Cette fonction transforma la liste d'argument variable terminee par NULL 
   Elle ne rend pas de satus d'erreur. Elle n'alloue pas la variable 
   VIDA_PRIV_CONS qui est alloue en dur. Ce dernier point pourrait etre 
   modifie.

   ENTREES :
   va_list Liste d'argument variable : Attention la syntaxe ici est du type
   K&R. Il faut ici obligatoirement compiler en cc -Xs. Un syntaxe particuliere
   devrait etre developpee pour compiler en ansi c

   SORTIES :
   La fonction rend la valeur une adresse sur unstructure de type consigne
   Lors de sortie en erreur elle rend un NULL. Il faudrait peut etre plutot
   que d'utiliser du static faire une alloc memeoire...
----------------------------------------------------------------------------*/

consigne *VidaConsigne( int n, ... )
{ 
  va_list ap;
  int     param = n;
 
  va_start( ap, n );
  
  VIDA_PRIV_CONS.mode[0]     = 0;
  VIDA_PRIV_CONS.scale_shift = 0.0;
  VIDA_PRIV_CONS.size_x      = 0.0;
  VIDA_PRIV_CONS.size_y      = 0.0;
  VIDA_PRIV_CONS.size_z      = 0.0;
  VIDA_PRIV_CONS.size_t      = 0.0;
  VIDA_PRIV_CONS.psize_x     = 1.0;
  VIDA_PRIV_CONS.psize_y     = 1.0;
  VIDA_PRIV_CONS.psize_z     = 1.0;
  VIDA_PRIV_CONS.unite       = VIDA_NOCALIB;

  while( param != 0 )
    { 
      switch( param )
	{ 
	case VMODE        : 
	  strcpy(VIDA_PRIV_CONS.mode, va_arg(ap,char *));
	  break;
	case VMDTYPE      : 
	  VIDA_PRIV_CONS.type        = (int)  va_arg(ap,int);
	  break;
	case VSIZE_X      : 
	  VIDA_PRIV_CONS.size_x      = (int)  va_arg(ap,int);
	  break;
	case VSIZE_Y      : 
	  VIDA_PRIV_CONS.size_y      = (int)  va_arg(ap,int);
	  break;
	case VSIZE_Z      : 
	  VIDA_PRIV_CONS.size_z      = (int)  va_arg(ap,int);
	  break;
	case VSIZE_T      : 
	  VIDA_PRIV_CONS.size_t      = (int)  va_arg(ap,int);
	  break;
	case VSCALE_SHIFT : 
	  VIDA_PRIV_CONS.scale_shift = (int)  va_arg(ap,int);
	  break;
	case VSIZE_PX     : 
	  VIDA_PRIV_CONS.psize_x     = (float)va_arg(ap,double);
	  break;
	case VSIZE_PY     : 
	  VIDA_PRIV_CONS.psize_y     = (float)va_arg(ap,double);
	  break;
	case VSIZE_PZ     : 
	  VIDA_PRIV_CONS.psize_z     = (float)va_arg(ap,double);
	  break;
	case VSIZE_PT     : 
	  VIDA_PRIV_CONS.psize_t     = (float)va_arg(ap,double);
	  break;
	case VUNITE       : 
	  VIDA_PRIV_CONS.unite       = (int)  va_arg(ap,int);
	  break;
	default           : 
	  fprintf(stderr, "VidaConsigne: parametre inconnu %d\n", param);
	  return((consigne *) NULL);
	  break;
	}
    
      param = (int)va_arg( ap, int );
    }
  
  if ( TestSize(VIDA_PRIV_CONS) || TestType(VIDA_PRIV_CONS) )
    { 
      fprintf(stderr, "VidaConsigne: consigne erronee\n");
      return((consigne *) NULL);
    }
  
  va_end( ap );
  
  return &VIDA_PRIV_CONS;
}
