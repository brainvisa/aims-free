/*
 *  lecture de fichiers FDF
 */

// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/io/fdfprocpar_g.h>

using namespace aims;
using namespace carto;
using namespace std;

namespace aims {

    string FdfProcPar::search( ifstream & file, string param ) {
        string line = "";
        size_t space_found;
    
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
