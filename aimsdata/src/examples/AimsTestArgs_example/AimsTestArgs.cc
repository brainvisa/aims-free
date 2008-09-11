
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
   Temporary command used to test new option declaration scheme
*/

#include <aims/data/data_g.h>
#include <aims/getopt/getopt2.h>


using namespace aims;
using namespace carto;
using namespace std;

int main( int argc, const char **argv )
{
  int result = EXIT_SUCCESS;

  try {
    // Definition of variables containing options values
    Reader< AimsData< float > > fileIn;
    Writer< AimsData< int8_t > > fileOut;
    char vChar = 0;
    unsigned char vUChar = 0;
    short vShort = 0;
    unsigned short vUShort = 0;
    long vLong = 0;
    unsigned long vULong = 0;
    int vInt = 0;
    unsigned int vUInt = 0;
    string vString = "default";
    vector<float> vVectorFloat;
    list<double> vListDouble;
    
    // Building application
    AimsApplication application( argc, argv, "Test new options parsing" );
    application.addOption( fileIn, "-i", "input image" );
    application.addOption( fileOut, "-o", "output image", true );
    application.addOption( vChar, "-char", "char value", true );
    application.addOption( vUChar, "-uchar", "unsigned char value", true );
    application.addOption( vShort, "-short", "short value", true );
    application.addOption( vUShort, "-ushort", "unsigned short value", true );
    application.addOption( vLong, "-long", "long value", true );
    application.addOption( vULong, "-ulong", "unsigned long value", true );
    application.addOption( vInt, "-int", "int value", true );
    application.addOption( vUInt, "-uint", "unsigned int value", true );
    application.addOption( vString, "-string", "string value", true );
    application.addOptionSeries( vVectorFloat, "-floats", "float vector" );
    application.addOptionSeries( vListDouble, "-doubles", "double list" );
    application.initialize();
    
    // Check optional options
    if ( fileOut.fileName().size() == 0 ) {
      fileOut.setFileName( fileIn.fileName() );
    }

    // Display options values
    cout << "fileIn = " << fileIn.fileName() << endl;
    cout << "fileOut = " << fileOut.fileName() << endl;
    cout << "char = " << int( vChar ) << endl;
    cout << "uchar = " << int( vUChar ) << endl;
    cout << "short = " << vShort << endl;
    cout << "ushort = " << vUShort << endl;
    cout << "long = " << vLong << endl;
    cout << "ulong = " << vULong << endl;
    cout << "int = " << vInt << endl;
    cout << "uint = " << vUInt << endl;
    cout << "string = " << vString << endl;
    {
      cout << "floats = ";
      vector<float>::const_iterator i = vVectorFloat.begin();
      if ( i != vVectorFloat.end() ) cout << *i++;
      for(; i != vVectorFloat.end(); ++i ) {
	cout << ", " << *i;
      }
      cout << endl;
    }
    {
      cout << "doubles = ";
      list<double>::const_iterator i = vListDouble.begin();
      if ( i != vListDouble.end() ) cout << *i++;
      for(; i != vListDouble.end(); ++i ) {
	cout << ", " << *i;
      }
      cout << endl;
    }
  }
  catch( user_interruption &e ) {
  }
  catch( std::exception &e ) {
    cerr << argv[ 0 ] << ": " << e.what() << endl;
    result = EXIT_FAILURE;
  }
  return result;
}

