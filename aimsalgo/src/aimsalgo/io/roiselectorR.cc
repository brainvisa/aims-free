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


#include <aims/roi/roiselector.h>
#include <aims/io/roiselectorR.h>
#include <aims/selection/selection.h>
#include <aims/io/selectionr.h>

using namespace aims;
using namespace carto;
using namespace std;


RoiSelectorReader::RoiSelectorReader(  const string& name, 
                                       const SyntaxSet& /* stx */ )
  : _name( name )
{
}


RoiSelectorReader::~RoiSelectorReader()
{
}


string RoiSelectorReader::removeExtension( const string& name )
{
  string res = name;
  string ext="";
  if ( res.length() > 4 )
    ext = res.substr( int(res.length() - 4), 4 );
  if (ext ==  string(".sel") )
    res = res.substr( 0, res.length() - 4 );
  return res;
}


void  RoiSelectorReader::read(RoiSelector& thing)
{
  SelectionSet selset;
  try
  {
    SelectionReader sr( _name );
    sr.read( selset );
  }
  catch( ... )
  {
    // maybe wrong extension
    SelectionReader sr( removeExtension( _name ) + ".sel" );
    sr.read( selset );
  }

  // convert to RoiSelector / Tree
  SelectionSet::iterator iss, ess = selset.end();
  Selection::iterator is, es;
  for( iss=selset.begin(); iss!=ess; ++iss )
  {
    Tree *tr = new Tree( true, "subset" );
    thing.insert( tr );
    tr->setProperty( "surname", iss->name() );
    for( is=iss->begin(), es=iss->end(); is!=es; ++is )
    {
      Tree *tr2 = new Tree( true, "subset" );
      tr->insert( tr2 );
      tr2->setProperty( "nomenName", *is );
    }
  }
}
