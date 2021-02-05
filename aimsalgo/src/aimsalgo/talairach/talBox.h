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


#ifndef AIMS_TALAIRACH_TALBOX_H
#define AIMS_TALAIRACH_TALBOX_H

#include <cstdlib>
#include <aims/data/data.h>
#include <aims/talairach/talBoxBase.h>


template< class T >
class TalairachBox : public TalairachBoxBase
{
public:
  TalairachBox() : TalairachBoxBase() {}
  inline virtual ~TalairachBox() {}
  void computeBox( const AimsData< T >& );
  Motion computeTransformationAndBox( const TalairachPoints&,
                                    const AimsData< T >& );

  // Returns a motion from current subject to a normalized template, where
  // AC (0, 0, 0) PC (0, 1, 0) and IHP (0, 1, cout0)
};


template< class T > inline
void TalairachBox< T >::computeBox( const AimsData< T >& d )
{
  int x, y, z, dx = d.dimX(), dy = d.dimY(), dz = d.dimZ();
  Point3df dsize( d.sizeX(), d.sizeY(), d.sizeZ() );
  Point3df boxmax( -10000.0f, -10000.0f, -10000.0f );
  Point3df boxmin( 10000.0f, 10000.0f, 10000.0f );
  Point3df pt, npt;
  
  for ( z=0; z<dz; z++ )
    for ( y=0; y<dy; y++ )
      for ( x=0; x<dx; x++ )
        if ( d( x, y, z ) )
          {
	    pt = Point3df( float( x ), float( y ), float( z ) );
	    pt[ 0 ] *= dsize[ 0 ];
	    pt[ 1 ] *= dsize[ 1 ];
	    pt[ 2 ] *= dsize[ 2 ];
	    npt = TalairachReferential::toTalairach( pt );

	    if ( npt[ 0 ] < boxmin[ 0 ] )  boxmin[ 0 ] = npt[ 0 ];
	    if ( npt[ 1 ] < boxmin[ 1 ] )  boxmin[ 1 ] = npt[ 1 ];
	    if ( npt[ 2 ] < boxmin[ 2 ] )  boxmin[ 2 ] = npt[ 2 ];
	    if ( npt[ 0 ] > boxmax[ 0 ] )  boxmax[ 0 ] = npt[ 0 ];
	    if ( npt[ 1 ] > boxmax[ 1 ] )  boxmax[ 1 ] = npt[ 1 ];
	    if ( npt[ 2 ] > boxmax[ 2 ] )  boxmax[ 2 ] = npt[ 2 ];
	  }

  if ( fabs( boxmin[ 0 ] ) > fabs( boxmax[ 0 ] ) )
    _scale[ 0 ] = 1.0f / fabs( boxmin[ 0 ] );
  else _scale[ 0 ] = 1.0f / fabs( boxmax[ 0 ] );

  std::cout << "Box Min : " << boxmin << "\tBox Max :" << boxmax << std::endl ;
  _scale[ 1 ] = 1.0f / fabs( boxmax[ 1 ] );
  // On peut ausi prendre
  // _scale[ 1 ] = 1.0f/_ACPCVec.norm()
  _scale[ 2 ] = 1.0f / fabs( boxmin[ 2 ] );
}


template< class T > inline 
Motion TalairachBox< T >::computeTransformationAndBox( const TalairachPoints& pt,
						     const AimsData< T >& d )
{
  computeTransformation( pt );
  computeBox( d );
  
  AimsData<float> rotation(3, 3) ;
  Point3df translation = -pt.ACmm() ;

  rotation(0, 0) = -_crossVec[ 0 ];
  rotation(0, 1) = -_crossVec[ 1 ];
  rotation(0, 2) = -_crossVec[ 2 ];

  rotation(1, 0) = _ACPCVec[ 0 ];
  rotation(1, 1) = _ACPCVec[ 1 ];
  rotation(1, 2) = _ACPCVec[ 2 ];

  rotation(2, 0) = _hemiVec[ 0 ];
  rotation(2, 1) = _hemiVec[ 1 ];
  rotation(2, 2) = _hemiVec[ 2 ];
  
  _transformation.setTranslation( Point3df(0., 0., 0.) );
  _transformation.setMatrix(rotation);  
  
  std::cout << "Scale : " << _scale << std::endl ;
  _transformation.scale( Point3df(1., 1., 1. ), 
    			 Point3df(1.0f/_scale[0], 1.0f/_scale[1], 1.0f/_scale[2] ) ) ;
  _transformation.setTranslation( _transformation.transform(translation) );
  return _transformation ;
}

#endif
