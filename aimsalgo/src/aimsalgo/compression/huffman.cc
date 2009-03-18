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


#include <cstdlib>
#include <aims/compression/huffman.h>
#include <aims/compression/lz.h>
#include <algorithm>

using namespace std;

void shellSort( HuffStatTmpStruct* a, int N )
{
  int i, j;
  HuffStatTmpStruct v;

  ASSERT( 13 <= N / 9 );
  ASSERT( 40 > N / 9 ); 

  for ( int h = 40; h > 0; h /= 3 )         // h = 40, 13, 4, 1
  {
    for ( i = h + 1; i <= N; ++i )
    {
      v = a[ i ];
      j = i;
      while ( ( j > h ) && ( v < a[ j - h ] ) )
      {
        a[ j ] = a[ j - h ];
        j -= h;
      }
      a[ j ] = v;
    }
  }
}


//
// class HuffStat
//
HuffStat::HuffStat()
{
  stat = new short[ NHUFFSYMBOLS ];
  memset( stat, 0, sizeof( short ) * NHUFFSYMBOLS );
}


HuffStat::~HuffStat()
{
  delete [] stat;
}


int HuffStat::_makeSortedTmp( HuffStatTmpStruct* s )
{
  int total = 0;
  for ( int j = 0; j < NHUFFSYMBOLS ; ++j )
  {
    s[ j ].i = j;
    s[ j ].n = stat[ j ];
    total += stat[ j ];
    stat[ j ] = stat[ j ] >> 1;
  }

  shellSort( s - 1, NHUFFSYMBOLS );
  return total;
}


//
// class LZHLEncoderStat
//

LZHLEncoderStat::LZHLEncoderStat()
{
static LZHLEncoderStat::Symbol theLZHLEncoderStatSymbolTable[ NHUFFSYMBOLS ] = 
{
 /* 000: */ { 7, 0x0014 }, /* 001: */ { 8, 0x0030 }, /* 002: */ { 8, 0x0031 },
 /* 003: */ { 8, 0x0032 }, /* 004: */ { 8, 0x0033 }, /* 005: */ { 8, 0x0034 },
 /* 006: */ { 8, 0x0035 }, /* 007: */ { 8, 0x0036 }, /* 008: */ { 8, 0x0037 },
 /* 009: */ { 8, 0x0038 }, /* 010: */ { 8, 0x0039 }, /* 011: */ { 8, 0x003A },
 /* 012: */ { 8, 0x003B }, /* 013: */ { 8, 0x003C }, /* 014: */ { 8, 0x003D },
 /* 015: */ { 8, 0x003E }, /* 016: */ { 8, 0x003F }, /* 017: */ { 8, 0x0040 },
 /* 018: */ { 8, 0x0041 }, /* 019: */ { 8, 0x0042 }, /* 020: */ { 8, 0x0043 },
 /* 021: */ { 8, 0x0044 }, /* 022: */ { 8, 0x0045 }, /* 023: */ { 8, 0x0046 },
 /* 024: */ { 8, 0x0047 }, /* 025: */ { 8, 0x0048 }, /* 026: */ { 8, 0x0049 },
 /* 027: */ { 8, 0x004A }, /* 028: */ { 8, 0x004B }, /* 029: */ { 8, 0x004C },
 /* 030: */ { 8, 0x004D }, /* 031: */ { 8, 0x004E }, /* 032: */ { 7, 0x0015 },
 /* 033: */ { 8, 0x004F }, /* 034: */ { 8, 0x0050 }, /* 035: */ { 8, 0x0051 },
 /* 036: */ { 8, 0x0052 }, /* 037: */ { 8, 0x0053 }, /* 038: */ { 8, 0x0054 },
 /* 039: */ { 8, 0x0055 }, /* 040: */ { 8, 0x0056 }, /* 041: */ { 8, 0x0057 },
 /* 042: */ { 8, 0x0058 }, /* 043: */ { 8, 0x0059 }, /* 044: */ { 8, 0x005A },
 /* 045: */ { 8, 0x005B }, /* 046: */ { 8, 0x005C }, /* 047: */ { 8, 0x005D },
 /* 048: */ { 7, 0x0016 }, /* 049: */ { 8, 0x005E }, /* 050: */ { 8, 0x005F },
 /* 051: */ { 8, 0x0060 }, /* 052: */ { 8, 0x0061 }, /* 053: */ { 8, 0x0062 },
 /* 054: */ { 8, 0x0063 }, /* 055: */ { 8, 0x0064 }, /* 056: */ { 8, 0x0065 },
 /* 057: */ { 8, 0x0066 }, /* 058: */ { 8, 0x0067 }, /* 059: */ { 8, 0x0068 },
 /* 060: */ { 8, 0x0069 }, /* 061: */ { 8, 0x006A }, /* 062: */ { 8, 0x006B },
 /* 063: */ { 8, 0x006C }, /* 064: */ { 8, 0x006D }, /* 065: */ { 8, 0x006E },
 /* 066: */ { 8, 0x006F }, /* 067: */ { 8, 0x0070 }, /* 068: */ { 8, 0x0071 },
 /* 069: */ { 8, 0x0072 }, /* 070: */ { 8, 0x0073 }, /* 071: */ { 8, 0x0074 },
 /* 072: */ { 8, 0x0075 }, /* 073: */ { 8, 0x0076 }, /* 074: */ { 8, 0x0077 },
 /* 075: */ { 8, 0x0078 }, /* 076: */ { 8, 0x0079 }, /* 077: */ { 8, 0x007A },
 /* 078: */ { 8, 0x007B }, /* 079: */ { 8, 0x007C }, /* 080: */ { 8, 0x007D },
 /* 081: */ { 8, 0x007E }, /* 082: */ { 8, 0x007F }, /* 083: */ { 8, 0x0080 },
 /* 084: */ { 8, 0x0081 }, /* 085: */ { 8, 0x0082 }, /* 086: */ { 8, 0x0083 },
 /* 087: */ { 8, 0x0084 }, /* 088: */ { 8, 0x0085 }, /* 089: */ { 8, 0x0086 },
 /* 090: */ { 8, 0x0087 }, /* 091: */ { 8, 0x0088 }, /* 092: */ { 8, 0x0089 },
 /* 093: */ { 8, 0x008A }, /* 094: */ { 8, 0x008B }, /* 095: */ { 8, 0x008C },
 /* 096: */ { 8, 0x008D }, /* 097: */ { 8, 0x008E }, /* 098: */ { 8, 0x008F },
 /* 099: */ { 8, 0x0090 }, /* 100: */ { 8, 0x0091 }, /* 101: */ { 8, 0x0092 },
 /* 102: */ { 8, 0x0093 }, /* 103: */ { 8, 0x0094 }, /* 104: */ { 8, 0x0095 },
 /* 105: */ { 8, 0x0096 }, /* 106: */ { 8, 0x0097 }, /* 107: */ { 8, 0x0098 },
 /* 108: */ { 8, 0x0099 }, /* 109: */ { 8, 0x009A }, /* 110: */ { 8, 0x009B },
 /* 111: */ { 8, 0x009C }, /* 112: */ { 8, 0x009D }, /* 113: */ { 8, 0x009E },
 /* 114: */ { 8, 0x009F }, /* 115: */ { 8, 0x00A0 }, /* 116: */ { 8, 0x00A1 },
 /* 117: */ { 8, 0x00A2 }, /* 118: */ { 8, 0x00A3 }, /* 119: */ { 8, 0x00A4 },
 /* 120: */ { 8, 0x00A5 }, /* 121: */ { 8, 0x00A6 }, /* 122: */ { 8, 0x00A7 },
 /* 123: */ { 8, 0x00A8 }, /* 124: */ { 8, 0x00A9 }, /* 125: */ { 8, 0x00AA },
 /* 126: */ { 8, 0x00AB }, /* 127: */ { 8, 0x00AC }, /* 128: */ { 8, 0x00AD },
 /* 129: */ { 8, 0x00AE }, /* 130: */ { 8, 0x00AF }, /* 131: */ { 8, 0x00B0 },
 /* 132: */ { 8, 0x00B1 }, /* 133: */ { 8, 0x00B2 }, /* 134: */ { 8, 0x00B3 },
 /* 135: */ { 8, 0x00B4 }, /* 136: */ { 8, 0x00B5 }, /* 137: */ { 8, 0x00B6 },
 /* 138: */ { 8, 0x00B7 }, /* 139: */ { 8, 0x00B8 }, /* 140: */ { 8, 0x00B9 },
 /* 141: */ { 8, 0x00BA }, /* 142: */ { 8, 0x00BB }, /* 143: */ { 8, 0x00BC },
 /* 144: */ { 8, 0x00BD }, /* 145: */ { 8, 0x00BE }, /* 146: */ { 8, 0x00BF },
 /* 147: */ { 8, 0x00C0 }, /* 148: */ { 8, 0x00C1 }, /* 149: */ { 8, 0x00C2 },
 /* 150: */ { 8, 0x00C3 }, /* 151: */ { 8, 0x00C4 }, /* 152: */ { 8, 0x00C5 },
 /* 153: */ { 8, 0x00C6 }, /* 154: */ { 8, 0x00C7 }, /* 155: */ { 8, 0x00C8 },
 /* 156: */ { 8, 0x00C9 }, /* 157: */ { 8, 0x00CA }, /* 158: */ { 8, 0x00CB },
 /* 159: */ { 8, 0x00CC }, /* 160: */ { 8, 0x00CD }, /* 161: */ { 8, 0x00CE },
 /* 162: */ { 8, 0x00CF }, /* 163: */ { 9, 0x01A0 }, /* 164: */ { 9, 0x01A1 },
 /* 165: */ { 9, 0x01A2 }, /* 166: */ { 9, 0x01A3 }, /* 167: */ { 9, 0x01A4 },
 /* 168: */ { 9, 0x01A5 }, /* 169: */ { 9, 0x01A6 }, /* 170: */ { 9, 0x01A7 },
 /* 171: */ { 9, 0x01A8 }, /* 172: */ { 9, 0x01A9 }, /* 173: */ { 9, 0x01AA },
 /* 174: */ { 9, 0x01AB }, /* 175: */ { 9, 0x01AC }, /* 176: */ { 9, 0x01AD },
 /* 177: */ { 9, 0x01AE }, /* 178: */ { 9, 0x01AF }, /* 179: */ { 9, 0x01B0 },
 /* 180: */ { 9, 0x01B1 }, /* 181: */ { 9, 0x01B2 }, /* 182: */ { 9, 0x01B3 },
 /* 183: */ { 9, 0x01B4 }, /* 184: */ { 9, 0x01B5 }, /* 185: */ { 9, 0x01B6 },
 /* 186: */ { 9, 0x01B7 }, /* 187: */ { 9, 0x01B8 }, /* 188: */ { 9, 0x01B9 },
 /* 189: */ { 9, 0x01BA }, /* 190: */ { 9, 0x01BB }, /* 191: */ { 9, 0x01BC },
 /* 192: */ { 9, 0x01BD }, /* 193: */ { 9, 0x01BE }, /* 194: */ { 9, 0x01BF },
 /* 195: */ { 9, 0x01C0 }, /* 196: */ { 9, 0x01C1 }, /* 197: */ { 9, 0x01C2 },
 /* 198: */ { 9, 0x01C3 }, /* 199: */ { 9, 0x01C4 }, /* 200: */ { 9, 0x01C5 },
 /* 201: */ { 9, 0x01C6 }, /* 202: */ { 9, 0x01C7 }, /* 203: */ { 9, 0x01C8 },
 /* 204: */ { 9, 0x01C9 }, /* 205: */ { 9, 0x01CA }, /* 206: */ { 9, 0x01CB },
 /* 207: */ { 9, 0x01CC }, /* 208: */ { 9, 0x01CD }, /* 209: */ { 9, 0x01CE },
 /* 210: */ { 9, 0x01CF }, /* 211: */ { 9, 0x01D0 }, /* 212: */ { 9, 0x01D1 },
 /* 213: */ { 9, 0x01D2 }, /* 214: */ { 9, 0x01D3 }, /* 215: */ { 9, 0x01D4 },
 /* 216: */ { 9, 0x01D5 }, /* 217: */ { 9, 0x01D6 }, /* 218: */ { 9, 0x01D7 },
 /* 219: */ { 9, 0x01D8 }, /* 220: */ { 9, 0x01D9 }, /* 221: */ { 9, 0x01DA },
 /* 222: */ { 9, 0x01DB }, /* 223: */ { 9, 0x01DC }, /* 224: */ { 9, 0x01DD },
 /* 225: */ { 9, 0x01DE }, /* 226: */ { 9, 0x01DF }, /* 227: */ { 9, 0x01E0 },
 /* 228: */ { 9, 0x01E1 }, /* 229: */ { 9, 0x01E2 }, /* 230: */ { 9, 0x01E3 },
 /* 231: */ { 9, 0x01E4 }, /* 232: */ { 9, 0x01E5 }, /* 233: */ { 9, 0x01E6 },
 /* 234: */ { 9, 0x01E7 }, /* 235: */ { 9, 0x01E8 }, /* 236: */ { 9, 0x01E9 },
 /* 237: */ { 9, 0x01EA }, /* 238: */ { 9, 0x01EB }, /* 239: */ { 9, 0x01EC },
 /* 240: */ { 9, 0x01ED }, /* 241: */ { 9, 0x01EE }, /* 242: */ { 9, 0x01EF },
 /* 243: */ { 9, 0x01F0 }, /* 244: */ { 9, 0x01F1 }, /* 245: */ { 9, 0x01F2 },
 /* 246: */ { 9, 0x01F3 }, /* 247: */ { 9, 0x01F4 }, /* 248: */ { 9, 0x01F5 },
 /* 249: */ { 9, 0x01F6 }, /* 250: */ { 9, 0x01F7 }, /* 251: */ { 9, 0x01F8 },
 /* 252: */ { 9, 0x01F9 }, /* 253: */ { 9, 0x01FA }, /* 254: */ { 9, 0x01FB },
 /* 255: */ { 7, 0x0017 }, /* 256: */ { 6, 0x0000 }, /* 257: */ { 6, 0x0001 },
 /* 258: */ { 6, 0x0002 }, /* 259: */ { 6, 0x0003 }, /* 260: */ { 7, 0x0008 },
 /* 261: */ { 7, 0x0009 }, /* 262: */ { 7, 0x000A }, /* 263: */ { 7, 0x000B },
 /* 264: */ { 7, 0x000C }, /* 265: */ { 7, 0x000D }, /* 266: */ { 7, 0x000E },
 /* 267: */ { 7, 0x000F }, /* 268: */ { 7, 0x0010 }, /* 269: */ { 7, 0x0011 },
 /* 270: */ { 7, 0x0012 }, /* 271: */ { 7, 0x0013 }, /* 272: */ { 9, 0x01FC },
 /* 273: */ { 9, 0x01FD }
};

  nextStat = HUFFRECALCLEN;

  symbolTable = new Symbol[ NHUFFSYMBOLS ];
  memcpy( symbolTable, theLZHLEncoderStatSymbolTable, 
	  sizeof( Symbol ) * NHUFFSYMBOLS );
}


LZHLEncoderStat::~LZHLEncoderStat()
{
  delete [] symbolTable;
}


void LZHLEncoderStat::_addGroup( int* groups, int group, int nBits )
{
  ASSERT( nBits <= 8 );

  //Bubble sort
  int j;
  for ( j = group; j > 0 && nBits < groups[ j - 1 ] ; j-- )
    groups[ j ] = groups[ j - 1 ];
  groups[ j ] = nBits;
}


void LZHLEncoderStat::calcStat( int* groups )
{
  HuffStatTmpStruct s[ NHUFFSYMBOLS ];
  int total = _makeSortedTmp( s );

  nextStat = HUFFRECALCLEN;

  int pos = 0;
  int nTotal = 0;
  for ( int group = 0; group < 14 ; ++group )
  {
    int avgGroup = ( total - nTotal ) / ( 16 - group );
    int i = 0, n = 0, nn = 0;
    for ( int nBits = 0 ; ; ++nBits )
    {
      int over = 0;
      int nItems = 1 << nBits;

      if ( pos + i + nItems > NHUFFSYMBOLS )
      {
        nItems = NHUFFSYMBOLS - pos;
        over = 1;
      }

      for ( ; i < nItems ; ++i )
        nn += s[ pos + i ].n;

      if ( over || nBits >= 8 || nn > avgGroup )
      {
        if ( nBits == 0 || abs( (int)( n - avgGroup ) ) >
                           abs( (int)( nn - avgGroup ) ) )
          n = nn;
        else
        nBits--;

        _addGroup( groups, group, nBits );
        nTotal += n;
        pos += 1 << nBits;
        break;
      }
      else
        n = nn;
    }
  }

  int bestNBits = 0, bestNBits15 = 0;
  int best = 0x7FFFFFFF;
  int i = 0, nn = 0, left = 0, j;
  for ( j = pos; j < NHUFFSYMBOLS ; ++j )
    left += s[ j ].n;
  for ( int nBits = 0 ; ; ++nBits )
  {
    int nItems = 1 << nBits;
    if ( pos + i + nItems > NHUFFSYMBOLS )
      break;

    for ( ; i < nItems ; ++i )
      nn += s[ pos + i ].n;

    int nItems15 = NHUFFSYMBOLS - ( pos + i );
    int nBits15; 
    for ( nBits15 = 0 ; ; ++nBits15 )
      if ( 1 << nBits15 >= nItems15 )
                break;
        
    ASSERT( left >= nn );
    if ( nBits <= 8 && nBits15 <= 8 )
    {
      int n = nn * nBits + ( left - nn ) * nBits15;
      if ( n < best )
      {
        best = n;
        bestNBits = nBits;
        bestNBits15 = nBits15;
      }
      else
        break; // PERF optimization
    }
  }

  _addGroup( groups, 14, bestNBits );
  _addGroup( groups, 15, bestNBits15 );

  pos = 0;
  for( j = 0; j < 16 ; ++j )
  {
    int nBits = groups[ j ];

    int nItems = 1 << nBits;
    int maxK = min( nItems, NHUFFSYMBOLS - pos );
    for ( int k = 0; k < maxK ; ++k )
    {
      int symbol = s[ pos + k ].i;
      symbolTable[ symbol ].nBits = nBits + 4;
      symbolTable[ symbol ].code = ( j << nBits ) | k;
    }

    pos += 1 << nBits;
  }
}


//
// class LZHLEncoder
//
size_t LZHLEncoder::calcMaxBuf( size_t rawSz )
{
  return rawSz + ( rawSz >> 1 ) + 32;
}


void LZHLEncoder::_callStat()
{
  _nextStat = 2; // to avoid recursion, >=2
  _put( NHUFFSYMBOLS - 2 );

  int groups[ 16 ];
  _stat->calcStat( groups );

  int lastNBits = 0;
  for ( int i = 0; i < 16 ; ++i )
  {
    int nBits = groups[ i ];
    ASSERT( nBits >= lastNBits && nBits <= 8 );
    int delta = nBits - lastNBits;
    lastNBits = nBits;
    _putBits( delta + 1, 1 );
  }
}


void LZHLEncoder::putRaw( const byte * src, size_t sz )
{
  for ( const byte* srcEnd = src + sz; src < srcEnd ; ++src )
    _put( *src );
}


void LZHLEncoder::putMatch( const byte* src, size_t nRaw, size_t matchOver, 
                            size_t disp )
{
  ASSERT ( nRaw <= maxRaw );
  ASSERT ( matchOver <= maxMatchOver );
  ASSERT ( disp < LZBUFSIZE );

  putRaw( src, nRaw );

  struct MatchOverItem { int symbol; int nBits; ushort bits; };
  static MatchOverItem _matchOverTable[] =
  {
    { 264, 1, 0x00 },

    { 265, 2, 0x00 },
    { 265, 2, 0x02 },

    { 266, 3, 0x00 },
    { 266, 3, 0x02 },
    { 266, 3, 0x04 },
    { 266, 3, 0x06 },

    { 267, 4, 0x00 },
    { 267, 4, 0x02 },
    { 267, 4, 0x04 },
    { 267, 4, 0x06 },
    { 267, 4, 0x08 },
    { 267, 4, 0x0A },
    { 267, 4, 0x0C },
    { 267, 4, 0x0E },
  };

  if ( matchOver < 8 )
    _put( 256 + matchOver );
  else if ( matchOver < 38 )
  {
    matchOver -= 8;
    MatchOverItem* item = &_matchOverTable[ matchOver >> 1 ];
    _put( item->symbol, item->nBits, item->bits | ( matchOver & 0x01 ) );
  }
  else
  {
    matchOver -= 38;
    MatchOverItem* item = &_matchOverTable[ matchOver >> 5 ];
    _put( item->symbol + 4 );
    _putBits( item->nBits + 4, ( item->bits << 4 ) | ( matchOver & 0x1F ) );
  }

  static struct DispItem { int nBits; ushort bits; } _dispTable[] = 
  {
    { 3, 0x0000 }, { 3, 0x0001 }, { 4, 0x0004 }, { 4, 0x0005 }, { 5, 0x000C },
    { 5, 0x000D }, { 5, 0x000E }, { 5, 0x000F }, { 6, 0x0020 }, { 6, 0x0021 },
    { 6, 0x0022 }, { 6, 0x0023 }, { 6, 0x0024 }, { 6, 0x0025 }, { 6, 0x0026 },
    { 6, 0x0027 }, { 7, 0x0050 }, { 7, 0x0051 }, { 7, 0x0052 }, { 7, 0x0053 },
    { 7, 0x0054 }, { 7, 0x0055 }, { 7, 0x0056 }, { 7, 0x0057 }, { 7, 0x0058 },
    { 7, 0x0059 }, { 7, 0x005A }, { 7, 0x005B }, { 7, 0x005C }, { 7, 0x005D },
    { 7, 0x005E }, { 7, 0x005F }, { 8, 0x00C0 }, { 8, 0x00C1 }, { 8, 0x00C2 },
    { 8, 0x00C3 }, { 8, 0x00C4 }, { 8, 0x00C5 }, { 8, 0x00C6 }, { 8, 0x00C7 },
    { 8, 0x00C8 }, { 8, 0x00C9 }, { 8, 0x00CA }, { 8, 0x00CB }, { 8, 0x00CC },
    { 8, 0x00CD }, { 8, 0x00CE }, { 8, 0x00CF }, { 8, 0x00D0 }, { 8, 0x00D1 },
    { 8, 0x00D2 }, { 8, 0x00D3 }, { 8, 0x00D4 }, { 8, 0x00D5 }, { 8, 0x00D6 },
    { 8, 0x00D7 }, { 8, 0x00D8 }, { 8, 0x00D9 }, { 8, 0x00DA }, { 8, 0x00DB },
    { 8, 0x00DC }, { 8, 0x00DD }, { 8, 0x00DE }, { 8, 0x00DF }, { 9, 0x01C0 },
    { 9, 0x01C1 }, { 9, 0x01C2 }, { 9, 0x01C3 }, { 9, 0x01C4 }, { 9, 0x01C5 },
    { 9, 0x01C6 }, { 9, 0x01C7 }, { 9, 0x01C8 }, { 9, 0x01C9 }, { 9, 0x01CA },
    { 9, 0x01CB }, { 9, 0x01CC }, { 9, 0x01CD }, { 9, 0x01CE }, { 9, 0x01CF },
    { 9, 0x01D0 }, { 9, 0x01D1 }, { 9, 0x01D2 }, { 9, 0x01D3 }, { 9, 0x01D4 },
    { 9, 0x01D5 }, { 9, 0x01D6 }, { 9, 0x01D7 }, { 9, 0x01D8 }, { 9, 0x01D9 },
    { 9, 0x01DA }, { 9, 0x01DB }, { 9, 0x01DC }, { 9, 0x01DD }, { 9, 0x01DE },
    { 9, 0x01DF }, { 9, 0x01E0 }, { 9, 0x01E1 }, { 9, 0x01E2 }, { 9, 0x01E3 },
    { 9, 0x01E4 }, { 9, 0x01E5 }, { 9, 0x01E6 }, { 9, 0x01E7 }, { 9, 0x01E8 },
    { 9, 0x01E9 }, { 9, 0x01EA }, { 9, 0x01EB }, { 9, 0x01EC }, { 9, 0x01ED },
    { 9, 0x01EE }, { 9, 0x01EF }, { 9, 0x01F0 }, { 9, 0x01F1 }, { 9, 0x01F2 },
    { 9, 0x01F3 }, { 9, 0x01F4 }, { 9, 0x01F5 }, { 9, 0x01F6 }, { 9, 0x01F7 },
    { 9, 0x01F8 }, { 9, 0x01F9 }, { 9, 0x01FA }, { 9, 0x01FB }, { 9, 0x01FC },
    { 9, 0x01FD }, { 9, 0x01FE }, { 9, 0x01FF }
  };

  #if LZBUFBITS < 8
  #error
  #endif

  DispItem* item = &_dispTable[ disp >> ( LZBUFBITS - 7 ) ];
  int nBits = item->nBits + ( LZBUFBITS - 7 );
  uint bits = ( ( ( uint )item->bits ) << ( LZBUFBITS - 7 ) ) |
                 ( disp & ( ( 1 << ( LZBUFBITS - 7 ) ) - 1 ) );
  #if LZBUFBITS >= 15
  if ( nBits > 16 )
  {
    ASSERT ( nBits <= 32 );
    _putBits( nBits - 16, bits >> 16 );
    _putBits( 16, bits & 0xFFFF );
  }
  else
  #endif
  {
    ASSERT ( nBits <= 16 );
    _putBits( nBits, bits );
  }
}


size_t LZHLEncoder::flush()
{
  _put( NHUFFSYMBOLS - 1 );
  while ( _nBits > 0 )
  {
    *_dst++ = (byte )( _bits >> 24 );
    _nBits -= 8;
    _bits <<= 8;
  }

  return _dst - _dstBegin;
}


//
// class LZHLDecoderStat
//
LZHLDecoderStat::LZHLDecoderStat()
{
static LZHLDecoderStat::Group theLZHLDecoderStatGroupTable[ 16 ] =
{
  /*   0: */ {   2,   0 }, /*   1: */ {   3,   4 }, /*   2: */ {   3,  12 },
  /*   3: */ {   4,  20 }, /*   4: */ {   4,  36 }, /*   5: */ {   4,  52 },
  /*   6: */ {   4,  68 }, /*   7: */ {   4,  84 }, /*   8: */ {   4, 100 },
  /*   9: */ {   4, 116 }, /*  10: */ {   4, 132 }, /*  11: */ {   4, 148 },
  /*  12: */ {   4, 164 }, /*  13: */ {   5, 180 }, /*  14: */ {   5, 212 },
  /*  15: */ {   5, 244 }
};

static short theLZHLDecoderStatSymbolTable[ NHUFFSYMBOLS ] =
{
  /*   0 */ 256,/*   1 */ 257,/*   2 */ 258,/*   3 */ 259,/*   4 */ 260,
  /*   5 */ 261,/*   6 */ 262,/*   7 */ 263,/*   8 */ 264,/*   9 */ 265,
  /*  10 */ 266,/*  11 */ 267,/*  12 */ 268,/*  13 */ 269,/*  14 */ 270,
  /*  15 */ 271,/*  16 */   0,/*  17 */  32,/*  18 */  48,/*  19 */ 255,
  /*  20 */   1,/*  21 */   2,/*  22 */   3,/*  23 */   4,/*  24 */   5,
  /*  25 */   6,/*  26 */   7,/*  27 */   8,/*  28 */   9,/*  29 */  10,
  /*  30 */  11,/*  31 */  12,/*  32 */  13,/*  33 */  14,/*  34 */  15,
  /*  35 */  16,/*  36 */  17,/*  37 */  18,/*  38 */  19,/*  39 */  20,
  /*  40 */  21,/*  41 */  22,/*  42 */  23,/*  43 */  24,/*  44 */  25,
  /*  45 */  26,/*  46 */  27,/*  47 */  28,/*  48 */  29,/*  49 */  30,
  /*  50 */  31,/*  51 */  33,/*  52 */  34,/*  53 */  35,/*  54 */  36,
  /*  55 */  37,/*  56 */  38,/*  57 */  39,/*  58 */  40,/*  59 */  41,
  /*  60 */  42,/*  61 */  43,/*  62 */  44,/*  63 */  45,/*  64 */  46,
  /*  65 */  47,/*  66 */  49,/*  67 */  50,/*  68 */  51,/*  69 */  52,
  /*  70 */  53,/*  71 */  54,/*  72 */  55,/*  73 */  56,/*  74 */  57,
  /*  75 */  58,/*  76 */  59,/*  77 */  60,/*  78 */  61,/*  79 */  62,
  /*  80 */  63,/*  81 */  64,/*  82 */  65,/*  83 */  66,/*  84 */  67,
  /*  85 */  68,/*  86 */  69,/*  87 */  70,/*  88 */  71,/*  89 */  72,
  /*  90 */  73,/*  91 */  74,/*  92 */  75,/*  93 */  76,/*  94 */  77,
  /*  95 */  78,/*  96 */  79,/*  97 */  80,/*  98 */  81,/*  99 */  82,
  /* 100 */  83,/* 101 */  84,/* 102 */  85,/* 103 */  86,/* 104 */  87,
  /* 105 */  88,/* 106 */  89,/* 107 */  90,/* 108 */  91,/* 109 */  92,
  /* 110 */  93,/* 111 */  94,/* 112 */  95,/* 113 */  96,/* 114 */  97,
  /* 115 */  98,/* 116 */  99,/* 117 */ 100,/* 118 */ 101,/* 119 */ 102,
  /* 120 */ 103,/* 121 */ 104,/* 122 */ 105,/* 123 */ 106,/* 124 */ 107,
  /* 125 */ 108,/* 126 */ 109,/* 127 */ 110,/* 128 */ 111,/* 129 */ 112,
  /* 130 */ 113,/* 131 */ 114,/* 132 */ 115,/* 133 */ 116,/* 134 */ 117,
  /* 135 */ 118,/* 136 */ 119,/* 137 */ 120,/* 138 */ 121,/* 139 */ 122,
  /* 140 */ 123,/* 141 */ 124,/* 142 */ 125,/* 143 */ 126,/* 144 */ 127,
  /* 145 */ 128,/* 146 */ 129,/* 147 */ 130,/* 148 */ 131,/* 149 */ 132,
  /* 150 */ 133,/* 151 */ 134,/* 152 */ 135,/* 153 */ 136,/* 154 */ 137,
  /* 155 */ 138,/* 156 */ 139,/* 157 */ 140,/* 158 */ 141,/* 159 */ 142,
  /* 160 */ 143,/* 161 */ 144,/* 162 */ 145,/* 163 */ 146,/* 164 */ 147,
  /* 165 */ 148,/* 166 */ 149,/* 167 */ 150,/* 168 */ 151,/* 169 */ 152,
  /* 170 */ 153,/* 171 */ 154,/* 172 */ 155,/* 173 */ 156,/* 174 */ 157,
  /* 175 */ 158,/* 176 */ 159,/* 177 */ 160,/* 178 */ 161,/* 179 */ 162,
  /* 180 */ 163,/* 181 */ 164,/* 182 */ 165,/* 183 */ 166,/* 184 */ 167,
  /* 185 */ 168,/* 186 */ 169,/* 187 */ 170,/* 188 */ 171,/* 189 */ 172,
  /* 190 */ 173,/* 191 */ 174,/* 192 */ 175,/* 193 */ 176,/* 194 */ 177,
  /* 195 */ 178,/* 196 */ 179,/* 197 */ 180,/* 198 */ 181,/* 199 */ 182,
  /* 200 */ 183,/* 201 */ 184,/* 202 */ 185,/* 203 */ 186,/* 204 */ 187,
  /* 205 */ 188,/* 206 */ 189,/* 207 */ 190,/* 208 */ 191,/* 209 */ 192,
  /* 210 */ 193,/* 211 */ 194,/* 212 */ 195,/* 213 */ 196,/* 214 */ 197,
  /* 215 */ 198,/* 216 */ 199,/* 217 */ 200,/* 218 */ 201,/* 219 */ 202,
  /* 220 */ 203,/* 221 */ 204,/* 222 */ 205,/* 223 */ 206,/* 224 */ 207,
  /* 225 */ 208,/* 226 */ 209,/* 227 */ 210,/* 228 */ 211,/* 229 */ 212,
  /* 230 */ 213,/* 231 */ 214,/* 232 */ 215,/* 233 */ 216,/* 234 */ 217,
  /* 235 */ 218,/* 236 */ 219,/* 237 */ 220,/* 238 */ 221,/* 239 */ 222,
  /* 240 */ 223,/* 241 */ 224,/* 242 */ 225,/* 243 */ 226,/* 244 */ 227,
  /* 245 */ 228,/* 246 */ 229,/* 247 */ 230,/* 248 */ 231,/* 249 */ 232,
  /* 250 */ 233,/* 251 */ 234,/* 252 */ 235,/* 253 */ 236,/* 254 */ 237,
  /* 255 */ 238,/* 256 */ 239,/* 257 */ 240,/* 258 */ 241,/* 259 */ 242,
  /* 260 */ 243,/* 261 */ 244,/* 262 */ 245,/* 263 */ 246,/* 264 */ 247,
  /* 265 */ 248,/* 266 */ 249,/* 267 */ 250,/* 268 */ 251,/* 269 */ 252,
  /* 270 */ 253,/* 271 */ 254,/* 272 */ 272,/* 273 */ 273
};

  symbolTable = new short[ NHUFFSYMBOLS ];
  memcpy( symbolTable, theLZHLDecoderStatSymbolTable, 
	  sizeof( short ) * NHUFFSYMBOLS );
  memcpy( groupTable, theLZHLDecoderStatGroupTable, sizeof( Group ) * 16 );
}


LZHLDecoderStat::~LZHLDecoderStat()
{
  delete [] symbolTable;
}


//
// class LZHLDecompressor
//
LZHLDecompressor::LZHLDecompressor()
{
  _nBits = 0;
  _bits = 0;
}


LZHLDecompressor::~LZHLDecompressor()
{
}


int LZHLDecompressor::_get( const byte*& src, const byte* srcEnd, int n )
{
  ASSERT ( n <= 8 );
  if( _nBits < n )
  {
    if ( src >= srcEnd )
    {
      _nBits = 0;
      return -1L;
    }
    _bits |= ( *src++ << ( 24 - _nBits ) );
    _nBits += 8;
  }

  int ret = _bits >> ( 32 - n );
  _bits <<= n;
  _nBits -= n;

  return ret;
}


bool LZHLDecompressor::decompress( byte* dst, size_t* dstSz, const byte* src,
                                   size_t* srcSz )
{
  byte* startDst = dst;
  const byte* startSrc = src;
  const byte* endSrc = src + *srcSz;
  const byte* endDst = dst + *dstSz;
  _nBits = 0;
  for ( ; ; )
  {
    int grp = _get( src, endSrc, 4 );
    if ( grp < 0 )
      return false;
    Group& group = groupTable[ grp ];

    int symbol;
    int nBits = group.nBits;
    if ( nBits == 0 )
      symbol = symbolTable[ group.pos ];
    else
    {
      ASSERT ( nBits <= 8 );
      int got = _get( src, endSrc, nBits );
      if ( got < 0 )
        return false;
      int pos = group.pos + got;
      if ( pos >= NHUFFSYMBOLS )
        return false;
      symbol = symbolTable[ pos ];
    }

    ASSERT ( symbol < NHUFFSYMBOLS );
    ++stat[ symbol ];
    int matchOver;
    if ( symbol < 256 )
    {
      if ( dst >= endDst )
        return false;
      *dst++ = ( byte )symbol;
      _toBuf( symbol );
      continue;
    }
    else if ( symbol == NHUFFSYMBOLS - 2 )
    {
      HuffStatTmpStruct s[ NHUFFSYMBOLS ];
      _makeSortedTmp( s );

      int i;
      for ( i = 0; i < NHUFFSYMBOLS ; ++i )
        symbolTable[ i ] = s[ i ].i;

      int lastNBits = 0;
      int pos = 0;
      for ( i = 0; i < 16 ; ++i )
      {
        int n;
        for ( n = 0 ; ; ++n )
          if ( _get( src, endSrc, 1 ) )
            break;
        lastNBits += n;

        groupTable[ i ].nBits = lastNBits;
        groupTable[ i ].pos = pos;
        pos += 1 << lastNBits;
      }
      ASSERT ( pos < NHUFFSYMBOLS + 255 );

      continue;
    }
    else if ( symbol == NHUFFSYMBOLS - 1 )
      break;

    static 
    struct MatchOverItem { int nExtraBits; int base; } _matchOverTable[] =
    { { 1,   8 }, { 2,  10 }, { 3,  14 }, { 4,  22 }, { 5,  38 }, { 6,  70 },
      { 7, 134 }, { 8, 262 } };

    if ( symbol < 256 + 8 )
      matchOver = symbol - 256;
    else
    {
      MatchOverItem* item = &_matchOverTable[ symbol - 256 - 8 ];
      int extra = 0;
      extra = _get( src, endSrc, item->nExtraBits );
      if ( extra < 0 )
        return false;
      matchOver = item->base + extra;
    }

    int dispPrefix = _get( src, endSrc, 3 );
    if ( dispPrefix < 0 )
      return false;

    static struct DispItem { int nBits; int disp; } _dispTable[] =
    { { 0,  0 }, { 0,  1 }, { 1,  2 }, { 2,  4 }, { 3,  8 }, { 4, 16 },
      { 5, 32 }, { 6, 64 } };

    DispItem* item = &_dispTable[ dispPrefix ];
    nBits = item->nBits + LZBUFBITS - 7;

    int disp = 0;
    ASSERT ( nBits <= 16 );
    if ( nBits > 8 )
    {
      nBits -= 8;
      disp |= _get( src, endSrc, 8 ) << nBits;
    }
    ASSERT ( nBits <= 8 );
    int got = _get( src, endSrc, nBits );
    if ( got < 0 )
      return false;
    disp |= got;

    disp += item->disp << ( LZBUFBITS - 7 );
    ASSERT ( disp >=0 && disp < LZBUFSIZE );

    int matchLen = matchOver + LZMIN;
    if ( dst + matchLen > endDst )
      return false;
    int pos = _bufPos - disp;
    if ( matchLen < disp )
      _bufCpy( dst, pos, matchLen );
    else
    {
      _bufCpy( dst, pos, disp );
      for ( int i=0; i < matchLen - disp; ++i )
        dst[ i + disp ] = dst[ i ];
    }
    _toBuf( dst, matchLen );
    dst += matchLen;
  }

  if ( dstSz )
    *dstSz -= dst - startDst;
  if ( srcSz )
    *srcSz -= src - startSrc;

  return true;
}
