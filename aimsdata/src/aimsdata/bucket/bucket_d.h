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




template<class T> AimsBucket<T> & 
AimsBucket<T>::operator = ( const BucketMap<T> & b2 )
  {
    b1.setSizeXYZT( b2.sizeX(), b2.sizeY(), b2.sizeZ(), b2.sizeT() );
    b1.clear();
    BucketMap<T>::const_iterator		ib, eb = b2.end();
    BucketMap<T>::BucketMap<T>::const_iterator	ibi, ebi;
    AimsBucketItem<T>				bitem;

    for( ib=b2.begin(); ib!=eb; ++ib )
      {
	std::list<AimsBucketItem<T> >	& bk = b1[ ib->first ];
	for( ibi=ib->second.begin(), ebi=ib->second.end(); ibi!=ebi; ++ibi )
	  {
	    bitem.location() = ibi->first;
	    bitem.value() = ibi->second;
	    bk.push_back( bitem );
	  }
      }
  }


