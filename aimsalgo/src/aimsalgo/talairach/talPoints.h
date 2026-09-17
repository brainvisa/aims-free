
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
