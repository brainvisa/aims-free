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


#include <stdio.h>
#include <vidaIO/kernel/vidaio.h>

/*----------------------------------------------------------------------------
        This function do the conversion SUN -> DEC or
                                        DEC -> SUN for the short int
----------------------------------------------------------------------------*/

static void ShortByteSwapp(short *val,int size)
{ 
  register int i;
  union { 
    char  octet[2];
    short entier;
  } v2s,resul;

  for(i=0;i<size;i+=2,val++)        /* size is the number of BTYTES !!!*/
    { 
      v2s.entier = *val;
      resul.octet[0] = v2s.octet[1];
      resul.octet[1] = v2s.octet[0];
      *val = resul.entier;
    }
}

/*----------------------------------------------------------------------------
int PuWrite(char *buff, int size, int nitems)   

   Cette fonction fait l'ecriture des donnees en aveugle en fonction des 
   information faite par la fonction appelante. I.e elle ecrit des bytes.
   Elle a besoin de ana_db pour connaitre le type de processeur ecrivain.
   Bien sur nous n'avons plus de Digital avec leur format byte reverse mais
   nous possedons toujours des images qui ont un jour ecrite en byte reverse.

   
   C'est pourquoi nous testons la valeur de ana_db->key.hkey_un0  contre les
   constantes : PROCESSOR_MIPS et PROCESSOR_SPARC

   ENTREES: Les paramatre habituel d'un fwrite.
     char        *buff
     int         size et nitems
     FILE       *fp stream classique

     struct AnalyseDb *ana_db  Pointeur sur une structure de donnees

   SORTIES :
     La valeur rendue par la fonction :
          0 : Si probleme ou EOF
          nb_bytes : sinon
----------------------------------------------------------------------------*/
int PuWrite(char *ptr,int size,int nitems,FILE *stream )
{ 
  int nb_write;

  nb_write = fwrite(ptr,size,nitems,stream);
  return(nb_write);
}

/*----------------------------------------------------------------------------
int PuRead(char *buff, int size, int nitems, struct AnalyseDb *ana_db)

   Cette fonction fait la lecture des donnees en aveugle en fonction des
   information faite par la fonction appelante. I.e elle lit des bytes.
   Elle a besoin de ana_db pour connaitre le type de processeur lecteur.
   Bien sur nous n'avons plus de Digital avec leur format byte reverse mais
   nous poussedons toujourts des images qui ont un jour lu en byte reverse.


   C'est pourquoi nous testons la valeur de ana_db->key.hkey_un0  contre les
   constantes : PROCESSOR_MIPS et PROCESSOR_SPARC

   ENTREES: Les paramatre habituel d'un fwrite.
     char        *buff
     int         size et nitems
     FILE       *fp stream classique

     struct AnalyseDb *ana_db  Pointeur sur une structure de donnees

   SORTIES :
     La valeur rendue par la fonction :
          0 : Si probleme ou EOF
          nb_bytes : sinon

----------------------------------------------------------------------------*/

int PuRead(char *ptr,int size,int nitems,FILE *stream,
            struct AnalyzeDb *ana_db)
{
  int nb_read;

  nb_read = fread(ptr,size,nitems,stream);
  if ((ana_db->key.hkey_un0 == 1) && (ana_db->img.bitpix != 8))
    { 
      ShortByteSwapp((short *)ptr,size*nitems);
    }
  return(nb_read);
}

















