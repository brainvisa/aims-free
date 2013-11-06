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
 *  General definitions
 */
#ifndef AIMS_DEF_GENERAL_H
#define AIMS_DEF_GENERAL_H

#include <aims/config/aimsdata_config.h>
#include <cartobase/type/types.h>
//#include <soma-io/allocator/allocator.h>
#include <aims/memmap/allocator.h>

namespace aims
{
  std::string aimsVersion();
}

// ABCD in ASCII
#define AIMS_MAGIC_NUMBER 0x41424344
  

/** Display formalism.
    There exists 2 formalisms to display a vector or matrix.
    The image one consider X to be along rows and Y along columns.
    The mathematical one is the contrary. An uint define the 
    formalism taken into account.
*/
AIMSDATA_API enum AimsDisplayFormalism
{
  /// Mathematical formalism
  AIMS_MATH_FORMALISM,
  /// Image formalism
  AIMS_IMAGE_FORMALISM
};


/** The 5 basic types of data.
    Useful enum to specify the type of data.
*/
AIMSDATA_API enum AimsBasicType
{
  AIMS_SCALAR,
  AIMS_VECTOR,
  AIMS_MATRIX,
  AIMS_VOLUME,
  AIMS_SEQVOL
};


/**@name General AIMS functions
*/
//@{
/// Clear the screen
AIMSDATA_API void    AimsClear        (void);
/// Print A.I.M.S. version
AIMSDATA_API void    AimsPrintVersion (void);
/// Send a string containing AIMS\_XXXX
AIMSDATA_API std::string AimsStringTypeOf(int type);
/// Return the number of components of AIMS\_XXXX
AIMSDATA_API int    AimsNCompOf      (int type);
/// Return the minimum value of a AIMS\_XXXX data
AIMSDATA_API double  AimsMinValOf     (int type);
/// Return the maximum value of a AIMS\_XXXX data
AIMSDATA_API double  AimsMaxValOf     (int type);
/// Return True if AIMS\_XXXX is signed
AIMSDATA_API bool    AimsIsSignedType (int type);
/// Return True if AIMS\_XXXX is complex
AIMSDATA_API bool    AimsIsComplexType(int type);
//@}



/**@name String, Error and warning messages*/
//@{
/// Read the next word from *str, put it in strRef and move *str behind the word
AIMSDATA_API void    AimsScanNflush(char **str,char *strRef);
/// Give an error message on display
AIMSDATA_API void    AimsError(const std::string& message);
/// Give a warning message on display
AIMSDATA_API void    AimsWarning(const std::string& message);
//@}



/**@name Getting data size of text files*/
//@{
AIMSDATA_API bool AimsIsScientificNumberChar(int c);
/// Return the dimemsion of the space data (1D, 2D, 3D or 4D)
AIMSDATA_API int AimsSpaceLevelOf  (const std::string& filename);
/// Return the X dimension of a text file
AIMSDATA_API int AimsFileDimXOf    (const std::string& filename);
/// Return the Y dimension of a text file
AIMSDATA_API int AimsFileDimYOf    (const std::string& filename);
/// Return the Z dimension of a text file
AIMSDATA_API int AimsFileDimZOf    (const std::string& filename);
/// Return the T dimension of a text file
AIMSDATA_API int AimsFileDimTOf    (const std::string& filename);
/// Return true if the file contains complex data
AIMSDATA_API bool AimsIsComplexData (const std::string& filename);
/// Return true if the file contains 2D point data
AIMSDATA_API bool AimsIsPoint2dData (const std::string& filename);
/// Return true if the file contains 3D point data
AIMSDATA_API bool AimsIsPoint3dData (const std::string& filename);
//@}


#endif

// mode:C++
