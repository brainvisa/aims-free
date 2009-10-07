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
#include <iostream>
#include <cstdlib>
#include <cartobase/algorithm/algorithm.h>


using namespace std;
using namespace carto;

//===========================================================================//
//                                                                           //
// The following code is just to have an idea of what an algorithm could be  //
// and how it could be used. It's all testing and debug code.                //
//                                                                           //
//===========================================================================//


  //------------------//
 //  Thresholder<T>  //
//------------------//

template <typename T>
class Thresholder : public Algorithm
{
public:

  Thresholder();

private:

  enum Mode_t { GREATER, BETWEEN };

  // vector<T> are used in this example code to replace AimsData<T>.
  // It is important (but should not be mandatory) to use a kind of
  // pointer to store big objects because it allows to chain
  // algorithms without having to copy objects. 
  std::vector<T> *_input;
  std::vector<T> *_output;
  Mode_t _mode;
  T _level1;
  T _level2;

  void call();
};



//-----------------------------------------------------------------------------
template <typename T>
Thresholder<T>::Thresholder() :
  Algorithm( "threshold" )
{
  inputParameter( _input, "input", "Input image" );
  outputParameter( _output, "output", "Output image" );
  inputParameter( _mode, "mode", "Thresholding mode" )
    .choice( "gt", GREATER )
    .choice( "bw", BETWEEN );
  inputParameter( _level1, "level1", "Frist threshold" );
  inputParameter( _level2, "level2", "Second threshold" ).optional();
}

//-----------------------------------------------------------------------------
template <typename T>
void Thresholder<T>::call()
{
  // TODO acquire << "input" << "output" << "level1" << "mode";
  _output->resize( _input->size() );
  for( size_t i = 0; i < _input->size(); ++i ) {
    const T &vi = (*_input)[ i ];
    T &vo = (*_output)[ i ];
    if ( _mode == GREATER ) {
      if ( vi > _level1 ) vo = vi;
      else vo = 0;
    } else if ( _mode == BETWEEN ) {
      if ( vi > _level1 && vi < _level2 ) vo = vi;
      else vo = 0;
    }
  }
  // TODO release >>"input" >> "output" >> "level1" >> "level2" >> "mode";

}


  //--------------------------//
 //  Thresholder singletons  //
//--------------------------//

static Thresholder<uint8_t> _threshold_uint8_t;
static Thresholder<int8_t> _threshold_int8_t;
static Thresholder<uint16_t> _threshold_uint16_t;
static Thresholder<int16_t> _threshold_int16_t;
static Thresholder<uint32_t> _threshold_uint32_t;
static Thresholder<int32_t> _threshold_int32_t;
static Thresholder<float> _threshold_float;
static Thresholder<double> _threshold_double;


/*

  //-------------------//
 //  VectorReader<T>  //
//-------------------//

template <typename T>
class VectorReader : public Algorithm
{
public:

  VectorReader();

private:

  std::string _input;
  std::vector<T> *_output;

  void call();
};


//-----------------------------------------------------------------------------
template <typename T>
Thresholder<T>::Thresholder() :
  Algorithm( "threshold" )
{
  inputParameter( _input, "input", "Input image" );
  outputParameter( _output, "output", "Output image" );
  inputParameter( _mode, "mode", "Thresholding mode" )
    .choice( "gt", GREATER )
    .choice( "bw", BETWEEN );
  inputParameter( _level1, "level1", "Frist threshold" );
  inputParameter( _level2, "level2", "Second threshold" ).optional();
}

*/




  //----------------------------------//
 //  Function that uses a threshold  //
//----------------------------------//

int main()
{
  try {
    string inputFileName;
    vector< int16_t > result;
    cout << "thresholding: " << flush;
    algo( "threshold" ) << inputFileName << result << 14 << execute;
    cout << "done" << endl;
    return 0;
  }
  catch( exception &e ) {
    cerr << e.what() << endl;
    exit( 1 );
  }
  catch( ... ) {
    cerr << "Unexpected exception" << endl;
    exit( 1 );
  }
}
