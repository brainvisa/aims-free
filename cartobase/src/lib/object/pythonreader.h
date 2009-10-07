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

#ifndef CARTOBASE_OBJECT_PYTHONREADER_H
#define CARTOBASE_OBJECT_PYTHONREADER_H

#include <cartobase/config/cartobase_config.h>
#include <cartobase/object/object.h>
#include <cartobase/object/syntax.h>
#include <iostream>

namespace carto
{
  class DataSource;

  class CARTOBASE_API PythonReader
  {
  public:
    typedef GenericObject* (*Helper)( GenericObject*, const std::string &, 
                                      PythonReader & );
    typedef std::map<std::string, Helper> HelperSet;

    PythonReader( const std::string& filename ,
          const SyntaxSet& rules = SyntaxSet(),
          const HelperSet& helpers = HelperSet() );
    PythonReader( const SyntaxSet& rules = SyntaxSet(),
          const HelperSet& helpers = HelperSet() );
    /// PythonReader takes ownership of the DataSource
    PythonReader( rc_ptr<DataSource> ds, const SyntaxSet& rules = SyntaxSet(),
          const HelperSet& helpers = HelperSet() );
    virtual ~PythonReader();

    virtual GenericObject* read();
    /** attemps to read an already allocated object, if file data matches 
    the object type.
    It currently only works for dictionaries */
    virtual void read( GenericObject & );
    virtual void read( Object & );
    /// reads an already allocated dictionary: just fills it (doesn't clear it)
    void readDictionary( GenericObject & obj );
    /// reads the remaining attributes of a string dictionary
    void readDictionary2( GenericObject & obj );
    /// reads the remaining attributes of an  int dictionary
    void readIntDictionary2( TypedObject<IntDictionary> & obj );


    /// obsolete: use attach() instead
    void open( const std::string& filename );
    void close();
    /// attach to an existing istream
    void attach( std::istream & s, int line_num = 1 );
    void attach( rc_ptr<DataSource> ds );
    void attach( const std::string& filename );
    rc_ptr<DataSource> dataSource();
    /// file name
    virtual std::string name() const;
    int line() const;
    std::string lineString() const;
    /// status of the stream
    bool operator!() const;
    /// is the stream open?
    bool is_open() const;
    /// have we hit EOF?
    bool eof() const;
    /// helper functions read basic data type
    const HelperSet & helpers() const;
    HelperSet & helpers();
    const SyntaxSet & syntaxes() const;
    SyntaxSet & syntaxes();

    /** read characters not in s
       @param s continue while characters outside s
       @return accepted characters
    */
    std::string readUntil(const std::string& s);
    /** skip characters not in s
       @param s continue while characters outside s
    */
    void skipUntil(const std::string& s);
    /** read characters in s
       @param s continue while characters in s
       @return accepted characters
    */
    std::string readWhile(const std::string& s);
    /** skip characters in s
       \param s continue while characters in s
       \return number of characters read
    */
    unsigned skipWhile(const std::string& s);
    GenericObject* read( GenericObject *parent, 
                         const std::string & semantic );
    std::string readVariableName();
    /** attempts to read the syntax and general type of next element and fills 
    \c syntax and \c type accordingly. \c type will be "dictionary" or 
    "list".
    \return false in case of failure (no syntax or not able to read it)
    */
    bool readSyntax( std::string & syntax );
    std::string readString( char separator, unsigned & charsread );

    /** utility function, dictionary helper (can be useful in more specialized 
    readers) */
    static GenericObject* dictHelper( GenericObject *, const std::string&, 
                                      PythonReader & r );

    static GenericObject* propertySetHelper(GenericObject *, const std::string&,
                                      PythonReader & r );

    template<typename T>
    static GenericObject* genericSequenceHelper( GenericObject *, const std::string&,
                                      PythonReader & r );

    template<typename T>
    static GenericObject* genericDictHelper( GenericObject *, const std::string&,
                                      PythonReader & r );

  protected:
    void init( const HelperSet & helpers );

  private:
    struct Private;
    Private *d;
  };

DECLARE_GENERIC_OBJECT_TYPE( PythonReader::Helper )
DECLARE_GENERIC_OBJECT_TYPE( PythonReader::HelperSet )

} // namespace carto

#endif
