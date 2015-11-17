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
#include <cmath>
#ifdef isnan
#warning isnan declared
#endif
#include <cartobase/config/cartobase_config.h>
#include <aims/getopt/getopt2.h>
#include <aims/utility/utility_g.h>
#include <aims/mesh/mesh_g.h>
#include <aims/io/io_g.h> 
#include <cartobase/exception/file.h>
#include <stdio.h>
#include <stdlib.h>

using namespace aims;
using namespace carto;
using namespace std;

//#if defined( __APPLE__ ) && !defined( isnan )
//#define isnan __isnan
//#endif

// I added a namespace, just in case.
namespace toro {

const double  PI = 3.141592653589793;

//-----------------------------------------------------------------------------
// surface_coordinates.c
//
// Included Roberto Toro's source file. Removed #pragma. Added (char *) before
// a calloc() because g++ complains of invalid conversion from void * to char*.
//-----------------------------------------------------------------------------

// surface coordinates over a mesh through the Laplace-Beltrami equation
// Roberto Toro, 2003

#define SIZESTACK	32

//==============================================
// Structures
//==============================================
typedef struct {
	int	a;
	int	b;
}int2D, *int2DPtr;

typedef struct {
	float	x;
	float	y;
	float	z;
}float3D, *float3DPtr,**float3DHandle;

typedef struct{
	int	a;
	int	b;
	int	c;
}int3D,*int3DPtr;

typedef struct {
	float3D	siz;
	float3D	ide;
}rect3D, *rect3DPtr;

typedef struct {
	int		n;
	int		t[SIZESTACK];
}NTriRec, *NtriPtr;

typedef struct {
	int		n;
	int		e[SIZESTACK];
}NEdgeRec, *NEdgePtr;

typedef struct {
	int	nd;		// number of data
	int	sd;		// size in bytes of each datum
	short	id;		// data ID
	char	name[256];	// data name
	
	char	*data;		// data handle
	void*	xcontainer;	// extra data container
}ContainerRec,*ContainerPtr;

typedef struct {
	int		np;
	int		nt;
	float3DPtr	p;
	int3DPtr	t;
	
	float3D		center;
	rect3D		bbox;
	char		name[256];

	void*		xcontainer;	// extra data container
}MeshRec, *MeshPtr;

//==============================================
// Prototypes
//==============================================
// surface coordinates for meshes
void surface_coordinates(MeshPtr m);

// vector algebra
double	norm3D(float3D a);
double	dot3D(float3D a, float3D b);
float3D	cross3D(float3D a, float3D b);
float3D	r3D(float3D a, float3D b);
float3D	s3D(float3D a, float3D b);
float3D	esc3D(float3D a, float b);

// memory allocation
char	*cvector_new(int size);
void	cvector_dispose(char *v);
double	*dvector_new(int size);
void	dvector_dispose(double *v);
int		*ivector_new(int size);
void	ivector_dispose(int *v);
int2D	*i2vector_new(int size);
void	i2vector_dispose(int2D *v);

// mesh handling
int	new_mesh(MeshPtr *m, float3D *p, int3D *t, int np, int nt);
void updatebbox(MeshPtr mesh);
float3D	*msh_getPointsPtr(MeshPtr m);
int3D	*msh_getTrianglesPtr(MeshPtr m);
int2D	*msh_getEdgesPtr(MeshPtr m);
NTriRec	*msh_getNeighborTrianglesPtr(MeshPtr m);
NEdgeRec *msh_getNeighborEdgesPtr(MeshPtr m);
float3D	*msh_getTexturePtr(MeshPtr m);
char* msh_getLabelPtr(MeshPtr m);

void msh_setNeighborTriangles(MeshPtr m);
void msh_setNeighborEdges(MeshPtr m);
void msh_setEdges(MeshPtr m);
void msh_storeEdge(int2D *E, int *n, int2D e);

void msh_setContainer(ContainerRec *c, int nd, int sd, short id,
                       const char *name);
void msh_addData(MeshPtr m, ContainerRec *c);
void msh_deleteData(MeshPtr m, ContainerRec c);
bool msh_findData(MeshPtr m, ContainerRec *c);

// laplace-beltrami equation
void msh_laplace_fe(MeshPtr m, double *U,int nDir, int *iDir, double *Dir,
					int nNeu, int *iNeu, double *Neu,
					int nMFC, int2D *eMFC, double *gMFC,double *hMFC);

// conjugated gradients
void msh_linbcgDN(int n,int en, double D[], double N[], int2D *E, double b[], double x[],
int itol, double tol, int itmax, int *iter, double *err);
double msh_snrmDN(int n, double sx[], int itol);
void msh_asolveDN(int n, double D[], double b[], double x[]);
void msh_atimesDN(int n, int en, double D[], double N[], int2D E[], double x[], double r[]);
void msh_SORDN(MeshPtr m, double D[], double N[], double B[], double X[],
				double tol, int itmax, int *iter, double *err);

// utility functions
float tTriArea(MeshPtr m, int nt);
int msh_esort(MeshPtr m, int indx, int *pstack, int *estack);
void dquicksort(int ir, int *arr, double *varr);

//==============================================
// Main
//==============================================
int main(int argc, char **argv)
{
        char	*file_verts, *file_trians, *file_coords;
	int		nv,nt;
	FILE	*verts, *trians, *coords;
	float	*p;
	int		*T;
	MeshPtr	mesh;
	float3D	*C;

	if (argc==1)
        {
            printf("\nGet surface coordinates for a mesh\n");
            printf("\nUsage: \t IN_fileVertices IN_fileTriangles IN_nvertices IN_ntriangles OUT_fileCoordinates\n\n");
        }
	
	// 1. get arguments
	file_verts=argv[1];
	file_trians=argv[2];
	nv=atoi(argv[3]);
	nt=atoi(argv[4]);
	file_coords=argv[5];
	
	// 2. open vertices and triangles
	verts=fopen(file_verts, "r");
	if (verts==NULL) exit(1);
	p=(float*)calloc(3*nv,sizeof(float));
	if (p==NULL)	exit(2);
	size_t res = fread(p,sizeof(float),3*nv,verts);
	fclose(verts);
        if( res != sizeof(float) * 3*nv )
          throw file_error( file_verts );
	
	trians=fopen(file_trians, "r");
	if (trians==NULL) exit(3);
	T=(int*)calloc(3*nt,sizeof(int));
	if (T==NULL)	exit(4);
	res = fread(T,sizeof(int),3*nt,trians);
	fclose(trians);
        if( res != sizeof(int) * 3*nt )
          throw file_error( file_trians );
	
	// 3. compute surface coordinates
	new_mesh(&mesh,(float3D*)p,(int3D*)T, nv, nt);
	surface_coordinates(mesh);
	
	// 4. save
	C=msh_getTexturePtr(mesh);
	coords=fopen(file_coords,"w");
	if (coords==NULL)	exit(4);
	fwrite(C,sizeof(float3D),(*mesh).np,coords);
	fclose(coords);
	
	// 5. free
	free(p);
	free(T);
	free(C);

	return 0;
}

//==============================================
// Surface coordinates
//==============================================
void surface_coordinates(MeshPtr m)
{
	float3D		*p;
	NTriRec		*NT;
	int2D		*E;
	float3D		*C;
	char		*M;
	int			i,j;
	int			ixmax,ixmin,iymax,iymin,izmax,izmin;
	
	int			nDir,iDir[2];
	double		Dir[2];
	
	int			nMFC,ms,sl;
	int2D		*eMFC;
	double		*gMFC,*hMFC,t;
	
	
	double		*X,*Y,*Z;
	int			*U;

	double		area,sum,X0,Y0,Z0;
	double		px,nx,py,ny,pz,nz;
        
        double		closeto0;

	if (m==NULL)	return;
	
	
	p = msh_getPointsPtr(m);
	NT = msh_getNeighborTrianglesPtr(m);
	E = msh_getEdgesPtr(m);
	C = msh_getTexturePtr(m);
	M = msh_getLabelPtr(m);
	
	X=dvector_new((*m).np);
	Y=dvector_new((*m).np);
	Z=dvector_new((*m).np);

	U=ivector_new((*m).np);

	
	// 0. calculate the total area
	area=0;
	for (i=0;i<(*m).np;i++)
	{
		for (j=0;j<NT[i].n;j++)
			area+=tTriArea(m,NT[i].t[j])/3.0;
	}
	
	//__________________________________________________________________________
	//1. Great circle X
	//__________________________________________________________________________
	//	1.1 find preliminary X poles
		printf("computing X great circle\n");
		ixmax=ixmin=0;
		for (i=0;i<(*m).np;i++) {
			if (p[ixmax].x<p[i].x)	ixmax=i;
			if (p[ixmin].x>p[i].x)	ixmin=i;
		}
	//	1.2 solve Laplace equation
		nDir=2;
		iDir[0]=ixmax;	iDir[1]=ixmin;
		Dir[0]= 1;		Dir[1]=-1;
		msh_laplace_fe(m,X, nDir,iDir,Dir, 0,0,0, 0,0,0,0);
	//	1.3. Find the great circle equidistant to the poles xmax,xmin
		for (i=0;i<(*m).np;i++)	U[i]=i;
		dquicksort((*m).np,U-1,X);
		sum=0;
		for (i=0;i<(*m).np;i++)
		{	for (j=0;j<NT[U[i]].n;j++)	sum+=tTriArea(m,NT[U[i]].t[j])/3.0;
			if (sum>area/2.0){	X0=X[U[i]];	break;	}
		}

	//__________________________________________________________________________
	//2. Great circle Z
	//__________________________________________________________________________
		printf("computing Z great circle\n");
	//	2.1 find preliminary Z poles
                closeto0=((*m).bbox.ide.y-(*m).bbox.siz.y)*0.1; // 10% of the total height
		izmax=izmin=-1;
		for (i=0;i<(*m).nt*3/2;i++)
			if ((X[E[i].a]-X0)*(X[E[i].b]-X0)<=0 && (fabs(p[E[i].a].y-(*m).center.y)<closeto0))
			{
				if (izmax==-1)	izmax=izmin=i;
				
				if (p[E[i].a].z>p[izmax].z)	izmax=E[i].a;
				if (p[E[i].b].z>p[izmax].z)	izmax=E[i].b;
				
				if (p[E[i].a].z<p[izmin].z)	izmin=E[i].a;
				if (p[E[i].b].z<p[izmin].z)	izmin=E[i].b;
			}
	//	2.2 solve Laplace equation
		nDir=2;
		iDir[0]=izmax;	iDir[1]=izmin;
		Dir[0]= 1;		Dir[1]=-1;
		msh_laplace_fe(m,Z, nDir,iDir,Dir, 0,0,0, 0,0,0,0);
	//	2.3. Find the great circle equidistant to the poles zmax,zmin
		for (i=0;i<(*m).np;i++)	U[i]=i;
		dquicksort((*m).np,U-1,Z);
		sum=0;
		for (i=0;i<(*m).np;i++)
		{	for (j=0;j<NT[U[i]].n;j++)	sum+=tTriArea(m,NT[U[i]].t[j])/3.0;
			if (sum>area/2.0){	Z0=Z[U[i]];	break;	}
		}

	//__________________________________________________________________________
	//3. Great circle Y
	//__________________________________________________________________________
		printf("computing Y great circle\n");
	//	3.1 Find antipodal pair Y
		iymax=iymin=-1;
		for (i=0;i<(*m).nt*3/2;i++)
		{
			if ( (X[E[i].a]-X0)*(X[E[i].b]-X0)<=0 &&
				(Z[E[i].a]-Z0)*(Z[E[i].b]-Z0)<=0 )
			{
				if (iymax==-1)	iymax=iymin=E[i].a;
				
				if (p[E[i].a].y>p[iymax].y)	iymax=E[i].a;
				if (p[E[i].b].y>p[iymax].y)	iymax=E[i].b;
				
				if (p[E[i].a].y<p[iymin].y)	iymin=E[i].a;
				if (p[E[i].b].y<p[iymin].y)	iymin=E[i].b;
			}
		}
	//	3.2 solve Laplace equation
		nDir=2;
		iDir[0]=iymax;	iDir[1]=iymin;
		Dir[0]= 1;		Dir[1]=-1;
		msh_laplace_fe(m,Y, nDir,iDir,Dir, 0,0,0, 0,0,0,0);
	//	3.3. Find the great circle equidistant to the poles zmax,zmin
		for (i=0;i<(*m).np;i++)	U[i]=i;
		dquicksort((*m).np,U-1,Y);
		sum=0;
		for (i=0;i<(*m).np;i++)
		{	for (j=0;j<NT[U[i]].n;j++)	sum+=tTriArea(m,NT[U[i]].t[j])/3.0;
			if (sum>area/2.0){	Y0=Y[U[i]];	break;	}
		}

	//__________________________________________________________________________
	// 4. Find X,Z antipodal pairs
	//__________________________________________________________________________
		ixmax=ixmin=izmax=izmin=-1;
		for (i=0;i<(*m).nt*3/2;i++)
		{
			if ( (Y[E[i].a]-Y0)*(Y[E[i].b]-Y0)<=0 &&
				(Z[E[i].a]-Z0)*(Z[E[i].b]-Z0)<=0 )
			{
				if (ixmax==-1)	ixmax=ixmin=E[i].a;
				
				if (p[E[i].a].x>p[ixmax].x)	ixmax=E[i].a;
				if (p[E[i].b].x>p[ixmax].x)	ixmax=E[i].b;
				
				if (p[E[i].a].x<p[ixmin].x)	ixmin=E[i].a;
				if (p[E[i].b].x<p[ixmin].x)	ixmin=E[i].b;
			}
			if ( (X[E[i].a]-X0)*(X[E[i].b]-X0)<=0 &&
				(Y[E[i].a]-Y0)*(Y[E[i].b]-Y0)<=0 )
			{
				if (izmax==-1)	izmax=izmin=E[i].a;
				
				if (p[E[i].a].z>p[izmax].z)	izmax=E[i].a;
				if (p[E[i].b].z>p[izmax].z)	izmax=E[i].b;
				
				if (p[E[i].a].z<p[izmin].z)	izmin=E[i].a;
				if (p[E[i].b].z<p[izmin].z)	izmin=E[i].b;
			}
		}
	//__________________________________________________________________________
	// 5. Set the Laplace coordinate system with
	//		Dirichlet boundaries at the X,Y,Z antipodal pairs
	//		and Multifreedom constrains at the X,Y,Z great circles
	//__________________________________________________________________________
	//	5.1 X coordinates
		printf("computing X coordinates\n");
		// multifreedom constraints
		nMFC=0;
		for (i=0;i<(*m).nt*3/2;i++)
			if ((X[E[i].a]-X0)*(X[E[i].b]-X0)<=0)	nMFC++;
		eMFC=i2vector_new(nMFC);
		gMFC=dvector_new(nMFC);
		hMFC=dvector_new(nMFC);
		nMFC=0;
		for (i=0;i<(*m).nt*3/2;i++)
			if ((X[E[i].a]-X0)*(X[E[i].b]-X0)<=0)
			{
				// slave is the point with coordinate closer to the intersection
				// to avoid an eventual zero division
				// t is the parametric intersection point from slave to master
				// (if t=0, this turns to be a Dirichlet condition)
				// the edge is stored slave (t=0) first
				if (fabs(X[E[i].a]-X0)>fabs(X[E[i].b]-X0)) {
					ms=E[i].a;
					sl=E[i].b;
				} else {
					ms=E[i].b;
					sl=E[i].a;
				}
				t=(X[sl]-X0)/(X[sl]-X[ms]);
				
				eMFC[nMFC]=(int2D){sl,ms};
				gMFC[nMFC]=t/(t-1);
				nMFC++;
			}
		// dirichlet boundary
		nDir=0;
		iDir[nDir]=ixmax;	Dir[nDir]= 1;	nDir++;
		iDir[nDir]=ixmin;	Dir[nDir]=-1;	nDir++;
		// solve Laplace
		msh_laplace_fe(m,X,nDir,iDir,Dir,0,0,0,nMFC,eMFC,gMFC,hMFC);
		// dispose
		i2vector_dispose(eMFC);
		dvector_dispose(gMFC);
		dvector_dispose(hMFC);

	//	5.2 Y coordinates
		printf("computing Y coordinates\n");
		// multifreedom constraints
		nMFC=0;
		for (i=0;i<(*m).nt*3/2;i++)
			if ((Y[E[i].a]-Y0)*(Y[E[i].b]-Y0)<=0)	nMFC++;
		eMFC=i2vector_new(nMFC);
		gMFC=dvector_new(nMFC);
		hMFC=dvector_new(nMFC);
		nMFC=0;
		for (i=0;i<(*m).nt*3/2;i++)
			if ((Y[E[i].a]-Y0)*(Y[E[i].b]-Y0)<=0)
			{
				if (fabs(Y[E[i].a]-Y0)>fabs(Y[E[i].b]-Y0)) {
					ms=E[i].a;
					sl=E[i].b;
				} else {
					ms=E[i].b;
					sl=E[i].a;
				}
				t=(Y[sl]-Y0)/(Y[sl]-Y[ms]);
				
				eMFC[nMFC]=(int2D){sl,ms};
				gMFC[nMFC]=t/(t-1);
				nMFC++;
			}
		// dirichlet boundary
		nDir=0;
		iDir[nDir]=iymax;	Dir[nDir]= 1;	nDir++;
		iDir[nDir]=iymin;	Dir[nDir]=-1;	nDir++;
		// solve Laplace
		msh_laplace_fe(m,Y,nDir,iDir,Dir,0,0,0,nMFC,eMFC,gMFC,hMFC);
		// dispose
		i2vector_dispose(eMFC);
		dvector_dispose(gMFC);
		dvector_dispose(hMFC);
		
	//	5.3 Z coordinates
		printf("computing Z coordinates\n");
		// multifreedom constraints
		nMFC=0;
		for (i=0;i<(*m).nt*3/2;i++)
			if ((Z[E[i].a]-Z0)*(Z[E[i].b]-Z0)<=0)	nMFC++;
		eMFC=i2vector_new(nMFC);
		gMFC=dvector_new(nMFC);
		hMFC=dvector_new(nMFC);
		nMFC=0;
		for (i=0;i<(*m).nt*3/2;i++)
			if ((Z[E[i].a]-Z0)*(Z[E[i].b]-Z0)<=0)
			{
				if (fabs(Z[E[i].a]-Z0)>fabs(Z[E[i].b]-Z0)){
					ms=E[i].a;
					sl=E[i].b;
				} else {
					ms=E[i].b;
					sl=E[i].a;
				}
				t=(Z[sl]-Z0)/(Z[sl]-Z[ms]);
				
				eMFC[nMFC]=(int2D){sl,ms};
				gMFC[nMFC]=t/(t-1);
				nMFC++;
			}
		// dirichlet boundary
		nDir=0;
		iDir[nDir]=izmax;	Dir[nDir]= 1;	nDir++;
		iDir[nDir]=izmin;	Dir[nDir]=-1;	nDir++;	
		// solve Laplace
		msh_laplace_fe(m,Z,nDir,iDir,Dir,0,0,0,nMFC,eMFC,gMFC,hMFC);
		// dispose
		i2vector_dispose(eMFC);
		dvector_dispose(gMFC);
		dvector_dispose(hMFC);
		
		X0=Y0=Z0=0;

	//__________________________________________________________________________
	// 6. Display information
	//__________________________________________________________________________
	//	6.1 draw the X,Z,Y antipodal pairs
		M[ixmax]=M[ixmin]=M[iymax]=M[iymin]=M[izmax]=M[izmin]=6;

	//	6.2 calculate positive v/s negative area
		px=nx=py=ny=pz=nz=0;
		for (i=0;i<(*m).np;i++)
		{
			sum=0;
			for (j=0;j<NT[i].n;j++)	sum+=tTriArea(m,NT[i].t[j]);
			
			if (X[i]-X0>0)	px+=sum/3.0;
			if (X[i]-X0<0)	nx+=sum/3.0;

			if (Y[i]-Y0>0)	py+=sum/3.0;
			if (Y[i]-Y0<0)	ny+=sum/3.0;

			if (Z[i]-Z0>0)	pz+=sum/3.0;
			if (Z[i]-Z0<0)	nz+=sum/3.0;
		}
                printf("x: +a/-a = %f / %f\n", px, nx);
                printf("y: +a/-a = %f / %f\n", py, ny);
                printf("z: +a/-a = %f / %f\n", pz, nz);

	//	6.3. Set texture as coordinates
		for (i=0;i<(*m).np;i++)
		{
			C[i].x=2*X[i];
			C[i].y=2*Y[i];
			C[i].z=2*Z[i];
/*			C[i].x=48*angle_from_laplace(X[i])/(2*PI);//2*X[i];
			C[i].y=48*angle_from_laplace(Y[i])/(2*PI);//2*Y[i];
			C[i].z=48*angle_from_laplace(Z[i])/(2*PI);//2*Z[i];*/
		}
	
	//__________________________________________________________________________
	//7. Dispose
	//__________________________________________________________________________
	dvector_dispose(X);
	dvector_dispose(Y);
	dvector_dispose(Z);

	ivector_dispose(U);
}


//==============================================
// Vector algebra
//==============================================
double norm3D(float3D a)
{
    double	xx;
    
    xx= sqrt(pow(a.x,2)+pow(a.y,2)+pow(a.z,2));
    return xx;
}
double dot3D(float3D a, float3D b)
{
    double xx;
    
    xx=a.x*b.x + a.y*b.y + a.z*b.z;
    return xx ;
}

float3D cross3D(float3D a, float3D b)
{
    float3D	xx;
    
    xx.x = a.y*b.z - a.z*b.y;
    xx.y = a.z*b.x - a.x*b.z;
    xx.z = a.x*b.y - a.y*b.x;
    return xx;
}
float3D r3D(float3D a, float3D b)
{
    float3D xx;
    
    xx.x=a.x-b.x;
    xx.y=a.y-b.y;
    xx.z=a.z-b.z;
    return xx;
}
float3D s3D(float3D a, float3D b)
{
    float3D xx;
    
    xx.x=a.x+b.x;
    xx.y=a.y+b.y;
    xx.z=a.z+b.z;
    return xx;
}
float3D esc3D(float3D a, float b)
{
    float3D	xx;
    
    xx.x = a.x*b;
    xx.y = a.y*b;
    xx.z = a.z*b;
    return xx;
}

//==============================================
// Memory allocation
//==============================================
char* cvector_new(int size)
{
    char *v;

    v=(char *)calloc(size,sizeof(char));
    if (!v){ printf("allocation failure in cvector_new()\n");}
    return v;
}
void cvector_dispose(char *v)
{
    free(v);
}
double* dvector_new(int size)
{
    double *v;

    v=(double *)calloc(size,sizeof(double));
    if (!v){printf("allocation failure in dvector_new\n");}
    
    return v;
}
void dvector_dispose(double *v)
{
    free((char *)v);
}
int* ivector_new(int size)
{
    int *v;

    v=(int *)calloc(size,sizeof(int));
    if (!v){ printf("allocation failure in ivector_new\n");}
    
    return v;
}
void ivector_dispose(int *v)
{
    free((char *)v);
}
int2D* i2vector_new(int size)
{
    int2D *v;

    v=(int2D *)calloc(size,sizeof(int2D));
    if (!v){printf("allocation failure in i2vector_new"); }
    
    return v;
}
void i2vector_dispose(int2D *v)
{
    free((char *)v);
}

//==============================================
// Mesh handling
//==============================================
int new_mesh(MeshPtr *m, float3D *p, int3D *t, int np, int nt)
{
    *m = (MeshPtr)calloc(1,sizeof(MeshRec));
    if (*m==NULL)	exit(0);

    (**m).np = np;
    (**m).nt = nt;
    (**m).p = p;
    (**m).t = t;
        
    (**m).xcontainer=0;

    updatebbox(*m);
    (**m).center = esc3D(s3D((**m).bbox.ide,(**m).bbox.siz),0.5);
    
    return 0;
}
void updatebbox(MeshPtr mesh)
{
	int		i;
	float3D		*p;
	
	p = (*mesh).p;
	
	(*mesh).bbox.siz=p[0];
	(*mesh).bbox.ide=p[0];
	for (i=0;i<(*mesh).np;i++)
	{
		if (p[i].x<(*mesh).bbox.siz.x)
			(*mesh).bbox.siz.x=p[i].x;
		if (p[i].y<(*mesh).bbox.siz.y)
			(*mesh).bbox.siz.y=p[i].y;
		if (p[i].z<(*mesh).bbox.siz.z)
			(*mesh).bbox.siz.z=p[i].z;
		
		if (p[i].x>(*mesh).bbox.ide.x)
			(*mesh).bbox.ide.x=p[i].x;
		if (p[i].y>(*mesh).bbox.ide.y)
			(*mesh).bbox.ide.y=p[i].y;
		if (p[i].z>(*mesh).bbox.ide.z)
			(*mesh).bbox.ide.z=p[i].z;
	}
}
float3D* msh_getPointsPtr(MeshPtr m)
{
	return (*m).p;
}
int3D* msh_getTrianglesPtr(MeshPtr m)
{
	return (*m).t;
}
int2D* msh_getEdgesPtr(MeshPtr m)
{
	ContainerRec	c;

	msh_setContainer(&c,(*m).nt*3/2,sizeof(int2D),0,"edge");
	if (msh_findData(m,&c)==false){	msh_addData(m,&c); msh_setEdges(m);}
	
	return (int2D*)c.data;
}
NTriRec* msh_getNeighborTrianglesPtr(MeshPtr m)
{
	ContainerRec	c;
	NTriRec	*tr;

	msh_setContainer(&c,(*m).np,sizeof(NTriRec),0,"ntri");
	if (msh_findData(m,&c)==false)
	{	msh_addData(m,&c);
		msh_setNeighborTriangles(m);
	}
	tr=(NTriRec*)c.data;
	return tr;
}
NEdgeRec* msh_getNeighborEdgesPtr(MeshPtr m)
{
	ContainerRec	c;

	msh_setContainer(&c,(*m).np,sizeof(NEdgeRec),0,"nedg");
	if (msh_findData(m,&c)==false){	msh_addData(m,&c); msh_setNeighborEdges(m);}
	
	return (NEdgeRec*)c.data;
}
float3D* msh_getTexturePtr(MeshPtr m)
{
    ContainerRec	c;

    msh_setContainer(&c,(*m).np,sizeof(float3D),0,"txtr");
    if (msh_findData(m,&c)==false)	msh_addData(m,&c);
    
    return (float3D*)c.data;
}
char* msh_getLabelPtr(MeshPtr m)
{
	ContainerRec	c;

	msh_setContainer(&c,(*m).np,sizeof(char),0,"labl");
	if (msh_findData(m,&c)==false)	msh_addData(m,&c);
	
	return (char*)c.data;
}

void msh_setNeighborTriangles(MeshPtr m)
{
	int			i;
	int3D		*T;
	NTriRec		*NT;
	
	T = msh_getTrianglesPtr(m);
	NT = msh_getNeighborTrianglesPtr(m);
	
	for (i=0;i<(*m).np;i++)
		NT[i].n = 0;
			
	for (i=0;i<(*m).nt;i++)
	{
		NT[T[i].a].t[NT[T[i].a].n++] = i;
		NT[T[i].b].t[NT[T[i].b].n++] = i;
		NT[T[i].c].t[NT[T[i].c].n++] = i;
	}	
	
	//TEST
	for (i=0;i<(*m).np;i++)
		if (NT[i].n>=SIZESTACK)
			printf("more neighbors than what we can handle");
}
void msh_setNeighborEdges(MeshPtr m)
{
	int			i;
	int2D		*E;
	NEdgeRec	*NE;
	
	E = msh_getEdgesPtr(m);
	NE = msh_getNeighborEdgesPtr(m);
	
	for (i=0;i<(*m).np;i++)
		NE[i].n = 0;
		
	for (i=0;i<(*m).nt*3/2;i++)
	{
		NE[E[i].a].e[NE[E[i].a].n++] = i;
		NE[E[i].b].e[NE[E[i].b].n++] = i;
	}
}
void msh_setEdges(MeshPtr m)
{
	int		i,n;
	int		a,b,c;
	int3D	*T;
	int2D	*E;

	T = msh_getTrianglesPtr(m);
	E = msh_getEdgesPtr(m);
	
	n=0;
	for (i=0;i<(*m).nt;i++)
	{
		a=T[i].a;	b=T[i].b;	c=T[i].c;
		
		if (a<b)	msh_storeEdge(E,&n,(int2D){a,b});
		else	msh_storeEdge(E,&n,(int2D){b,a});
		
		if (b<c)	msh_storeEdge(E,&n,(int2D){b,c});
		else	msh_storeEdge(E,&n,(int2D){c,b});
		
		if (c<a)	msh_storeEdge(E,&n,(int2D){c,a});
		else	msh_storeEdge(E,&n,(int2D){a,c});
		
		if (n>(*m).nt*3/2)
			printf("more edges than what we can handle");
	}
}
void msh_storeEdge(int2D *E, int *n, int2D e)
{
	int	i,max,min;
	int	j;
	bool	ismin=false,ismax=false,isequal=false;
	
	max=(*n);min=0;i=(*n)/2;
	if ((*n)>0)
		do
		{
			if (E[i].a<e.a){	min=i;	ismin=true;}
			else
			if (E[i].a>e.a){	max=i;	ismax=true;}
			else
			if (E[i].b<e.b){	min=i;	ismin=true;}
			else
			if (E[i].b>e.b){	max=i;	ismax=true;}
			else
			{				isequal=true;break;}
			
			i = (max+min)/2;
		}
		while(((max-min)>1 || !ismin) && max>0);

	if (isequal==false)
	{
		i=max;
		for (j=*n;j>i;j--)	E[j]=E[j-1];
		E[i]=e;
		(*n)++;
	}
}


void msh_setContainer(ContainerRec *c, int nd, int sd, short id,
                       const char *name)
{
	int	i;
        
	(*c).nd=nd;				// number of data objects
	(*c).sd=sd;				// size in bytes of each data object
	(*c).id=id;				// identification number
	for (i=0;i<4;i++)
			(*c).name[i]=name[i];		// name
	(*c).name[4]=(char)0;
	
	(*c).xcontainer=0;
}
void msh_addData(MeshPtr m, ContainerRec *c)
{
	ContainerPtr	newxcH,xcH;
	void*		temp;
	
	newxcH=(ContainerPtr)calloc(1,sizeof(ContainerRec));
	
	(*c).data = (char *) calloc((*c).nd,(*c).sd);
	if ((*c).data==NULL)	printf("not enough memory");
	
	if ((*m).xcontainer)
	{
		xcH=(ContainerPtr)(*m).xcontainer;
		while((void*)xcH)
		{
			if ((*xcH).id>=(*c).id)	(*c).id++;

			temp=(*xcH).xcontainer;
			if (temp)	xcH=(ContainerPtr)temp;
			else		break;
		}
		(*xcH).xcontainer=(void*)newxcH;
	}
	else
		(*m).xcontainer=(void*)newxcH;
	
	*newxcH=*c;
}
void msh_deleteData(MeshPtr m, ContainerRec c)
{
    ContainerPtr	xcH,parent;
    void*			temp;
    
    if ((*m).xcontainer)
    {
        xcH=(ContainerPtr)(*m).xcontainer;
        if ((*xcH).id==c.id)
        {
            (*m).xcontainer=(*xcH).xcontainer;
            
            free((*xcH).data);
            free(xcH);
        }
        else	
        while((void*)xcH)
        {
            if ((*xcH).id==c.id)
            {
                (*parent).xcontainer=(*xcH).xcontainer;
                
                free((*xcH).data);
                free(xcH);
                break;
            }

            parent=xcH;
            temp=(*xcH).xcontainer;
            if (temp)	xcH=(ContainerPtr)temp;
            else		break;
        }
    }
}
bool msh_findData(MeshPtr m, ContainerRec *c)
{
	ContainerPtr	xcH;
	void*		temp;
	bool		found=false;
	bool		cmp;
	int		i;
	
	if ((*m).xcontainer)
	{
		xcH=(ContainerPtr)(*m).xcontainer;
		while((void*)xcH)
		{
			cmp=true;
            for (i=0;i<4;i++)
	            if ((*xcH).name[i]!=(*c).name[i])
				{
	                cmp=false;
	                break;
				}
            if (	((*c).id>0 && (*xcH).id==(*c).id) ||
                    ((*c).name[0]>0 && cmp))
            {
				*c=*xcH;
				found=true;
            }
			temp=(*xcH).xcontainer;
			if (temp)	xcH=(ContainerPtr)temp;
			else		break;
		}
	}
	return found;
}


//==============================================
// Laplace-Beltrami equation
//==============================================
void msh_laplace_fe(MeshPtr m, double *U,int nDir, int *iDir, double *Dir,
											int nNeu, int *iNeu, double *Neu,
											int nMFC, int2D *eMFC, double *gMFC,double *hMFC)
//	Solves the Laplace equation over a mesh.
//	Accepts Dirichlet, Neumann and inhomoegneous Multifreedom boundary conditions
//	input:	m: the mesh, U:vector for the resulting value in each point,
//			nDir: number of Dirichlet conditions, iDir[]: vector with the mesh point index for the Dirichlet conditions,
//			Dir[]: vector with the value of the Dirichlet condition,
//			nNeu: number of Neumann conditions, iNeu[]: vector with the mesh point index for the Neumann conditions,
//			Neu[]: vector with the value of the Neumann condition,
//			nMFC: number of Multifreedom conditions, eMFC[]: vector with the mesh edge index for the Multifreedom conditions,
//			mMFC[]: vector with the multiplicative value of the Multifreedom condition,
//			nMFC[]: vector with the additive value of the Multifreedom condition,
{
	float3D		*p;
	int2D		*E,*tempE;
	NEdgeRec	*NE;
	int			i,j,k,indx;
	float3D		a,b,c;
	float3D		ir,jr,is,js;
	int			pstack[SIZESTACK],estack[SIZESTACK];
	double		coef;
	double		*D,*N,*B;
	char		*M;
	int			iter=0;
	double		err=0;
	bool		isDirichlet;
	double		g,h;
	int			ms,sl;

	if (m==NULL)	return;
	
	p = msh_getPointsPtr(m);		// points of the mesh
	E = msh_getEdgesPtr(m);			// Edges of the mesh
	NE = msh_getNeighborEdgesPtr(m);// Neighbor Edges for each point

	D=dvector_new((*m).np);		// Diagonal
	N=dvector_new((*m).nt*3/2);	// Neighbor (non diagonal)
	B=dvector_new((*m).np);		// Boundary
	
	if (nMFC>0)
		M=cvector_new((*m).np);	// Mark to avoid double masters or slaves in MFC

	// 1. prepare the matrix
	// ---------------------
	//	1.1. border conditions
	//		1.1.a Dirichlet
			for (i=0;i<nDir;i++)
			{
				B[iDir[i]]=Dir[i];
				if (nMFC>0)
					M[iDir[i]]=1;
			}
	//		1.1.b Neumann
			for (i=0;i<nNeu;i++)
				B[iNeu[i]]+=Neu[i];

	//	1.2. coefficient matrix
			#define cot(x,y) dot3D(x,y)/norm3D(cross3D(x,y))
	for (i=0;i<(*m).np;i++)
	{
				isDirichlet=false;
				for (k=0;k<nDir;k++)
                                    if (i==iDir[k]){	isDirichlet=true;	break;	}
				
				if (isDirichlet==false)
				{
					msh_esort(m,i,pstack,estack);
					D[i]=0;
					for (j=0;j<NE[i].n;j++)
					{
						a=p[pstack[(j+2)%NE[i].n]];
						b=p[pstack[(j+1)%NE[i].n]];
						c=p[pstack[j]];
						
						if (E[estack[(j+1)%NE[i].n]].a!=i)	indx=E[estack[(j+1)%NE[i].n]].a;
						else					indx=E[estack[(j+1)%NE[i].n]].b;
						
						ir=r3D(p[i],a);	ir=esc3D(ir,1/norm3D(ir));
						jr=r3D(b,a);	jr=esc3D(jr,1/norm3D(jr));
						
						is=r3D(b,c);	is=esc3D(is,1/norm3D(is));
						js=r3D(p[i],c);	js=esc3D(js,1/norm3D(js));

						coef = 0.5*(cot(ir,jr)+cot(is,js));
						
						D[i]+=coef;

						isDirichlet=false;
						for (k=0;k<nDir;k++)
							if (indx==iDir[k]){	isDirichlet=true;	break;	}
						if (isDirichlet==false)
							N[estack[(j+1)%NE[i].n]]=-coef;
						else
							B[i]+=B[indx]*coef;
					}
				}
				else
					D[i]=1;
			}
			#undef cot

	//	1.3. multifreedom constraints of the form Usl=g*Ums+h
			if (nMFC>0)
			{
				tempE = i2vector_new((*m).nt*3/2);
				for (i=0;i<(*m).nt*3/2;i++)
					tempE[i]=E[i];

				for (i=0;i<nMFC;i++)
				if (M[eMFC[i].a]+M[eMFC[i].b]==0)	// (avoid modifying twice)
				{
					g=gMFC[i];
					h=hMFC[i];
					sl=eMFC[i].a;
					ms=eMFC[i].b;
					// 1st. Master Diagonal and Boundary absorves slave's.
					//      Slave Diagonal is set to 1
					D[ms]+=g*g*D[sl];
					B[ms]+=g*h*B[sl];
					D[sl]=1;
					// 2nd. Slave Neighbors adjoined to master's. Inhomogeneous
					//      conditions are substracted
					for (j=0;j<NE[sl].n;j++)
					{
						if (E[NE[sl].e[j]].a==sl)
						{	E[NE[sl].e[j]].a=ms;
							B[E[NE[sl].e[j]].b]-=h*N[NE[sl].e[j]];
						}
						else
						{	E[NE[sl].e[j]].b=ms;
							B[E[NE[sl].e[j]].a]-=h*N[NE[sl].e[j]];
						}
						
						N[NE[sl].e[j]]*=g;
					}
					// 3rd. Master and slave are marked out
					M[ms]=1;
					M[sl]=1;
				}
			}

	// 2. solve
	// ---------
	//	2.1 conjugate gradients in the mesh sparse matrix
		msh_linbcgDN((*m).np,(*m).nt*3/2, D,N,E,B, U, 2, 1e-10, 5*(*m).np, &iter, &err);
	//	2.2 if there are multifreedom constraints, solve for the slaves
		if (nMFC>0)
		{
			// solve for the Slaves
			for (i=0;i<nMFC;i++)
			{
				g=gMFC[i];
				h=hMFC[i];
				sl=eMFC[i].a;
				ms=eMFC[i].b;
			
				U[sl]=g*U[ms]+h;
			}
			
			// restore the Edges vector and dispose the temporal
			for (i=0;i<(*m).nt*3/2;i++)
				E[i]=tempE[i];
			i2vector_dispose(tempE);
		}	
			
	// 3. Dispose
	// ----------
		dvector_dispose(D);
		dvector_dispose(N);
		dvector_dispose(B);
		
		if (nMFC>0)
			cvector_dispose(M);
}

//==============================================
// Conjugated gradients
//==============================================
#define EPS 1.0e-18
void msh_linbcgDN(int n,int en, double D[], double N[], int2D *E, double b[], double x[], int itol, double tol, int itmax, int *iter, double *err)
// Solves A * x = b for x[1..n], given b[1..n], by the iterative biconjugate gradient method.
// On input x[1..n] should be set to an initial guess of the solution (or all zeros); itol is 1,2,3,
// or 4, specifying which convergence test is applied (see text); itmax is the maximum number
// of allowed iterations; and tol is the desired convergence tolerance. On output, x[1..n] is
// reset to the improved solution, iter is the number of iterations actually taken, and err is the
// estimated error. The matrix A is referenced only through the user-supplied routines atimes,
// which computes the product of either A or its transpose on a vector; and asolve, which solves
// A*x = b or A^T*x = b for some preconditioner matrix A (possibly the trivial diagonal part of A).
//
// RT:
// In our PDEs, for each entry of the A=D matrix just the neighbors are non-zero
// So, data in the D matrix is organized in the NT matrix.
{
	int		j;
	double		ak,akden,bk,bkden,bknum,bnrm,dxnrm,xnrm,zm1nrm,znrm;
	double		*p,*pp,*r,*rr,*z,*zz;//Double precision is a good idea in this routine.

	p=dvector_new(n);
	pp=dvector_new(n);
	r=dvector_new(n);
	rr=dvector_new(n);
	z=dvector_new(n);
	zz=dvector_new(n);
	
	//Calculate initial residual.
	*iter=0;
	msh_atimesDN(n,en,D,N,E,x,r);	// not transposed
									// Input to atimes is x[1..n], output is r[1..n];
									// the final 0 indicates that the matrix (not its
									// transpose) is to be used.
	for (j=0;j<n;j++) {
		r[j]=b[j]-r[j];
		rr[j]=r[j];
	}
// atimes(n,r,rr,0);				// Uncomment this line to get the "minimum resid-
									// ual" variant of the algorithm.
	if (itol == 1) {
		bnrm=msh_snrmDN(n,b,itol);
		msh_asolveDN(n,D,r,z);		// not transposed
									// Input to asolve is r[1..n], output is z[1..n];
									// the final 0 indicates that the matrix A (not
									// its transpose) is to be used.
	}
	else if (itol == 2) {
		msh_asolveDN(n,D,b,z);		// not transposed
		bnrm=msh_snrmDN(n,z,itol);
		msh_asolveDN(n,D,r,z);		// not transposed
	}
	else if (itol == 3 || itol == 4) {
		msh_asolveDN(n,D,b,z);		// not transposed
		bnrm=msh_snrmDN(n,z,itol);
		msh_asolveDN(n,D,r,z);		// not transposed
		znrm=msh_snrmDN(n,z,itol);
	} else printf("illegal itol in linbcg");
	while (*iter <= itmax) {		// Main loop.
		++(*iter);
		msh_asolveDN(n,D,rr,zz);		// transposed
									// Final 1 indicates use of transpose matrix A^T.
		for (bknum=0.0,j=0;j<n;j++) bknum += z[j]*rr[j];
		// Calculate coefficient bk and direction vectors p and pp.
		if (*iter == 1) {
			for (j=0;j<n;j++) {
				p[j]=z[j];
				pp[j]=zz[j];
			}
		}
		else {
			bk=bknum/bkden;
			for (j=0;j<n;j++) {
				p[j]=bk*p[j]+z[j];
				pp[j]=bk*pp[j]+zz[j];
			}
		}
                if (isnan(bknum))
                {
                    exit(99);
                }
		bkden=bknum;				// Calculate coefficient ak, new iterate x, and new
									// residuals r and rr.
		msh_atimesDN(n,en,D,N,E,p,z);		// not transposed
		for (akden=0.0,j=0;j<n;j++) akden += z[j]*pp[j];
		ak=bknum/akden;
		msh_atimesDN(n,en,D,N,E,pp,zz);	// transposed
		for (j=0;j<n;j++) {
			x[j] += ak*p[j];
			r[j] -= ak*z[j];
			rr[j] -= ak*zz[j];
		}
		msh_asolveDN(n,D,r,z);		// not transposed
									// Solve A*z = r and check stopping criterion.
		if (itol == 1)
			*err=msh_snrmDN(n,r,itol)/bnrm;
		else if (itol == 2)
			*err=msh_snrmDN(n,z,itol)/bnrm;
		else if (itol == 3 || itol == 4) {
			zm1nrm=znrm;
			znrm=msh_snrmDN(n,z,itol);
			if (fabs(zm1nrm-znrm) > EPS*znrm) {
				dxnrm=fabs(ak)*msh_snrmDN(n,p,itol);
				*err=znrm/fabs(zm1nrm-znrm)*dxnrm;
			} else {
				*err=znrm/bnrm;		// Error may not be accurate, so loop again.
				continue;
			}
			xnrm=msh_snrmDN(n,x,itol);
			if (*err <= 0.5*xnrm) *err /= xnrm;
			else {
				*err=znrm/bnrm;		// Error may not be accurate, so loop again.
				continue;
			}
		}
		if (*err <= tol) break;
	}
	
	dvector_dispose(p);
	dvector_dispose(pp);
	dvector_dispose(r);
	dvector_dispose(rr);
	dvector_dispose(z);
	dvector_dispose(zz);
}

double msh_snrmDN(int n, double sx[], int itol)
// Compute one of two norms for a vector sx[1..n], as signaled by itol. Used by linbcg.
{
	int i,isamax;
	double ans;
	if (itol <= 3) {
		ans = 0.0;
		for (i=0;i<n;i++) ans += sx[i]*sx[i];	// Vector magnitude norm.
		return sqrt(ans);
	} else {
		isamax=1;
		for (i=0;i<n;i++) {							// Largest component norm.
			if (sx[i] > fabs(sx[isamax])) isamax=i;
		}
		return fabs(sx[isamax]);
	}
}
void msh_asolveDN(int n, double D[], double b[], double x[])
{
	for (int i=0;i<n;i++)
		x[i] = D[i] != 0.0 ? b[i]/D[i] : b[i];
}
void msh_atimesDN(int n, int en, double D[], double N[], int2D E[], double x[], double r[])
{
	int	i;
	
	for (i=0;i<n;i++)
		r[i]=D[i]*x[i];
	
	for (i=0;i<en;i++)
	{
		r[E[i].a]+= N[i]*x[E[i].b];
		r[E[i].b]+= N[i]*x[E[i].a];
	}
}

void msh_SORDN(MeshPtr m, double D[], double N[], double B[], double X[], double tol, int itmax, int *iter, double *err)
{
	int				i,j,indx;
	double			resid,omega=1;
	int2D			*E;
	NEdgeRec		*NE;
	
	E = msh_getEdgesPtr(m);
	NE = msh_getNeighborEdgesPtr(m);

	for ((*iter)=0;(*iter)<itmax;(*iter)++)
	{
		(*err)=0.0;
		for (i=0;i<(*m).np;i++)
		{
			resid=B[i]-D[i]*X[i];
			for (j=0;j<NE[i].n;j++)
			{
				if (E[NE[i].e[j]].a!=i)	indx=E[NE[i].e[j]].a;
				else					indx=E[NE[i].e[j]].b;
				
				resid-=N[NE[i].e[j]]*X[indx];
			}
			X[i]=X[i]+omega*resid/D[i];

			(*err)+=fabs(resid);
		}
		if (*err<=tol)	break;
	}
}

//==============================================
// Utility functions
//==============================================
float tTriArea(MeshPtr m, int nt)
{
	float3D	*p;
	int3D	*t;
	float	aa;
	
	p = (*m).p;
	t = (*m).t;
	
	aa=norm3D( cross3D(r3D(p[t[nt].a],p[t[nt].c]),r3D(p[t[nt].b],p[t[nt].a])) )/2.0;

	return aa;
}
int msh_esort(MeshPtr m, int indx, int *pstack, int *estack)
{
	int			i,j,n;
	int			nt,ne;
	float3D		*p;
	int3D		*T;
	NTriRec		*NT;
	int2D		*E;
	NEdgeRec	*NE;
	int			tstack[SIZESTACK];
	int			val;
	bool		isNest;
	int			old_n;
	
	p = msh_getPointsPtr(m);
	T = msh_getTrianglesPtr(m);
	NT = msh_getNeighborTrianglesPtr(m);
	E = msh_getEdgesPtr(m);
	NE = msh_getNeighborEdgesPtr(m);
	
	nt = NT[indx].n;
	for (i=0;i<nt;i++)
		tstack[i]=NT[indx].t[i];

	//1. stack the first two vertices of the first triangle
	//	 find and store the first egde
	n=0;
	ne=0;
	
	if (T[tstack[0]].a==indx){	pstack[n++]=T[tstack[0]].b;
								pstack[n++]=T[tstack[0]].c;	}
	else
	if (T[tstack[0]].b==indx){	pstack[n++]=T[tstack[0]].c;
								pstack[n++]=T[tstack[0]].a;	}
	else
	if (T[tstack[0]].c==indx){	pstack[n++]=T[tstack[0]].a;
								pstack[n++]=T[tstack[0]].b;	}

	for (j=0;j<nt-1;j++)
		tstack[j]=tstack[j+1];
	
	for (j=0;j<NE[indx].n;j++)
	if (E[NE[indx].e[j]].a==pstack[0] || E[NE[indx].e[j]].b==pstack[0])
	{	estack[ne++]=NE[indx].e[j];	break;}
	for (j=0;j<NE[indx].n;j++)
	if (E[NE[indx].e[j]].a==pstack[1] || E[NE[indx].e[j]].b==pstack[1])
	{	estack[ne++]=NE[indx].e[j];	break;}
	
	//2. stack further points to the right
	do
	{
		isNest = false;
		old_n = n;
		for (i=0;i<nt;i++)
		{
			if (T[tstack[i]].a==pstack[n-1])
			{	val=T[tstack[i]].b;
				for (j=0;j<n;j++)	if (val==pstack[j])	isNest=true;
				if (!isNest)	pstack[n++]=val;
				break;
			}
			else
			if (T[tstack[i]].b==pstack[n-1])
			{	val=T[tstack[i]].c;
				for (j=0;j<n;j++)	if (val==pstack[j])	isNest=true;
				if (!isNest)	pstack[n++]=val;
				break;
			}
			else
			if (T[tstack[i]].c==pstack[n-1])
			{	val=T[tstack[i]].a;
				for (j=0;j<n;j++)	if (val==pstack[j])	isNest=true;
				if (!isNest)	pstack[n++]=val;
				break;
			}
		}
		for (j=i;j<nt-1;j++)
			tstack[j]=tstack[j+1];
		nt--;
		
		for (j=0;j<NE[indx].n;j++)
		if (E[NE[indx].e[j]].a==pstack[n-1] || E[NE[indx].e[j]].b==pstack[n-1])
		{	estack[ne++]=NE[indx].e[j];	break;}

	}while(n>old_n && nt>2);		// n==old_n => nest, nstack==1 => first_p==last_p
	
	return n;
}

#define SWAP(a,b) temp=(a); (a)=(b); (b)=temp;
#define MM 7
#define NSTACK 50
void dquicksort(int ir, int *arr, double *varr)
// slightly modified Quicksort from Numerical Recipes in C
// input: arr=index vector, varr= double value vector, ir=vector size
// output: arr sorted from min to max
{
	int	l=1;
	int	jstack=0, istack[NSTACK];
	int	a,temp;

	for (;;)
	{
		if (ir-l<MM)
		{
			for (int j=l+1; j<=ir; j++)
			{
				a=arr[j];
				int i=j-1;
				for (; i>=1; i--)
				{
					if ( varr[arr[i]]<=varr[a] )
						break;
					arr[i+1]=arr[i];
				}
				arr[i+1]=a;
			}
			if (jstack ==0)
				break;
			ir=istack[jstack--];
			l=istack[jstack--];
		}
		else
		{
			int k=(l+ir) >> 1;
			SWAP(arr[k],arr[l+1])
			if ( varr[arr[l+1]] > varr[arr[ir]] )
			{
				SWAP(arr[l+1],arr[ir])
			}
			if ( varr[arr[l]] > varr[arr[ir]] )
			{
				SWAP(arr[l],arr[ir])
			}
			if ( varr[arr[l+1]] > varr[arr[l]] )
			{
				SWAP(arr[l+1],arr[l])
			}
			int i=l+1;
			int j=ir;
			a=arr[l];
			for (;;)
			{
				do i++; while( varr[arr[i]] < varr[a] );
				do j--; while( varr[arr[j]] > varr[a] );
				if (j<i)
					break;
				SWAP(arr[i],arr[j]);
			}
			arr[l]=arr[j];
			arr[j]=a;
			jstack+=2;
			
			if (jstack>NSTACK)
			{
				printf("dquicksort\n");
				//NSTACK too small in sort
				return;
			}
			if (ir-i+1>=j-1)
			{
				istack[jstack]=ir;
				istack[jstack-1]=i;
				ir=j-1;
			}
			else
			{
				istack[jstack]=j-1;
				istack[jstack-1]=l;
				l=i;
			}
		}
	}
}

//-----------------------------------------------------------------------------
// end of surface_coordinates.c
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// mesh2Sphere.c
//
// Included Roberto Toro's source file. Removed #pragma and all that was
// already defined in surface_coordinates.c
//-----------------------------------------------------------------------------

// laplace-beltrami over a sphere (dirichlet conditions -1,+1 at the poles)
float g_em_harmonics[512]={	
							1.00000000000000, 0.96017605125000, 0.92035210250000, 0.88052815375000, 0.84070420500000,
							0.80088025625000, 0.76105630750000, 0.72123235875000, 0.68140841000000, 0.63897514000000,
							0.62566871000000, 0.61236228000000, 0.59905585000000, 0.58574942000000, 0.57244299000000,
							0.55913656000000, 0.54583013000000, 0.53564651666667, 0.52546290333333, 0.51527929000000,
							0.50628711750000, 0.49729494500000, 0.48830277250000, 0.47931060000000, 0.47138139500000,
							0.46345219000000, 0.45792816500000, 0.45240414000000, 0.44688011500000, 0.44135609000000,
							0.43426554000000, 0.42717499000000, 0.42022759000000, 0.41542266000000, 0.41061773000000,
							0.40581280000000, 0.40003754666667, 0.39426229333333, 0.38848704000000, 0.38426593000000,
							0.38222253000000, 0.37795896500000, 0.37369540000000, 0.36945697333333, 0.36521854666667,
							0.36098012000000, 0.35658319000000, 0.35218626000000, 0.34723115000000, 0.34416249000000,
							0.33990001500000, 0.33563754000000, 0.33207006333333, 0.32850258666667, 0.32493511000000,
							0.32192370000000, 0.32146204000000, 0.31658140000000, 0.31170076000000, 0.30911565666667,
							0.30653055333333, 0.30394545000000, 0.30085295500000, 0.29776046000000, 0.29439834000000,
							0.29217690500000, 0.28995547000000, 0.28685711500000, 0.28375876000000, 0.28073308000000,
							0.27770740000000, 0.27468172000000, 0.27407864000000, 0.26926082000000, 0.26799724000000,
							0.26432067000000, 0.26166349500000, 0.25900632000000, 0.25827509000000, 0.25490630000000,
							0.25299427000000, 0.25133610000000, 0.24879822000000, 0.24626034000000, 0.24356134000000,
							0.24126829000000, 0.23876484500000, 0.23626140000000, 0.23521683000000, 0.23280239000000,
							0.22945222000000, 0.22870363000000, 0.22493730000000, 0.22374172500000, 0.22254615000000,
							0.21962409000000, 0.21842812000000, 0.21609493000000, 0.21331905000000, 0.21119159000000,
							0.20906413000000, 0.20712914000000, 0.20519415000000, 0.20333605000000, 0.20297825000000,
							0.20077564000000, 0.19827932000000, 0.19584453000000, 0.19501199000000, 0.19313936000000,
							0.19126673000000, 0.18919508000000, 0.18781886000000, 0.18564902000000, 0.18381105000000,
							0.18305048000000, 0.18121995000000, 0.17867383000000, 0.17791472000000, 0.17537352000000,
							0.17465691000000, 0.17195807000000, 0.17067376000000, 0.16900049000000, 0.16778342000000,
							0.16517833000000, 0.16402040000000, 0.16255434000000, 0.16149811000000, 0.15997708000000,
							0.15716885000000, 0.15617490500000, 0.15518096000000, 0.15354230000000, 0.15145667000000,
							0.14999968000000, 0.14925636000000, 0.14708392000000, 0.14561006000000, 0.14431795000000,
							0.14213172000000, 0.14051592000000, 0.13889498000000, 0.13764985000000, 0.13640159000000,
							0.13446414000000, 0.13425836000000, 0.13162099000000, 0.13037430000000, 0.12912761000000,
							0.12793604000000, 0.12593231000000, 0.12499724000000, 0.12374388000000, 0.12301020000000,
							0.12118306000000, 0.11972722000000, 0.11814676000000, 0.11703368000000, 0.11491345000000,
							0.11397808000000, 0.11221533000000, 0.11134135000000, 0.11043078000000, 0.10835493000000,
							0.10684173000000, 0.10554413000000, 0.10516541000000, 0.10329884000000, 0.10216175000000,
							0.10074545000000, 0.09907018000000, 0.09862638000000, 0.09737607000000, 0.09577937000000,
							0.09430736000000, 0.09296854000000, 0.09255683000000, 0.09039403000000, 0.08934227000000,
							0.08876959000000, 0.08712226000000, 0.08530658000000, 0.08420894000000, 0.08377215000000,
							0.08165161000000, 0.08028018000000, 0.07969181000000, 0.07872528000000, 0.07686730000000,
							0.07597684000000, 0.07494645000000, 0.07289722000000, 0.07188097000000, 0.07035693000000,
							0.06941156000000, 0.06860622000000, 0.06716582000000, 0.06617359000000, 0.06479642000000,
							0.06378989000000, 0.06308818000000, 0.06191645000000, 0.06032592000000, 0.05920655000000,
							0.05760932000000, 0.05678081500000, 0.05595231000000, 0.05417852000000, 0.05361692000000,
							0.05191611000000, 0.05103591000000, 0.04998863000000, 0.04810179000000, 0.04666962000000,
							0.04629311000000, 0.04525017000000, 0.04362446000000, 0.04296560000000, 0.04160564000000,
							0.04027436000000, 0.03863633000000, 0.03823184000000, 0.03715204000000, 0.03549206000000,
							0.03502371000000, 0.03373144000000, 0.03238219000000, 0.03074061000000, 0.02994172000000,
							0.02917591000000, 0.02711291000000, 0.02648273500000, 0.02585256000000, 0.02458071000000,
							0.02330169000000, 0.02200439000000, 0.02121164000000, 0.01963169000000, 0.01805174000000,
							0.01803836000000, 0.01686032000000, 0.01560804000000, 0.01412341000000, 0.01310045000000,
							0.01207749000000, 0.01105453000000, 0.01003157000000, 0.00900861000000, 0.00780362000000,
							0.00650301666667, 0.00520241333333, 0.00390181000000, 0.00260120666667, 0.00130060333333,
  							0,-0.000750301111111, -0.00130060333333, -0.00260120666667, -0.00390181000000,
  							-0.00520241333333, -0.00650301666667, -0.00780362000000, -0.00900861000000, -0.01003157000000,
  							-0.01105453000000, -0.01207749000000, -0.01310045000000, -0.01412341000000, -0.01560804000000,
  							-0.01686032000000, -0.01803836000000, -0.01805174000000, -0.01963169000000, -0.02121164000000,
  							-0.02200439000000, -0.02330169000000, -0.02458071000000, -0.02585256000000, -0.02648273500000,
  							-0.02711291000000, -0.02917591000000, -0.02994172000000, -0.03074061000000, -0.03238219000000,
  							-0.03373144000000, -0.03502371000000, -0.03549206000000, -0.03715204000000, -0.03823184000000,
  							-0.03863633000000, -0.04027436000000, -0.04160564000000, -0.04296560000000, -0.04362446000000,
  							-0.04525017000000, -0.04629311000000, -0.04666962000000, -0.04810179000000, -0.04998863000000,
  							-0.05103591000000, -0.05191611000000, -0.05361692000000, -0.05417852000000, -0.05595231000000,
  							-0.05678081500000, -0.05760932000000, -0.05920655000000, -0.06032592000000, -0.06191645000000,
  							-0.06308818000000, -0.06378989000000, -0.06479642000000, -0.06617359000000, -0.06716582000000,
  							-0.06860622000000, -0.06941156000000, -0.07035693000000, -0.07188097000000, -0.07289722000000,
  							-0.07494645000000, -0.07597684000000, -0.07686730000000, -0.07872528000000, -0.07969181000000,
  							-0.08028018000000, -0.08165161000000, -0.08377215000000, -0.08420894000000, -0.08530658000000,
  							-0.08712226000000, -0.08876959000000, -0.08934227000000, -0.09039403000000, -0.09255683000000,
  							-0.09296854000000, -0.09430736000000, -0.09577937000000, -0.09737607000000, -0.09862638000000,
  							-0.09907018000000, -0.10074545000000, -0.10216175000000, -0.10329884000000, -0.10516541000000,
  							-0.10554413000000, -0.10684173000000, -0.10835493000000, -0.11043078000000, -0.11134135000000,
  							-0.11221533000000, -0.11397808000000, -0.11491345000000, -0.11703368000000, -0.11814676000000,
  							-0.11972722000000, -0.12118306000000, -0.12301020000000, -0.12374388000000, -0.12499724000000,
  							-0.12593231000000, -0.12793604000000, -0.12912761000000, -0.13037430000000, -0.13162099000000,
  							-0.13425836000000, -0.13446414000000, -0.13640159000000, -0.13764985000000, -0.13889498000000,
  							-0.14051592000000, -0.14213172000000, -0.14431795000000, -0.14561006000000, -0.14708392000000,
  							-0.14925636000000, -0.14999968000000, -0.15145667000000, -0.15354230000000, -0.15518096000000,
  							-0.15617490500000, -0.15716885000000, -0.15997708000000, -0.16149811000000, -0.16255434000000,
  							-0.16402040000000, -0.16517833000000, -0.16778342000000, -0.16900049000000, -0.17067376000000,
  							-0.17195807000000, -0.17465691000000, -0.17537352000000, -0.17791472000000, -0.17867383000000,
  							-0.18121995000000, -0.18305048000000, -0.18381105000000, -0.18564902000000, -0.18781886000000,
  							-0.18919508000000, -0.19126673000000, -0.19313936000000, -0.19501199000000, -0.19584453000000,
  							-0.19827932000000, -0.20077564000000, -0.20297825000000, -0.20333605000000, -0.20519415000000,
  							-0.20712914000000, -0.20906413000000, -0.21119159000000, -0.21331905000000, -0.21609493000000,
  							-0.21842812000000, -0.21962409000000, -0.22254615000000, -0.22374172500000, -0.22493730000000,
  							-0.22870363000000, -0.22945222000000, -0.23280239000000, -0.23521683000000, -0.23626140000000,
  							-0.23876484500000, -0.24126829000000, -0.24356134000000, -0.24626034000000, -0.24879822000000,
  							-0.25133610000000, -0.25299427000000, -0.25490630000000, -0.25827509000000, -0.25900632000000,
  							-0.26166349500000, -0.26432067000000, -0.26799724000000, -0.26926082000000, -0.27407864000000,
  							-0.27468172000000, -0.27770740000000, -0.28073308000000, -0.28375876000000, -0.28685711500000,
  							-0.28995547000000, -0.29217690500000, -0.29439834000000, -0.29776046000000, -0.30085295500000,
  							-0.30394545000000, -0.30653055333333, -0.30911565666667, -0.31170076000000, -0.31658140000000,
  							-0.32146204000000, -0.32192370000000, -0.32493511000000, -0.32850258666667, -0.33207006333333,
  							-0.33563754000000, -0.33990001500000, -0.34416249000000, -0.34723115000000, -0.35218626000000,
  							-0.35658319000000, -0.36098012000000, -0.36521854666667, -0.36945697333333, -0.37369540000000,
  							-0.37795896500000, -0.38222253000000, -0.38426593000000, -0.38848704000000, -0.39426229333333,
  							-0.40003754666667, -0.40581280000000, -0.41061773000000, -0.41542266000000, -0.42022759000000,
  							-0.42717499000000, -0.43426554000000, -0.44135609000000, -0.44688011500000, -0.45240414000000,
  							-0.45792816500000, -0.46345219000000, -0.47138139500000, -0.47931060000000, -0.48830277250000,
  							-0.49729494500000, -0.50628711750000, -0.51527929000000, -0.52546290333333, -0.53564651666667,
  							-0.54583013000000, -0.55913656000000, -0.57244299000000, -0.58574942000000, -0.59905585000000,
  							-0.61236228000000, -0.62566871000000, -0.63897514000000, -0.68140841000000, -0.72123235875000,
  							-0.76105630750000, -0.80088025625000, -0.84070420500000, -0.88052815375000, -0.92035210250000,
  							-0.96017605125000, -1.00000000000000};


//=========================================
// mesh to sphere
//=========================================

float em_angleFromLaplace(float laplace)
{
	float	l0,l1;
	float	angle;
	
	float	rpos,rneg;
	float	ex=0.39;
	
	int a0=510;
	int a1=511;

	for (int i=0; i<511; ++i)
		if (g_em_harmonics[i]<=laplace)
		{
			a0=i-1;
			a1=i;
			break;
		}
	
	l0=g_em_harmonics[a0];
	l1=g_em_harmonics[a1];
	
	//TEST
	rpos=sqrt(pow(sin(a0*PI/511.0),2)+pow(1-cos(a0*PI/511.0),2));
	rneg=sqrt(pow(sin(a0*PI/511.0),2)+pow(1+cos(a0*PI/511.0),2));
	l0= -(pow(rpos,ex)-pow(rneg,ex))/(pow(rpos,ex)+pow(rneg,ex));
	rpos=sqrt(pow(sin(a1*PI/511.0),2)+pow(1-cos(a1*PI/511.0),2));
	rneg=sqrt(pow(sin(a1*PI/511.0),2)+pow(1+cos(a1*PI/511.0),2));
	l1= -(pow(rpos,ex)-pow(rneg,ex))/(pow(rpos,ex)+pow(rneg,ex));

	angle=a0*PI/511.0 + (a1-a0)*(PI/511.0)*(laplace-l0)/(l1-l0);
	
	return angle;
}


float3D em_getPointFromSphericalCoordinate(float3D c)
{
    float	n;
    float3D	p;

    p.x=cos(em_angleFromLaplace(c.x/2.0));
    p.y=cos(em_angleFromLaplace(c.y/2.0));
    p.z=cos(em_angleFromLaplace(c.z/2.0));
    
    n=sqrt(p.x*p.x+p.y*p.y+p.z*p.z); //n=1; -> not normalized
	p=esc3D(p,1/n);
    
    return p;
}


void em_sphereFromTexture(MeshPtr m)
{
	float3D		*p;
	int3D		*T;
	float3D		*C;
	float		area,R;

	p=msh_getPointsPtr(m);
	T=msh_getTrianglesPtr(m);
	C=msh_getTexturePtr(m);
	
	area=0;
	for (int i=0;i<(*m).nt;i++)
		area+=tTriArea(m,i);
	area=area/3.0;
	
	R=sqrt(area/PI);
	
	for (int i=0; i<(*m).np; ++i)
	{
                p[i]=em_getPointFromSphericalCoordinate(C[i]);
                p[i]=esc3D(p[i],R);
		p[i]=s3D(p[i],(*m).center);
	}
		
}

//-----------------------------------------------------------------------------
// mesh2Sphere.c
//-----------------------------------------------------------------------------

// sphere -> 2D coord texture (Denis 02/01/2004)
Texture2d sphere2Texture( const AimsSurfaceTriangle & mesh, 
                          const Point3df & center )
{
  const vector<Point3df>	& vert = mesh.vertex();
  Texture2d			tex;
  vector<Point3df>::size_type	n = vert.size();
  float				u, v, r = ( vert[0] - center ).norm();

  tex[0].reserve( n );

  for ( vector<Point3df>::size_type i=0; i<n; ++i )
    {
      // parallel
#if defined(__GNUC__) && defined (__sun__)
      u = ::acos( ( vert[i] - center )[0] / r );
#else
      u = acos( ( vert[i] - center )[0] / r );
#endif
      // meridian
#if defined(__GNUC__) && defined (__sun__)
      v = ::acos( ( vert[i] - center )[1] / r );
#else
      v = acos( ( vert[i] - center )[1] / r );
#endif
      tex[0].push_back( Point2df( u, v ) );
    }

  return tex;
}

} // namespace toro


using namespace toro;

int main( int argc, const char* argv[] )
{ 
 int result = EXIT_SUCCESS;

  try
  {
    Reader< AimsSurfaceTriangle > meshReader;
    Writer< AimsSurfaceTriangle > meshWriter;
    Writer< Texture2d >		  texWriter;

     // option(s) management
    AimsApplication
    application( argc, argv,
                 "Build a spherical coordinate system on a mesh" );
    application.addOption( meshReader, "-i", "input mesh" );
    application.addOption( meshWriter, "-o", "result" );
    application.addOption( texWriter, "-t", "output 2D coord texture", true );
    application.initialize();

    //
    // read triangulation
    //
    cout << "reading triangulation   : " << flush;
    AimsSurfaceTriangle surface;
    meshReader >> surface;
    cout << "done" << endl;

    MeshPtr	mesh;
    new_mesh( &mesh,(float3D*) &surface.vertex()[0], (int3D*)
	    &surface.polygon()[ 0 ], surface.vertex().size(),
	    surface.polygon().size() );
    cout << "Surface coordinates" << endl;
	  surface_coordinates(mesh);
    cout << "Sphere from texture" << endl;
	  em_sphereFromTexture(mesh);

    // Remove normals
    surface.normal().clear();

    cout << "writing result : " << flush;
    meshWriter << surface;

    if ( !texWriter.fileName().empty() )
      {
        cout << "Making 2D coords texture" << endl;
        Texture2d	tex = sphere2Texture( surface, 
                                              Point3df( mesh->center.x, 
                                                        mesh->center.y, 
                                                        mesh->center.z ) );
        texWriter.write( tex );
      }

    cout << "done" << endl;
  }
  catch ( user_interruption& e )
  {
  }
  catch ( std::exception& e )
  {

    cerr << argv[ 0 ] << ": " << e.what() << endl;
    result = EXIT_FAILURE;

  }

  return result;
}
