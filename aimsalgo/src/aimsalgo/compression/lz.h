/* Copyright (c) 1995-2007 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */


#ifndef AIMS_COMPRESSION_LZ_H
#define AIMS_COMPRESSION_LZ_H

#include <aims/config/aimsalgo_config.h>
#include <aims/compression/lzhl.h>
#include <aims/compression/huffman.h>
#include <string.h>


#define LZBUFMASK ( (LZBUFSIZE) - 1 )


//
// class LZBuffer
//
class AIMSALGO_API LZBuffer
{
  protected:
    inline LZBuffer();
    inline ~LZBuffer();

    static int _wrap( size_t pos );
    static int _distance( int diff );

    void _toBuf( byte );
    void _toBuf( const byte*, size_t sz );
    void _bufCpy( byte* dst, int pos, size_t sz );
    int _nMatch( int pos, const byte* p, int nLimit );

    byte* _buf;
    size_t _bufPos;
};


//
// class LZHLCompressor
//
class AIMSALGO_API LZHLCompressor : private LZBuffer
{
  public:
    LZHLCompressor();
    ~LZHLCompressor();

    static size_t calcMaxBuf( size_t rawSz )
    { return LZHLEncoder::calcMaxBuf( rawSz ); }

    size_t compress( byte* dst, const byte* src, size_t sz );

  private:
    LZHLEncoderStat _stat;
    ushort* _table;

    void _wrapTable();
    uint _updateTable( uint hash, const byte* src, uint pos, int len );
};


//
// class LZHLDecompressor
//
class AIMSALGO_API LZHLDecompressor 
  : private LZBuffer, private LZHLDecoderStat
{
  public:
    LZHLDecompressor();
    ~LZHLDecompressor();

    bool decompress( byte* dst, size_t* dstSz, const byte* src, 
		     size_t* srcSz );

  private:
    size_t _bits;
    int _nBits;

    int _get( const byte*& src, const byte* srcEnd, int n );
};



//----------------------------------------------------------------------------

inline 
LZBuffer::LZBuffer()
{
  _buf= new byte[ LZBUFSIZE ];
  _bufPos = 0;
}


inline
LZBuffer::~LZBuffer()
{
  delete [] _buf;
}


inline
int LZBuffer::_wrap( size_t pos )
{
  return pos & LZBUFMASK;
}


inline
int LZBuffer::_distance( int diff )
{
  return diff & LZBUFMASK;
}


inline
void LZBuffer::_toBuf( byte c )
{
  _buf[ _wrap( _bufPos++ ) ] = c;
}


inline
void LZBuffer::_toBuf( const byte* src, size_t sz )
{
  ASSERT( sz < LZBUFSIZE );
  int begin = _wrap( _bufPos );
  int end = begin + sz;
  if ( end > LZBUFSIZE )
  {
    size_t left = LZBUFSIZE - begin;
    memcpy( _buf + begin, src, left );
    memcpy( _buf, src + left, sz - left );
  }
  else
    memcpy( _buf + begin, src, sz );
  _bufPos += sz;
}


inline
void LZBuffer::_bufCpy( byte* dst, int pos, size_t sz )
{
  ASSERT( sz < LZBUFSIZE );
  int begin = _wrap( pos );
  int end = begin + sz;
  if ( end > LZBUFSIZE )
  {
    size_t left = LZBUFSIZE - begin;
    memcpy( dst, _buf + begin, left );
    memcpy( dst + left, _buf, sz - left );
  }
  else
    memcpy( dst, _buf + begin, sz );
}


#endif
