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

// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

/*
 *  IO classes
 */

/* this source is only here to force instanciation of some of the most useful 
   Reader templates */

#include <aims/io/fileFormat_d.h>
#include <aims/io/reader_d.h>
#include <aims/io/writer_d.h>
#include <aims/transformation/affinetransformation3d.h>
#include <aims/transformation/transformationgraph3d.h>
#include <aims/io_soma/trmformatchecker.h>
#include <aims/io_soma/xfmformatchecker.h>
#include <aims/io_soma/ltaformatchecker.h>
#include <aims/io_soma/trm_header_formatchecker.h>
#include <aims/io_soma/trm_compose_formatchecker.h>
#include <aims/io_soma/trmingraphformatchecker.h>
#include <aims/io_soma/trmgraphformatchecker.h>
#include <soma-io/datasourceinfo/datasourceinfoloader.h>

using namespace aims;
using namespace soma;
using namespace std;

namespace aims
{

template<> void 
FileFormatDictionary<AffineTransformation3d>::registerBaseFormats()
{
}


template<> void
FileFormatDictionary<Transformation3d>::registerBaseFormats()
{
}


template<> void
FileFormatDictionary<TransformationGraph3d>::registerBaseFormats()
{
}


template<> Transformation3d*
FileFormat<Transformation3d>::read( const std::string & filename,
                                    const carto::AllocatorContext & context,
                                    carto::Object options )
{
  AffineTransformation3d	*object = new AffineTransformation3d;
  try
  {
    if( read( filename, *object, context, options ) )
      return object;
  }
  catch( std::exception & )
  {
    delete object;
    throw;
  }
  delete object;
  return 0;
}


template class FileFormatDictionary<AffineTransformation3d>;
template class FileFormat<AffineTransformation3d>;

template class FileFormatDictionary<Transformation3d>;
template class FileFormat<Transformation3d>;
template class Reader<Transformation3d>;
template class Writer<Transformation3d>;

template class FileFormatDictionary<TransformationGraph3d>;
template class FileFormat<TransformationGraph3d>;
template class Reader<TransformationGraph3d>;
template class Writer<TransformationGraph3d>;

} // namespace aims


static bool _motiondic()
{
  FileFormatDictionary<AffineTransformation3d>::init();
  FileFormatDictionary<Transformation3d>::init();
  FileFormatDictionary<TransformationGraph3d>::init();

  // register soma-io checker for the TRM format
  {
    vector<string>  exts(1);
    exts[0] = "trm";
    DataSourceInfoLoader::registerFormat( "TRM",
                                          new TrmFormatChecker, exts );
  }

  // register soma-io checker for the XFM format
  {
    vector<string>  exts(1);
    exts[0] = "xfm";
    DataSourceInfoLoader::registerFormat( "XFM",
                                          new XfmFormatChecker, exts );
  }

  // register soma-io checker for the LTA format
  {
    vector<string>  exts(1);
    exts[0] = "lta";
    DataSourceInfoLoader::registerFormat( "LTA",
                                          new LtaFormatChecker, exts );
  }

  // register soma-io checker for the TRMHEADER format
  {
    vector<string>  exts(1);
    exts[0] = "trmhdr";
    DataSourceInfoLoader::registerFormat( "TRMHEADER",
                                          new TrmHeaderFormatChecker, exts );
  }

  // register soma-io checker for the TRMCOMPOSE format
  {
    vector<string>  exts(1);
    exts[0] = "trmc";
    DataSourceInfoLoader::registerFormat( "TRMCOMPOSE",
                                          new TrmComposeFormatChecker, exts );
  }

  // register soma-io checker for the TRMCOMPOSE format for non-affine
  // transforms
  {
    vector<string>  exts(1);
    exts[0] = "trmc";
    DataSourceInfoLoader::registerFormat( "TRMCHAIN",
                                          new TrmChainFormatChecker, exts );
  }

  // register soma-io checker for the TRMINGRAPH format
  {
    vector<string>  exts(1);
    exts[0] = "yaml";
    DataSourceInfoLoader::registerFormat( "TRMINGRAPH",
                                          new TrmInGraphFormatChecker, exts );
  }

  // register soma-io checker for the TRMGRAPH format
  {
    vector<string>  exts(1);
    exts[0] = "yaml";
    DataSourceInfoLoader::registerFormat( "TRMGRAPH",
                                          new TrmGraphFormatChecker, exts );
  }

  return true;
}

static bool motiondic __attribute__((unused)) = _motiondic();


#include <soma-io/io/formatdictionary_d.h>
#include <soma-io/io/reader_d.h>
#include <soma-io/io/writer_d.h>

namespace soma
{
  template class FormatDictionary<Transformation3d>;
  template class Reader<Transformation3d>;
  template class Writer<Transformation3d>;

  template class FormatDictionary<TransformationGraph3d>;
  template class Reader<TransformationGraph3d>;
  template class Writer<TransformationGraph3d>;
}

