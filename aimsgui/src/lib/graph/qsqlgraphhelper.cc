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

#include <aims/graph/qsqlgraphhelper.h>
#include <soma-io/reader/pythonreader.h>
#include <qsqlquery.h>

using namespace aims;
using namespace carto;
using namespace std;


Object QSqlGraphHelper::intHelper( const QVariant & res, bool & ok )
{
  int value = res.toInt( &ok );
  if( ok )
    return Object::value( value );
  return none();
}


Object QSqlGraphHelper::floatHelper( const QVariant & res, bool & ok )
{
  float value = (float) res.toDouble( &ok );
  if( ok )
    return Object::value( value );
  return none();
}


Object QSqlGraphHelper::doubleHelper( const QVariant & res, bool & ok )
{
  double value = res.toDouble( &ok );
  if( ok )
    return Object::value( value );
  return none();
}


Object QSqlGraphHelper::stringHelper( const QVariant & res, bool & ok )
{
  if( res.isNull() )
  {
    ok = false;
    return none();
  }
  ok = true;
  string value = res.toString().utf8().data();
  return Object::value( value );
}


Object QSqlGraphHelper::pythonHelper( const QVariant & res, bool & ok )
{
  if( res.isNull() )
  {
    ok = false;
    return none();
  }
  string value = res.toString().utf8().data();
  ok = true;
  istringstream sst( value );
  PythonReader pr;
  pr.attach( sst );
  return pr.read( 0, "" );
}


Object QSqlGraphHelper::pythonHelperWithSyntax( const QVariant & res,
                                                bool & ok,
                                                const string & stype )
{
  if( res.isNull() )
  {
    ok = false;
    return none();
  }
  string value = res.toString().utf8().data();
  ok = true;
  istringstream sst( value );
  SyntaxSet ss;
  ss[ "__generic__" ][ "__fallback__" ] = Semantic( stype, false );
  PythonReader pr( ss );
  pr.attach( sst );
  return pr.read( 0, "__fallback__" );
}


Object QSqlGraphHelper::intVectorHelper( const QVariant & res, bool & ok )
{
  return pythonHelperWithSyntax( res, ok, "int_vector" );
}


Object QSqlGraphHelper::floatVectorHelper( const QVariant & res, bool & ok )
{
  return pythonHelperWithSyntax( res, ok, "float_vector" );
}


Object QSqlGraphHelper::stringVectorHelper( const QVariant & res, bool & ok )
{
  return pythonHelperWithSyntax( res, ok, "string_vector" );
}


Object QSqlGraphHelper::typedValue( const QVariant & res, const string & att,
                                    const vector<string> & sem,
                                    string & attname, bool & ok )
{
  Object value;
  ok = false;

  typedef Object (*HelperFunc)( const QVariant &, bool & );
  static map<string, HelperFunc> semanticTypes;
  if( semanticTypes.empty() )
  {
    semanticTypes[ "Int()" ] = intHelper;
    semanticTypes[ "Float()" ] = floatHelper;
    semanticTypes[ "Double()" ] = doubleHelper;
    semanticTypes[ "String()" ] = stringHelper;
    semanticTypes[ "List()" ] = pythonHelper;
    semanticTypes[ "IntVector()" ] = intVectorHelper;
    semanticTypes[ "FloatVector()" ] = floatVectorHelper;
    semanticTypes[ "StringVector()" ] = stringVectorHelper;
    semanticTypes[ "Dictionary()" ] = pythonHelper;
  }
  const string & type = sem[0];
  map<string, HelperFunc>::const_iterator ihelper
    = semanticTypes.find( type );
  if( ihelper != semanticTypes.end() )
    value = ihelper->second( res, ok );
  else
    value = stringHelper( res, ok );
  if( !sem[1].empty() )
    attname = sem[1];
  else
    attname = att;
  return value;
}


