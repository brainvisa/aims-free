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
#include <aims/io/baseFormats_sparsematrix.h>


using namespace aims;
using namespace std;

namespace aims
{

template<> void
FileFormatDictionary<SparseMatrix>::registerBaseFormats()
{
  std::vector<std::string>      ext;
  ext.push_back( "imas" );
  ImasFormat      *fm = new ImasFormat;
  registerFormat( "IMASPARSE", fm, ext );
}


template<> void
FileFormatDictionary<SparseOrDenseMatrix>::registerBaseFormats()
{
  std::vector<std::string>      ext;
  ext.push_back( "imas" );
  ImasSorDFormat      *fm = new ImasSorDFormat;
  registerFormat( "IMASPARSE", fm, ext );
}


template class FileFormatDictionary<SparseMatrix>;
template class FileFormat<SparseMatrix>;
template class FileFormatDictionary<SparseOrDenseMatrix>;
template class FileFormat<SparseOrDenseMatrix>;

} // namespace aims


static bool _sparsematdic()
{
  FileFormatDictionary<SparseMatrix>::init();
  FileFormatDictionary<SparseOrDenseMatrix>::init();
  return true;
}

static bool sparsematdic __attribute__((unused)) = _sparsematdic();

