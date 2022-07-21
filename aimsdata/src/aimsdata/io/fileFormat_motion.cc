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
#include <aims/io/baseFormats_motion.h>
#include <aims/io_soma/trm_header_formatchecker.h>
#include <aims/io_soma/trm_compose_formatchecker.h>
#include <soma-io/datasourceinfo/datasourceinfoloader.h>

using namespace aims;
using namespace soma;
using namespace std;

namespace aims
{

#if defined( __APPLE__ ) && (__GNUC__-0) < 4
template<> FileFormat<Motion>::~FileFormat() {}
#endif

template<> void 
FileFormatDictionary<Motion>::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "trm" );
  TrmFormat	*fm = new TrmFormat;
  registerFormat( "TRM", fm, ext );
}


template class FileFormatDictionary<Motion>;
template class FileFormat<Motion>;

} // namespace aims


static bool _motiondic()
{
  FileFormatDictionary<Motion>::init();

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

  return true;
}

static bool motiondic __attribute__((unused)) = _motiondic();

