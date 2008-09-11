/* Copyright (c) 1995-2005 CEA
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

/*
 *  Data reader class
 */
#ifndef AIMS_IO_ARGR_H
#define AIMS_IO_ARGR_H

#include <cartobase/object/syntax.h>
#include <set>

class Graph;

namespace aims
{

  /// Wrapper for a specific GraphReader
  class LowLevelArgReader
  {
  public:
    LowLevelArgReader();
    virtual ~LowLevelArgReader();
    /// in case of failure, either raise an exception or return 0
    virtual Graph* read( const std::string & filename, 
			 int subobjectsfilter = -1 ) = 0;
  };

  /* This class is a wrapper for several GraphReaders: standard GraphReader, 
     FoldReader, FRGReader (defined in sigraph library), RoiArgReader (defined
     in aimsalgo), etc. New readers can be added as plugins.
   */
  class ArgReader
  {
  public:
    ArgReader( const std::string& filename );
    ~ArgReader();
    Graph *read( int subobjectsfilter = -1 ) const;
    /** sets the reader to use for graphs of a specific syntax. The reader 
	then belongs to ArgReader (never delete it) */
    static void registerReader( const std::string & syntax, 
				LowLevelArgReader *reader );
    static bool hasReader( const std::string & syntax );
    /// removes and deletes a reader
    static void deleteReader( const std::string & syntax );
    static std::set<std::string> readers();

  private:
    struct StaticPrivate;
    static StaticPrivate* staticPrivate();

    std::string	_name;
  };

  /// Wrapper for the standard GraphReader
  class LowLevelStandardArgReader : public LowLevelArgReader
  {
  public:
    LowLevelStandardArgReader();
    virtual ~LowLevelStandardArgReader();
    virtual Graph* read( const std::string & filename, 
			 int subobjectsfilter = -1 );

  protected:
    carto::SyntaxSet	*_syntax;
  };

}

#endif
