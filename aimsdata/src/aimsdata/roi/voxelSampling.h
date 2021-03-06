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

#ifndef AIMS_ROI_VOXELSAMPLING_H
#define AIMS_ROI_VOXELSAMPLING_H

#include <aims/vector/vector.h>
#include <cartobase/smart/rcptr.h>
#include <string>

namespace aims
{
    ////////////////////
   //                //
  //  VoxelSampler  //
 //                //
////////////////////

class VoxelSampler : public carto::RCObject
{
public:
  virtual ~VoxelSampler();

  virtual void next() = 0;
  virtual bool isValid() const = 0;
  virtual void restart() = 0;
  virtual const Point3df &value() const = 0;
  virtual std::string info() const = 0;
};


    //////////////////////////
   //                      //
  //  SimpleVoxelSampler  //
 //                      //
//////////////////////////

class SimpleVoxelSampler : public VoxelSampler
{
public:
  
  SimpleVoxelSampler( int );
  virtual ~SimpleVoxelSampler();

  void next();
  bool isValid() const;
  void restart();
  const Point3df &value() const;
  std::string info() const;

private:

  Point3df offset;
  Point3df point;
};

} // namespace aims

#endif
