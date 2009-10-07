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


#include <aims/topology/topoClassifBase.h>


TopologicalClassificationBase::TopologicalClassificationBase()
  : _statInv( false )
{
}


void TopologicalClassificationBase::computeLocalCCNumbers( const Point3d& pt,
							   int label )
{
  _topo->fillNeighborhood( pt, label );
  _topo->computeLocalCCNumbers();
}


void TopologicalClassificationBase::computeLocalCCNumbers( const Point3d& pt,
							   int label1,
							   int label2 )
{
  _topo->fillNeighborhood( pt, label1, label2 );
  _topo->computeLocalCCNumbers();
}


void TopologicalClassificationBase::computeLocalCCNumbersComplement( 
							 const Point3d& pt,
							 int label )
{
  _topo->fillNeighborhoodComplement( pt, label );
  _topo->computeLocalCCNumbers();
}


bool TopologicalClassificationBase::isSimplePoint()
{
  if ( _topo->Cbar() == 1 && _topo->Cstar() == 1 )  return true;
  else return false;
}


bool TopologicalClassificationBase::isCurvesPoint()
{
  if ( _topo->Cbar() == 1 && _topo->Cstar() > 1 )  return true;
  else return false;
}


bool TopologicalClassificationBase::isSurfacesPoint()
{
  if ( _topo->Cbar() > 1 )  return true;
  else return false;
}


bool TopologicalClassificationBase::isRealSurfacePoint()
{
  int *x = _topo->X();

  if ( x[ 1 ] && x[ 6 ] )
    if ( x[ 1 ] != x[ 6 ] )  return true;

  if ( x[ 2 ] && x[ 5 ] )
    if ( x[ 2 ] != x[ 5 ] )  return true;

  if ( x[ 3 ] && x[ 4 ] )
    if ( x[ 3 ] != x[ 4 ] )  return true;

  return false;
}


bool TopologicalClassificationBase::isSimplePoint( const Point3d& pt,
						   int label )
{
  _topo->fillNeighborhood( pt, label );

  int status = bdd( _topo->X() );

  if ( _statInv )  return status ? false : true;
  else return status ? true : false;
}


bool TopologicalClassificationBase::isSimplePoint( const Point3d& pt,
						   int label1,
						   int label2 )
{
  _topo->fillNeighborhood( pt, label1, label2 );

  int status = bdd( _topo->X() );

  if ( _statInv )  return status ? false : true;
  else return status ? true : false;
}


bool TopologicalClassificationBase::isSimplePointComplement( 
                                                 const Point3d& pt,
					         int label )
{
  _topo->fillNeighborhoodComplement( pt, label );

  int status = bdd( _topo->X() );

  if ( _statInv )  return status ? false : true;
  else return status ? true : false;
}


int TopologicalClassificationBase::bdd( int *X )
{
  _statInv = false;

  if ( X[ Point4 ] )
    if ( X[ Point10 ] )
      if ( X[ Point13 ] )
	if ( X[ Point12 ] )
	  if ( X[ Point15 ] )  return !X[ Point21 ];
	  else
	    Lbl0: if ( X[ Point21 ] )  return 1;
	    else  return !X[ Point24 ];
	else
	  Lbl1: if ( X[ Point15 ] )
	    Lbl2: if ( X[ Point20 ] )  return X[ Point21 ];
	    else  return 1;
	  else
	    Lbl3: if ( X[ Point14 ] )
	      Lbl4: if ( X[ Point20 ] )  return 0;
	      else
		Lbl5: if ( X[ Point21 ] )  return 0;
		else  return !X[ Point24 ];
	    else
	      Lbl6: if ( X[ Point20 ] )  goto Lbl0;
	      else
		Lbl7: if ( X[ Point21 ] )  return 1;
		else
		  Lbl8: if ( X[ Point23 ] )  return X[ Point24 ];
		  else  return 1;
      else if ( X[ Point12 ] )
	Lbl9: if ( X[ Point15 ] )
	  Lbl10: if ( X[ Point21 ] )  return 1;
	  else  return !X[ Point22 ];
	else
	  Lbl11: if ( X[ Point21 ] )  return !X[ Point16 ];
	  else
	    Lbl12: if ( X[ Point16 ] )
	      if ( X[ Point22 ] )  return 0;
	      else  return !X[ Point24 ];
	    else
	      Lbl13: if ( X[ Point22 ] )  return !X[ Point24 ];
	      else
		Lbl14: if ( X[ Point24 ] )  return 1;
		else  return !X[ Point25 ];
      else
	Lbl15: if ( X[ Point15 ] )
	  Lbl16: if ( X[ Point20 ] )  return 0;
	  else
	    Lbl17: if ( X[ Point21 ] )  return 0;
	    else return !X[ Point22 ];
	else
	  Lbl18: if ( X[ Point14 ] )
	    Lbl19: if ( X[ Point20 ] )  return 0;
	    else
	      Lbl20: if ( X[ Point21 ] )  return 0;
	      else  goto Lbl12;
	  else
	    Lbl21: if ( X[ Point20 ] )  goto Lbl11;
	    else
	      Lbl22: if ( X[ Point21 ] )  return !X[ Point16 ];
	      else
		Lbl23: if ( X[ Point16 ] )
		  if ( X[ Point22 ] )  return 0;
		  else  goto Lbl8;
		else
		  Lbl24: if ( X[ Point22 ] )  goto Lbl8;
		  else
		    Lbl25: if ( X[ Point23 ] )  return 0;
		    else
		      Lbl26: if ( X[ Point24 ] )  return 0;
		      else  return !X[ Point25 ];
    else if ( X[ Point13 ] )
      if ( X[ Point12 ] )
	Lbl27: if ( X[ Point18 ] )
	  Lbl28: if ( X[ Point15 ] )  return X[ Point21 ];
	  else  return 0;
	else if ( X[ Point15 ] )  return 1;
	else  goto Lbl5;
      else
	Lbl29: if ( X[ Point9 ] )
	  if ( X[ Point18 ] )  return 0;
	  else  if ( X[ Point15 ] )
	    Lbl30: if ( X[ Point20 ] )  return 0;
	    else  return !X[ Point21 ];
	  else
	    Lbl31: if ( X[ Point14 ] )  goto Lbl4;
	    else
	      Lbl32: if ( X[ Point20 ] )  goto Lbl5;
	      else if ( X[ Point21 ] )  return 0;
	      else goto Lbl8;
	else
	  Lbl33: if ( X[ Point18 ] )  goto Lbl1;
	  else if ( X[ Point15 ] )
	    Lbl34: if ( X[ Point17 ] )
	      {
		_statInv = !_statInv;
		goto Lbl30;
	      }
	    else return 1;
	  else
	    Lbl35: if ( X[ Point14 ] )
	      if ( X[ Point17 ] )
		Lbl36: if ( X[ Point20 ] )  goto Lbl5;
		else  return 0;
	      else goto Lbl5;
	    else
	      Lbl37: if ( X[ Point17 ] )
		Lbl38: if ( X[ Point20 ] )
		  {
		    _statInv = !_statInv;
		    goto Lbl5;
		  }
		else  return X[ Point21 ];
	      else if ( X[ Point20 ] )
		{
		  _statInv = !_statInv;
		  goto Lbl5;
		}
	      else goto Lbl7;
    else if ( X[ Point11 ] )
      if ( X[ Point12 ] )
	Lbl39: if ( X[ Point18 ] )  return 0;
	else if ( X[ Point15 ] )  goto Lbl17;
	else goto Lbl20;
      else
	Lbl40: if ( X[ Point9 ] )
	  if ( X[ Point18 ] )  return 0;
	  else if ( X[ Point15 ] )  goto Lbl16;
	  else
	    Lbl41: if ( X[ Point14 ] )  goto Lbl19;
	    else
	      Lbl42: if ( X[ Point20 ] )  goto Lbl20;
	      else if ( X[ Point21 ] )  return 0;
	      else goto Lbl23;
	else
	  Lbl43: if ( X[ Point18 ] )  goto Lbl15;
	  else if ( X[ Point15 ] )
	    Lbl44: if ( X[ Point17 ] )
	      Lbl45: if ( X[ Point20 ] )  goto Lbl17;
	      else return 0;
	    else goto Lbl17;
	  else
	    Lbl46: if ( X[ Point14 ] )
	      if ( X[ Point17 ] )
		Lbl47: if ( X[ Point20 ] )  goto Lbl20;
		else  return 0;
	      else goto Lbl20;
	    else
	      Lbl48: if ( X[ Point17 ] )
		Lbl49: if ( X[ Point20 ] )
		  Lbl50: if ( X[ Point21 ] )  return !X[ Point16 ];
		  else
		    Lbl51: if ( X[ Point16 ] )
		      if ( X[ Point22 ] )  return 0;
		      else  return X[ Point24 ];
		    else
		      Lbl52: if ( X[ Point22 ] )  return X[ Point24 ];
		      else  return 0;
		else
		  Lbl53: if ( X[ Point21 ] )  return !X[ Point16 ];
		  else  return 0;
	      else  if ( X[ Point20 ] )  goto Lbl50;
	      else  goto Lbl22;
    else if ( X[ Point12 ] )
      Lbl54: if ( X[ Point18 ] )  goto Lbl9;
      else if ( X[ Point15 ] )
	Lbl55: if ( X[ Point21 ] )  return 1;
	else
	  Lbl56: if ( X[ Point19 ] )  return X[ Point22 ];
	  else  return 1;
      else
	Lbl57: if ( X[ Point21 ] )  return !X[ Point16 ];
	else
	  Lbl58: if ( X[ Point19 ] )
	    Lbl59: if ( X[ Point16 ] )
	      if ( X[ Point22 ] )  return !X[ Point24 ];
	      else  return 0;
	    else  goto Lbl52;
	  else if ( X[ Point16 ] )  return !X[ Point24 ];
	  else
	    Lbl60: if ( X[ Point22 ] )  return X[ Point24 ];
	    else  goto Lbl14;
    else
      Lbl61: if ( X[ Point9 ] )
	if ( X[ Point18 ] )  goto Lbl15;
	else if ( X[ Point15 ] )
	  Lbl62: if ( X[ Point20 ] )  return 0;
	  else if ( X[ Point21 ] )  return 0;
	  else goto Lbl56;
	else
	  Lbl63: if ( X[ Point14 ] )
	    if ( X[ Point20 ] )  return 0;
	    else if ( X[ Point21 ] )  return 0;
	    else  goto Lbl58;
	  else
	    Lbl64: if ( X[ Point20 ] )  goto Lbl57;
	    else if ( X[ Point21 ] )  return !X[ Point16 ];
	    else
	      Lbl65: if ( X[ Point19 ] )
		Lbl66: if ( X[ Point16 ] )
		  Lbl67: if ( X[ Point22 ] )  goto Lbl8;
		  else  return 0;
		else  return 0;
	      else if ( X[ Point16 ] )  goto Lbl8;
	      else
		Lbl68: if ( X[ Point22 ] )  return 0;
		else  goto Lbl25;
      else
	Lbl69: if ( X[ Point18 ] )
	  Lbl70: if ( X[ Point15 ] )
	    Lbl71: if ( X[ Point20 ] )  goto Lbl10;
	    else
	      {
		_statInv = !_statInv;
		goto Lbl17;
	      }
	  else
	    Lbl72: if ( X[ Point14 ] )
	      if ( X[ Point20 ] )  goto Lbl11;
	      else
		Lbl73: if ( X[ Point21 ] )  return !X[ Point16 ];
		else  goto Lbl59;
	    else
	      Lbl74: if ( X[ Point20 ] )
		Lbl75: if ( X[ Point21 ] )  return X[ Point16 ];
		else
		  Lbl76: if ( X[ Point16 ] )
		    Lbl77: if ( X[ Point22 ] )  return !X[ Point24 ];
		    else  return X[ Point24 ];
		  else return 0;
	      else
		Lbl78: if ( X[ Point21 ] )  return X[ Point16 ];
		else  goto Lbl66;
	else if ( X[ Point15 ] )
	  Lbl79: if ( X[ Point17 ] )
	    Lbl80: if ( X[ Point20 ] )  goto Lbl55;
	    else  return X[ Point21 ];
	  else  goto Lbl55;
	else
	  Lbl81: if ( X[ Point14 ] )
	    if ( X[ Point17 ] )
	      Lbl82: if ( X[ Point20 ] )  goto Lbl57;
	      else  goto Lbl53;
	    else  goto Lbl57;
	  else
	    Lbl83: if ( X[ Point17 ] )
	      Lbl84: if ( X[ Point20 ] )
		Lbl85: if ( X[ Point21 ] )  return X[ Point16 ];
		else
		  Lbl86: if ( X[ Point19 ] )
		    Lbl87: if ( X[ Point16 ] )  goto Lbl52;
		    else  return 0;
		  else if ( X[ Point16 ] )  return X[ Point24 ];
		  else  return 0;
	      else
		Lbl88: if ( X[ Point21 ] )  return X[ Point16 ];
		else  return 0;
	    else if ( X[ Point20 ] )  goto Lbl85;
	    else if ( X[ Point21 ] )  return X[ Point16 ];
	    else  goto Lbl65;
  else
    if ( X[ Point10 ] )
      if ( X[ Point13 ] )
	if ( X[ Point7 ] )
	  if ( X[ Point12 ] )  goto Lbl28;
	  else if ( X[ Point3 ] )  return 0;
	  else  goto Lbl1;
	else if ( X[ Point12 ] )
	  Lbl89: if ( X[ Point15 ] )  return X[ Point21 ];
	  else  goto Lbl0;
	else if ( X[ Point3 ] )
	  Lbl90: if ( X[ Point15 ] )  return 0;
	  else  goto Lbl3;
	else
	  Lbl91: if ( X[ Point15 ] )  goto Lbl2;
	  else if ( X[ Point14 ] )
	    Lbl92: if ( X[ Point20 ] )  goto Lbl0;
	    else
	      {
		_statInv = !_statInv;
		goto Lbl5;
	      }
	  else if ( X[ Point6 ] )  return 0;
	  else  goto Lbl6;
      else if ( X[ Point5 ] )
	if ( X[ Point7 ] )
	  if ( X[ Point12 ] )  return 0;
	  else if ( X[ Point3 ] )  return 0;
	  else  goto Lbl15;
	else if ( X[ Point12 ] )
	  Lbl93: if ( X[ Point15 ] )  return 0;
	  else  goto Lbl11;
	else if ( X[ Point3 ] )
	  Lbl94: if ( X[ Point15 ] )  return 0;
	  else  goto Lbl18;
	else
	  Lbl95: if ( X[ Point15 ] )  goto Lbl16;
	  else if ( X[ Point14 ] )
	    Lbl96: if ( X[ Point20 ] )  goto Lbl11;
	    else  goto Lbl50;
	  else if ( X[ Point6 ] )  return 0;
	  else  goto Lbl21;
      else if ( X[ Point7 ] )
	if ( X[ Point12 ] )  goto Lbl9;
	else if ( X[ Point3 ] )  goto Lbl15;
	else  goto Lbl70;
      else if ( X[ Point12 ] )
	Lbl97: if ( X[ Point15 ] )  goto Lbl10;
	else
	  Lbl98: if ( X[ Point21 ] )
	    Lbl99: if ( X[ Point8 ] )  return X[ Point16 ];
	    else  return 1;
	  else
	    Lbl100: if ( X[ Point8 ] )  goto Lbl76;
	    else if ( X[ Point16 ] )  goto Lbl77;
	    else  goto Lbl13;
      else if ( X[ Point3 ] )
	Lbl101: if ( X[ Point15 ] )  goto Lbl16;
	else if ( X[ Point14 ] )
	  Lbl102: if ( X[ Point20 ] )  return 0;
	  else
	    Lbl103: if ( X[ Point21 ] )  return 0;
	    else  goto Lbl100;
	else
	  Lbl104: if ( X[ Point20 ] )  goto Lbl98;
	  else
	    Lbl105: if ( X[ Point21 ] )  goto Lbl99;
	    else
	      Lbl106: if ( X[ Point8 ] )  goto Lbl66;
	      else
		Lbl107: if ( X[ Point16 ] )  goto Lbl67;
		else  goto Lbl24;
      else
	Lbl108: if ( X[ Point15 ] )  goto Lbl71;
	else if ( X[ Point14 ] )
	  Lbl109: if ( X[ Point20 ] )  goto Lbl98;
	  else
	    Lbl110: if ( X[ Point21 ] )  goto Lbl99;
	    else
	      Lbl111: if ( X[ Point8 ] )  goto Lbl87;
	      else  goto Lbl52;
	else if ( X[ Point6 ] )  return 0;
	else  goto Lbl104;
    else if ( X[ Point13 ] )
      if ( X[ Point1 ] )
	if ( X[ Point7 ] )
	  if ( X[ Point12 ] )  return 0;
	  else if ( X[ Point3 ] )  return 0;
	  else  goto Lbl29;
	else if ( X[ Point12 ] )
	  if ( X[ Point18 ] )  return 0;
	  else
	    Lbl112: if ( X[ Point15 ] )  return 0;
	    else  goto Lbl5;
	else if ( X[ Point3 ] )
	  if ( X[ Point9 ] )
	    if ( X[ Point18 ] )  return 0;
	    else if ( X[ Point15 ] )  return 0;
	    else  goto Lbl31;
	  else if ( X[ Point18 ] )  goto Lbl90;
	  else if ( X[ Point15 ] )  return 0;
	  else  goto Lbl35;
	else if ( X[ Point9 ] )
	  if ( X[ Point18 ] )  return 0;
	  else if ( X[ Point15 ] )  goto Lbl30;
	  else if ( X[ Point14 ] )
	    if ( X[ Point20 ] )  goto Lbl5;
	    else
	      {
		_statInv = !_statInv;
		goto Lbl0;
	      }
	  else if ( X[ Point6 ] )  return 0;
	  else  goto Lbl32;
	else
	  Lbl113: if ( X[ Point18 ] )  goto Lbl91;
	  else if ( X[ Point15 ] )  goto Lbl34;
	  else if ( X[ Point14 ] )
	    Lbl114: if ( X[ Point17 ] )  goto Lbl38;
	    else
	      {
		_statInv = !_statInv;
		goto Lbl5;
	      }
	  else if ( X[ Point6 ] )  return 0;
	  else  goto Lbl37;
      else if ( X[ Point7 ] )
	if ( X[ Point12 ] )  goto Lbl27;
	else if ( X[ Point3 ] )  goto Lbl29;
	else if ( X[ Point9 ] )
	  if ( X[ Point18 ] )  goto Lbl1;
	  else if ( X[ Point15 ] )
	    {
	      _statInv = !_statInv;
	      goto Lbl30;
	    }
	  else if ( X[ Point14 ] )  goto Lbl36;
	  else  goto Lbl38;
	else if ( X[ Point0 ] )  return 0;
	else  goto Lbl33;
      else if ( X[ Point12 ] )
	if ( X[ Point18 ] )  goto Lbl89;
	else
	  {
	    _statInv = !_statInv;
	    goto Lbl112;
	  }
      else if ( X[ Point3 ] )
	if ( X[ Point9 ] )
	  if ( X[ Point18 ] )  goto Lbl90;
	  else if ( X[ Point15 ] )  goto Lbl30;
	  else if ( X[ Point14 ] )
	    if ( X[ Point20 ] )  return 0;
	    else
	      {
		_statInv = !_statInv;
		goto Lbl0;
	      }
	  else  goto Lbl38;
	else if ( X[ Point18 ] )
	  if ( X[ Point15 ] )  goto Lbl2;
	  else if ( X[ Point14 ] )  goto Lbl92;
	  else  return 0;
	else if ( X[ Point15 ] )  goto Lbl34;
	else if ( X[ Point14 ] )goto Lbl114;
	else  return 0;
      else if ( X[ Point9 ] )
	if ( X[ Point18 ] )  goto Lbl91;
	else if ( X[ Point15 ] )
	  {
	    _statInv = !_statInv;
	    goto Lbl30;
	  }
	else if ( X[ Point14 ] )  goto Lbl38;
	else if ( X[ Point6 ] )  return 0;
	else  goto Lbl38;
      else if ( X[ Point0 ] ) return 0;
      else  goto Lbl113;
    else if ( X[ Point5 ] )
      if ( X[ Point1 ] )
	if ( X[ Point11 ] )
	  if ( X[ Point7 ] )
	    if ( X[ Point12 ] )  return 0;
	    else if ( X[ Point3 ] )  return 0;
	    else  goto Lbl40;
	  else if ( X[ Point12 ] )
	    if ( X[ Point18 ] )  return 0;
	    else if ( X[ Point15 ] )  return 0;
	    else  goto Lbl20;
	  else if ( X[ Point3 ] )
	    if ( X[ Point9 ] )
	      if ( X[ Point18 ] )  return 0;
	      else if ( X[ Point15 ] ) return 0;
	      else  goto Lbl41;
	    else if ( X[ Point18 ] )  goto Lbl94;
	    else if ( X[ Point15 ] )  return 0;
	    else  goto Lbl46;
	  else if ( X[ Point9 ] )
	    if ( X[ Point18 ] )  return 0;
	    else if ( X[ Point15 ] )  goto Lbl16;
	    else if ( X[ Point14 ] )
	      if ( X[ Point20 ] )  goto Lbl20;
	      else
		Lbl115: if ( X[ Point21 ] )  return 0;
		else  goto Lbl51;
	    else if ( X[ Point6 ] )  return 0;
	    else goto Lbl42;
	  else
	    Lbl116: if ( X[ Point18 ] )  goto Lbl95;
	    else if ( X[ Point15 ] )  goto Lbl44;
	    else if ( X[ Point14 ] )
	      Lbl117: if ( X[ Point17 ] )  goto Lbl49;
	      else  goto Lbl50;
	    else if ( X[ Point6 ] )  return 0;
	    else  goto Lbl48;
	else if ( X[ Point7 ] )
	  if ( X[ Point12 ] )  return 0;
	  else if ( X[ Point3 ] )  return 0;
	  else  goto Lbl61;
	else if ( X[ Point12 ] )
	  if ( X[ Point18 ] )  goto Lbl93;
	  else if ( X[ Point15 ] )  return 0;
	  else  goto Lbl57;
	else if ( X[ Point3 ] )
	  if ( X[ Point9 ] )
	    if ( X[ Point18 ] )  goto Lbl94;
	    else if ( X[ Point15 ] )  return 0;
	    else  goto Lbl63;
	  else if ( X[ Point18 ] )
	    if ( X[ Point15 ] )  return 0;
	    else  goto Lbl72;
	  else if ( X[ Point15 ] )  return 0;
	  else  goto Lbl81;
	else if ( X[ Point9 ] )
	  if ( X[ Point18 ] )  goto Lbl95;
	  else if ( X[ Point15 ] )  goto Lbl62;
	  else if ( X[ Point14 ] )
	    if ( X[ Point20 ] )  goto Lbl57;
	    else if ( X[ Point21 ] )  return !X[ Point16 ];
	    else goto Lbl86;
	  else if ( X[ Point6 ] )  return 0;
	  else  goto Lbl64;
	else
	  Lbl118: if ( X[ Point18 ] )
	    Lbl119: if ( X[ Point15 ] )  goto Lbl71;
	    else if ( X[ Point14 ] )
	      Lbl120: if ( X[ Point20 ] )  goto Lbl75;
	      else
		Lbl121: if ( X[ Point21 ] )  return X[ Point16 ];
		else  goto Lbl87;
	    else if ( X[ Point6 ] )  return 0;
	    else  goto Lbl74;
	  else if ( X[ Point15 ] )  goto Lbl79;
	  else if ( X[ Point14 ] )
	    Lbl122: if ( X[ Point17 ] )  goto Lbl84;
	    else  goto Lbl85;
	  else if ( X[ Point6 ] )  return 0;
	  else  goto Lbl83;
      else if ( X[ Point11 ] )
	if ( X[ Point7 ] )
	  if ( X[ Point12 ] )  goto Lbl39;
	  else if ( X[ Point3 ] )  goto Lbl40;
	  else if ( X[ Point9 ] )
	    if ( X[ Point18 ] )  goto Lbl15;
	    else if ( X[ Point15 ] )  goto Lbl45;
	    else if ( X[ Point14 ] )  goto Lbl47;
	    else  goto Lbl49;
	  else if ( X[ Point0 ] )  return 0;
	  else  goto Lbl43;
	else if ( X[ Point12 ] )
	  if ( X[ Point18 ] )  goto Lbl93;
	  else if ( X[ Point15 ] )  goto Lbl17;
	  else  goto Lbl50;
	else if ( X[ Point3 ] )
	  if ( X[ Point9 ] )
	    if ( X[ Point18 ] )  goto Lbl94;
	    else if ( X[ Point15 ] )  goto Lbl16;
	    else if ( X[ Point14 ] )
	      if ( X[ Point20 ] )  return 0;
	      else  goto Lbl115;
	    else  goto Lbl49;
	  else if ( X[ Point18 ] )
	    if ( X[ Point15 ] )  goto Lbl16;
	    else if ( X[ Point14 ] )  goto Lbl96;
	    else  return 0;
	  else if ( X[ Point15 ] )  goto Lbl44;
	  else if ( X[ Point14 ] )  goto Lbl117;
	  else  return 0;
	else if ( X[ Point9 ] )
	  if ( X[ Point18 ] )  goto Lbl95;
	  else if ( X[ Point15 ] )  goto Lbl45;
	  else if ( X[ Point14 ] )  goto Lbl49;
	  else if ( X[ Point6 ] )  return 0;
	  else  goto Lbl49;
	else if ( X[ Point0 ] )  return 0;
	else  goto Lbl116;
      else if ( X[ Point7 ] )
	Lbl123: if ( X[ Point12 ] )  goto Lbl54;
	else if ( X[ Point3 ] )  goto Lbl61;
	else if ( X[ Point9 ] )
	  Lbl124: if ( X[ Point18 ] )  goto Lbl70;
	  else if ( X[ Point15 ] )  goto Lbl80;
	  else if ( X[ Point14 ] )  goto Lbl82;
	  else  goto Lbl84;
	else if ( X[ Point0 ] )  return 0;
	else  goto Lbl69;
      else if ( X[ Point12 ] )
	if ( X[ Point18 ] )
	  if ( X[ Point15 ] )  goto Lbl10;
	  else  goto Lbl75;
	else if ( X[ Point15 ] )  goto Lbl55;
	else  goto Lbl85;
      else if ( X[ Point3 ] )
	if ( X[ Point9 ] )
	  if ( X[ Point18 ] )
	    if ( X[ Point15 ] )  goto Lbl16;
	    else if ( X[ Point14 ] )
	      if ( X[ Point20 ] )  return 0;
	      else
		Lbl125: if ( X[ Point21 ] )  return 0;
		else  goto Lbl76;
	    else  goto Lbl74;
	  else if ( X[ Point15 ] ) goto Lbl62;
	  else if ( X[ Point14 ] )
	    if ( X[ Point20 ] )  return 0;
	    else if ( X[ Point21 ] )  return 0;
	    else  goto Lbl86;
	  else  goto Lbl84;
	else if ( X[ Point18 ] )
	  if ( X[ Point15 ] )  goto Lbl71;
	  else if ( X[ Point14 ] )  goto Lbl120;
	  else  return 0;
	else if ( X[ Point15 ] )  goto Lbl79;
	else if ( X[ Point14 ] )  goto Lbl122;
	else  return 0;
      else if ( X[ Point9 ] )
	if ( X[ Point18 ] )  goto Lbl119;
	else if ( X[ Point15 ] )  goto Lbl80;
	else if ( X[ Point14 ] )  goto Lbl84;
	else if ( X[ Point6 ] )  return 0;
	else  goto Lbl84;
      else if ( X[ Point0 ] )  return 0;
      else  goto Lbl118;
    else if ( X[ Point1 ] )
      if ( X[ Point11 ] )
	if ( X[ Point7 ] )
	  if ( X[ Point12 ] )  goto Lbl39;
	  else if ( X[ Point3 ] )  goto Lbl40;
	  else if ( X[ Point9 ] )
	    if ( X[ Point18 ] )  return 0;
	    else if ( X[ Point15 ] )
	      Lbl126: if ( X[ Point20 ] )  goto Lbl17;
	      else
		{
		  _statInv = !_statInv;
		  goto Lbl10;
		}
	    else if ( X[ Point14 ] )
	      if ( X[ Point20 ] )  goto Lbl20;
	      else
		Lbl127: if ( X[ Point21 ] )  return 0;
		else  goto Lbl59;
	    else if ( X[ Point20 ] )  goto Lbl125;
	    else if ( X[ Point21 ] )  return 0;
	    else  goto Lbl66;
	  else
	    Lbl128: if ( X[ Point18 ] )  goto Lbl70;
	    else if ( X[ Point15 ] )
	      Lbl129: if ( X[ Point17 ] )
		Lbl130: if ( X[ Point20 ] )
		  {
		    _statInv = !_statInv;
		    goto Lbl17;
		  }
		else  return X[ Point21 ];
	      else
		{
		  _statInv = !_statInv;
		  goto Lbl17;
		}
	    else if ( X[ Point14 ] )
	      if ( X[ Point17 ] )
		Lbl131: if ( X[ Point20 ] )  goto Lbl73;
		else  goto Lbl53;
	      else  goto Lbl73;
	    else if ( X[ Point17 ] )
	      Lbl132: if ( X[ Point20 ] )  goto Lbl121;
	      else  goto Lbl88;
	    else if ( X[ Point20 ] )  goto Lbl121;
	    else  goto Lbl78;
	else if ( X[ Point12 ] )
	  if ( X[ Point18 ] )  return 0;
	  else if ( X[ Point15 ] )  goto Lbl17;
	  else  goto Lbl103;
	else if ( X[ Point3 ] )
	  if ( X[ Point9 ] )
	    if ( X[ Point18 ] )  return 0;
	    else if ( X[ Point15 ] )  goto Lbl16;
	    else if ( X[ Point14 ] )  goto Lbl102;
	    else
	      Lbl133: if ( X[ Point20 ] )  goto Lbl103;
	      else if ( X[ Point21 ] )  return 0;
	      else  goto Lbl106;
	  else if ( X[ Point18 ] )  goto Lbl101;
	  else if ( X[ Point15 ] )  goto Lbl44;
	  else if ( X[ Point14 ] )
	    if ( X[ Point17 ] )
	      if ( X[ Point20 ] )  goto Lbl103;
	      else  return 0;
	    else  goto Lbl103;
	  else
	    Lbl134: if ( X[ Point17 ] )
	      Lbl135: if ( X[ Point20 ] )  goto Lbl110;
	      else
		Lbl136: if ( X[ Point21 ] )  goto Lbl99;
		else  return 0;
	    else if ( X[ Point20 ] )  goto Lbl110;
	    else  goto Lbl105;
	else if ( X[ Point9 ] )
	  if ( X[ Point18 ] )  return 0;
	  else if ( X[ Point15 ] )  goto Lbl126;
	  else if ( X[ Point14 ] )
	    if ( X[ Point20 ] )  goto Lbl103;
	    else
	      Lbl137: if ( X[ Point21 ] )  return 0;
	      else  goto Lbl111;
	  else if ( X[ Point6 ] )  return 0;
	  else  goto Lbl133;
	else
	  Lbl138: if ( X[ Point18 ] )  goto Lbl108;
	  else if ( X[ Point15 ] )  goto Lbl129;
	  else if ( X[ Point14 ] )
	    Lbl139: if ( X[ Point17 ] )  goto Lbl135;
	    else  goto Lbl110;
	  else if ( X[ Point6 ] )  return 0;
	  else  goto Lbl134;
      else if ( X[ Point7 ] )
	if ( X[ Point12 ] )  goto Lbl54;
	else if ( X[ Point3 ] )  goto Lbl61;
	else if ( X[ Point9 ] )  goto Lbl124;
	else  return 0;
      else if ( X[ Point12 ] )
	Lbl140: if ( X[ Point18 ] )  goto Lbl97;
	else if ( X[ Point15 ] )  goto Lbl55;
	else
	  Lbl141: if ( X[ Point21 ] )  goto Lbl99;
	  else
	    Lbl142: if ( X[ Point8 ] )  goto Lbl86;
	    else if ( X[ Point19 ] )  goto Lbl52;
	    else if ( X[ Point16 ] )  return X[ Point24 ];
	    else  goto Lbl60;
      else if ( X[ Point3 ] )
	Lbl143: if ( X[ Point9 ] )
	  if ( X[ Point18 ] )  goto Lbl101;
	  else if ( X[ Point15 ] )  goto Lbl62;
	  else if ( X[ Point14 ] )
	    if ( X[ Point20 ] )  return 0;
	    else if ( X[ Point21 ] )  return 0;
	    else  goto Lbl142;
	  else
	    Lbl144: if ( X[ Point20 ] )  goto Lbl141;
	    else if ( X[ Point21 ] )  goto Lbl99;
	    else
	      Lbl145: if ( X[ Point8 ] )  return 0;
	      else if ( X[ Point19 ] )  return 0;
	      else  if ( X[ Point16 ] )  return 0;
	      else  goto Lbl68;
	else if ( X[ Point18 ] )
	  Lbl146: if ( X[ Point15 ] )  goto Lbl71;
	  else if ( X[ Point14 ] )  goto Lbl109;
	  else  return 0;
	else if ( X[ Point15 ] )  goto Lbl79;
	else if ( X[ Point14 ] )
	  Lbl147: if ( X[ Point17 ] )
	    Lbl148: if ( X[ Point20 ] )  goto Lbl141;
	    else  goto Lbl136;
	  else  goto Lbl141;
	else
	  Lbl149: if ( X[ Point17 ] )  return 0;
	  else if ( X[ Point20 ] )  return 0;
	  else if ( X[ Point21 ] )  return 0;
	  else  goto Lbl145;
      else if ( X[ Point9 ] )
	Lbl150: if ( X[ Point18 ] )  goto Lbl108;
	else if ( X[ Point15 ] )  goto Lbl80;
	else if ( X[ Point14 ] )  goto Lbl148;
	else if ( X[ Point6 ] )  return 0;
	else  goto Lbl144;
      else
	Lbl151: if ( X[ Point18 ] )  return 0;
	else if ( X[ Point15 ] )  return 0;
	else if ( X[ Point14 ] )  return 0;
	else if ( X[ Point6 ] )  return 0;
	else  goto Lbl149;
    else if ( X[ Point11 ] )
      if ( X[ Point7 ] )
	if ( X[ Point12 ] )
	  if ( X[ Point18 ] )  goto Lbl9;
	  else if ( X[ Point15 ] )
	    {
	      _statInv = !_statInv;
	      goto Lbl17;
	    }
	  else  goto Lbl73;
	else if ( X[ Point3 ] )
	  if ( X[ Point9 ] )
	    if ( X[ Point18 ] )  goto Lbl15;
	    else if ( X[ Point15 ] )
	      Lbl152: if ( X[ Point20 ] )  return 0;
	      else
		{
		  _statInv = !_statInv;
		  goto Lbl10;
		}
	    else if ( X[ Point14 ] )
	      if ( X[ Point20 ] )  return 0;
	      else  goto Lbl127;
	    else if ( X[ Point20 ] )  goto Lbl73;
	    else if ( X[ Point21 ] )  return !X[ Point16 ];
	    else  goto Lbl66;
	  else  goto Lbl128;
	else if ( X[ Point9 ] )
	  if ( X[ Point18 ] )  goto Lbl70;
	  else if ( X[ Point15 ] )  goto Lbl130;
	  else if ( X[ Point14 ] )  goto Lbl131;
	  else  goto Lbl132;
	else if ( X[ Point0 ] )  return 0;
	else  goto Lbl128;
      else if ( X[ Point12 ] )
	if ( X[ Point18 ] )  goto Lbl97;
	else if ( X[ Point15 ] )
	  {
	    _statInv = !_statInv;
	    goto Lbl17;
	  }
	else goto Lbl110;
      else if ( X[ Point3 ] )
	if ( X[ Point9 ] )
	  if ( X[ Point18 ] )  goto Lbl101;
	  else if ( X[ Point15 ] )  goto Lbl152;
	  else if ( X[ Point14 ] )
	    if ( X[ Point20 ] )  return 0;
	    else  goto Lbl137;
	  else  goto Lbl135;
	else if ( X[ Point18 ] )  goto Lbl146;
	else if ( X[ Point15 ] )  goto Lbl129;
	else if ( X[ Point14 ] )  goto Lbl139;
	else  return 0;
      else if ( X[ Point9 ] )
	if ( X[ Point18 ] )  goto Lbl108;
	else if ( X[ Point15 ] )  goto Lbl130;
	else if ( X[ Point14 ] )  goto Lbl135;
	else if ( X[ Point6 ] )  return 0;
	else  goto Lbl135;
      else if ( X[ Point0 ] )  return 0;
      else  goto Lbl138;
    else if ( X[ Point7 ] )
      if ( X[ Point2 ] )  return 0;
      else goto Lbl123;
    else if ( X[ Point2 ] )
      if ( X[ Point12 ] )  return 0;
      else if ( X[ Point3 ] )  return 0;
      else if ( X[ Point9 ] )  return 0;
      else if ( X[ Point0 ] )  return 0;
      else  goto Lbl151;
    else if ( X[ Point12 ] )  goto Lbl140;
    else if ( X[ Point3 ] )  goto Lbl143;
    else if ( X[ Point9 ] )  goto Lbl150;
    else if ( X[ Point0 ] )  goto Lbl151;
    else if ( X[ Point18 ] )  goto Lbl108;
    else if ( X[ Point15 ] )  goto Lbl79;
    else if ( X[ Point14 ] )  goto Lbl147;
    else if ( X[ Point6 ] )  goto Lbl149;
    else if ( X[ Point17 ] )  goto Lbl144;
    else if ( X[ Point20 ] )  goto Lbl141;
    else if ( X[ Point21 ] )  goto Lbl99;
    else if ( X[ Point8 ] )  goto Lbl65;
    else if ( X[ Point19 ] )  goto Lbl107;
    else if ( X[ Point16 ] )  goto Lbl8;
    else if ( X[ Point22 ] )  goto Lbl8;
    else if ( X[ Point23 ] )  goto Lbl14;
    else
      {
	_statInv = !_statInv;
	goto Lbl26;
      }
}
