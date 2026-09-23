/* Copyright (c) 1995-2005 CEA
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

// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/fibers/trxbundlereader.h>
// #include <aims/transformation/affinetransformation3d.h>
#include <trx/trx.h>

using namespace aims;
using namespace carto;
using namespace std;


//-----------------------------------------------------------------------------
TrxBundleReader::TrxBundleReader()
  : BundleFormatReader()
{
}


//-----------------------------------------------------------------------------
TrxBundleReader::~TrxBundleReader()
{
}


//-----------------------------------------------------------------------------
Object TrxBundleReader::readHeader()
{
  // cout << "TrxBundleReader::readHeader\n";
  auto trx = trx::load_any( filename() );
  try
  {
    Object hdr = Object::value( Dictionary() );

    // std::cout << "dtype       : " << trx.positions.dtype << "\n";
    // std::cout << "streamlines : " << trx.num_streamlines() << "\n";
    // std::cout << "vertices    : " << trx.num_vertices() << "\n";

    hdr->setProperty( "curves_data_type", trx.positions.dtype );
    hdr->setProperty( "curves_count", trx.num_streamlines() );

    vector<string> groups;
    vector<unsigned long> counts;
    for (const auto& [name, arr] : trx.groups)
    {
        groups.push_back( name );
        counts.push_back( arr.rows );
    }
    hdr->setProperty( "bundles", groups );
    hdr->setProperty( "curve3d_count", counts );

    trx.close();;
    return hdr;
  }
  catch( ... )
  {
    trx.close();;
    throw;
  }
}


template <typename T> void TrxBundleReader::read_trx()
{
  auto reader = trx::load<T>( filename() );
  auto & trx = *reader;
  size_t i, n;

  try
  {
    BundleInfo currentBundle( 0, "trx_bundle" );
    startBundle( currentBundle );

    trx.for_each_streamline([&](size_t idx, uint64_t start, uint64_t length)
    {
      // Zero-copy block view of this streamline's vertices.
      auto pts = trx.streamlines->_data.block(
        static_cast<Eigen::Index>(start), 0,
        static_cast<Eigen::Index>(length), 3);

      // pts is an Eigen expression — no heap allocation.
      FiberInfo fiberInfo( idx );
      startFiber( currentBundle, fiberInfo );
      // cout << "fiber: " << pts.rows() << ", start: " << start << ", len: " << length << endl;
      for( i=0, n=pts.rows(); i<n; ++i )
      {
        Point3df pos( pts( i, 0 ), pts( i, 1 ), pts( i, 2 ) );
        addFiberPoint( currentBundle, fiberInfo, pos );
      }

      terminateFiber( currentBundle, fiberInfo );
    });

    trx.close();
    terminateBundle( currentBundle );
    noMoreBundle();
  }
  catch( ... )
  {
    trx.close();
    throw;
  }
}



void TrxBundleReader::read()
{
  // cout << "TrxBundleReader::read\n";
  Object hdr = readHeader();
  string dtype = hdr->getProperty( "curves_data_type" )->getString();
  if( dtype == "float32" )
  {
    read_trx<float>();
    return;
  }
  throw runtime_error( string( "unsuppoorted data type: " ) + dtype );
}


BundleFormatReader* TrxBundleReader::create()
{
  return new TrxBundleReader;
}
