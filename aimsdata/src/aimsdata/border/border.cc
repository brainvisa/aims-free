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
 *  Offset border class methods
 */
#include <aims/def/assert.h>
#include <aims/border/border.h>
using aims::Border;

void Border::_setBorder(int dimx,int dimy,int dimz,int width)
{
  ASSERT(dimx>=0 && dimy>=0 && dimz>=0 && width>=0);
  _borderWidth = width;
  _oFirstPoint = ((dimx + 2 * width) * (dimy + 2 * width + 1) + 1) * width;
  _oLine       = dimx + 2 * width;
  _oPointBetweenLine = 2 * width;
  _oSlice      =  (dimy + 2 * width) * (dimx + 2 * width);
  _oLineBetweenSlice = 2 * width * (dimx + 2 * width);
  _oVolume     = (dimz + 2 * width) * (dimy + 2 * width) * (dimx + 2 * width);
  _oSliceBetweenVolume = 2 * width * _oSlice;
  _oLastPoint  = _oVolume - _oFirstPoint - 1;
}
