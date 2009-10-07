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

//=============================================================================
//	H E A D E R  F I L E S
//=============================================================================

#if defined( _WIN32 ) || ( defined( __GNUC__ ) && ( __GNUC__-0 >= 3 ) )
/* Bon OK je sais, il ne faut pas, c'est mal, il ne faut SURTOUT JAMAIS le 
   faire, sous aucun pretexte, et sans exception, on ne doit jamais dire 
   "ah oui mais la, vraiment, c'est le seul moyen!", si on le fait une fois, 
   on est tente de le refaire. On ne doit pas non plus se refugier derriere 
   le pretexte "oui mais aussi leur classe de base est mal faite, et dans les 
   headers de tel systeme/compilateur ca passe tres bien alors je peux me 
   permettre une fois", nonnonnon, ca n'est pas du tout la bonne solution.

   Bref, vous avez compris: je le fais...
   (Et Dimitri va m'engueuler...)

   Denis 03/07/01
*/
#define protected public
#include <streambuf>
#undef protected

#endif	// _WIN32

#include <cartobase/stream/fistreambuf.h>
#include <cartobase/stream/uncomment.h>
#include <cartobase/stream/counter.h>
#include <limits.h>
#include <assert.h>
#include <stdio.h>

using namespace carto;
using namespace std;

//=============================================================================
//	M E T H O D S
//=============================================================================

template <class Extractor>
fistreambuf<Extractor>::fistreambuf(streambuf* source,
	const Extractor& extractor, bool d)
	: _source(source), _extractor(extractor), _delete(d)
{
}


template <class Extractor>
fistreambuf<Extractor>::fistreambuf(streambuf* source,
	bool d)
	: _source(source), _delete(d)
{
}


template <class Extractor>
fistreambuf<Extractor>::~fistreambuf()
{
	sync();
	if (_delete)
		delete _source;
}


template <class Extractor>
int
fistreambuf<Extractor>::underflow()
{
	int result = EOF;
	if (gptr() < egptr())
		result = *gptr();
	else if (_source != 0)
	{
		result = _extractor(*_source);
		if (result != EOF)
		{
			assert(result >= 0 && result <= UCHAR_MAX);
			_buffer = result;
			setg(&_buffer , &_buffer , &_buffer + 1);
		}
	}
	return result;
}


template <class Extractor>
int
fistreambuf<Extractor>::sync()
{
	int result = 0;
	if (_source != 0)
	{
		if (gptr() < egptr())
		{
			result = _source->sputbackc(*gptr());
			setg(0, 0, 0);
		}
		if (_source->sync() == EOF)
			result = EOF;
	}
	return result;
}


template <class Extractor>
streambuf*
fistreambuf<Extractor>::setbuf(char* p , int len)
{
	return _source == 0 ? 0 : _source->setbuf(p, len);
}


template <class Extractor>
streambuf::pos_type 
fistreambuf<Extractor>::seekoff( off_type type, 
                                 ios_base::seekdir dir, 
                                 ios_base::openmode mode )
{
  return _source->seekoff( type, dir, mode );
}

//=============================================================================
//	T E M P L A T E  I N S T A N C I A T I O N
//=============================================================================

template class fistreambuf<UncommentExtractor>;
template class fistreambuf<CounterExtractor>;
