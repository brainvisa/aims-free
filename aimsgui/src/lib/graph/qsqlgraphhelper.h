/* This software and supporting documentation are distributed by
*     Institut Federatif de Recherche 49
*     CEA/NeuroSpin, Batiment 145,
*     91191 Gif-sur-Yvette cedex
*     France
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

#ifndef AIMS_GRAPH_QSQLGRAPHHELPER_H
#define AIMS_GRAPH_QSQLGRAPHHELPER_H

#include <cartobase/object/object.h>
#include <qvariant.h>

namespace aims
{

  class QSqlGraphHelper
  {
  public:
    static carto::Object intHelper( const QVariant & res, bool & ok );
    static carto::Object floatHelper( const QVariant & res, bool & ok );
    static carto::Object doubleHelper( const QVariant & res, bool & ok );
    static carto::Object stringHelper( const QVariant & res, bool & ok );
    static carto::Object pythonHelper( const QVariant & res, bool & ok );
    static carto::Object pythonHelperWithSyntax( const QVariant & res,
                                                 bool & ok,
                                                 const std::string & stype );
    static carto::Object intVectorHelper( const QVariant & res, bool & ok );
    static carto::Object floatVectorHelper( const QVariant & res, bool & ok );
    static carto::Object stringVectorHelper( const QVariant & res, bool & ok );
    static carto::Object typedValue( const QVariant & res,
                                     const std::string & att,
                                     const std::vector<std::string> & sem,
                                     std::string & attname, bool & ok );
  };

}

#endif

