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

//=============================================================================
//	H E A D E R  F I L E S
//=============================================================================

#include <cartobase/object/achecker.h>
#include <cartobase/object/syntobject.h>

using namespace carto;
using namespace std;


//=============================================================================
//	M E T H O D S
//=============================================================================

AttributedChecker::AttributedChecker(const SyntaxSet& syntax) : _syntax(syntax)
{
}


AttributedChecker::~AttributedChecker()
{
}


set<string>
AttributedChecker::check(const GenericObject& object) const
{
	set<string> result;

        const SyntaxedInterface	*si 
          = object.getInterface<const SyntaxedInterface>();
        if( !si || !si->hasSyntax() )
	  return result;
	// get the syntactic attribute
	SyntaxSet::const_iterator s = _syntax.find(si->getSyntax());
	if(s == _syntax.end())
	  {
	    result.insert( "<syntax not found>" );
	    return result;
	  }

	// make sure all mandatory semantic attributes are here
	set<string> attributes;
        Object	it = object.objectIterator();
        for( it=object.objectIterator(); it->isValid(); it->next() )
          attributes.insert( it->key() );
	for (SemanticSet::const_iterator i = s->second.begin(); 
	     i != s->second.end(); ++i)
	{
		if (i->second.needed == true 
		    && attributes.find(i->first) == attributes.end())
		{
			result.insert(i->first);
		}
	}

	return result;
}
