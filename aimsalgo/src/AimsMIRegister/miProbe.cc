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

#include "miProbe.h"
#include <aims/math/math_g.h>
#include <aims/utility/utility_g.h>
#include <aims/information/pdf.h>
#include <aims/optimization/optimization_g.h>
#include <aims/io/io_g.h>
#include <iomanip>

using namespace aims;
using namespace carto;
using namespace std;


template <class T, int D>
MIRegistrationProbe<T,D>::MIRegistrationProbe( int numLevel, int maxIteration, int verbosity) :
  _iteration(0), _verbosity( verbosity )
{
  _pdfOverIterations = new VolumeRef<float>( numLevel, numLevel, maxIteration ) ;
  _pdfOverIterations->setVoxelSize(1., 1., 1., 1. ) ;
  

  //   cout << "CONSTRUCT\txsize= " << _pdfOverIterations->getSizeX()
  //        << "\tysize= " << _pdfOverIterations->getSizeY()  << endl ;
  
}

template <class T, int D>
MIRegistrationProbe<T,D>::~MIRegistrationProbe( )
{
  delete _pdfOverIterations ;
}

template <class T, int D>
MIRegistrationProbe<T,D> *
MIRegistrationProbe<T,D>::clone()
{
  MIRegistrationProbe<T,D> * probe = 
    new MIRegistrationProbe<T,D>( (*_pdfOverIterations)->getSizeX(),
                                  (*_pdfOverIterations)->getSizeZ(),
                                  this->_verbosity ) ;
  
  probe->_pdfOverIterations = this->_pdfOverIterations ;
  probe->_iteration = this->_iteration ;
  probe->_pdfOverIterations->setVoxelSize(1., 1., 1., 1. ) ;
  
  //   cout << "CLONE\txsize= " << probe->_pdfOverIterations->getSizeX()
  //        << "\tysize= " << probe->_pdfOverIterations->getSizeY()  << endl ;
  
  return probe ;
}

template <class T, int D>
void 
MIRegistrationProbe<T,D>::test( const AimsVector<T,D> & parameters, 
				const AttributedObject& specific,
				const float * costFunction)
{
  string info ;
  
  if ( _verbosity > 0)
    {
      cout <<"TestProbe\t"<<( specific.getProperty("info", info ) ? info : "")
	   << endl ;
      cout << "Current parameters = " <<  parameters 
	   << "\t value = " << ( costFunction != 0 ? 
				 *costFunction : 0 ) << endl ;
    }
  
  //   set<string> attributes( specific.getPropertys() ) ;
  
  //   set<string>::const_iterator iter( attributes.begin() ), 
  //     last( attributes.end() ) ;
  
  //   cout << endl << endl ;
  
  //   while( iter != last ){
  //     cout << "\t" << *iter ;
  //     ++iter ;
  //   }
  //   cout << endl << endl ;
  
  AimsVector<float,3> gcRef, gcTest ;


  if (_verbosity > 0 )
    {
      if( ! specific.hasProperty("gc_ref") )
	cout << "No gravity center for reference image" << endl ;
      else{
	specific.getProperty("gc_ref", gcRef ) ;
	cout << "Reference image gravity center : " << gcRef << endl ;
      }
      
      if( ! specific.hasProperty("gc_test") )
	cout << "No gravity center for test image" << endl ;
      else{
	specific.getProperty("gc_test", gcTest ) ;
	cout << "Test image gravity center : " << gcTest << endl ;
      }
    }
}

template <class T, int D>
void 
MIRegistrationProbe<T,D>::iteration( const AimsVector<T,D> & parameters, 
				     const AttributedObject& specific,
				     const float * costFunction)
{

  if ( _verbosity > 0)
    {
      cout << "IterationProbe" << endl ;
      cout << "Current parameters = " <<  parameters 
	   << "\t value = " << ( costFunction != 0 ? 
				 *costFunction : 0 ) << endl ;
    }
  VolumeRef<float> joint ;
  if( ! specific.hasProperty("joint_histogram" ) )
    cout << "No joint histogram" << endl ;
  else {
    specific.getProperty("joint_histogram", joint ) ;
    
    
    //     cout << "Iteration : " << _iteration << endl;
    //     cout << "FINAL\txsize= " << _pdfOverIterations->getSizeX()
    // 	 << "\tysize= " << _pdfOverIterations->getSizeY()
    // 	 << "\tzsize= " << _pdfOverIterations->getSizeZ() << endl ;
    //     cout << "INITIAL\txsize= " << joint.getSizeX()
    // 	 << "\tysize= " << joint.dimY()  << endl ;
    
    for( int y = 0 ; y < joint.getSizeY()  ; ++y )
      for( int x = 0 ; x < joint.getSizeX()  ; ++x ){
	      (*_pdfOverIterations)(x, y, _iteration ) = joint(x, y, 0) ;
      }
    
    //     cout << "Copy done" << endl ;
    
    //     VolumeRef<float>::iterator destIter(&((*_pdfOverIterations)(0, 0, _iteration)));
    //     VolumeRef<float>::const_iterator fromIter( joint.begin() ),
    //       fromLast( joint.end() ) ;
    //     while( fromIter != fromLast ){
    //       *destIter = *fromIter ;
    //       ++fromIter ; ++destIter ;
    //     }
  }
  
  ++_iteration ;
  
}

template <class T, int D>
void 
MIRegistrationProbe<T,D>::end( )
{
  Writer< VolumeRef<float> > w( "jointpdf.ima" ) ;
  w.write( *_pdfOverIterations ) ;
  
  *_pdfOverIterations = 0 ;
  _iteration = 0 ;
}


/* this source is only here to force instanciation of some of the most useful
    templates */
template class MIRegistrationProbe< float, 6 >;
