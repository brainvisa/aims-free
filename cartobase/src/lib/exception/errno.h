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

#ifndef CARTOBASE_EXCEPTION_ERRNO_H
#define CARTOBASE_EXCEPTION_ERRNO_H


//=============================================================================
//	H E A D E R  F I L E S
//=============================================================================

#include <cartobase/config/cartobase_config.h>
#include <stdexcept>
#include <string>


//=============================================================================
//	C L A S S  D E C L A R A T I O N
//=============================================================================


namespace carto
{


/**

    System exceptions memorize the value of errno

*/
class CARTOBASE_API errno_error : public std::runtime_error
{

  public:

    //---------------------------------------------------------------------
    /**	@name Constructors and Destructor*/
    //---------------------------------------------------------------------
    //@{

    /**	Memorize the value of "errno" when thrown
     */
    errno_error();

    ///
    virtual ~errno_error() throw ();

    //@}

    //---------------------------------------------------------------------
    /**	@name Data access*/
    //---------------------------------------------------------------------
    //@{

    ///
    int thrown_errno() const;

    //@}

  protected:

    //---------------------------------------------------------------------
    /**	@name Constructors and Destructor*/
    //---------------------------------------------------------------------
    //@{

    /**	For use by derived classes only!
	@param what_arg "what" string given by the derived class
    */
    errno_error( const std::string& what_arg );

    //@}

  private:

    //---------------------------------------------------------------------
    /**	@name Initialization*/
    //---------------------------------------------------------------------
    //@{

    static std::string init();

    //@}

    //---------------------------------------------------------------------
    /**	@name Data*/
    //---------------------------------------------------------------------
    //@{

    /// value of errno at the time the exception is thrown
    const int _errno_err;

    //@}

};


}  // namespace carto


#endif
