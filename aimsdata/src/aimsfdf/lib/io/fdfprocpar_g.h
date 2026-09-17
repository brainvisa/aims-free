/*
 *  lecture de fichiers FDF
 */
#ifndef AIMS_IO_FDFPROCPAR_G_H
#define AIMS_IO_FDFPROCPAR_G_H

#include <cartobase/stream/fileutil.h>
#include <cartobase/exception/ioexcept.h>
#include <cartobase/type/byte_order.h>
#include <cartobase/type/string_conversion.h>

#include <aims/io/fdfutil.h>
#include <aims/io/fdfprocpar.h>

#include <iostream>
#include <vector>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <regex.h>
#include <dirent.h>

#include <fstream>
#include <sstream>
#include <iterator>
#include <algorithm>

namespace aims
{

    template< class T >
    inline int FdfParType<T>::getFdfType() { return 1; }

    template< class T >
    inline T FdfParType<T>::getFdfDefaultValue() { return 0; }

    template< class T >
    inline char FdfParType<T>::getFdfSeparator() { return ' '; }

    template<>
    inline int FdfParType<std::string>::getFdfType() { return 2; }

    template<>
    inline std::string FdfParType<std::string>::getFdfDefaultValue() { return ""; }

    template<>
    inline char FdfParType<std::string>::getFdfSeparator() { return '\n'; }

    template< class T >
    std::vector<T> FdfProcPar::values(std::string param) {
        uint   start, end;
        int    count;
        int    fdftype;
        std::vector<T> values;
        T value;
        std::string line;
        
        // Open file to read
        std::ifstream file( _name.c_str(), std::ios::in | std::ios::binary );
        
        if (!file) {
            carto::io_error::launchErrnoExcept( _name );
        }
        fdftype = FdfParType<T>::getFdfType();

        // Search parameter in file
        count = this->search(file, param, fdftype, 1);
        char paramsep = FdfParType<T>::getFdfSeparator();

        for (int index = 0; index < count; index++) {

            // Read value
            getline(file, line, paramsep);

            // Trim right
            line.erase(line.find_last_not_of (" ") + 1);

            // Remove starting and ending double quotes
            start = ( line.find( '"' ) == 0 ? 1 : 0 );
            end = line.size() - 1;
            if ( line.rfind( '"' ) == end ) {
                end--;
            }
            line = line.substr( start, end - start + 1 );
            try {
                carto::stringTo(line, value);
                values.push_back(value);
            }
            catch(...) {
            }
        }

        file.close();
    
        return values;
    }

    template< class T >
    T FdfProcPar::value(std::string param, T defaultvalue) {
        std::vector<T> foundvalues = values<T>( param );

        if ( foundvalues.size() > 0 ) {
            return foundvalues[0];
        }
        else {
            return defaultvalue;
        }
    }
    
    template< class T >
    T FdfProcPar::value(std::string param) {
        T defaultvalue = FdfParType<T>::getFdfDefaultValue();
        return value<T>(param, defaultvalue);
    }

}

#endif
