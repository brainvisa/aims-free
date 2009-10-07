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


#ifndef AIMS_TALAIRACH_TALPOINTS_H
#define AIMS_TALAIRACH_TALPOINTS_H

#include <aims/vector/vector.h>


class TalairachPoints
{
public:

  TalairachPoints( const Point3df& pAC, 
		   const Point3df& pPC, 
		   const Point3df& pHemi );
  TalairachPoints( const Point3d& pAC=Point3d(0,0,0), 
		   const Point3d& pPC=Point3d(0,0,0), 
		   const Point3d& pHemi=Point3d(0,0,0),
                   float sx=1.0f, float sy=1.0f, float sz=1.0f );
  virtual ~TalairachPoints() { }

  void setAC( const Point3d& pt ) { m_AC = pt; }
  void setPC( const Point3d& pt ) { m_PC = pt; }
  void setHemi( const Point3d& pt ) { m_Hemi = pt; }

  void setSizeXYZ( float, float, float );
  
  Point3d& AC() { return m_AC; }
  const Point3d& AC() const { return m_AC; }
  Point3d& PC() { return m_PC; }
  const Point3d& PC() const { return m_PC; }
  Point3d& Hemi() { return m_Hemi; }
  const Point3d& Hemi() const { return m_Hemi; }

  Point3df& ACmm() { return m_ACmm; }
  const Point3df& ACmm() const { return m_ACmm; }
  Point3df& PCmm() { return m_PCmm; }
  const Point3df& PCmm() const { return m_PCmm; }
  Point3df& Hemimm() { return m_Hemimm; }
  const Point3df& Hemimm() const { return m_Hemimm; }

private:

  Point3d m_AC;
  Point3d m_PC;
  Point3d m_Hemi;

  Point3df m_ACmm;
  Point3df m_PCmm;
  Point3df m_Hemimm;
};


#endif
