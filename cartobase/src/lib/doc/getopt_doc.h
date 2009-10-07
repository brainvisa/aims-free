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

// This file is only here for Doxygen (doc generation tool)
// Nothing to do with the code, in fact...

namespace carto
{

  /*! \class CartoApplication

  CartoApplication is normally the entry point of the options parsing system. 
  It inherits OptionsParser to benefit from its addOption(), addOptionSeries() 
  and alias() methods, and parsing is triggered by the initialize() function.

  CartoApplication adds the commandline description, and some standard 
  common options which all cartograph commands will provide:
  - \c -h / \c --help prints the commandline and its parameters descriptions
  - \c --verbose sets the verbosity level of the commandline (more messages 
    may be printed when the verbosity level increases)
  - \c --version shows version information about cartograph libraries and 
    possibly of upper-level libraries based on cartograph
  - \c --info prints various information about the libraries, like plugins 
    list, supported IO formats and objects types, versions, external shared 
    data paths, etc.

  CartoApplication may be subclassed in upper-level libraries which may add 
  some other common options. For instance, AIMS programs will use 
  AimsApplication instead of CartoApplication.

  A normal Cartograph program will generally have a main() function looking 
  like the following example:

  \code
#include <cstdlib>
  #include <cartobase/getopt/getopt.h>
  #include <iostream>

  using namespace carto;
  using namespace std;

  int main( int argc, const char **argv )
  {
    try
    {
      CartoApplication app( argc, argv, "The commandline help description" );
      string inputparam1;
      int inputparam2 = 0;
      app.addOption( inputparam1, "-i1", "first input parameter" );
      app.addOption( inputparam2, "-i2", "second input parameter", true );

      // commandline parsing is done by initialize()
      app.initialize();

      // now inputparam1 is filled, and inputparam2 too if provided
    }
    catch( user_interruption & )
    {
      // when the -h option is used, the help is displayed, and a 
      // user_interruption exception is thrown to avoid running the 
      // normal program execution. So we must silently catch it because 
      // it is not really an error
    }
    catch( exception & e )
    {
      // all other exceptions are catched here (commandline errors like 
      // missing or unrecognized option, or any other exception thrown 
      // during the program execution)

      cerr << e.what() << endl;
      return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
  }
  \endcode
  */


  /*! \class OptionsParser

  Generic commandline options parser

  OptionsParser parses the commandline options and fills in parameter 
  variables. It is generally used via "application" subclasses like 
  CartoApplication.

  Commandline parameters are declared and described using the addOption() 
  or addOptionSeries() methods. These template methods allow to use arbitrary 
  parameters types.

  Once declared, options are parsed by calling parse(). parse() does not 
  report errors for missing parameters: this is done by calling check() after 
  parameters have been parsed.

  Parameters types are handled by wrapper classes: OptionBase, specialized 
  into SingleOption (by addOption) and OptionSeries (by addOptionSeries). 
  Common types are understood (int, float and other scalar types, strings...), 
  and other parameter types may be added by specializing the SingleOption 
  template for the new types. This is done, for instance, for Reader and 
  Writer classes in aims (and soon in cartobase).

  On the commandline, parameter switches may have several names (see the 
  alias() method). They may be omitted when they are non-ambiguous: for 
  instance if the command \c cartoDummy accepts a \c "-i" parameter and a 
  \c "-o" parameter, invoking it as:
  \code
  cartoDummy -i file1 -o file2
  \endcode
  is the same as invoking:
  \code
  cartodummy file1 file2
  \endcode
  But if the \c "-o" parameter is optional and if you need to pass a third, 
  optional parameter "-x", then you have to specify it to avoid confusion 
  with the missing \c "-o" parameter.

  */

}

