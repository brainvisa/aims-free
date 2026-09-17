#ifndef AIMS_IO_FDFUTIL_H
#define AIMS_IO_FDFUTIL_H

#include <cartobase/type/string_conversion.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iterator>
#include <algorithm>

void removeCharacters(std::string &line, std::string character);

void tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters = " ");

std::string parseLine(std::string line);

template <class T>
void stringToVector (std::string value, std::vector<T>& values)
{
    std::vector<std::string> tokens;
                                                                                                                            
    // value consists of something like {256,256}
    std::string::size_type startBracketPosition = value.find_first_of("{", 0);
    std::string::size_type endBracketPosition = value.find_first_of("}", startBracketPosition);
                                                                                                                            
    if ( startBracketPosition != std::string::npos && endBracketPosition != std::string::npos) {
        std::string elements = value.substr(startBracketPosition + 1, endBracketPosition - startBracketPosition - 1);
                                                                                                                            
                                                                                                                            
        tokenize(elements, tokens, ",");
    }
                                                                                                                            
    T element;

    for(unsigned int i=0; i<tokens.size(); i++) {
        carto::stringTo(tokens[i], element);
        values.push_back(element);
    }
}

template <class T>
void printVector (std::ostream& os, std::string name, const std::vector<T>& vect) 
{
  int size = vect.size();

  os << name << " {";
                                                                                                                     
  for(int i=0; i < size; i++) {
      os << vect[i];

      if (i < size - 1)
        os << ", ";
  }
                                                                                                                             
  os << "}" << std::endl;
}

#endif
