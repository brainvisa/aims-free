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
 *  UNIX-like argument and option management
 */
#ifndef AIMS_GETOPT_GETOPT_H
#define AIMS_GETOPT_GETOPT_H

#include <aims/config/aimsdata_config.h>
#include <aims/def/general.h>

/**@name UNIX-like line options and arguments.
    This package allow you to create command lines and to manage easily
    the arguments.
*/
//@{
  /**@name Description of the command line : usage.
      Those 2 defines create a char double pointer that you can fill with
      the description of the command line syntax. This text is then sent
      to the stderr stream whenever a problem appears. It can also be sent
      by using the AimsUsage function ( see below).\\
      \\Here is an example to show you the way to use\\  
      \\BEGIN\_USAGE(usage)
      \\"----------------------------------------------------------------",
      \\"AimsCurvature2D [-h[elp]] -i[nput] <filein> -o[utput] <fileout>",
      \\"----------------------------------------------------------------",
      \\"2D curvature of an intensity image f(x,y) = I",
      \\"----------------------------------------------------------------",
      \\"     filein  : source matrix",
      \\"     fileout : destination float matrix",
      \\"----------------------------------------------------------------",
      \\END\_USAGE\\
      \\ Include file : getopt.h
  */
  //@{
  /// Starts the command line description
#define BEGIN_USAGE(usage)    static const char *usage[]={
  /// Ends the command line description
#define END_USAGE             0 };
  //@}

  /** The different option types.
      Eleven base types are recognized on the command line ( char, byte,
      short, ushort, int, word, long, ulong, float,double, and char*).
      The option AIMS_OPT_FLAG means that is a single option without
      parameter.
  */
AIMSDATA_API enum AimsArgType
{
  ///
  AIMS_OPT_END,
  ///
  AIMS_OPT_FLAG,
  ///
  AIMS_OPT_STRING,
  ///
  AIMS_OPT_CHAR,
  ///
  AIMS_OPT_BYTE,
  ///
  AIMS_OPT_SHORT,
  ///
  AIMS_OPT_USHORT,
  ///
  AIMS_OPT_INT,
  ///
  AIMS_OPT_WORD,
  ///
  AIMS_OPT_LONG,
  ///
  AIMS_OPT_ULONG,
  ///
  AIMS_OPT_FLOAT,
  ///
  AIMS_OPT_DOUBLE
};

#define AIMS_OPT_CALLFUNC      1

AIMSDATA_API struct AimsOption
{ char        ShortName;
  const char  *LongName;
  AimsArgType Type;
  const void  *Arg;
  int         Flags;
  int         Required;
};

AIMSDATA_API void AimsOptionSetFatalFunc(void (*f)(const char *, ...));

  /**@name Functions.
      In your main, you must give an AimsOption structure which gives the
      program the way to process the options. Here is an example of use :\\
      \\BEGIN\_USAGE(usage)
      \\"----------------------------------------------------------------",
      \\"AimsCurvature2D [-h[elp]] -i[nput] <filein> -o[utput] <fileout>",
      \\"----------------------------------------------------------------",
      \\"2D curvature of an intensity image f(x,y) = I",
      \\"----------------------------------------------------------------",
      \\"     filein  : source matrix",
      \\"     fileout : destination float matrix",
      \\"----------------------------------------------------------------",
      \\END\_USAGE\\ \\
      \\
      \\void Usage(void)
      \\{ AimsUsage(usage);
      \\}
      \\
      \\void main(int argc,char **argv)
      \\{  char *filein="none",*fileout="none";
      \\long format;
      \\long type;
      \\float number = 15;
      \\
      \\AimsOption opt[] = {
      \\{ 'h',"help"  ,AIMS\_OPT\_FLAG  ,Usage   ,AIMS\_OPT\_CALLFUNC,0,},
      \\{ 'i',"input" ,AIMS\_OPT\_STRING,&filein ,0                ,1},
      \\{ 'o',"output",AIMS\_OPT\_STRING,&fileout,0                ,1},
      \\{ 0  ,0       ,AIMS\_OPT\_END   ,0       ,0                ,0}};
      \\
      \\AimsParseOptions(&argc,argv,opt,usage);\\
      \\ ...
      \\}
      \\
      \\ In option structure, the first element gives a short name of the
      \\ option; the second element gives a long name of the option; the third
      \\ element gives the type of the option; the fourth element gives a 
      \\ pointer to the data to be filled or a pointer to the function to
      \\ run if the option is valid; the fifth element tells the parser
      \\ wether the given pointer points a data or a function; and the last
      \\ element tells the parser wether the option is required or not.
  */
  //@{
  /// Parse all the options and returns eventually an error message
AIMSDATA_API void AimsParseOptions(int *argc,char **argv,const AimsOption *opt,
                      const char **usage,int allowNegNum=1);
  /// Return usage on stderr stream
AIMSDATA_API void AimsUsage(const char **usage_text);
  //@}

//@}

#endif
