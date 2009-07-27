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
