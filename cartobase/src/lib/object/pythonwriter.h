/* Copyright (c) 1995-2006 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
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

#ifndef CARTOBASE_OBJECT_PYTHONWRITER_H
#define CARTOBASE_OBJECT_PYTHONWRITER_H

#include <cartobase/config/cartobase_config.h>
#include <cartobase/object/syntax.h>
#include <cartobase/object/object.h>
#include <cartobase/datasource/datasource.h>
#include <fstream>

namespace carto
{
  class GenericObject;
  class Object;

  class CARTOBASE_API PythonWriter
  {
  public:
    typedef void (*Helper)( const GenericObject &, PythonWriter & w, 
			    int indent, bool writeInternals );
    typedef std::map<std::string, Helper> HelperSet;
    typedef void (*CatchFunction)( PythonWriter &, std::exception &,
                                   const GenericObject & );

    PythonWriter( const std::string& filename,
		  const SyntaxSet& rules = SyntaxSet(),
		  const HelperSet& helpers = HelperSet() );
    PythonWriter( const SyntaxSet& rules = SyntaxSet(),
		  const HelperSet& helpers = HelperSet() );
    /// PythonWriter takes ownership of the DataSource
    PythonWriter( rc_ptr<DataSource> ds, const SyntaxSet& rules = SyntaxSet(),
		  const HelperSet& helpers = HelperSet() );
    virtual ~PythonWriter();

    void open(const std::string& filename);
    ///	attach to an existing (and open) stream
    void attach( std::ostream & s );
    void attach( rc_ptr<DataSource> ds );
    void close();
    virtual std::string name() const;
    bool operator ! () const;
    bool is_open() const;
    rc_ptr<DataSource> dataSource() { return _datasource; }

    /// Writes a generic object to stream
    void write( const GenericObject & object, bool writeInternals = false, 
		bool writevariable = true );
    void write( const Object & object, bool writeInternals = false, 
		bool writevariable = true );
    /// Writes an element according to the syntax
    void write( const GenericObject & object, int indent, 
		const std::string & syntax = "", 
		const std::string & semantic = "", 
		bool writeInternals = false );
    bool isInternal( const std::string & syntax, 
		     const std::string & semantic );
    void setSingleLineMode( bool x ) { _singleLine = x; }
    bool singleLine() const { return _singleLine; }
    /** if set, the catch function is used instead of throwing the regular
      exception during writing */
    void setCatchFunction( CatchFunction f );

    /// utility function
    static void writeString( DataSource &, std::string );

  protected:
    void init( const HelperSet & helpers );

    SyntaxSet		_rules;
    rc_ptr<DataSource>	_datasource;
    HelperSet		_helpers;
    bool		_singleLine;
    CatchFunction       _catchFunction;
  };

DECLARE_GENERIC_OBJECT_TYPE( PythonWriter::Helper )
DECLARE_GENERIC_OBJECT_TYPE( PythonWriter::HelperSet )

  } // namespace carto

#endif
