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


#ifndef AIMS_ROI_CLUSTERARG_H
#define AIMS_ROI_CLUSTERARG_H

#include <aims/data/data.h>
#include <aims/connectivity/connectivity.h>
#include <float.h>

class Graph;

namespace aims
{

  class ClusterArgMaker
  {
  public:
    ClusterArgMaker( const std::string & outfilename, Connectivity::Type conn 
		     = Connectivity::CONNECTIVITY_18_XYZ, 
		     int minsz = 0, const std::string & mtxfile = "", 
		     bool mkmesh = true, float maxclear = 5., 
		     float maxerr = 0.1, uint minfac = 50, bool bin = false, 
		     float low = -FLT_MAX, float up = FLT_MAX );
    ~ClusterArgMaker();

    void setFilename( const std::string & n ) { _fileout = n; }
    std::string filename() const { return( _fileout ); }
    void setConnectivity( Connectivity::Type conn ) { _connectivity = conn; }
    Connectivity::Type connectivity() const { return( _connectivity ); }
    void setMinClusterSize( int sz ) { _minsize = sz; }
    int minClusterSize() const { return( _minsize ); }
    void setTransformationFilename( const std::string & mtxfile )
    { _matrix = mtxfile; }
    std::string transformationFilename() const { return( _matrix); }
    void setMeshFlag( bool x ) { _domesh = x; }
    bool meshFlag() const { return( _domesh ); }
    void setDecimationMaxClearance( float x ) { _deciMaxClearance = x; }
    float decimationMaxClearance() const { return( _deciMaxClearance ); }
    void setDecimationMaxError( float x ) { _deciMaxError = x; }
    float decimationMaxError() const { return( _deciMaxError ); }
    void setMinFacetNumber( uint x ) { _minFacetNumber = x; }
    uint minFacetNumber() const { return( _minFacetNumber ); }
    void setBinarize( bool x ) { _binarize = x; }
    bool binarize() const { return( _binarize ); }
    void setLowThreshold( float x ) { _lowth = x; }
    float lowThreshold() const { return( _lowth ); }
    void setUpperThershold( float x ) { _upth = x; }
    float upperThreshold() const { return( _upth ); }

    template<typename T> void make( Graph & gr, const AimsData<T> & data );

  private:
    std::string		_fileout;
    Connectivity::Type	_connectivity;
    int			_minsize;
    std::string		_matrix;
    bool		_domesh;
    float		_deciMaxClearance;
    float		_deciMaxError;
    uint		_minFacetNumber;
    bool		_binarize;
    float		_lowth;
    float		_upth;
  };

}


#endif
