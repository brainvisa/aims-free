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
