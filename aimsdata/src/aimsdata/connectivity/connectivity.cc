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

#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

/*
 * Connectivity class methods
 */
#include <cstdlib>
#include <aims/connectivity/connectivity.h>
using aims::Connectivity;
#include <aims/def/general.h>
#include <math.h>


Connectivity::Connectivity(int oline, int oslice, Connectivity::Type type) : 
  _offset(0), _xyzOffset(0), _dir(0)
{
  _type        = type;
  _nbNeighbors = 0;
  _xSize       = oline;
  _sliceSize   = oslice;
  setConnectivity();
}


Connectivity::~Connectivity()
{
  if( _xyzOffset != 0 ){
    delete[] _xyzOffset;
    _xyzOffset = 0 ;
  }
  if(_offset != 0 ){
    delete[] _offset;
    _offset = 0 ;
  }
  if( _dir != 0 ){
    delete[] _dir;
    _dir = 0 ;
  }
}


Connectivity::Type Connectivity::type_from_string(const std::string &str)
{
  const int number = std::atoi(str.c_str());
  switch(number) {
  case 6:
    return CONNECTIVITY_6_XYZ;
  case 18:
    return CONNECTIVITY_18_XYZ;
  case 26:
    return CONNECTIVITY_26_XYZ;
  case 4:
  case 5:
  case 8:
  case 9:
    AimsError("Connectivity::type_from_string(): "
              "connectivity not implemented");
  default:
    AimsError("Connectivity::type_from_string(): "
              "not a valid connectivity string");
  };
  return CONNECTIVITY_26_XYZ;  // return dummy value to prevent chaos
}


std::string Connectivity::type_to_string(Connectivity::Type type)
{
  switch(type) {
  case CONNECTIVITY_4_XY        : return "4XY";
  case CONNECTIVITY_4_XZ        : return "4XZ";
  case CONNECTIVITY_4_YZ        : return "4YZ";
  case CONNECTIVITY_6_XYZ       : return "6";
  case CONNECTIVITY_8_XY        : return "8XY";
  case CONNECTIVITY_8_XZ        : return "8XZ";
  case CONNECTIVITY_8_YZ        : return "8YZ";
  case CONNECTIVITY_18_XYZ      : return "18";
  case CONNECTIVITY_26_XYZ      : return "26";
  case CONNECTIVITY_9_XY_Zminus : return "9XYZ-";
  case CONNECTIVITY_9_XY_Zplus  : return "9XYZ+";
  case CONNECTIVITY_9_XZ_Yminus : return "9XZY-";
  case CONNECTIVITY_9_XZ_Yplus  : return "9XZY+";
  case CONNECTIVITY_9_YZ_Xminus : return "9YZX-";
  case CONNECTIVITY_9_YZ_Xplus  : return "9YZX+";
  case CONNECTIVITY_5_XminusY   : return "5X-Y";
  case CONNECTIVITY_5_XplusY    : return "5X+Y";
  case CONNECTIVITY_5_XYminus   : return "5XY-";
  case CONNECTIVITY_5_XYplus    : return "5XY+";
  case CONNECTIVITY_5_XminusZ   : return "5X-Z";
  case CONNECTIVITY_5_XplusZ    : return "5X+Z";
  case CONNECTIVITY_5_XZminus   : return "5XZ-";
  case CONNECTIVITY_5_XZplus    : return "5XZ+";
  case CONNECTIVITY_5_YminusZ   : return "5Y-Z";
  case CONNECTIVITY_5_YplusZ    : return "5Y+Z";
  case CONNECTIVITY_5_YZminus   : return "5YZ-";
  case CONNECTIVITY_5_YZplus    : return "5YZ+";
  default:
    AimsError("Connectivity::type_to_string: "
              "not a valid connectivity");
  };
  return "";
}


void Connectivity::setConnectivity()
{
  if( _xyzOffset ){
    delete[] _xyzOffset;
    _xyzOffset = 0 ;
  }
  if(_offset){
    delete[] _offset;
    _offset = 0 ;
  }
  if( _dir ){
    delete[] _dir;
    _dir = 0 ;
  }
  
  switch (_type)
  {
    case CONNECTIVITY_4_XY        : setConnectivity_4_XY();       break;
    case CONNECTIVITY_4_XZ        : setConnectivity_4_XZ();       break;
    case CONNECTIVITY_4_YZ        : setConnectivity_4_YZ();       break;
    case CONNECTIVITY_6_XYZ       : setConnectivity_6_XYZ();      break;
    case CONNECTIVITY_8_XY        : setConnectivity_8_XY();       break;
    case CONNECTIVITY_8_XZ        : setConnectivity_8_XZ();       break;
    case CONNECTIVITY_8_YZ        : setConnectivity_8_YZ();       break;
    case CONNECTIVITY_18_XYZ      : setConnectivity_18_XYZ();     break;
    case CONNECTIVITY_26_XYZ      : setConnectivity_26_XYZ();     break;
    case CONNECTIVITY_9_XY_Zminus : setConnectivity_9_XY_Zminus();break;
    case CONNECTIVITY_9_XY_Zplus  : setConnectivity_9_XY_Zplus(); break;
    case CONNECTIVITY_9_XZ_Yminus : setConnectivity_9_XZ_Yminus();break;
    case CONNECTIVITY_9_XZ_Yplus  : setConnectivity_9_XZ_Yplus(); break;
    case CONNECTIVITY_9_YZ_Xminus : setConnectivity_9_YZ_Xminus();break;
    case CONNECTIVITY_9_YZ_Xplus  : setConnectivity_9_YZ_Xplus(); break;
    case CONNECTIVITY_5_XminusY   : setConnectivity_5_XminusY();  break;
    case CONNECTIVITY_5_XplusY    : setConnectivity_5_XplusY();   break;
    case CONNECTIVITY_5_XYminus   : setConnectivity_5_XYminus();  break;
    case CONNECTIVITY_5_XYplus    : setConnectivity_5_XYplus();   break;
    case CONNECTIVITY_5_XminusZ   : setConnectivity_5_XminusZ();  break;
    case CONNECTIVITY_5_XplusZ    : setConnectivity_5_XplusZ();   break;
    case CONNECTIVITY_5_XZminus   : setConnectivity_5_XZminus();  break;
    case CONNECTIVITY_5_XZplus    : setConnectivity_5_XZplus();   break;
    case CONNECTIVITY_5_YminusZ   : setConnectivity_5_YminusZ();  break;
    case CONNECTIVITY_5_YplusZ    : setConnectivity_5_YplusZ();   break;
    case CONNECTIVITY_5_YZminus   : setConnectivity_5_YZminus();  break;
    case CONNECTIVITY_5_YZplus    : setConnectivity_5_YZplus();   break;
    default                            : 
      AimsError("void Connectivity::setConnectivity() : \
                 not a valid connectivity");
  }

  // build offsets set
  _xyzOffsets.clear();
  for( int i=0; i<_nbNeighbors; ++i )
    _xyzOffsets.insert( _xyzOffset[i] );
}


void Connectivity::setConnectivity_4_XY()
{
  _offset                  = new int[4];
  _xyzOffset               = new Point3d[4];
  _dir                     = new Point3df[4];

  _xyzOffset[_nbNeighbors] = Point3d(-1,0,0);
  _offset[_nbNeighbors]    = _xyzOffset[_nbNeighbors].item(0) + 
                             _xSize * _xyzOffset[_nbNeighbors].item(1);
  _dir[_nbNeighbors]       = Point3df(-1,0,0);
  _nbNeighbors++;

  _xyzOffset[_nbNeighbors] = Point3d(0,-1,0);
  _offset[_nbNeighbors]    = _xyzOffset[_nbNeighbors].item(0) + 
                             _xSize * _xyzOffset[_nbNeighbors].item(1);
  _dir[_nbNeighbors]       = Point3df(0,-1,0);
  _nbNeighbors++;

  _xyzOffset[_nbNeighbors] = Point3d(0,1,0);
  _offset[_nbNeighbors]    = _xyzOffset[_nbNeighbors].item(0) + 
                             _xSize * _xyzOffset[_nbNeighbors].item(1);
  _dir[_nbNeighbors]       = Point3df(0,1,0);
  _nbNeighbors++;

  _xyzOffset[_nbNeighbors] = Point3d(1,0,0);
  _offset[_nbNeighbors]    = _xyzOffset[_nbNeighbors].item(0) + 
                             _xSize * _xyzOffset[_nbNeighbors].item(1);
  _dir[_nbNeighbors]       = Point3df(1,0,0);
  _nbNeighbors++;
}


void Connectivity::setConnectivity_4_XZ()
{
  _offset                  = new int[4];
  _xyzOffset               = new Point3d[4];
  _dir                     = new Point3df[4];

  _xyzOffset[_nbNeighbors] = Point3d(-1,0,0);
  _offset[_nbNeighbors]    = _xyzOffset[_nbNeighbors].item(0) + 
                             _sliceSize * _xyzOffset[_nbNeighbors].item(2);
  _dir[_nbNeighbors]       = Point3df(-1,0,0);
  _nbNeighbors++;

  _xyzOffset[_nbNeighbors] = Point3d(0,0,-1);
  _offset[_nbNeighbors]    = _xyzOffset[_nbNeighbors].item(0) + 
                             _sliceSize * _xyzOffset[_nbNeighbors].item(2);
  _dir[_nbNeighbors]       = Point3df(0,0,-1);
  _nbNeighbors++;

  _xyzOffset[_nbNeighbors] = Point3d(0,0,1);
  _offset[_nbNeighbors]    = _xyzOffset[_nbNeighbors].item(0) + 
                             _sliceSize * _xyzOffset[_nbNeighbors].item(2);
  _dir[_nbNeighbors]       = Point3df(0,0,1);
  _nbNeighbors++;

  _xyzOffset[_nbNeighbors] = Point3d(1,0,0);
  _offset[_nbNeighbors]    = _xyzOffset[_nbNeighbors].item(0) + 
                             _sliceSize * _xyzOffset[_nbNeighbors].item(2);
  _dir[_nbNeighbors]       = Point3df(1,0,0);
  _nbNeighbors++;
}


void Connectivity::setConnectivity_4_YZ()
{ 
  
  _offset                  = new int[4];
  _xyzOffset               = new Point3d[4];
  _dir                     = new Point3df[4];

  _xyzOffset[_nbNeighbors] = Point3d(0,-1,0);
  _offset[_nbNeighbors]    = _xSize * _xyzOffset[_nbNeighbors].item(1) + 
                             _sliceSize * _xyzOffset[_nbNeighbors].item(2);
  _dir[_nbNeighbors]       = Point3df(0,-1,0);
  _nbNeighbors++;

  _xyzOffset[_nbNeighbors] = Point3d(0,0,-1);
  _offset[_nbNeighbors]    = _xSize * _xyzOffset[_nbNeighbors].item(1) + 
                             _sliceSize * _xyzOffset[_nbNeighbors].item(2);
  _dir[_nbNeighbors]       = Point3df(0,0,-1);
  _nbNeighbors++;

  _xyzOffset[_nbNeighbors] = Point3d(0,0,1);
  _offset[_nbNeighbors]    = _xSize * _xyzOffset[_nbNeighbors].item(1) + 
                             _sliceSize * _xyzOffset[_nbNeighbors].item(2);
  _dir[_nbNeighbors]       = Point3df(0,0,1);
  _nbNeighbors++;

  _xyzOffset[_nbNeighbors] = Point3d(0,1,0);
  _offset[_nbNeighbors]    = _xSize * _xyzOffset[_nbNeighbors].item(1) + 
                             _sliceSize * _xyzOffset[_nbNeighbors].item(2);
  _dir[_nbNeighbors]       = Point3df(0,1,0);
  _nbNeighbors++;
}


void Connectivity::setConnectivity_6_XYZ()
{ 
  
  _offset                  = new int[6];
  _xyzOffset               = new Point3d[6];
  _dir                     = new Point3df[6];

  _xyzOffset[_nbNeighbors] = Point3d(-1,0,0);
  _offset[_nbNeighbors]    =              _xyzOffset[_nbNeighbors].item(0) + 
                             _xSize     * _xyzOffset[_nbNeighbors].item(1) +
                             _sliceSize * _xyzOffset[_nbNeighbors].item(2) ;
  _dir[_nbNeighbors]       = Point3df(-1,0,0);
  _nbNeighbors++;

  _xyzOffset[_nbNeighbors] = Point3d(0,-1,0);
  _offset[_nbNeighbors]    =              _xyzOffset[_nbNeighbors].item(0) + 
                             _xSize     * _xyzOffset[_nbNeighbors].item(1) +
                             _sliceSize * _xyzOffset[_nbNeighbors].item(2) ;
  _dir[_nbNeighbors]       = Point3df(0,-1,0);
  _nbNeighbors++;

  _xyzOffset[_nbNeighbors] = Point3d(0,0,-1);
  _offset[_nbNeighbors]    =              _xyzOffset[_nbNeighbors].item(0) + 
                             _xSize     * _xyzOffset[_nbNeighbors].item(1) +
                             _sliceSize * _xyzOffset[_nbNeighbors].item(2) ;
  _dir[_nbNeighbors]       = Point3df(0,0,-1);
  _nbNeighbors++;

  _xyzOffset[_nbNeighbors] = Point3d(0,0,1);
  _offset[_nbNeighbors]    =              _xyzOffset[_nbNeighbors].item(0) + 
                             _xSize     * _xyzOffset[_nbNeighbors].item(1) +
                             _sliceSize * _xyzOffset[_nbNeighbors].item(2) ;
  _dir[_nbNeighbors]       = Point3df(0,0,1);
  _nbNeighbors++;

  _xyzOffset[_nbNeighbors] = Point3d(0,1,0);
  _offset[_nbNeighbors]    =              _xyzOffset[_nbNeighbors].item(0) + 
                             _xSize     * _xyzOffset[_nbNeighbors].item(1) +
                             _sliceSize * _xyzOffset[_nbNeighbors].item(2) ;
  _dir[_nbNeighbors]       = Point3df(0,1,0);
  _nbNeighbors++;

  _xyzOffset[_nbNeighbors] = Point3d(1,0,0);
  _offset[_nbNeighbors]    =              _xyzOffset[_nbNeighbors].item(0) + 
                             _xSize     * _xyzOffset[_nbNeighbors].item(1) +
                             _sliceSize * _xyzOffset[_nbNeighbors].item(2) ;
  _dir[_nbNeighbors]       = Point3df(1,0,0);
  _nbNeighbors++;
}


void Connectivity::setConnectivity_8_XY()
{ 
  
  _offset                  = new int[8];
  _xyzOffset               = new Point3d[8];
  _dir                     = new Point3df[8];

  _xyzOffset[_nbNeighbors] = Point3d(-1,-1,0);
  _offset[_nbNeighbors]    =              _xyzOffset[_nbNeighbors].item(0) + 
                             _xSize     * _xyzOffset[_nbNeighbors].item(1) ;
  _dir[_nbNeighbors]       = Point3df(-M_SQRT1_2,-M_SQRT1_2,0);
  _nbNeighbors++;

  _xyzOffset[_nbNeighbors] = Point3d(-1,0,0);
  _offset[_nbNeighbors]    =              _xyzOffset[_nbNeighbors].item(0) + 
                             _xSize     * _xyzOffset[_nbNeighbors].item(1) ;
  _dir[_nbNeighbors]       = Point3df(-1,0,0);
  _nbNeighbors++;

  _xyzOffset[_nbNeighbors] = Point3d(-1,1,0);
  _offset[_nbNeighbors]    =              _xyzOffset[_nbNeighbors].item(0) + 
                             _xSize     * _xyzOffset[_nbNeighbors].item(1) ;
  _dir[_nbNeighbors]       = Point3df(-M_SQRT1_2,M_SQRT1_2,0);
  _nbNeighbors++;

  _xyzOffset[_nbNeighbors] = Point3d(0,-1,0);
  _offset[_nbNeighbors]    =              _xyzOffset[_nbNeighbors].item(0) + 
                             _xSize     * _xyzOffset[_nbNeighbors].item(1) ;
  _dir[_nbNeighbors]       = Point3df(0,-1,0);
  _nbNeighbors++;

  _xyzOffset[_nbNeighbors] = Point3d(0,1,0);
  _offset[_nbNeighbors]    =              _xyzOffset[_nbNeighbors].item(0) + 
                             _xSize     * _xyzOffset[_nbNeighbors].item(1) ;
  _dir[_nbNeighbors]       = Point3df(0,1,0);
  _nbNeighbors++;

  _xyzOffset[_nbNeighbors] = Point3d(1,-1,0);
  _offset[_nbNeighbors]    =              _xyzOffset[_nbNeighbors].item(0) + 
                             _xSize     * _xyzOffset[_nbNeighbors].item(1) ;
  _dir[_nbNeighbors]       = Point3df(M_SQRT1_2,-M_SQRT1_2,0);
  _nbNeighbors++;

  _xyzOffset[_nbNeighbors] = Point3d(1,0,0);
  _offset[_nbNeighbors]    =              _xyzOffset[_nbNeighbors].item(0) + 
                             _xSize     * _xyzOffset[_nbNeighbors].item(1) ;
  _dir[_nbNeighbors]       = Point3df(1,0,0);
  _nbNeighbors++;

  _xyzOffset[_nbNeighbors] = Point3d(1,1,0);
  _offset[_nbNeighbors]    =              _xyzOffset[_nbNeighbors].item(0) + 
                             _xSize     * _xyzOffset[_nbNeighbors].item(1) ;
  _dir[_nbNeighbors]       = Point3df(M_SQRT1_2,M_SQRT1_2,0);
  _nbNeighbors++;
}


void Connectivity::setConnectivity_8_XZ()
{ 
  
  _offset                  = new int[8];
  _xyzOffset               = new Point3d[8];
  _dir                     = new Point3df[8];

  _xyzOffset[_nbNeighbors] = Point3d(-1,0,-1);
  _offset[_nbNeighbors]    =              _xyzOffset[_nbNeighbors].item(0) + 
                             _sliceSize * _xyzOffset[_nbNeighbors].item(2) ;
  _dir[_nbNeighbors]       = Point3df(-M_SQRT1_2,0,-M_SQRT1_2);
  _nbNeighbors++;

  _xyzOffset[_nbNeighbors] = Point3d(-1,0,0);
  _offset[_nbNeighbors]    =              _xyzOffset[_nbNeighbors].item(0) + 
                             _sliceSize * _xyzOffset[_nbNeighbors].item(2) ;
  _dir[_nbNeighbors]       = Point3df(-1,0,0);
  _nbNeighbors++;

  _xyzOffset[_nbNeighbors] = Point3d(-1,0,1);
  _offset[_nbNeighbors]    =              _xyzOffset[_nbNeighbors].item(0) + 
                             _sliceSize * _xyzOffset[_nbNeighbors].item(2) ;
  _dir[_nbNeighbors]       = Point3df(-M_SQRT1_2,0,M_SQRT1_2);
  _nbNeighbors++;

  _xyzOffset[_nbNeighbors] = Point3d(0,0,-1);
  _offset[_nbNeighbors]    =              _xyzOffset[_nbNeighbors].item(0) + 
                             _sliceSize * _xyzOffset[_nbNeighbors].item(2) ;
  _dir[_nbNeighbors]       = Point3df(0,0,-1);
  _nbNeighbors++;

  _xyzOffset[_nbNeighbors] = Point3d(0,0,1);
  _offset[_nbNeighbors]    =              _xyzOffset[_nbNeighbors].item(0) + 
                             _sliceSize * _xyzOffset[_nbNeighbors].item(2) ;
  _dir[_nbNeighbors]       = Point3df(0,0,1);
  _nbNeighbors++;

  _xyzOffset[_nbNeighbors] = Point3d(1,0,-1);
  _offset[_nbNeighbors]    =              _xyzOffset[_nbNeighbors].item(0) + 
                             _sliceSize * _xyzOffset[_nbNeighbors].item(2) ;
  _dir[_nbNeighbors]       = Point3df(M_SQRT1_2,0,-M_SQRT1_2);
  _nbNeighbors++;

  _xyzOffset[_nbNeighbors] = Point3d(1,0,0);
  _offset[_nbNeighbors]    =              _xyzOffset[_nbNeighbors].item(0) + 
                             _sliceSize * _xyzOffset[_nbNeighbors].item(2) ;
  _dir[_nbNeighbors]       = Point3df(1,0,0);
  _nbNeighbors++;

  _xyzOffset[_nbNeighbors] = Point3d(1,0,1);
  _offset[_nbNeighbors]    =              _xyzOffset[_nbNeighbors].item(0) + 
                             _sliceSize * _xyzOffset[_nbNeighbors].item(2) ;
  _dir[_nbNeighbors]       = Point3df(M_SQRT1_2,0,M_SQRT1_2);
  _nbNeighbors++;
}


void Connectivity::setConnectivity_8_YZ()
{ 
  
  _offset                  = new int[8];
  _xyzOffset               = new Point3d[8];
  _dir                     = new Point3df[8];

  _xyzOffset[_nbNeighbors] = Point3d(0,-1,-1);
  _offset[_nbNeighbors]    =     _xSize * _xyzOffset[_nbNeighbors].item(1) + 
                             _sliceSize * _xyzOffset[_nbNeighbors].item(2) ;
  _dir[_nbNeighbors]       = Point3df(0,-M_SQRT1_2,-M_SQRT1_2);
  _nbNeighbors++;

  _xyzOffset[_nbNeighbors] = Point3d(0,-1,0);
  _offset[_nbNeighbors]    =     _xSize * _xyzOffset[_nbNeighbors].item(1) + 
                             _sliceSize * _xyzOffset[_nbNeighbors].item(2) ;
  _dir[_nbNeighbors]       = Point3df(0,-1,0);
  _nbNeighbors++;

  _xyzOffset[_nbNeighbors] = Point3d(0,-1,1);
  _offset[_nbNeighbors]    =     _xSize * _xyzOffset[_nbNeighbors].item(1) + 
                             _sliceSize * _xyzOffset[_nbNeighbors].item(2) ;
  _dir[_nbNeighbors]       = Point3df(0,-M_SQRT1_2,M_SQRT1_2);
  _nbNeighbors++;

  _xyzOffset[_nbNeighbors] = Point3d(0,0,-1);
  _offset[_nbNeighbors]    =     _xSize * _xyzOffset[_nbNeighbors].item(1) + 
                             _sliceSize * _xyzOffset[_nbNeighbors].item(2) ;
  _dir[_nbNeighbors]       = Point3df(0,0,-1);
  _nbNeighbors++;

  _xyzOffset[_nbNeighbors] = Point3d(0,0,1);
  _offset[_nbNeighbors]    =     _xSize * _xyzOffset[_nbNeighbors].item(1) + 
                             _sliceSize * _xyzOffset[_nbNeighbors].item(2) ;
  _dir[_nbNeighbors]       = Point3df(0,0,1);
  _nbNeighbors++;

  _xyzOffset[_nbNeighbors] = Point3d(0,1,-1);
  _offset[_nbNeighbors]    =     _xSize * _xyzOffset[_nbNeighbors].item(1) + 
                             _sliceSize * _xyzOffset[_nbNeighbors].item(2) ;
  _dir[_nbNeighbors]       = Point3df(0,M_SQRT1_2,-M_SQRT1_2);
  _nbNeighbors++;

  _xyzOffset[_nbNeighbors] = Point3d(0,1,0);
  _offset[_nbNeighbors]    =     _xSize * _xyzOffset[_nbNeighbors].item(1) + 
                             _sliceSize * _xyzOffset[_nbNeighbors].item(2) ;
  _dir[_nbNeighbors]       = Point3df(0,1,0);
  _nbNeighbors++;

  _xyzOffset[_nbNeighbors] = Point3d(0,1,1);
  _offset[_nbNeighbors]    =     _xSize * _xyzOffset[_nbNeighbors].item(1) + 
                             _sliceSize * _xyzOffset[_nbNeighbors].item(2) ;
  _dir[_nbNeighbors]       = Point3df(0,M_SQRT1_2,M_SQRT1_2);
  _nbNeighbors++;
}


void Connectivity::setConnectivity_18_XYZ()
{
  _offset      = new int[18];
  _xyzOffset   = new Point3d[18];
  _dir         = new Point3df[18];
  for (short x=-1;x<=1;x++)
    for (short y=-1;y<=1;y++)
      for (short z=-1;z<=1;z++)
	if ( (abs(x)+abs(y)+abs(z)) < 3 && (abs(x)+abs(y)+abs(z)) != 0 )
	  {
	    _xyzOffset[_nbNeighbors] = Point3d(x,y,z);
	    _offset[_nbNeighbors]  =  _xyzOffset[_nbNeighbors].item(0) +  
	      _xSize * _xyzOffset[_nbNeighbors].item(1) + 
	      _sliceSize * _xyzOffset[_nbNeighbors].item(2) ;
	    _dir[_nbNeighbors]     = Point3df((float)x, (float)y,
					      (float)z) 
	      / norm( Point3df((float)x, (float)y, (float)z) );
	    _nbNeighbors++;
	  }
}


void Connectivity::setConnectivity_26_XYZ()
{  
  _offset      = new int[26];
  _xyzOffset   = new Point3d[26];
  _dir         = new Point3df[26];
  for (short x=-1;x<=1;x++)
    for (short y=-1;y<=1;y++)
      for (short z=-1;z<=1;z++)
      { if ((abs(x)+abs(y)+abs(z))!=0)
        { _xyzOffset[_nbNeighbors] = Point3d(x,y,z);
          _offset[_nbNeighbors] =  _xyzOffset[_nbNeighbors].item(0) +  
                                _xSize     * _xyzOffset[_nbNeighbors].item(1) +
                                _sliceSize * _xyzOffset[_nbNeighbors].item(2) ;
          _dir[_nbNeighbors]    = Point3df((float)x,
                                                      (float)y,
                                                      (float)z) /
                                  norm(Point3df((float)x,
                                                           (float)y,
                                                           (float)z)); 
          _nbNeighbors++;
        }
      }
}


void Connectivity::setConnectivity_9_XY_Zminus()
{  
  _offset     = new int[9];
  _xyzOffset  = new Point3d[9];
  _dir        = new Point3df[9];
  for (short x=-1;x<=1;x++)
    for (short y=-1;y<=1;y++)
    { _xyzOffset[_nbNeighbors] = Point3d(x,y,-1);
      _offset[_nbNeighbors]  = _xyzOffset[_nbNeighbors].item(0) +  
                               _xSize     * _xyzOffset[_nbNeighbors].item(1) +
                               _sliceSize * (-1) ;
      _dir[_nbNeighbors]     = Point3df((float)x,
                                                   (float)y,
                                                   -1) /
                               norm(Point3df((float)x,
                                                        (float)y,
                                                        -1)); 
      _nbNeighbors++;
    }
}


void Connectivity::setConnectivity_9_XY_Zplus()
{ 
  _offset     = new int[9];
  _xyzOffset  = new Point3d[9];
  _dir        = new Point3df[9];
  for (short x=-1;x<=1;x++)
    for (short y=-1;y<=1;y++)
    { _xyzOffset[_nbNeighbors] = Point3d(x,y,+1);
      _offset[_nbNeighbors]    =              _xyzOffset[_nbNeighbors].item(0) +
                                 _xSize     * _xyzOffset[_nbNeighbors].item(1) +
                                 _sliceSize * (+1) ;
      _dir[_nbNeighbors]       = Point3df((float)x,
                                                    (float)y,
                                                    1) /
                                 norm(Point3df((float)x,
                                                        (float)y,
                                                        1)); 
      _nbNeighbors++;
    }
}


void Connectivity::setConnectivity_9_XZ_Yminus()
{
  _offset     = new int[9];
  _xyzOffset  = new Point3d[9];
  _dir        = new Point3df[9];
  for (short x=-1;x<=1;x++)
    for (short z=-1;z<=1;z++)
    { _xyzOffset[_nbNeighbors] = Point3d(x,-1,z);
      _offset[_nbNeighbors]    =              _xyzOffset[_nbNeighbors].item(0) +
                                 _xSize     * (-1)                         + 
                                 _sliceSize * _xyzOffset[_nbNeighbors].item(2) ;
      _dir[_nbNeighbors]       = Point3df((float)x,
                                                    -1,
                                                    (float)z) /
                                 norm(Point3df((float)x,
                                                        -1,
                                                        (float)z));
      _nbNeighbors++;
    }
}


void Connectivity::setConnectivity_9_XZ_Yplus()
{  
  _offset     = new int[9];
  _xyzOffset  = new Point3d[9];
  _dir        = new Point3df[9];
  for (short x=-1;x<=1;x++)
    for (short z=-1;z<=1;z++)
    { _xyzOffset[_nbNeighbors] = Point3d(x,+1,z);
      _offset[_nbNeighbors]    =              _xyzOffset[_nbNeighbors].item(0) +
                                 _xSize     * (+1)                         + 
                                 _sliceSize * _xyzOffset[_nbNeighbors].item(2) ;
      _dir[_nbNeighbors]       = Point3df((float)x,
                                                    1,
                                                    (float)z) /
                                 norm(Point3df((float)x,
                                                        1,
                                                        (float)z));
      _nbNeighbors++;
    }
}


void Connectivity::setConnectivity_9_YZ_Xminus()
{
  _offset     = new int[9];
  _xyzOffset  = new Point3d[9];
  _dir        = new Point3df[9];
  for (short y=-1;y<=1;y++)
    for (short z=-1;z<=1;z++)
    { _xyzOffset[_nbNeighbors] = Point3d(-1,y,z);
      _offset[_nbNeighbors]    =              (-1)                         +  
                                 _xSize     * _xyzOffset[_nbNeighbors].item(1) +
                                 _sliceSize * _xyzOffset[_nbNeighbors].item(2) ;
      _dir[_nbNeighbors]       = Point3df(-1,
                                                    (float)y,
                                                    (float)z) /
                                 norm(Point3df(-1,
                                                        (float)y,
                                                        (float)z));
      _nbNeighbors++;
    }
}


void Connectivity::setConnectivity_9_YZ_Xplus()
{
  _offset     = new int[9];
  _xyzOffset  = new Point3d[9];
  _dir        = new Point3df[9];
  for (short y=-1;y<=1;y++)
    for (short z=-1;z<=1;z++)
    { _xyzOffset[_nbNeighbors] = Point3d(+1,y,z);
      _offset[_nbNeighbors]    =              (+1)                         +  
                                 _xSize     * _xyzOffset[_nbNeighbors].item(1) +
                                 _sliceSize * _xyzOffset[_nbNeighbors].item(2) ;
      _dir[_nbNeighbors]       = Point3df(1,
                                                    (float)y,
                                                    (float)z) /
                                 norm(Point3df(1,
                                                        (float)y,
                                                        (float)z));
      _nbNeighbors++;
    }
}


void Connectivity::setConnectivity_5_XminusY()
{
  _offset      = new int[5];
  _xyzOffset   = new Point3d[5];
  _dir         = new Point3df[5];
  for (short x=-1;x<=0;x++)
    for (short y=-1;y<=1;y++)
      { if ((abs(x)+abs(y))!=0)
        { _xyzOffset[_nbNeighbors] = Point3d(x,y,0);
          _offset[_nbNeighbors]    =  _xyzOffset[_nbNeighbors].item(0) +  
                                     _xSize * _xyzOffset[_nbNeighbors].item(1) ;
          _dir[_nbNeighbors]       = Point3df((float)x,
                                                        (float)y,
                                                        0) /
                                     norm(Point3df((float)x,
                                                            (float)y,
                                                            0));
          _nbNeighbors++;
        }
      }
}


void Connectivity::setConnectivity_5_XplusY()
{
  _offset      = new int[5];
  _xyzOffset   = new Point3d[5];
  _dir         = new Point3df[5];
  for (short x=0;x<=1;x++)
    for (short y=-1;y<=1;y++)
      { if ((abs(x)+abs(y))!=0)
        { _xyzOffset[_nbNeighbors] = Point3d(x,y,0);
          _offset[_nbNeighbors]    =  _xyzOffset[_nbNeighbors].item(0) +  
                                    _xSize * _xyzOffset[_nbNeighbors].item(1) ; 
          _dir[_nbNeighbors]       = Point3df((float)x,
                                                        (float)y,
                                                        0) /
                                     norm(Point3df((float)x,
                                                            (float)y,
                                                            0));
          _nbNeighbors++;
        }
      }
}


void Connectivity::setConnectivity_5_XYminus()
{
  _offset      = new int[5];
  _xyzOffset   = new Point3d[5];
  _dir         = new Point3df[5];
  for (short x=-1;x<=1;x++)
    for (short y=-1;y<=0;y++)
      { if ((abs(x)+abs(y))!=0)
        { _xyzOffset[_nbNeighbors] = Point3d(x,y,0);
          _offset[_nbNeighbors]    =  _xyzOffset[_nbNeighbors].item(0) +  
                                     _xSize * _xyzOffset[_nbNeighbors].item(1) ;
          _dir[_nbNeighbors]       = Point3df((float)x,
                                                        (float)y,
                                                        0) /
                                     norm(Point3df((float)x,
                                                            (float)y,
                                                            0));
          _nbNeighbors++;
        }
      }
}


void Connectivity::setConnectivity_5_XYplus()
{
  _offset      = new int[5];
  _xyzOffset   = new Point3d[5];
  _dir         = new Point3df[5];
  for (short x=-1;x<=1;x++)
    for (short y=0;y<=1;y++)
      { if ((abs(x)+abs(y))!=0)
        { _xyzOffset[_nbNeighbors] = Point3d(x,y,0);
          _offset[_nbNeighbors]    = _xyzOffset[_nbNeighbors].item(0) +  
                                     _xSize * _xyzOffset[_nbNeighbors].item(1) ; 
          _dir[_nbNeighbors]       = Point3df((float)x,
                                                        (float)y,
                                                        0) /
                                     norm(Point3df((float)x,
                                                            (float)y,
                                                            0));
          _nbNeighbors++;
        }
      }
}


void Connectivity::setConnectivity_5_XminusZ()
{
  _offset      = new int[5];
  _xyzOffset   = new Point3d[5];
  _dir         = new Point3df[5];
  for (short x=-1;x<=0;x++)
    for (short z=-1;z<=1;z++)
      { if ((abs(x)+abs(z))!=0)
        { _xyzOffset[_nbNeighbors] = Point3d(x,0,z);
          _offset[_nbNeighbors]    = _xyzOffset[_nbNeighbors].item(0) +  
                                 _sliceSize * _xyzOffset[_nbNeighbors].item(2) ;
          _dir[_nbNeighbors]       = Point3df((float)x,
                                                        0,
                                                        (float)z) /
                                     norm(Point3df((float)x,
                                                            0,
                                                            (float)z));
          _nbNeighbors++;
        }
      }
}


void Connectivity::setConnectivity_5_XplusZ()
{
  _offset      = new int[5];
  _xyzOffset   = new Point3d[5];
  _dir         = new Point3df[5];
  for (short x=0;x<=1;x++)
    for (short z=-1;z<=1;z++)
      { if ((abs(x)+abs(z))!=0)
        { _xyzOffset[_nbNeighbors] = Point3d(x,0,z);
          _offset[_nbNeighbors]    =   _xyzOffset[_nbNeighbors].item(0) +  
                                 _sliceSize * _xyzOffset[_nbNeighbors].item(2) ;
          _dir[_nbNeighbors]       = Point3df((float)x,
                                                        0,
                                                        (float)z) /
                                     norm(Point3df((float)x,
                                                            0,
                                                            (float)z));
          _nbNeighbors++;
        }
      }
}



void Connectivity::setConnectivity_5_XZminus()
{
  _offset      = new int[5];
  _xyzOffset   = new Point3d[5];
  _dir         = new Point3df[5];
  for (short x=-1;x<=1;x++)
    for (short z=-1;z<=0;z++)
      { if ((abs(x)+abs(z))!=0)
        { _xyzOffset[_nbNeighbors] = Point3d(x,0,z);
          _offset[_nbNeighbors]    = _xyzOffset[_nbNeighbors].item(0) +  
                                 _sliceSize * _xyzOffset[_nbNeighbors].item(2) ;
          _dir[_nbNeighbors]       = Point3df((float)x,
                                                        0,
                                                        (float)z) /
                                     norm(Point3df((float)x,
                                                            0,
                                                            (float)z));
          _nbNeighbors++;
        }
      }
}


void Connectivity::setConnectivity_5_XZplus()
{
  _offset      = new int[5];
  _xyzOffset   = new Point3d[5];
  _dir         = new Point3df[5];
  for (short x=-1;x<=1;x++)
    for (short z=0;z<=1;z++)
      { if ((abs(x)+abs(z))!=0)
        { _xyzOffset[_nbNeighbors] = Point3d(x,0,z);
          _offset[_nbNeighbors]    = _xyzOffset[_nbNeighbors].item(0) +  
                                 _sliceSize * _xyzOffset[_nbNeighbors].item(2) ;
          _dir[_nbNeighbors]       = Point3df((float)x,
                                                        0,
                                                        (float)z) /
                                     norm(Point3df((float)x,
                                                            0,
                                                            (float)z));
          _nbNeighbors++;
        }
      }
}


void Connectivity::setConnectivity_5_YminusZ()
{
  _offset      = new int[5];
  _xyzOffset   = new Point3d[5];
  _dir         = new Point3df[5];
  for (short y=-1;y<=0;y++)
    for (short z=-1;z<=1;z++)
      { if ((abs(y)+abs(z))!=0)
        { _xyzOffset[_nbNeighbors] = Point3d(0,y,z);
          _offset[_nbNeighbors]    = _xSize * _xyzOffset[_nbNeighbors].item(1) +
                                 _sliceSize * _xyzOffset[_nbNeighbors].item(2) ;
          _dir[_nbNeighbors]       = Point3df(0,
                                                        (float)y,
                                                        (float)z) /
                                     norm(Point3df(0,
                                                            (float)y,
                                                            (float)z));
          _nbNeighbors++;
        }
      }
}


void Connectivity::setConnectivity_5_YplusZ()
{
  _offset      = new int[5];
  _xyzOffset   = new Point3d[5];
  _dir         = new Point3df[5];
  for (short y=0;y<=1;y++)
    for (short z=-1;z<=1;z++)
      { if ((abs(y)+abs(z))!=0)
        { _xyzOffset[_nbNeighbors] = Point3d(0,y,z);
          _offset[_nbNeighbors]    = _xSize * _xyzOffset[_nbNeighbors].item(1) +
                                 _sliceSize * _xyzOffset[_nbNeighbors].item(2) ;
          _dir[_nbNeighbors]       = Point3df(0,
                                                        (float)y,
                                                        (float)z) /
                                     norm(Point3df(0,
                                                            (float)y,
                                                            (float)z));
          _nbNeighbors++;
        }
      }
}


void Connectivity::setConnectivity_5_YZminus()
{
  _offset      = new int[5];
  _xyzOffset   = new Point3d[5];
  _dir         = new Point3df[5];
  for (short y=-1;y<=1;y++)
    for (short z=-1;z<=0;z++)
      { if ((abs(y)+abs(z))!=0)
        { _xyzOffset[_nbNeighbors] = Point3d(0,y,z);
          _offset[_nbNeighbors]    = _xSize * _xyzOffset[_nbNeighbors].item(1) +
                                 _sliceSize * _xyzOffset[_nbNeighbors].item(2) ;
          _dir[_nbNeighbors]       = Point3df(0,
                                                        (float)y,
                                                        (float)z) /
                                     norm(Point3df(0,
                                                            (float)y,
                                                            (float)z));
          _nbNeighbors++;
        }
      }
}


void Connectivity::setConnectivity_5_YZplus()
{
  _offset      = new int[5];
  _xyzOffset   = new Point3d[5];
  _dir         = new Point3df[5];
  for (short y=-1;y<=1;y++)
    for (short z=0;z<=1;z++)
      { if ((abs(y)+abs(z))!=0)
        { _xyzOffset[_nbNeighbors] = Point3d(0,y,z);
          _offset[_nbNeighbors]    = _xSize * _xyzOffset[_nbNeighbors].item(1) +
                                 _sliceSize * _xyzOffset[_nbNeighbors].item(2) ;
          _dir[_nbNeighbors]       = Point3df(0,
                                                        (float)y,
                                                        (float)z) /
                                     norm(Point3df(0,
                                                            (float)y,
                                                            (float)z));
          _nbNeighbors++;
        }
      }
}


