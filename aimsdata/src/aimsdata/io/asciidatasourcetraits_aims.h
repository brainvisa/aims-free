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

#ifndef AIMS_IO_ASCIIDATASOURCETRAITS_AIMS_H
#define AIMS_IO_ASCIIDATASOURCETRAITS_AIMS_H

#include <soma-io/utilities/asciidatasourcetraits.h>
#include <aims/vector/vector.h>

namespace soma
{

  template<typename T, int D>
  class AsciiDataSourceTraits<AimsVector<T, D> >
  {
  public:
    static bool read( DataSource & ds, AimsVector<T, D> & item );
    static bool write( DataSource & ds, const AimsVector<T, D> & item );
  };


  // -----------

#ifndef DOXYGEN_HIDE_INTERNAL_CLASSES

  template<typename T, int D>
  inline
  bool AsciiDataSourceTraits<AimsVector<T, D> >::read( DataSource & ds,
                                                       AimsVector<T, D> & x )
  {
    char c;

    if( !StreamUtil::skip( ds ) )
      return false;
    c = ds.getch();
    if( c != '(' )
      return false;

    for( int i=0; i<D; ++i )
    {
      if( !StreamUtil::skip( ds ) )
        return false;
      if( !AsciiDataSourceTraits<T>::read( ds, x[i] ) )
        return false;
      if( i < D-1 )
      {
        if( !StreamUtil::skip( ds ) )
            return false;
        c = ds.getch();
        if( c != ',' )
          return false;
      }
    }

    if( !StreamUtil::skip( ds ) )
      return false;
    c = ds.getch();
    if( c != ')' )
      return false;

    return true;
  }


  template<typename T, int D>
  inline
  bool AsciiDataSourceTraits<AimsVector<T, D> >::write(
    DataSource & ds, const AimsVector<T, D> & x )
  {
    ds.putch( '(');


    for( int i=0; i<D; ++i )
    {
      AsciiDataSourceTraits<T>::write( ds, x[i] );
      if( i < D-1 )
      {
        ds.putch( ',' );
        ds.putch( ' ' );
      }
    }

    ds.putch( ')');
    return ds.isOpen();
  }


  template <typename T, int D>
  inline DataSource & operator << ( DataSource & ds,
                                    const AimsVector<T, D> & x )
  {
    AsciiDataSourceTraits<AimsVector<T, D> >::write( ds, x );
    return ds;
  }

#endif

}

#endif

