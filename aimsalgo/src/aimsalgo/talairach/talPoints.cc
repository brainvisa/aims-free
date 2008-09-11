/* Copyright (c) 1995-2007 CEA
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

#include <aims/talairach/talPoints.h>


TalairachPoints::TalairachPoints( const Point3df& ac, const Point3df& pc,
                                  const Point3df& hemi )
  : m_ACmm( ac ), m_PCmm( pc ), m_Hemimm( hemi )
{
  m_AC = Point3d( (short)m_ACmm[0], (short)m_ACmm[1], (short)m_ACmm[2] ) ;
  m_PC = Point3d( (short)m_PCmm[0], (short)m_PCmm[1], (short)m_PCmm[2] ) ;
  m_Hemi = Point3d( (short)m_Hemimm[0], (short)m_Hemimm[1], (short)m_Hemimm[2] ) ;
}

TalairachPoints::TalairachPoints( const Point3d& ac, const Point3d& pc,
                                  const Point3d& hemi, float sx, float sy,
				  float sz )
  : m_AC( ac ), m_PC( pc ), m_Hemi( hemi )
{
  setSizeXYZ( sx, sy, sz );
}

void TalairachPoints::setSizeXYZ( float sx, float sy, float sz )
{
  Point3df res( sx, sy, sz );

  for ( int i=0; i<3; i++ )
    {
      m_ACmm[ i ] = (float)m_AC[ i ] * res[ i ];
      m_PCmm[ i ] = (float)m_PC[ i ] * res[ i ];
      m_Hemimm[ i ] = (float)m_Hemi[ i ] * res[ i ];
    }
}
