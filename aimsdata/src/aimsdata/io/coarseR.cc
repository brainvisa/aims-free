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

/*
 *  Data reader class
 */

#include <aims/io/coarseR.h>
#include <aims/io/finder.h>
#include <aims/io/reader.h>
#include <aims/io/process.h>
#include <aims/utility/converter_volume.h>

using namespace aims;
using namespace carto;
using namespace std;

namespace aims
{
  namespace internal
  {
    class CoarseConverter : public Process
    {
    public:
      CoarseConverter( int b, int frm, AimsData<int16_t> & d )
	: Process(), border( b ), frame( frm ), data( d ) {}
      virtual ~CoarseConverter() {}
      int		border;
      int		frame;
      AimsData<int16_t>	& data;
    };
  }
}

CoarseReader::CoarseReader( const std::string & f )
  : _filename( f )
{
}


CoarseReader::~CoarseReader()
{
}


static bool direct( Process & p, const string & fname, Finder & f )
{
  string			fmt = f.format();
  Reader<AimsData<int16_t> >	r( fname );
  aims::internal::CoarseConverter & cc = (aims::internal::CoarseConverter &) p;
  return( r.read( cc.data, cc.border, &fmt, cc.frame ) );
}


template<typename T>
static bool convert( Process & p, const string & fname, Finder & f )
{
  string			fmt = f.format();
  Reader<T>			r( fname );
  aims::internal::CoarseConverter	& cc = (aims::internal::CoarseConverter &) p;
  T				data;
  r.read( data, cc.border, &fmt, cc.frame );
  cc.data = AimsData<int16_t>( data.dimX(), data.dimY(), data.dimZ(), 
			     data.dimT() );
  ShallowConverter<T, AimsData<int16_t> >	conv;
  conv.convert( data, cc.data );
  return( true );
}


void CoarseReader::read( AimsData<int16_t> & data, int border, 
			 const std::string*, int frame )
{
  internal::CoarseConverter	p( border, frame, data );
  p.registerProcessType( "Volume", "S16", &direct );
  p.registerProcessType( "Volume", "S8", &convert<AimsData<int8_t> > );
  p.registerProcessType( "Volume", "U8", &convert<AimsData<uint8_t> > );
  p.registerProcessType( "Volume", "U16", &convert<AimsData<uint16_t> > );
  p.registerProcessType( "Volume", "S32", &convert<AimsData<int32_t> > );
  p.registerProcessType( "Volume", "U32", &convert<AimsData<uint32_t> > );
  p.registerProcessType( "Volume", "FLOAT", &convert<AimsData<float> > );
  p.registerProcessType( "Volume", "DOUBLE", &convert<AimsData<double> > );

  // ### remove after everything has been moved to intN_t/uintN_t
  p.registerProcessType( "Volume", "S8", &convert<AimsData<char> > );
  p.registerProcessType( "Volume", "S64", &convert<AimsData<int64_t> > );
  p.registerProcessType( "Volume", "U64", &convert<AimsData<uint64_t> > );

  p.execute( _filename );
}
