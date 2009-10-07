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

/*
 *  Offset border class
 */
#ifndef AIMS_BORDER_BORDER_H
#define AIMS_BORDER_BORDER_H

#include <aims/config/aimsdata_config.h>

namespace aims {


/** The base class to manage borders on data containers
*/
class AIMSDATA_API Border
{  public: 
    /**@name Constructor and Destructor*/
    //@{

      /** The constructor precalculates offsets to speed-up
          access to data during loops
          @param dimx X dimension
          @param dimy Y dimension
          @param dimz Z dimension
          @param width border width
      */
      Border(int dimx,int dimy,int dimz,int width=0);

      ///
      virtual ~Border();
    //@}
    
    /** Usefull offsets for A.I.M.S. data */
    //@{
      /// Size of the border
      int borderWidth() const;
      /// Offset from the start of the allocated memory to the first point
      int oFirstPoint() const;
      /// Offset from the start of the allocated memory to the last point
      int oLastPoint() const;
      /// Offset between 2 consecutive lines
      int oLine() const;
      /// Offset between the end of a line and the start of the consecutive line
      int oPointBetweenLine() const;
      /// Offset between 2 consecutive slices
      int oSlice() const;
      /// Number of lines between 2 consecutive slices
      int oLineBetweenSlice() const;
      /// Offset between 2 consecutive volumes
      int oVolume() const;
      /// Number of slices between 2 consecutive volumes
      int oSliceBetweenVolume() const;
    //@}

 protected: 
    /**@name Data*/
    //@{

    /// Border width            
    int _borderWidth;
    /// Offset up to first point
    int _oFirstPoint;
    /// Offset up to last point
    int _oLastPoint;
    /// Length of a line
    int _oLine;
    /// Offset between two consecutive lines
    int _oPointBetweenLine;
    /// Length of a slice
    int _oSlice;
    /// Offset between two consecutive slices
    int _oLineBetweenSlice;
    /// Length of a volume
    int _oVolume;
    /// Offset between two consecutive volumes
    int _oSliceBetweenVolume;

    //@}

    /**@name Functions*/
    //@{
    /// Function that sets up all protected datas
    void _setBorder(int dimx,int dimy,int dimz,int width);
    //@}

};


inline
Border::Border(int dimx,int dimy,int dimz,int width)
{
    _setBorder(dimx,dimy,dimz,width);
}


inline
Border::~Border()
{
}


inline
int Border::borderWidth() const
{
    return _borderWidth;
}


inline
int Border::oFirstPoint() const
{
    return _oFirstPoint;
}


inline
int Border::oLastPoint() const
{
    return _oLastPoint;
}


inline
int Border::oLine() const
{
    return _oLine;
}


inline
int Border::oPointBetweenLine() const
{
    return _oPointBetweenLine;
}


inline
int Border::oSlice() const
{
    return _oSlice;
}


inline
int Border::oLineBetweenSlice() const
{
    return _oLineBetweenSlice;
}


inline
int Border::oVolume() const
{
    return _oVolume;
}


inline
int Border::oSliceBetweenVolume() const
{
    return _oSliceBetweenVolume;
}


}


#endif
