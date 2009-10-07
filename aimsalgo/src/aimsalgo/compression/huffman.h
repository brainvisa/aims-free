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


#ifndef AIMS_COMPRESSION_HUFFMAN_H
#define AIMS_COMPRESSION_HUFFMAN_H

#include <aims/config/aimsalgo_config.h>
#include <aims/compression/lzhl.h>
#include <aims/def/general.h>
#include <aims/def/assert.h>


#define NHUFFSYMBOLS ( 256 + 16 + 2 )


//
// struct HuffStatTmpStruct
//
struct AIMSALGO_API HuffStatTmpStruct
{
  short i;
  short n;
  bool operator<(const HuffStatTmpStruct& x)
  {
    short tmp = x.n - n;
    return tmp ? (tmp < 0): (x.i - i < 0);
  }
};


//
// class HuffStat
//
class AIMSALGO_API HuffStat
{
  public:
    HuffStat();
    ~HuffStat();

    short* stat;

  protected:
    int _makeSortedTmp( HuffStatTmpStruct* );
};


//
// class LZHLEncoderStat
//
class AIMSALGO_API LZHLEncoderStat : public HuffStat
{
  public:
    LZHLEncoderStat();
    ~LZHLEncoderStat();

    struct Symbol { short nBits; ushort code; };
    int nextStat;
    Symbol* symbolTable;

    void calcStat( int* groups );

  private:
    static void _addGroup( int* groups, int group, int nBits );
};


//
// class LZHLEncoder
//
class AIMSALGO_API LZHLEncoder
{
  public:
    LZHLEncoder( LZHLEncoderStat* stat, byte* dst );
    ~LZHLEncoder();

    enum { maxMatchOver = 517, maxRaw = 64 };
    static size_t calcMaxBuf( size_t rawSz );
    size_t flush();
    void putRaw( const byte* src, size_t sz );
    void putMatch( const byte* src, size_t nRaw, size_t matchOver, size_t disp);

  private:
    LZHLEncoderStat* _stat;
    short* _sstat;
    int& _nextStat;

    byte* _dst;
    byte* _dstBegin;
    uint _bits;
    int _nBits;

    void _callStat();
    void _put( ushort symbol );
    void _put( ushort symbol, int codeBits, uint code );
    void _putBits( int codeBits, uint code );
};


//
// class LZHLDecoderStat
//
class AIMSALGO_API LZHLDecoderStat : public HuffStat
{
  public:
    LZHLDecoderStat();
    ~LZHLDecoderStat();

    struct Group { int nBits; int pos; };

    Group groupTable[ 16 ];

    short* symbolTable;
};


//----------------------------------------------------------------------

inline 
LZHLEncoder::LZHLEncoder( LZHLEncoderStat* stat, byte* dst )
            : _stat( stat ), _sstat( stat->stat ), _nextStat( stat->nextStat )
{
  _dst = _dstBegin = dst;
  _bits = 0;
  _nBits = 0;
}


inline 
LZHLEncoder::~LZHLEncoder()
{
}


inline
void LZHLEncoder::_putBits( int codeBits, uint code )
{
  ASSERT( codeBits <= 16 );
  _bits |= ( code << ( 32 - _nBits - codeBits ) );
  _nBits += codeBits;
  if ( _nBits >= 16 )
  {
    *_dst++ = (byte)( _bits >> 24 );
    *_dst++ = (byte)( _bits >> 16 );
    _nBits -= 16;
    _bits <<= 16;
  }
}


inline
void LZHLEncoder::_put( ushort symbol )
{
  ASSERT( symbol < NHUFFSYMBOLS );
  if ( --_nextStat <= 0 )
    _callStat();

  ++_sstat[ symbol ];

  LZHLEncoderStat::Symbol* item = &_stat->symbolTable[ symbol ];
  ASSERT( item->nBits >= 0 );

  _putBits( item->nBits, item->code );
}


inline
void LZHLEncoder::_put( ushort symbol, int codeBits, uint code )
{
  ASSERT( symbol < NHUFFSYMBOLS );
  ASSERT( codeBits <= 4 );
  if ( --_nextStat <= 0 )
    _callStat();

  ++_sstat[ symbol ];

  LZHLEncoderStat::Symbol* item = &_stat->symbolTable[ symbol ];
  ASSERT( item->nBits >= 0 );

  int nBits = item->nBits;
  _putBits( nBits + codeBits, ( item->code << codeBits ) | code );
}


#endif
