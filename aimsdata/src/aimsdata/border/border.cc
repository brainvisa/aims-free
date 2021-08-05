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
 *  Offset border class methods
 */
#include <aims/def/assert.h>
#include <aims/border/border.h>
using aims::Border;

void Border::_setBorder(int dimx,int dimy,int dimz,int width)
{
  ASSERT(dimx>=0 && dimy>=0 && dimz>=0 && width>=0);
  _borders[0] = _borders[1] =_borders[2] = _borders[3] = _borders[4] = _borders[5] = width;
  _oFirstPoint = ((dimx + 2 * width) * (dimy + 2 * width + 1) + 1) * width;
  _oLine       = dimx + 2 * width;
  _oPointBetweenLine = 2 * width;
  _oSlice      =  (dimy + 2 * width) * (dimx + 2 * width);
  _oLineBetweenSlice = 2 * width * (dimx + 2 * width);
  _oVolume     = (dimz + 2 * width) * (dimy + 2 * width) * (dimx + 2 * width);
  _oSliceBetweenVolume = 2 * width * _oSlice;
}

void Border::_setBorder(int dimx,int dimy,int dimz,
                        int bx1, int bx2,
                        int by1, int by2,
                        int bz1, int bz2)
{
  std::vector<int> borders(6);
  borders[0] = bx1;
  borders[1] = bx2;
  borders[2] = by1;
  borders[3] = by2;
  borders[4] = bz1;
  borders[5] = bz2;
  
  _setBorder(dimx, dimy, dimz, borders);
}

void Border::_setBorder(int dimx,int dimy,int dimz,
                        const std::vector<int> & borders)
{
  ASSERT(dimx>=0 && dimy>=0 && dimz>=0);
  
//   stridex = 1
//   stridey = dimx + bx1 + bx2;
//   stridez = stridey * (dimy + by1 + by2);
//   _oFirstPoint = bz1 * stridez + by1 * stridey + bx1;
  int s = std::min(borders.size(), 
                   _borders.size());
  for(int i=0; i<s; ++i) {
    ASSERT(borders[i] >= 0);
    _borders[i] = borders[i];
  }
  
  _oFirstPoint = (_borders[4] * (dimy + _borders[2] + _borders[3]) + _borders[2]) 
                 * (dimx + _borders[0] + _borders[1]) + _borders[0];
  _oLine       = dimx + _borders[0] + _borders[1];
  _oPointBetweenLine = _borders[0] + _borders[1];
  _oSlice      = (dimy + _borders[2] + _borders[3]) 
                 * (dimx + _borders[0] + _borders[1]);
  _oLineBetweenSlice = (_borders[2] + _borders[3]) 
                       * (dimx + _borders[0] + _borders[1]);
  _oVolume     = (dimz + _borders[4] + _borders[5]) 
                 * (dimy + _borders[2] + _borders[3]) 
                 * (dimx + _borders[0] + _borders[1]);
  _oSliceBetweenVolume = (_borders[4] + _borders[5]) * _oSlice;
}


const std::vector<int> & Border::borders() const
{
    return _borders;
}
