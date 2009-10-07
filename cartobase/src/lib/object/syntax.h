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

#ifndef CARTOBASE_OBJECT_SYNTAX_H
#define CARTOBASE_OBJECT_SYNTAX_H

//=============================================================================
//	H E A D E R  F I L E S
//=============================================================================

#include <cartobase/config/cartobase_config.h>
#include <string>
#include <map>

namespace carto
{

  //===========================================================================
  //	T Y P E D E F
  //===========================================================================

  /**	Specify properties for semantic attributes - mainly for I/O purposes
   */
  struct CARTOBASE_API Semantic
  {
    inline Semantic();
    inline Semantic( const std::string & t, bool need = false, 
		     bool intern = false );
    /**	type of semantic attribute, such as `\c float'
     */
    std::string type;

    /**	is this semantic attribute mandatory?
     */
    bool needed;
    ///	internal attributes are not written to disk
    bool internal;
  };


  /**	This lookup table associates a semantic attribute with its properties
   */
  typedef std::map<std::string,Semantic> SemanticSet;


  /**	Specify syntax for syntactic attributes
   */
  typedef SemanticSet Syntax;


  /**	This lookup table associates a syntactic attribute with its syntax
   */
  typedef std::map<std::string,Syntax> SyntaxSet;


  inline Semantic::Semantic() 
    : needed( false ), internal( false )
  {
  }


  inline Semantic::Semantic( const std::string & t, bool need, 
			     bool intern )
    : type( t ), needed( need ), internal( intern )
  {
  }

}

#endif
