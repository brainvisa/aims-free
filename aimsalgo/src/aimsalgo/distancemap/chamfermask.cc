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

#include <cstdlib>
#include <cstdlib>
#include <aims/distancemap/chamfer.h>
#include <aims/data/data.h>

static void AimsSplitMask (const AimsDistmapMask& in,
                           AimsDistmapMask& forward,AimsDistmapMask& backward);
static AimsDistmapMask AimsCreateSquareMask(const AimsData<short>& vol,
                                            int xsize,int ysize,int zsize,
                                            float mult_factor);
static AimsDistmapMask AimsCreateConnectivityMask(const AimsData<short>& in,
						  Connectivity::Type c);

static AimsDistmapMask AimsDestroyMultiple(const AimsDistmapMask& m);
static void AimsFillMaskEuclideanDistance(AimsDistmapMask& mask,
                                          const AimsData<short>& vol,
                                          float mult_factor);
static void AimsChangeMaskLength(AimsDistmapMask& m,int newl);
static void AimsFillMaskOffset(AimsDistmapMask& mask,
                               const AimsData<short>& vol);



/*---------------------- class AimsDistmapMaskPoint --------------------------*/

AimsDistmapMaskPoint::AimsDistmapMaskPoint()
{
}


AimsDistmapMaskPoint::AimsDistmapMaskPoint(const AimsDistmapMaskPoint& mask)
{
  x      = mask.x;
  y      = mask.y;
  z      = mask.z;
  Offset = mask.Offset;
  Dist   = mask.Dist;
}


AimsDistmapMaskPoint::~AimsDistmapMaskPoint()
{
}


/*----------------------- class AimsDistmapMask ------------------------------*/


AimsDistmapMask::AimsDistmapMask() 
{
  FirstPoint = 0;
}


AimsDistmapMask::AimsDistmapMask(int xsize,int ysize,int zsize,int length)
{
  xCubeSize  = xsize;
  yCubeSize  = ysize;
  zCubeSize  = zsize;
  Length     = length;
  FirstPoint = new AimsDistmapMaskPoint[length];
}


AimsDistmapMask::AimsDistmapMask(const AimsDistmapMask& mask)
{
  xCubeSize  = mask.xCubeSize;
  yCubeSize  = mask.yCubeSize;
  zCubeSize  = mask.zCubeSize;
  Length     = mask.Length;
  FirstPoint = new AimsDistmapMaskPoint[mask.Length];
  for (int i=0; i<mask.Length; ++i) {
    FirstPoint[i].x      = mask.FirstPoint[i].x;
    FirstPoint[i].y      = mask.FirstPoint[i].y;
    FirstPoint[i].z      = mask.FirstPoint[i].z;
    FirstPoint[i].Offset = mask.FirstPoint[i].Offset;
    FirstPoint[i].Dist   = mask.FirstPoint[i].Dist;
  }
}


AimsDistmapMask::~AimsDistmapMask() 
{ if (FirstPoint) delete[] FirstPoint; 
}


AimsDistmapMask& AimsDistmapMask::operator = (const AimsDistmapMask& mask)
{
  xCubeSize  = mask.xCubeSize;
  yCubeSize  = mask.yCubeSize;
  zCubeSize  = mask.zCubeSize;
  Length     = mask.Length;
  if (FirstPoint) delete[] FirstPoint;
  FirstPoint = new AimsDistmapMaskPoint[mask.Length];
  for (int i=0; i<mask.Length; ++i) {
    FirstPoint[i].x      = mask.FirstPoint[i].x;
    FirstPoint[i].y      = mask.FirstPoint[i].y;
    FirstPoint[i].z      = mask.FirstPoint[i].z;
    FirstPoint[i].Offset = mask.FirstPoint[i].Offset;
    FirstPoint[i].Dist   = mask.FirstPoint[i].Dist;
  }
  return *this;
}


/*----------------------------- Mask functions -------------------------------*/



void AimsCreateAndSplitConnectivityMask ( const AimsData<short> &vol,
				      Connectivity::Type connectivity,
				      AimsDistmapMask &forward,
				      AimsDistmapMask &backward
				      )
{
  ASSERT(vol.dimT()==1);
  AimsDistmapMask fullmask;

  fullmask = AimsCreateConnectivityMask(vol, connectivity);

  AimsSplitMask(fullmask,forward,backward);
}

void AimsCreateAndSplitCubicMask (const AimsData<short>& vol,
                                  int xsize,int ysize,int zsize,
                                  AimsDistmapMask& forward,
                                  AimsDistmapMask& backward,
                                  float mult_factor)
{
  ASSERT(vol.dimT()==1);

  AimsDistmapMask fullmask;

  fullmask = AimsCreateSquareMask (vol,xsize,ysize,zsize,mult_factor);
  AimsSplitMask(fullmask,forward,backward);
}

/*-------------------Static---- Mask functions ------------------------------*/
static AimsDistmapMask AimsCreateConnectivityMask(const AimsData<short>& in,
						  Connectivity::Type c)
{
  int                  sx=0, sy=0, sz=0, i;
  AimsDistmapMaskPoint *mpptr;
  
  
  Connectivity vc(in.oLine(), in.oSlice(), c); 
  // Parcours pour la détermination de la largeur du masque
  for ( i=vc.nbNeighbors(); i--; ) {
      if (abs((int)vc.xyzOffset(i).item(0))>sx) 
	sx = abs((int)vc.xyzOffset(i).item(0));
      if (abs((int)vc.xyzOffset(i).item(1))>sy) 
	sy = abs((int)vc.xyzOffset(i).item(1));
      if (abs((int)vc.xyzOffset(i).item(2))>sz) 
	sz = abs((int)vc.xyzOffset(i).item(2));
    }

  AimsDistmapMask newmask( sx, sy, sz, vc.nbNeighbors() );
  
  mpptr = newmask.FirstPoint;
  for ( i= vc.nbNeighbors(); i--; ) {
      mpptr->x = vc.xyzOffset(i).item(0);
      mpptr->y = vc.xyzOffset(i).item(1);
      mpptr->z = vc.xyzOffset(i).item(2);
      mpptr->Offset = vc.offset(i);
      mpptr->Dist = 1;
      mpptr++;
    }


  return( newmask );
}

static void AimsSplitMask (const AimsDistmapMask& in,
                           AimsDistmapMask& forward,AimsDistmapMask& backward)
{
  AimsDistmapMaskPoint *reader, *fwriter, *bwriter;

  forward.xCubeSize = backward.xCubeSize = in.xCubeSize;
  forward.yCubeSize = backward.yCubeSize = in.yCubeSize;
  forward.zCubeSize = backward.zCubeSize = in.zCubeSize;

  forward.Length  = 0;
  backward.Length = 0;
  reader = in.FirstPoint;

  int	i;
  for ( i=in.Length; i>0; --i) {
    if ((reader->z < 0)                   ||
        ((reader->z==0) && (reader->y<0)) ||
        ((reader->z==0) && (reader->y==0) && (reader->x<0)) )
	   forward.Length++;
    else 
      backward.Length++;
    reader++;
  }

  forward.FirstPoint  = new AimsDistmapMaskPoint[forward.Length];
  backward.FirstPoint = new AimsDistmapMaskPoint[backward.Length];

  reader = in.FirstPoint;
  fwriter = forward.FirstPoint;
  bwriter = backward.FirstPoint;
  for ( i=in.Length; i>0; --i) {
    if ((reader->z < 0) ||
        ((reader->z==0) && (reader->y<0)) ||
        ((reader->z==0) && (reader->y==0) && (reader->x<0)) )
    {
      fwriter->x      = reader->x;
      fwriter->y      = reader->y;
      fwriter->z      = reader->z;
      fwriter->Offset = reader->Offset;
      fwriter->Dist   = reader->Dist;
      fwriter++;
      reader++;
    }
    else
    {
      bwriter->x      = reader->x;
      bwriter->y      = reader->y;
      bwriter->z      = reader->z;
      bwriter->Offset = reader->Offset;
      bwriter->Dist   = reader->Dist;
      bwriter++;
      reader++;
    }
  }
}


static AimsDistmapMask AimsCreateSquareMask(const AimsData<short>& vol,
                                            int xsize,int ysize,int zsize,
                                            float mult_factor)
{
  ASSERT(vol.dimT()==1);

  AimsDistmapMaskPoint *mpptr, *fptr;
  int                 newl,sym[24],lsym;

  AimsDistmapMask newmask(xsize/2,ysize/2,zsize/2,
			  (xsize/2+1)*(ysize/2+1)*(zsize/2+1));
  
  mpptr = newmask.FirstPoint;
  newl  = 0;

  for (int x=0;x<=xsize/2;x++)
    for (int y=0;y<=ysize/2;y++)
      for (int z=0;z<=zsize/2;z++) {
        if ((x+y+z)!=0) {
	  mpptr->x = x;
          mpptr->y = y;
          mpptr->z = z;
          ++newl;
          ++mpptr;
	}
      }

  newmask.Length = newl;

  newmask = AimsDestroyMultiple(newmask);
  AimsFillMaskEuclideanDistance(newmask,vol,mult_factor);

  AimsDistmapMask finalmask(newmask.xCubeSize,newmask.yCubeSize,
                            newmask.zCubeSize,8*(newmask.Length));

  mpptr            = newmask.FirstPoint;
  fptr             = finalmask.FirstPoint;
  finalmask.Length = 0;
  int		i;

  for (newl=newmask.Length; newl>0; --newl) {
    sym[0] = mpptr->x;
    sym[1] = mpptr->y;
    sym[2] = mpptr->z;
    lsym = 1;
    if (sym[0]) {
      sym[3] = -sym[0];
      for ( i=1;i<3;i++) sym[i+3]=sym[i];
      lsym=2;
    }
    if (sym[1]) {
      for ( i=0;i<lsym;i++) sym[3*(lsym+i)+1] = -sym[1];
      for ( i=0;i<lsym;i++) sym[3*(lsym+i)]   =  sym[3*i];
      for ( i=0;i<lsym;i++) sym[3*(lsym+i)+2] =  sym[2];
      lsym *=2;
    }
    if (sym[2]) {
     for ( i=0;i<lsym;i++) sym[3*(lsym+i)+2] = -sym[2];
      for ( i=0;i<lsym;i++) sym[3*(lsym+i)]   =  sym[3*i];
      for ( i=0;i<lsym;i++) sym[3*(lsym+i)+1] =  sym[3*i+1];
      lsym *=2;
    }
    for ( i=0;i<lsym;i++)
    {
      fptr->x    = sym[3*i];
      fptr->y    = sym[3*i+1];
      fptr->z    = sym[3*i+2];
      fptr->Dist = mpptr->Dist;
      fptr++;
    }
    finalmask.Length += lsym;
    mpptr++;
  }

  AimsChangeMaskLength(finalmask,finalmask.Length);
  AimsFillMaskOffset(finalmask,vol);	

  return( finalmask);
}


static AimsDistmapMask AimsDestroyMultiple(const AimsDistmapMask& m)
{ AimsDistmapMaskPoint *ptr_base, *ptr_search;
  int  new_l, base;
  float qx,qy,qz,bingo;

  ptr_base = m.FirstPoint;
  for ( base=m.Length;base>0; base--)
  { ptr_base->Offset = 0;
    ptr_base++;
  }

  bingo=1000.;

  ptr_base = m.FirstPoint;
  for ( base=m.Length;base>0;base--)
  { if (ptr_base->Offset==0)
    { ptr_search = m.FirstPoint;
      for (int search=m.Length;search>0;search--)
      { if ((ptr_search != ptr_base) && (ptr_search->Offset==0))
        { if (ptr_base->z==0)
          { if (ptr_search->z==0) qz = bingo;
            else qz = -bingo;
          }
          else  qz = (float)(ptr_search->z) / (float)(ptr_base->z);
          if (ptr_base->y==0)
          { if (ptr_search->y==0) qy = bingo;
            else qy = -bingo;
          }
          else  qy = (float)(ptr_search->y) / (float)(ptr_base->y);
          if (ptr_base->x==0)
          { if (ptr_search->x==0) qx = bingo;
            else qx = -bingo;
          }
          else  qx = (float)(ptr_search->x) / (float)(ptr_base->x);
          if (((qz==qy) || (qz==bingo) || (qy==bingo)) &&
              ((qx==qy) || (qx==bingo) || (qy==bingo)) &&
              ((qz==qx) || (qz==bingo) || (qx==bingo))   )
            ptr_search->Offset = -1;
        }
        ptr_search++;
      }
    }
    ptr_base++;
  }

  new_l=0;
  ptr_base = m.FirstPoint;
  for ( base=m.Length;base>0;base--)
  { if (ptr_base->Offset==0) new_l++;
      ptr_base++;
  }

  AimsDistmapMask newmask(m.xCubeSize,m.yCubeSize,m.zCubeSize,new_l);
  ptr_base   = m.FirstPoint;
  ptr_search = newmask.FirstPoint;
  for ( base=m.Length;base>0;base--) {
    if (ptr_base->Offset==0) {
      ptr_search->x = ptr_base->x;
      ptr_search->y = ptr_base->y;
      ptr_search->z = ptr_base->z;
      ptr_search++;
    }
    ptr_base++;
  }
  return(newmask);
}


static void AimsFillMaskEuclideanDistance(AimsDistmapMask& mask,
                                          const AimsData<short>& vol,
                                          float mult_factor)
{
  ASSERT(vol.dimT()==1);

  AimsDistmapMaskPoint *ptr;
  float                euclid,x,y,z;

  ptr = mask.FirstPoint;

  for (int l=mask.Length;l>0;l--)
  {
    x = (float)ptr->x * vol.sizeX();
    y = (float)ptr->y * vol.sizeY();
    z = (float)ptr->z * vol.sizeZ();
    euclid = (float)sqrt((double)(x*x + y*y + z*z));
    ptr->Dist = (int)((double)(mult_factor * euclid) + 0.5);
    ptr++;
  }
}


static void AimsChangeMaskLength(AimsDistmapMask& m,int newl)
{
  AimsDistmapMaskPoint *in, *out;

  AimsDistmapMask newmask(m.xCubeSize,m.yCubeSize,m.zCubeSize,newl);
  in  = m.FirstPoint;
  out = newmask.FirstPoint;
  for (int i=newl;i>0;i--)
  {
    out->x    = in->x;
    out->y    = in->y;
    out->z    = in->z;
    out->Dist = in->Dist;
    in++;
    out++;
  }
  m = newmask;
}


static void AimsFillMaskOffset(AimsDistmapMask& mask,
                               const AimsData<short>& vol)
{
  ASSERT(vol.dimT()==1);

  AimsDistmapMaskPoint *ptr;

  ptr = mask.FirstPoint;
  for (int l=mask.Length;l>0;l--)
  { ptr->Offset = ptr->x + vol.oLine() * ptr->y + vol.oSlice() * ptr->z;
    ptr++;
  }
}
