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

#ifndef CARTOBASE_OBJECT_AREADER_H
#define CARTOBASE_OBJECT_AREADER_H

//=============================================================================
//	H E A D E R  F I L E S
//=============================================================================

#include <cartobase/config/cartobase_config.h>
#ifndef CARTOBASE_OBJECT_SYNTAX_H
#include <cartobase/object/syntax.h>
#endif
#include <cartobase/object/object.h>
#include <cartobase/datasource/datasource.h>

namespace carto
{

  //===========================================================================
  //	C L A S S  D E C L A R A T I O N
  //===========================================================================

  /**	To read GenericObject from a file
   */
  class CARTOBASE_API AttributedReader
  {

  public:

    //---------------------------------------------------------------------
    /**	@name Associated types*/
    //---------------------------------------------------------------------
    //@{

    typedef void (*Helper)( GenericObject&, const std::string&, 
                            DataSource &);
    typedef std::map<std::string,Helper> HelperSet;

    //@}

    //---------------------------------------------------------------------
    /**	@name Constructors, destructor*/
    //---------------------------------------------------------------------
    //@{

    AttributedReader( rc_ptr<DataSource> ds,
                      const SyntaxSet& rules,
                      const HelperSet& helpers = HelperSet() );

    /**	configure the parametres
	@param filename name of the file to read into
	@param rules list of all possible semantic attributes
	@param helpers for reading basic data types
    */
    AttributedReader(const std::string& filename,
		     const SyntaxSet& rules,
		     const HelperSet& helpers = HelperSet());

    /**	configure the parametres
	@param rules list of all possible semantic attributes
	@param helpers for reading basic data types
    */
    AttributedReader(const SyntaxSet& rules,
		     const HelperSet& helpers = HelperSet());

    /**	clean up
     */
    virtual ~AttributedReader();

    //@}

    //---------------------------------------------------------------------
    /**	@name Open, close*/
    //---------------------------------------------------------------------
    //@{

    /**	open a new file
	@param filename name of the file to read from
    */
    void open(const std::string& filename);
    void attach( rc_ptr<DataSource> ds );
    ///	attach to an existing istream
    void attach( std::istream & stream, int line_num = 1 );

    /**	detach or close the stream
     */
    void close();
    rc_ptr<DataSource> dataSource() { return _datasource; }

    //@}

    //---------------------------------------------------------------------
    /**	@name Query*/
    //---------------------------------------------------------------------
    //@{

    ///	name of the file to read from
    virtual std::string name() const;

    ///
    int line() const;

    //@}

    //---------------------------------------------------------------------
    /**	@name Helpers*/
    //---------------------------------------------------------------------
    //@{

    /**	return the status of the stream
	@return true if ifstream::operator!() returns true
    */
    bool operator!() const;

    /**	is the stream open?
	@return ifstream::is_open()
    */
    bool is_open() const;

    /**	have we hit EOF?
	@return true if EOF already hit, false otherwise
    */
    bool eof() const;

    //@}

  protected:

    //---------------------------------------------------------------------
    /**	@name Helpers*/
    //---------------------------------------------------------------------
    //@{

    /**	construct the value of a semantic attribute
	@param object GenericObject to attach to
	@param semantic semantic attribute name
    */
    void readAttribute(GenericObject& object, 
		       const std::string& semantic);

    // DO NOT USE! DEPRECIATED
    void readAttribute(GenericObject* object, 
		       const std::string& semantic);

    //@}

    //---------------------------------------------------------------------
    /**	@name Data*/
    //---------------------------------------------------------------------
    //@{

    ///	rules for every syntactic attribute
    SyntaxSet _rules;

    ///	filtering stream to read from
    rc_ptr<DataSource> _datasource;

    ///	helper functions read basic data type
    HelperSet _helpers;

    //@}

  private:

    /**	install helpers for basic data types
     */
    void init();

    //---------------------------------------------------------------------
    /**	@name Disable copy*/
    //---------------------------------------------------------------------
    //@{

    /// Copy constructor
    AttributedReader(const AttributedReader&);

    /// Assignment operator
    AttributedReader& operator=(const AttributedReader&);

    //@}
  };

}

#endif 
