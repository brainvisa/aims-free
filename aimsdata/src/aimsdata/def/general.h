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

// Ignore __attribute__ on non-GCC compilers
#if !(defined(__GNUC__) || defined(__attribute__))
#define __attribute__(a) /* nothing */
#endif

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
AIMSDATA_API void    AimsError(const std::string& message) __attribute__((__noreturn__));
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
