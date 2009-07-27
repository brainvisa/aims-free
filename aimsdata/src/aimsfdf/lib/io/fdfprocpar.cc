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
 *  lecture de fichiers FDF
 */

#include <aims/io/fdfprocpar_g.h>

using namespace aims;
using namespace carto;
using namespace std;

namespace aims {

    string FdfProcPar::search( ifstream & file, string param ) {
        string line = "";
        uint space_found;
    
        while (getline(file, line, '\n')) {
            space_found = line.find_first_of(" ");
            if (space_found != string::npos) {
                if (param.compare(0, param.length(), line, 0, space_found) == 0) {
                    // Line starts with the parameter name
                    break;
                }
            }
        }
    
        return line;
    }
    
    int FdfProcPar::search( ifstream & file, string param, int type, int active ) {
        int result = 0, linetype, lineactive;

        string line = "";
        vector<string> tokens;
    
        line = this->search( file, param );
        if ( line != "\0") {
            // Parameter was found, we tokenize it to check it is active and right type
            tokenize(line, tokens, " ");
    
            if ( tokens.size() < 10 ) {
                cout << "Problem getting values for parameter : \'" << param << "\'" << endl;
            }
            else {
                stringTo(tokens[2], linetype);
                stringTo(tokens[9], lineactive);
                if ((lineactive == active) && ( linetype == type )) {
                    // Get the number of elements
                    getline(file, line, ' ');
                    line.erase(line.find_last_not_of (" ") + 1);
                    stringTo(line, result);
                }
            }
        }

        return result;
    }
}
