/* Copyright (c) 1995-2007 CEA
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


#ifndef PCASTRATEGY_D_H
#define PCASTRATEGY_D_H

#include <aims/classification/pcastrategy.h>


template <class T>
aims::PcaStrategy<T>::PcaStrategy( const PcaStrategy<T>& pcaStrat ) :
  aims::ClassifStrategy<T>( pcaStrat )
{
  myDistance = pcaStrat.myDistance ;
}


template <class T>
aims::PcaStrategy<T>::PcaStrategy( int nbIterations,
			     DistanceType distanceType, 
			     const std::vector< aims::Individuals<T> >& codeVector ) :
  aims::ClassifStrategy<T>( nbIterations )
{
  switch( distanceType ) {
  case NORM1 :
    myDistance = aims::Distance<T>::norm1 ;
    break ;
  case NORM2 :
    myDistance = aims::Distance<T>::norm2 ;
    break ;
  case NORM2SQR :
    myDistance = aims::Distance<T>::norm2sqr ;
    break ;
  case INFNORM :
    myDistance = aims::Distance<T>::infiniteNorm ;
    break ;
  }
  
  // TODO
}


template <class T>
aims::PcaStrategy<T>::~PcaStrategy()
{
}


template <class T>
aims::ClassifStrategy<T> * aims::PcaStrategy<T>::clone() const
{
  return new aims::PcaStrategy<T>( *this ) ;
}


template <class T>
void aims::PcaStrategy<T>::analyse( const std::vector< std::list< aims::Individuals<T> > >& classes )
{
}


template <class T>
int aims::PcaStrategy<T>::agregation( const aims::Individuals<T>& individual )
{

  return this->indMin ;
}


template <class T>
bool aims::PcaStrategy<T>::classificationCompleted( int nbChanges )
{
  if ( nbChanges == 0 ) return false ;
  else return true ;
}


template <class T>
float aims::PcaStrategy<T>::distance( const aims::Individuals<T>& individual, int classe )
{
  T::not_implemented();
  return 0.0; // ### BEN ALORS ? IL MANQUE LE CODE DE CETTE FONCTION !
}

#endif
