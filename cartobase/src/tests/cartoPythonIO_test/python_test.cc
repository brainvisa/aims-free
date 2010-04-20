
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

#include <cstdlib>
#include <cartobase/getopt/getopt.h>
#include <cartobase/wip/io/reader.h>
#include <cartobase/wip/io/writer.h>
#include <cartobase/object/property.h>
#include <cartobase/smart/rcptr.h>
#include <cartobase/datasource/streamdatasource.h>
#include <iostream>

using namespace std;
using namespace carto;


int main( int argc, const char** argv )
{
  string	input;
  string	output;

  try
    {
      CartoApplication	app( argc, argv, 
                             "Generic objects and python IO tests" );
      app.addOption( input, "-i", "input MINF file to be read [default: " 
                     "don't perform IO test]", true );
      app.addOption( output, "-o", "output MINF file to be written [default: " 
                     "don't perform writing test]", true );

      app.initialize();

      cout << "access / storage tests...\n";
      ValueObject<Dictionary>	dic;
      /*
      dic[ "toto" ] = (int) 12;
      dic[ "tutu" ] = string( "hello" );
      dic[ "tata" ] = string( "0x1e" );
      */
      dic.setProperty( "toto", Object::value( (int) 12 ) );
      dic.setProperty( "tutu", Object::value( string( "hello" ) ) );
      dic.setProperty( "tata", Object::value( string( "0x1e" ) ) );

      cout << "initial values:\ntoto : " 
           << dic.getProperty( "toto" )->value<int>() 
	   << endl;
      cout << "tutu : \"" << dic.getProperty( "tutu" )->value<string>() 
           << "\"" << endl;
      cout << "tata : \"" << dic.getProperty( "tata" )->value<string>() 
           << "\"" << endl;
      cout << "changing field 'tutu' from 'hello' to 'byebye'...\n";
      dic.setProperty( "tutu", Object::value( string( "byebye" ) ) );
      cout << "const access test...\n";
      const GenericObject	& de = dic;
      cout << "toto: " << de.getProperty( "toto" )->value<int>() << endl;
      cout << "tutu: " << de.getProperty( "tutu" )->value<string>() 
           << endl;
      cout << "numeric test, toto: " 
           << de.getProperty( "toto" )->getScalar() << endl;
      cout << "              tata: " 
           << de.getProperty( "tata" )->getScalar() << endl;
      cout << "              tutu: (should fail)" << endl;
      try
	{
	  cout << "tutu = " << de.getProperty( "tutu" )->getScalar() 
	       << " PROBLEM, error not detected !" << endl;
	}
      catch( exception & e )
	{
	  cerr << e.what() << " (normal)" << endl;
	}
      cout << "string test,  toto: " 
           << de.getProperty( "toto" )->getString() << endl;
      cout << "              tata: " 
           << de.getProperty( "tata" )->getString() << endl;
      cout << "              tutu: " 
           << de.getProperty( "tutu" )->getString() << endl;
      // this should fail
      cout << "attempt to read missing field...\n";
      try
	{
	  Object z = de.getProperty( "titi" );
          if( !z.get() )
            cout << "null object (normal)" << endl;
          else
            cerr << "strange, should have failed!" << endl;
	}
      catch( exception & e )
	{
	  cout << e.what() << " (normal)" << endl;
	}
      cout << "changing type of 'tutu' from string to double...\n";
      dic.setProperty( "tutu", Object::value( double( 12.5 ) ) );
      cout << "tutu: " << de.getProperty( "tutu" )->value<double>() 
           << endl;
      cout << "changing ref to 'tutu'...\n";
      dic.getProperty( "tutu" )->value<double>() = 45.8;
      cout << "tutu: " << de.getProperty( "tutu" )->value<double>() 
           << endl;

      if( !input.empty() )
        {
          cout << "Reading " << input << "...\n";
          Reader<GenericObject>	pr( input );
          GenericObject	*ao = pr.read();
          pr.close();
          cout << "Reading OK\n";

          if( output.empty() )
            {
              cout << "output to stdout:\n\n";
              Writer<GenericObject>	pw( cout );
              pw.write( *ao );
              cout << endl;
              delete ao;
            }
          else
            {
              cout << "Writing " << output << "...\n";
              Writer<GenericObject>	pw( output );
              pw.write( *ao );
              pw.close();
              cout << "Writing OK.\n";

              cout << "Re-reading output...\n";
              Reader<GenericObject>	pr2( output );
              delete ao;
              ao = pr2.read();
              pr2.close();
              cout << "Re-read OK\n";

              cout << "output to stdout:\n\n";
              Writer<GenericObject>	pw2( cout );
              pw2.write( *ao );
              cout << endl;
              delete ao;

              cout << "testing PropertySet reading...\n";
              ValueObject<PropertySet>	ps;
              Reader<GenericObject>	pr3( output );
              pr3.read( ps );
              cout << "OK, now writing:" << endl;
              pw2.write( ps );
            }
        }
    }
  catch( user_interruption & )
    {
    }
  catch( exception & e )
    {
      cerr << e.what() << endl;
      return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}


