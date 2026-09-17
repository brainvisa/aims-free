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
