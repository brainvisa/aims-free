
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
