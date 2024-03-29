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

// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include "fdfutil.h"
#include <cartobase/exception/ioexcept.h>

using namespace carto;

// Remove a particular type of character from a string
void removeCharacters(std::string &line, std::string character)
{
    std::string::size_type characterPosition = line.find_first_of(character, 0);
    while ( characterPosition != std::string::npos ) {
        line.erase(characterPosition, character.size());
        characterPosition = line.find_first_of(character, characterPosition);
    }
}

void tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters)
{
    // Skip delimiters at beginning.
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    std::string::size_type pos = str.find_first_of(delimiters, lastPos);

    while (std::string::npos != pos || std::string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}

std::string parseLine(std::string line)
{
    // sanity check
    if( line.find( '\0' ) != std::string::npos )
      throw parse_error( "invalid character encountered", "", "", 0 );
    // strip *
    removeCharacters(line, "*");
    removeCharacters(line, "\"");
    removeCharacters(line, "[");
    removeCharacters(line, "]");

    // Need to deal with space between {}
    std::string::size_type startBracketPosition = line.find_first_of("{", 0);
    std::string::size_type endBracketPosition = line.find_first_of("}", startBracketPosition);
                                                                                                                            
    if ( startBracketPosition != std::string::npos && endBracketPosition != std::string::npos) {
        std::string element = line.substr(startBracketPosition, endBracketPosition - startBracketPosition);

        // Find whitespace within {} and erase
        std::string::size_type whiteSpacePosition = line.find_first_of(" ", startBracketPosition);

        while (whiteSpacePosition != std::string::npos)
        {
            line.erase(whiteSpacePosition, 1);
            whiteSpacePosition = line.find_first_of(" ", whiteSpacePosition);
        }
    }

    return line;
}
