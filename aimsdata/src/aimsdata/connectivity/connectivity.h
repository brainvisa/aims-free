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

#ifndef AIMS_CONNECTIVITY_CONNECTCLASS_H
#define AIMS_CONNECTIVITY_CONNECTCLASS_H

#include <aims/config/aimsdata_config.h>
#include <aims/vector/vector.h>

namespace aims
{

  /** Topology of a data container
   */
  class Connectivity
  {
  public:
    /// The different kinds of connectivity
    enum Type
      {
        CONNECTIVITY_4_XY,
        CONNECTIVITY_4_XZ,
        CONNECTIVITY_4_YZ,
        CONNECTIVITY_6_XYZ,
        CONNECTIVITY_8_XY,
        CONNECTIVITY_8_XZ,
        CONNECTIVITY_8_YZ,
        CONNECTIVITY_18_XYZ,
        CONNECTIVITY_26_XYZ,
        CONNECTIVITY_9_XY_Zminus,
        CONNECTIVITY_9_XY_Zplus,
        CONNECTIVITY_9_XZ_Yminus,
        CONNECTIVITY_9_XZ_Yplus,
        CONNECTIVITY_9_YZ_Xminus,
        CONNECTIVITY_9_YZ_Xplus,
        CONNECTIVITY_5_XminusY,
        CONNECTIVITY_5_XplusY,
        CONNECTIVITY_5_XYminus,
        CONNECTIVITY_5_XYplus,
        CONNECTIVITY_5_XminusZ,
        CONNECTIVITY_5_XplusZ,
        CONNECTIVITY_5_XZminus,
        CONNECTIVITY_5_XZplus,
        CONNECTIVITY_5_YminusZ,
        CONNECTIVITY_5_YplusZ,
        CONNECTIVITY_5_YZminus,
        CONNECTIVITY_5_YZplus,
      };

    Connectivity( int oline, int oslice, Type type);
    virtual ~Connectivity();
    
    /// Get the type of the connectivity
    Type type() const { return _type; }
    /// Get the number of neighbors for that connectivity
    int nbNeighbors() const { return _nbNeighbors; }
    /// Get the linear offset of the nth element
    int offset( int n ) const { return _offset[n]; }
    /// Get the X/Y/Z offsets of the nth element
    const Point3d& xyzOffset( int n ) const { return _xyzOffset[n]; }
    /// Get the normalized directive vector of the nth element
    const Point3df& dir( int n ) const { return _dir[n]; }

  Connectivity& 
  operator=(const Connectivity& other)
  {
    if( &other == this )
      return *this ;
    
    _xSize = other._xSize ;
    _sliceSize = other._sliceSize ;
    _type = other._type ;
    
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
    setConnectivity() ;
    return *this ;
  }

  protected: 
    int _xSize;
    int _sliceSize;
    /// Type of connectivity            
    Type _type;
    /// Number of neighbors
    int _nbNeighbors;
    /// Linear offset pointer
    int *_offset;
    /// xyz offset pointer
    Point3d* _xyzOffset;
    /// Normalized vectors for the different directions
    Point3df* _dir;

    /// Set the right connectivity according to the given type
    void setConnectivity();
    /// 4-neighbor connectivity in (xy) plane
    void setConnectivity_4_XY();
    /// 4-neighbor connectivity in (xz) plane
    void setConnectivity_4_XZ();
    /// 4-neighbor connectivity in (yz) plane
    void setConnectivity_4_YZ();
    /// 6-neighbor connectivity in (xyz) space
    void setConnectivity_6_XYZ();
    /// 8-neighbor connectivity in (xy) plane
    void setConnectivity_8_XY();
    /// 8-neighbor connectivity in (xz) plane
    void setConnectivity_8_XZ();
    /// 8-neighbor connectivity in (yz) plane
    void setConnectivity_8_YZ();
    /// 18-neighbor connectivity in (xyz) space
    void setConnectivity_18_XYZ();
    /// 26-neighbor connectivity in (xyz) space
    void setConnectivity_26_XYZ();
    /// 9-neighbor connectivity in (xyz) inferior in z space
    void setConnectivity_9_XY_Zminus();
    /// 9-neighbor connectivity in (xyz) superior in z space
    void setConnectivity_9_XY_Zplus();
    /// 9-neighbor connectivity in (xyz) inferior in y space
    void setConnectivity_9_XZ_Yminus();
    /// 9-neighbor connectivity in (xyz) superior in y space
    void setConnectivity_9_XZ_Yplus();
    /// 9-neighbor connectivity in (xyz) inferior in x space
    void setConnectivity_9_YZ_Xminus();
    /// 9-neighbor connectivity in (xyz) superior in x space
    void setConnectivity_9_YZ_Xplus();
    /// 5-neighbor connectivity in (xy) inferior in x plane
    void setConnectivity_5_XminusY();
    /// 5-neighbor connectivity in (xy) superior in x plane
    void setConnectivity_5_XplusY();
    /// 5-neighbor connectivity in (xy) inferior in y plane
    void setConnectivity_5_XYminus();
    /// 5-neighbor connectivity in (xy) superior in y plane
    void setConnectivity_5_XYplus();
    /// 5-neighbor connectivity in (xz) inferior in x plane
    void setConnectivity_5_XminusZ();
    /// 5-neighbor connectivity in (xz) superior in x plane
    void setConnectivity_5_XplusZ();
    /// 5-neighbor connectivity in (xz) inferior in z plane
    void setConnectivity_5_XZminus();
    /// 5-neighbor connectivity in (xz) superior in z plane
    void setConnectivity_5_XZplus();
    /// 5-neighbor connectivity in (yz) inferior in y plane
    void setConnectivity_5_YminusZ();
    /// 5-neighbor connectivity in (yz) superior in y plane
    void setConnectivity_5_YplusZ();
    /// 5-neighbor connectivity in (yz) inferior in z plane
    void setConnectivity_5_YZminus();
    /// 5-neighbor connectivity in (yz) superior in z plane
    void setConnectivity_5_YZplus();
  };

}


#endif
