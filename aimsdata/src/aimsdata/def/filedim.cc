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

// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

/*
 *  DimX,DimY,DimZ,DimT of text files
 */
#include <aims/def/general.h>
#include <fstream>
#include <ctype.h>

using namespace std;


bool AimsIsScientificNumberChar(int c)
{
  if ( isdigit(c) || c==(int)'.' || c==(int)'+' || c==(int)'-' || 
                     c==(int)'e' || c==(int)'E')
    return true;
  else
    return false;
}


int AimsSpaceLevelOf(const string& filename)
{
  int level = 0;
  char c;

  ifstream ifst(filename.c_str());
  do
  {
    c = 0;
    ifst >> c;
    if (c=='[') level++;
  }
  while (c=='[');
  ifst.close();
  
  return level;
}

int AimsFileDimTOf(const string& filename)
{
  char c[7] = {0,0,0,0,0,0,0};
  int nb = 0,dimt;
    
  ifstream ifst(filename.c_str());
 
  while (!ifst.eof())
  {
    c[0] = c[1];
    c[1] = c[2];
    c[2] = c[3];
    c[3] = c[4];
    c[4] = c[5];
    c[5] = c[6];
    ifst >> c[6];
    if (c[0]==']' && c[1]==']' && c[2]==']' && c[3]==',' && 
        c[4]=='[' && c[5]=='[' && c[6]=='[')
      nb++;
  }
  dimt = nb+1;
  ifst.close();
  return dimt;
}

int AimsFileDimZOf(const string& filename)
{
  char c[5] = {0,0,0,0,0},old;
  int nb = 0,dimz;
    
  ifstream ifst(filename.c_str());
 
  while (!ifst.eof())
  {
    old  = c[0];
    c[0] = c[1];
    c[1] = c[2];
    c[2] = c[3];
    c[3] = c[4];
    ifst >> c[4];
    if (c[0]==']' && c[1]==']' && c[2]==',' && c[3]=='[' && c[4]=='[' 
        && old!=']')
      nb++;
  }
  dimz = nb/AimsFileDimTOf(filename)+1;
  ifst.close();
  return dimz;
}


int AimsFileDimYOf(const string& filename)
{ char c[3] = {0,0,0},old1 = 0,old2;
  int nb = 0,dimy;
    
  ifstream ifst(filename.c_str());
 
  while (!ifst.eof())
  {
    old2 = old1;
    old1 = c[0];
    c[0] = c[1];
    c[1] = c[2];
    ifst >> c[2];
    if (c[0]==']' && c[1]==',' && c[2]=='[' && old2!=']' && old1!=']')
      nb++;
  }
  dimy = nb/(AimsFileDimTOf(filename)*AimsFileDimZOf(filename))+1;
  ifst.close();
  return dimy;
}


int AimsFileDimXOf(const string& filename)
{
  char c[3] = {0,0,0};
  int nb = 0,dimx;
  bool is2dorcplx = AimsIsComplexData(filename);
  bool is3d       = AimsIsPoint3dData(filename); 

  ifstream ifst(filename.c_str());
 
  while (!ifst.eof())
  {
    c[0] = c[1];
    c[1] = c[2];
    ifst >> c[2];
    if (c[0]!=']' && c[1]==',' && c[2]!='[')
      nb++;
  }
  if (is2dorcplx) nb = (nb-1)/2;
  if (is3d)       nb = (nb-2)/3;
  dimx = nb/(AimsFileDimTOf(filename)*AimsFileDimZOf(filename)*
                                      AimsFileDimYOf(filename))+1;
  ifst.close();
  return dimx;
}


bool AimsIsComplexData(const string& filename)
{
  ifstream ifst(filename.c_str());
  char     c;
  bool     result = false;
  int      ncomer=0;

  while (!ifst.eof())
  {
    ifst >> c;
    if (c=='(') 
    {
      while (c!=')')
      {
        ifst >> c;
        if (c==',') ncomer++;
      }
      if (ncomer==1) result=true;
      break;
    }
  }
  ifst.close();
  return result;
}


bool AimsIsPoint2dData(const string& filename)
{
  return AimsIsComplexData(filename.c_str());
}


bool AimsIsPoint3dData(const string& filename)
{ ifstream ifst(filename.c_str());
  char     c;
  bool     result = false;
  int      ncomer=0;

  while (!ifst.eof())
  {
    ifst >> c;
    if (c=='(') 
    {
      while (c!=')')
      {
        ifst >> c;
        if (c==',') ncomer++;
      }
      if (ncomer==2) result=true;
      break;
    }
  }
  ifst.close();
  return result;
}
