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


// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/roi/clusterArg_d.h>
#include <aims/def/path.h>

#include <graph/graph/gwriter.h>
#include <aims/io/datatypecode.h>
#include <stdio.h>
#include <float.h>

using namespace aims;
using namespace carto;
using namespace std;


ClusterArgMaker::ClusterArgMaker( const string & out, Connectivity::Type conn, 
                                  int minsz, const string & mtxfile,
                                  bool mkmesh, float maxclear,
                                  float maxerr, uint minfac, bool bin,
                                  float low, float up )
  : _fileout( out ), _connectivity( conn ), _minsize( minsz ), 
    _matrix( mtxfile ), _domesh( mkmesh ), _deciMaxClearance( maxclear ), 
    _deciMaxError( maxerr ), _minFacetNumber( minfac ), _binarize( bin ), 
    _lowth( low ), _upth( up )
{
}


ClusterArgMaker::~ClusterArgMaker()
{
}


template
void ClusterArgMaker::make( Graph & gr,
                            const rc_ptr<Volume<uint8_t> > & data );
template
void ClusterArgMaker::make( Graph & gr,
                            const rc_ptr<Volume<int16_t> > & data );
template
void ClusterArgMaker::make( Graph & gr,
                            const rc_ptr<Volume<float> > & data );
