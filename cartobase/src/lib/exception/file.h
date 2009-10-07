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


#ifndef CARTOBASE_EXCEPTION_FILE_H
#define CARTOBASE_EXCEPTION_FILE_H


//=============================================================================
//	H E A D E R  F I L E S
//=============================================================================

#include <cartobase/config/cartobase_config.h>
#include <cartobase/exception/errno.h>


//=============================================================================
//	C L A S S  D E C L A R A T I O N
//=============================================================================


namespace carto
{

/** Exceptions thrown during file manipulation
 */
class CARTOBASE_API file_error : public errno_error
{

  public:

    //---------------------------------------------------------------------
    /**	@name Constructors, destructor*/
    //---------------------------------------------------------------------
    //@{

    /**	Only use this version
	@param filename_arg name of the file
    */
    file_error( const std::string& filename_arg );

    /*	DO NOT USE! DEPRECIATED
	@param what_arg not used
	@param filename_arg name of the file
    */
    file_error( const std::string& what_arg, const std::string& filename_arg );

    ///
    virtual ~file_error() throw ();

    //@}

    //---------------------------------------------------------------------
    /**	@name Data access*/
    //---------------------------------------------------------------------
    //@{

    ///
    const char* filename() const;

    //@}

  private:

    //---------------------------------------------------------------------
    /**	@name Initialization*/
    //---------------------------------------------------------------------
    //@{

    /**	format the "what" message
	@param filename_arg name of the file
    */
    static std::string init( const std::string& filename_arg );

    //@}

    //---------------------------------------------------------------------
    /**	@name Data*/
    //---------------------------------------------------------------------
    //@{

    /// the name of the file is supplied by the user
    const std::string _filename;

    //@}

};


}  // namespace carto


#endif
