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
