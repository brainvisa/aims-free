
%ModuleHeaderCode
#include <aims/mesh/curv.h>
%End

namespace aims
{

  class LaplacianWeights
  {
%TypeHeaderCode
#include <aims/mesh/curv.h>
%End
  };


  class TextureSmoothing
  {
%TypeHeaderCode
#include <aims/scalespace/meshDiffuse.h>
%End

  public:

    static void surfaceBlockSmooth( const std::string & latt_filename,
				    const std::string *val_filename,
				    const std::string & path_name, unsigned nscans,
				    float dt, unsigned iter, unsigned scans_block,
				    unsigned nodes_block, unsigned nnodes,
				    unsigned ncolPINV, const std::string *outfiles, const float HMAX,const float Umax);
%Docstring
    Gaussian diffusion along a surface.
    From C / Cmex soures by Alexandre Andrade, modified
    Parameters
    ----------
    latt_filename:
        name of the .pinv matrix and .neigh
    (neighbours array)
    val_filename array of .val files to use as textures
    path_name:
        directory to read .val files from
    nscans:
        number of scans (textures)
    dt:
        time increment for each iteration
    iter:
        nb of iterations to process (! ie total diff time /dt)
    scans_block:
        number of scans to process at a time in memory
    nodes_block:
        number of nodes in each block to process
    nnodes:
        number of nodes in the mesh
    ncolPINV:
        number of columns in the PINV matrix
    outfiles:
        file names of output textures to save
%End


    static void surfaceBlockSmooth( const std::string & latt_filename,
				    const TimeTexture_FLOAT & tex, float dt, unsigned iter,
				    unsigned ncolPINV, TimeTexture_FLOAT & outtex,
				    unsigned nodes_block, const float HMAX,const float Umax );

    static TimeTexture_FLOAT derivative( const std::string & latt_filename,
					  const Texture_FLOAT & tex,
					  unsigned ncolPINV );

    static Texture_FLOAT FiniteElementSmoothing(
      const Texture_FLOAT &inittex,
      float dt, float dur,
      float Smax,
      const aims::LaplacianWeights & weightLapl);
%Docstring
    Smoothing using finite element appraoch
    Smax is a threshold for the smoothed signal estimate
    dt is the time increment for each iteration
    dur is the duration of the heat equation
    !! dur is the TOTAL time of diffusion, not the nb of iteration as for surfaceBlockSmooth

    weightLapl must be previously be obrtained using AimsMeshWeightFiniteElementLaplacian() for the considered mesh.
%End

    static TimeTexture_FLOAT *FiniteElementSmoothing(
      const TimeTexture_FLOAT &inittex,
      float dt, float dur,
      float Smax,
      const aims::LaplacianWeights & weightLapl ) /Factory/;

    static Texture_FLOAT FiniteElementSmoothingWithSource(
      const Texture_FLOAT &inittex, const Texture_FLOAT &sourcetex,
      float dt, float dur,
      const aims::LaplacianWeights & weightLapl );

    static TimeTexture_FLOAT *FiniteElementSmoothingWithSource(
      const TimeTexture_FLOAT &inittex, const TimeTexture_FLOAT &sourcetex,
      float dt, float dur,
      const aims::LaplacianWeights & weightLapl );

  };

};


aims::LaplacianWeights AimsMeshWeightFiniteElementLaplacian( const AimsTimeSurface_3_VOID & mesh, const float Wmax);
%MethodCode
    const AimsSurface<3, Void> & mesh = a0->begin()->second;
    aims::LaplacianWeights *weights = new aims::LaplacianWeights(
        AimsMeshWeightFiniteElementLaplacian( mesh, a1 ) );
    sipRes = weights;
%End

