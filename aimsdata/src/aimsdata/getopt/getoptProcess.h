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

