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

#ifndef CARTOBASE_ALGORITHM_ALGORITHM_H
#define CARTOBASE_ALGORITHM_ALGORITHM_H


//===========================================================================//
//                                                                           //
// WARNING : All code here is under construction. Do not use it.             //
//                                                                           //
//===========================================================================//


#include <cartobase/object/object.h>
#include <cartobase/object/property.h>

namespace carto {

  //-----------------//
 //  BaseParameter  //
//-----------------//

class BaseParameter
{
public:
  virtual ~BaseParameter();
};


  //----------------//
 //  Parameter<T>  //
//----------------//

template <typename T>
  class ParameterModifier;

template <typename T>
class Parameter : public BaseParameter
{
 public:
  inline Parameter() {}
  inline Parameter( T &ref, const std::string &name, 
                    const std::string &doc ) :
    _pValue( &ref ),
    _name( name ),
    _documentation( doc ) {}
  virtual ~Parameter();
  
 private:
  
  friend class ParameterModifier<T>;
  
  T *_pValue;
  std::string _name;
  std::string _documentation;
  bool _input;
  bool _output;
  std::vector< std::pair< std::string, T > > _choices;
  bool _optional;
};


  //-------------//
 //  Algorithm  //
//-------------//

class Algorithm
{
public:

  Algorithm( const std::string &name );


protected:

  template <typename T>
  ParameterModifier<T> inputParameter( T &ref, const std::string &name, 
                                       const std::string &documentation );
  template <typename T>
  ParameterModifier<T> outputParameter( T &ref, const std::string &name, 
                                        const std::string &documentation );

private:

  std::string _name;

  // It is not going to be a vector in the future, maybe a kind of ParameterSet
  std::vector< rc_ptr< BaseParameter > > _parameters;
};


  //------------------------//
 //  ParameterModifier<T>  //
//------------------------//

template <typename T>
class ParameterModifier
{
public:
  
  inline ParameterModifier( const ParameterModifier<T> &pm ) :
  _parameter( pm._parameter ) {}

  inline ParameterModifier( Parameter<T> &p ) :
  _parameter( p ) {}
  
  inline ParameterModifier<T> optional( bool o = true ) 
  { 
    _parameter._optional = o;
    return *this;
  }
  
  inline ParameterModifier<T> choice( const T &value )
  {
    return choices( "", value );
  }

  inline ParameterModifier<T> choice( const std::string &label,
                                       const T &value )
  { 
    _parameter._choices.push_back( std::pair<std::string, T>( label,
                                                              value ) );
    return *this; 
  }
  
 private:
  
  Parameter<T> &_parameter;
};


  //-------------------//
 //  AlgorithmCaller  //
//-------------------//

class AlgorithmCaller
{
public:

  AlgorithmCaller( const std::string &algorithmName );
  template <typename T>
  AlgorithmCaller &operator <<( const T & );

  class LaunchExecution {};

private:

  std::string _name;
  std::vector< carto::Object > _unnamedParameters;
  carto::Object _namedParameters;
};

extern const AlgorithmCaller::LaunchExecution execute;

typedef AlgorithmCaller algo;





  //----------------//
 //  Parameter<T>  //
//----------------//

//-----------------------------------------------------------------------------
template <typename T>
Parameter<T>::~Parameter() {}


  //-------------//
 //  Algorithm  //
//-------------//

//-----------------------------------------------------------------------------
template <typename T>
ParameterModifier<T>
Algorithm::inputParameter( T &ref, const std::string &name, 
                           const std::string &doc )
{
  _parameters.push_back( rc_ptr<BaseParameter>( new Parameter<T>( ref, name, doc ) ) );
  // TODO set _input = true;
  return ParameterModifier<T>( static_cast< Parameter<T> &>( *_parameters.rbegin()->get() ) );
}


//-----------------------------------------------------------------------------
template <typename T>
ParameterModifier<T>
Algorithm::outputParameter( T &ref, const std::string &name, 
                            const std::string &doc )
{
  _parameters.push_back( rc_ptr<BaseParameter>( new Parameter<T>( ref, name, doc ) ) );
  // TODO set _output = true;
  return ParameterModifier<T>( static_cast< Parameter<T> &>( *_parameters.rbegin()->get() ) );
}


  //-------------------//
 //  AlgorithmCaller  //
//-------------------//

template <typename T>
AlgorithmCaller &AlgorithmCaller::operator <<( const T &v )
{
  _unnamedParameters.push_back( Object::value( v ) );
  return *this;
}

template <>
AlgorithmCaller &AlgorithmCaller::
operator <<( const  AlgorithmCaller::LaunchExecution & );



} // namespace carto


#endif //ifndef CARTOBASE_ALGORITHM_ALGORITHM_H
