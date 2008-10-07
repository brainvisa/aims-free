/*
 *  Copyright (C) 1999-2005 CEA
 *
 *  This software and supporting documentation were developed by
 *  	CEA/DSV/SHFJ
 *  	4 place du General Leclerc
 *  	91401 Orsay cedex
 *  	France
 *
 *  Probe for MIRegister
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
  _pdfOverIterations = new AimsData<float>( numLevel, numLevel, maxIteration ) ;
  _pdfOverIterations->setSizeXYZT(1., 1., 1., 1. ) ;
  

  //   cout << "CONSTRUCT\txsize= " << _pdfOverIterations->dimX() 
  //        << "\tysize= " << _pdfOverIterations->dimY()  << endl ;
  
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
    new MIRegistrationProbe<T,D>( _pdfOverIterations->dimX(), 
				  _pdfOverIterations->dimZ(), this->_verbosity ) ;
  
  probe->_pdfOverIterations = this->_pdfOverIterations ;
  probe->_iteration = this->_iteration ;
  probe->_pdfOverIterations->setSizeXYZT(1., 1., 1., 1. ) ;
  
  //   cout << "CLONE\txsize= " << probe->_pdfOverIterations->dimX() 
  //        << "\tysize= " << probe->_pdfOverIterations->dimY()  << endl ;
  
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
  AimsData<float> joint ;
  if( ! specific.hasProperty("joint_histogram" ) )
    cout << "No joint histogram" << endl ;
  else {
    specific.getProperty("joint_histogram", joint ) ;
    
    
    //     cout << "Iteration : " << _iteration << endl;
    //     cout << "FINAL\txsize= " << _pdfOverIterations->dimX() 
    // 	 << "\tysize= " << _pdfOverIterations->dimY()  
    // 	 << "\tzsize= " << _pdfOverIterations->dimZ() << endl ;
    //     cout << "INITIAL\txsize= " << joint.dimX() 
    // 	 << "\tysize= " << joint.dimY()  << endl ;
    
    for( int y = 0 ; y < joint.dimY()  ; ++y )
      for( int x = 0 ; x < joint.dimX()  ; ++x ){
	(*_pdfOverIterations)(x, y, _iteration ) = joint(x, y, 0) ;
      }
    
    //     cout << "Copy done" << endl ;
    
    //     AimsData<float>::iterator destIter(&((*_pdfOverIterations)(0, 0, _iteration)));
    //     AimsData<float>::const_iterator fromIter( joint.begin() ),
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
  Writer< AimsData<float> > w( "jointpdf.ima" ) ;
  w.write( *_pdfOverIterations ) ;
  
  *_pdfOverIterations = 0 ;
  _iteration = 0 ;
}


/* this source is only here to force instanciation of some of the most useful
    templates */
template class AIMSDATA_API MIRegistrationProbe< float, 6 >;
