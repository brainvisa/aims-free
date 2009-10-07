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


#ifndef AIMS_SCALESPACE_MESHDIFFUSE_H
#define AIMS_SCALESPACE_MESHDIFFUSE_H

#include <aims/config/aimsalgo_config.h>
#include <aims/mesh/texture.h>
#include <aims/mesh/curv.h>
#include <string>

namespace aims
{


  class TextureSmoothing
  {

  public : 

    /**	Gaussian diffusion along a surface.
	From C / Cmex soures by Alexandre Andrade, modified
	\param	latt_filename name of the .pinv matrix and .neigh 
	(neighbours array)
	\param	val_filename array of .val files to use as textures
	\param	path_name directory to read .val files from
	\param	nscans number of scans (textures)
	\param	dt time increment for each iteration
	\param	iter nb of iterations to process (! ie total diff time /dt)
	\param	scans_block number of scans to process at a time in memory
	\param	nodes_block number of nodes in each block to process
	\param	nnodes number of nodes in the mesh
	\param	ncolPINV number of columns in the PINV matrix
	\param	outfiles file names of output textures to save
    */
    //From A. Andrade...
    AIMSALGO_API 
    static void surfaceBlockSmooth( const std::string & latt_filename, 
				    const std::string *val_filename, 
				    const std::string & path_name, unsigned nscans, 
				    float dt, unsigned iter, unsigned scans_block, 
				    unsigned nodes_block, unsigned nnodes, 
				    unsigned ncolPINV, const std::string *outfiles, const float HMAX,const float Umax);


    AIMSALGO_API 
    static void surfaceBlockSmooth( const std::string & latt_filename, 
				    const Texture1d & tex, float dt, unsigned iter, 
				    unsigned ncolPINV, Texture1d & outtex, 
				    unsigned nodes_block, const float HMAX,const float Umax );

    AIMSALGO_API 
    static TimeTexture<float> derivative( const std::string & latt_filename, 
					  const Texture<float> & tex, 
					  unsigned ncolPINV );
    //... end A. Andrade

    // Smoothing using finite element appraoch
    // Smax is a threshold for the smoothed signal estimate
    // dt is the time increment for each iteration
    // dur is the duration of the heat equation 
    // !! dur is the TOTAL time of diffusion, not the nb of iteration as for surfaceBlockSmooth
    AIMSALGO_API 
    static Texture<float> FiniteElementSmoothing( const Texture<float> &inittex, 
						  float dt, float dur,
						  float Smax,
						  const std::map<unsigned, std::set< std::pair<unsigned,float> > > & weightLapl);
    
    static Texture<float> FiniteElementSmoothingWithSource( const Texture<float> &inittex, const Texture<float> &sourcetex, 
							    float dt, float dur,
							    const std::map<unsigned, std::set< std::pair<unsigned,float> > > & weightLapl);
  private : 

    /**	Gaussian diffusion along a surface - subroutine.
	From C / Cmex soures by Alexandre Andrade, modified. This function 
	is called by \a surfaceBlockSmooth
    */
    AIMSALGO_API 
    static void tayd_core( const int *IND_NEIGHS, const float *PINV, 
			   const float *values, float dt, float *output, 
			   unsigned ncolPINV, unsigned nodes_iter, 
			   unsigned first_node, const float HMAX,const float Umax);


    /** Estimate local derivative - subroutine.
	From C / Cmex soures by Alexandre Andrade, modified. 
    */
    AIMSALGO_API 
    static TimeTexture<float> derivative_tayd_core(  const int *IND_NEIGHS, const float *PINV, 
						     const float *values, 
						     unsigned ncolPINV, unsigned nnodes );
 
  };
 
}


#endif
