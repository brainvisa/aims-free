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


#include <aims/compression/lz.h>
#include <algorithm>

using namespace std;

#ifdef LZSLOWHASH
#define LZHASHSHIFT 5
#define UPDATE_HASH( hash, c )          \
    {                                   \
      hash ^= (c);                      \
      hash = ROTL( hash, LZHASHSHIFT ); \
    }
#define UPDATE_HASH_EX( hash, src )                      \
    {                                                    \
      hash ^= ROTL( (src)[ 0 ], LZHASHSHIFT * LZMATCH ); \
      hash ^= (src)[ LZMATCH ];                          \
      hash = ROTL( hash, LZHASHSHIFT );                  \
    }

#define HASH_POS(hash) ((( (hash) * 214013 + 2531011 ) >> ( 32 - LZTABLEBITS)) )

#else

#define LZHASHSHIFT ( ( ( LZTABLEBITS ) + ( LZMATCH ) - 1 ) / ( LZMATCH ) )

#define UPDATE_HASH( hash, c ) { hash = ( hash << LZHASHSHIFT ) ^ ( c ); }
#define UPDATE_HASH_EX( hash, src )  \
                    { hash = ( hash << LZHASHSHIFT ) ^ (src)[ LZMATCH ]; }
#define LZHASHMASK ( ( LZTABLESIZE ) - 1 )
#define HASH_POS( hash ) ( ( hash ) & LZHASHMASK )
#endif



inline 
uint _calcHash( const byte* src )
{
  uint hash = 0;
  const byte* pEnd = src + LZMATCH;
  for( const byte* p = src; p < pEnd ; )
    UPDATE_HASH( hash, *p++ );

  return hash;
}


//
// class LZBuffer
//
inline
int LZBuffer::_nMatch( int pos, const byte* p, int nLimit )
{
  ASSERT( nLimit < LZBUFSIZE );
  int begin = pos;
  if ( LZBUFSIZE - begin >= nLimit )
  {
    for ( int i = 0; i < nLimit ; i++ )
      if ( _buf[ begin + i ] != p[ i ] )
        return i;
    return nLimit;
  }
  else
  {
    int i;
    for ( i = begin; i < LZBUFSIZE ; i++ )
      if ( _buf[ i ] != p[ i - begin ] )
        return i - begin;
    int shift = LZBUFSIZE - begin;
    int n = nLimit - ( LZBUFSIZE - begin );
    for ( i = 0; i < n ; i++ )
      if ( _buf[ i ] != p[ shift + i ] )
        return shift + i;
    return nLimit;
  }
}


//
// class LZHLCompressor
//
LZHLCompressor::LZHLCompressor()
{
  _table = new ushort[ LZTABLESIZE ];
  for ( int i = 0; i < LZTABLESIZE ; i++ )
    _table[ i ] = (ushort)(-1);
}


LZHLCompressor::~LZHLCompressor()
{
  delete [] _table;
}


inline 
uint 
LZHLCompressor::_updateTable( uint hash, const byte* src, uint pos,
                              int len )
{
  if ( len <= 0 )
    return 0;

  if ( len > LZSKIPHASH )
  {
    src++;
    hash = 0;
    const byte* pEnd = src + len + LZMATCH;
    for ( const byte* p = src + len; p < pEnd ; )
      UPDATE_HASH( hash, *p++ );
    return hash;
  }

  UPDATE_HASH_EX( hash, src );
  src++;

  for ( int i = 0; i < len ; i++ )
  {
    _table[ HASH_POS( hash ) ] = ( ushort )_wrap( pos + i );
    UPDATE_HASH_EX( hash, src + i );
  }

  return hash;
}


size_t LZHLCompressor::compress( byte* dst, const byte* src, size_t sz )
{
  LZHLEncoder coder( &_stat, dst );
  const byte* srcEnd = src + sz;

  uint hash = 0;
  if ( sz >= LZMATCH )
  {
    const byte* pEnd = src + LZMATCH;
    for ( const byte* p = src; p < pEnd ; )
      UPDATE_HASH( hash, *p++ );
  }

  for ( ; ; )
  {
    int srcLeft = srcEnd - src;
    if ( srcLeft < LZMATCH )
    {
      if ( srcLeft )
      {
        _toBuf( src, srcLeft );
        coder.putRaw( src, srcLeft );
        src += srcLeft;
      }
      break; //forever
    }

    int nRaw = 0;
    int maxRaw = min( (int) (srcLeft - LZMATCH), 
		       ( int )LZHLEncoder::maxRaw );

    #ifdef LZLAZYMATCH
    int lazyMatchLen = 0;
    int lazyMatchHashPos = 0;
    uint lazyMatchBufPos = 0;
    int lazyMatchNRaw = 0;
    uint lazyMatchHash = 0;
    bool lazyForceMatch = false;
    #endif

    for ( ; ; )
    {
      uint hash2 = HASH_POS( hash );

      int hashPos = _table[ hash2 ];
      int wrapBufPos = _wrap( _bufPos );
      _table[ hash2 ] = ( ushort )wrapBufPos;

      int matchLen = 0;
      if ( hashPos != (ushort)(-1) && hashPos != wrapBufPos )
      {
        int matchLimit = min( min( _distance( wrapBufPos - hashPos ),
                                               srcLeft - nRaw ), 
                               ( int )( LZMIN + LZHLEncoder::maxMatchOver ) );
        matchLen = _nMatch( hashPos, src + nRaw, matchLimit );

        #ifdef LZOVERLAP
        if( _wrap( hashPos + matchLen ) == wrapBufPos )
        {
          ASSERT( matchLen != 0 );
          int xtraMatchLimit = min( LZMIN + LZHLEncoder::maxMatchOver - 
                                     matchLen, srcLeft - nRaw - matchLen );
          int xtraMatch;
          for ( xtraMatch = 0; xtraMatch < xtraMatchLimit ; xtraMatch++ )
          {
            if ( src[ nRaw + xtraMatch ] != src[ nRaw + xtraMatch + matchLen ] )
              break;
          }
          matchLen += xtraMatch;
        }
        #endif

        #ifdef LZBACKWARDMATCH
        if ( matchLen >= LZMIN - 1 ) //to ensure that buf will be overwritten
        {
          int xtraMatchLimit = min( LZMIN + LZHLEncoder::maxMatchOver - 
                                     matchLen, nRaw );
          int d = (int)_distance( _bufPos - hashPos );
          xtraMatchLimit = min( min( xtraMatchLimit, d - matchLen ),
                                LZBUFSIZE - d );
          int xtraMatch;
          for ( xtraMatch = 0; xtraMatch < xtraMatchLimit ; ++xtraMatch )
          {
            if ( _buf[ _wrap( hashPos - xtraMatch - 1 ) ] != 
                                  src[ nRaw - xtraMatch - 1 ] )
              break;
          }
          if ( xtraMatch > 0 )
          {
            ASSERT( matchLen + xtraMatch >= LZMIN );
            ASSERT( matchLen + xtraMatch <= _distance( _bufPos - hashPos ) );

            nRaw -= xtraMatch;
            _bufPos -= xtraMatch;
            hashPos -= xtraMatch;
            matchLen += xtraMatch;
            wrapBufPos = _wrap( _bufPos );
            hash = _calcHash( src + nRaw );

            #ifdef LZLAZYMATCH
            lazyForceMatch = true;
            #endif

          }
        }
        #endif
      }

      #ifdef LZLAZYMATCH
      if ( lazyMatchLen >= LZMIN )
      {
        if ( matchLen > lazyMatchLen )
        {
          coder.putMatch( src, nRaw, matchLen - LZMIN, 
                          _distance( wrapBufPos - hashPos ) );
          hash = _updateTable( hash, src + nRaw, _bufPos + 1,
                               min( (int) ( matchLen - 1 ),
                                    (int) ( srcEnd - ( src + nRaw + 1 ) ) ) );
          _toBuf( src + nRaw, matchLen );
          src += nRaw + matchLen;
          break; // for( nRaw )
        }
        else
        {
          nRaw = lazyMatchNRaw;
          _bufPos = lazyMatchBufPos;

          hash = lazyMatchHash;
          UPDATE_HASH_EX( hash, src + nRaw );

          coder.putMatch( src, nRaw, lazyMatchLen - LZMIN,
                          _distance( _bufPos - lazyMatchHashPos ) );
          hash = _updateTable( hash, src + nRaw + 1, _bufPos + 2, 
			       min( (int) ( lazyMatchLen - 2 ),
				    (int)( srcEnd - ( src + nRaw + 2 ) ) ) );
          _toBuf( src + nRaw, lazyMatchLen );
          src += nRaw + lazyMatchLen;
          break; // for( nRaw )
        }
      }
      #endif

      if ( matchLen >= LZMIN )
      {
        #ifdef LZLAZYMATCH
        if ( !lazyForceMatch )
        {
          lazyMatchLen = matchLen;
          lazyMatchHashPos = hashPos;
          lazyMatchNRaw = nRaw;
          lazyMatchBufPos = _bufPos;
          lazyMatchHash = hash;
        }
        else
        #endif
        {
          coder.putMatch( src, nRaw, matchLen - LZMIN,
                          _distance( wrapBufPos - hashPos ) );
          hash = _updateTable( hash, src + nRaw, _bufPos + 1, 
                               min( (int) ( matchLen - 1 ),
				    (int) ( srcEnd - ( src + nRaw + 1 ) ) ) );
          _toBuf( src + nRaw, matchLen );
          src += nRaw + matchLen;
          break; // for( nRaw )
        }
      }

      #ifdef LZLAZYMATCH
      ASSERT( !lazyForceMatch );
      #endif

      if ( nRaw + 1 > maxRaw )
      {
        #ifdef LZLAZYMATCH
        if ( lazyMatchLen >= LZMIN )
        {
          coder.putMatch( src, nRaw, lazyMatchLen - LZMIN, 
                          _distance( _bufPos - lazyMatchHashPos ) );
          hash = _updateTable( hash, src + nRaw, _bufPos + 1,
			       min( (int) ( lazyMatchLen - 1 ),
				    (int) ( srcEnd - ( src + nRaw + 1 ) ) ) );
          _toBuf( src + nRaw, lazyMatchLen );
          src += nRaw + lazyMatchLen;
          break; // for( nRaw )
        }
        #endif

        if ( nRaw + LZMATCH >= srcLeft && srcLeft <= LZHLEncoder::maxRaw )
        {
          _toBuf( src + nRaw, srcLeft - nRaw );
          nRaw = srcLeft;
        }

        coder.putRaw( src, nRaw );
        src += nRaw;
        break; // for( nRaw )
      }

      UPDATE_HASH_EX( hash, src + nRaw );
      _toBuf( src[ nRaw++ ] );
    } // for( nRaw )
  }

  return coder.flush();
}
