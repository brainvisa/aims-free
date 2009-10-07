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

#ifndef CARTOBASE_EXCEPTION_FORMAT_H
#define CARTOBASE_EXCEPTION_FORMAT_H


#include <cartobase/config/cartobase_config.h>
#include <cartobase/exception/ioexcept.h>
#include <stdexcept>


namespace carto
{

//////////////////////
//  invalid_number  //
//////////////////////

class CARTOBASE_API invalid_number : public format_error
{

  public:

    invalid_number( const std::string &value );
    inline invalid_number( const std::string &message, bool ) :
      format_error( message, "" ) {}

};


///////////////////////
//  invalid_integer  //
///////////////////////

class CARTOBASE_API invalid_integer : public invalid_number
{

  public:

    invalid_integer( const std::string &value );
    inline invalid_integer( const std::string &message, bool fullMessage ) :
    invalid_number( message, fullMessage ) {}

};


////////////////////////////////
//  invalid_positive_integer  //
////////////////////////////////

class CARTOBASE_API invalid_positive_integer : public invalid_integer
{

  public:

    invalid_positive_integer( const std::string &value );
    inline invalid_positive_integer( const std::string &message, 
                                     bool fullMessage ) :
    invalid_integer( message, fullMessage ) {}

};


//////////////////////////
//  out_of_range_error  //
//////////////////////////

/// there's already a out_of_range class in \<stdexcept\> !
class CARTOBASE_API out_of_range_error : public std::range_error
{

  public:

    out_of_range_error( const std::string &value );
    inline out_of_range_error( const std::string &message, bool ) :
    std::range_error( message ) {}

};


///////////////////////
//  invalid_boolean  //
///////////////////////

class CARTOBASE_API invalid_boolean : public format_error
{

  public:

    invalid_boolean( const std::string &value );
    inline invalid_boolean( const std::string &message, bool ) :
      format_error( message, "" ) {}

};


}


#endif
