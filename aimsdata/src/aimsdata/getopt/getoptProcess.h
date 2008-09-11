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
 *  ...
 */
#ifndef AIMS_GETOPT_GETOPTPROCESS_H
#define AIMS_GETOPT_GETOPTPROCESS_H

#include <aims/getopt/getopt2.h>
#include <aims/io/process.h>


namespace aims
{

  struct ProcessInput
  {
  public:
    ProcessInput( const Process & p ) : _proc( p ) {}
    std::string filename;
    const Process & process() const { return( _proc ); }

  private:
    const Process	& _proc;
  };

}

namespace carto
{
  //---------------------------------------------------------------------------
  template <>
  std::string SingleOption< aims::ProcessInput >::info() const
  {
    std::string	inf;
    if ( _optional )
      inf = "[ ";
    inf += names() + " <file name (read only): ";

    const std::map<std::string, std::map<std::string, 
      aims::Process::ProcFunc> > & 
      procs = _value.process().processTypes();
    std::map<std::string, std::map<std::string, aims::Process::ProcFunc> >
      ::const_iterator	ipl, epl = procs.end();
    std::map<std::string, aims::Process::ProcFunc>::const_iterator	ip, ep;

    std::string	ptypes;
    unsigned	count = 0;

    for( ipl=procs.begin(); ipl!=epl; ++ipl )
      for( ip=ipl->second.begin(), ep=ipl->second.end(); ip!=ep; ++ip )
	{
	  if( count != 0 )
	    ptypes += std::string( ", " );
	  ++count;
	  ptypes += ipl->first + " of " + ip->first;
	}

    if( count > 1 )
      inf += std::string( "{ " ) + ptypes + " }";
    else
      inf += ptypes;

    inf += std::string( ">  " ) + _info;
    if( _optional )
      inf += std::string( " ]" );
    return inf;
  }

  //---------------------------------------------------------------------------
  template <>
  bool SingleOption< aims::ProcessInput >::feed( const std::string &value )
  {
    if ( _valueRead )
      return false;
    else
      {
	_value.filename = value;
	_valueRead = true;
	return true;
      }
  }

}

#endif

