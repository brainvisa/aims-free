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
#include <cstdlib>
#include <aims/getopt/getopt.h>
#include <limits.h>
#include <errno.h>
#include <cstdlib>
#include <iostream>
#include <cartobase/plugin/plugin.h>
#include <string.h>

using namespace carto;
using namespace std;

static int AimsOptionCount(const AimsOption *opt)
{ int ret = 0;
  while (opt[ret].Type != AIMS_OPT_END) ++ret;
  return(ret);
}


static int AimsOptionRequiredCount(const AimsOption *opt)
{ int nopt = AimsOptionCount(opt),ret = 0;
  for (int i=0;i<nopt;i++)
    ret += (opt[i].Required == 1);
  return(ret);
}


static int AimsOptionMatch(const AimsOption *opt,const char *s,int lng)
{ int  nopt,q, matchlen=0;
  char *p;

  nopt = AimsOptionCount(opt);
  if (lng) 
  {
#ifdef __BORLANDC__	// Borland strchr() doesn't take a const char* !!
    if ((p = strchr((char *)s, '=')) != NULL)
#else
    if ((p = strchr(s, '=')) != NULL)
#endif
      matchlen = p - s;
    else
      matchlen = strlen(s);
  }
  for (q=0;q<nopt;q++)
  { if (lng) 
    { if (!opt[q].LongName) continue;
      if (strncmp(s,opt[q].LongName, matchlen) == 0) return(q);
    } 
    else
    { if (!opt[q].ShortName) continue;
      if (*s == opt[q].ShortName) return(q);
    }
  }
  return(-1);
}


static char *AimsOptionString(const AimsOption *opt,int lng)
{ static char ret[31];
  if (lng) 
  { strcpy(ret,"--");
    strncpy(ret + 2, opt->LongName, 28);
  } 
  else 
  { ret[0] = '-';
    ret[1] = opt->ShortName;
    ret[2] = '\0';
  }
  return(ret);
}


static int AimsOptionNeedsArgument(const AimsOption *opt)
{ return(   opt->Type == AIMS_OPT_STRING
         || opt->Type == AIMS_OPT_CHAR
         || opt->Type == AIMS_OPT_BYTE
         || opt->Type == AIMS_OPT_SHORT
         || opt->Type == AIMS_OPT_USHORT         
	 || opt->Type == AIMS_OPT_INT
	 || opt->Type == AIMS_OPT_WORD
	 || opt->Type == AIMS_OPT_LONG
	 || opt->Type == AIMS_OPT_ULONG
 	 || opt->Type == AIMS_OPT_FLOAT
	 || opt->Type == AIMS_OPT_DOUBLE  );
}



static void AimsArgvRemove(int *argc, char **argv, int i)
{ if (i >= *argc) return;
  while (i++ < *argc) argv[i-1] = argv[i];
  --*argc;
}


void AimsOptionExecute(const AimsOption *opt,char *arg,int lng)
{ switch (opt->Type) 
  { case AIMS_OPT_FLAG : 
      if (opt->Flags & AIMS_OPT_CALLFUNC)
	((void (*)(void))opt->Arg)();
      else
        *((int *)opt->Arg) = 1;
      break;
    case AIMS_OPT_STRING :     
      if (opt->Flags & AIMS_OPT_CALLFUNC)
	((void (*)(char *))opt->Arg)(arg);
      else
        *((char **)opt->Arg) = arg;
      break;
    case AIMS_OPT_CHAR  :
    case AIMS_OPT_SHORT :
    case AIMS_OPT_INT   :
    case AIMS_OPT_LONG  : 
      { long tmp;
	char *e;
	tmp = strtol(arg,&e,10);
	if (*e) { cerr << "invalid number `" << arg << "'" << endl;exit(99); }
        if (errno == ERANGE                        || 
            ((opt->Type == AIMS_OPT_CHAR)  && 
             (tmp > SCHAR_MAX || tmp < SCHAR_MIN)) ||
            ((opt->Type == AIMS_OPT_SHORT) && 
             (tmp > SHRT_MAX || tmp < SHRT_MIN))   ||
            ((opt->Type == AIMS_OPT_INT)   && 
             (tmp > INT_MAX || tmp < INT_MIN)) )
	 { cerr << "number `" << arg << "' to `" << AimsOptionString(opt,lng) << 
             "' out of range" << endl;
      exit(99);
    }
        if (opt->Type == AIMS_OPT_CHAR) 
        { if (opt->Flags & AIMS_OPT_CALLFUNC)
            ((void (*)(char))opt->Arg)((char)tmp);
          else
            *((char *)opt->Arg) = (char)tmp;
        } 
        else if (opt->Type == AIMS_OPT_SHORT) 
        { if (opt->Flags & AIMS_OPT_CALLFUNC)
            ((void (*)(short))opt->Arg)((short)tmp);
          else
            *((short *)opt->Arg) = (short)tmp;
        } 
        else if (opt->Type == AIMS_OPT_INT) 
        { if (opt->Flags & AIMS_OPT_CALLFUNC)
            ((void (*)(int))opt->Arg)((int)tmp);
          else
            *((int *)opt->Arg) = (int)tmp;
        } 
        else /* AIMS_OPT_LONG */
        { if (opt->Flags & AIMS_OPT_CALLFUNC)
            ((void (*)(long))opt->Arg)(tmp);
          else
            *((long *)opt->Arg) = tmp;
        }
        break;
      }
    case AIMS_OPT_BYTE   :
    case AIMS_OPT_USHORT :
    case AIMS_OPT_WORD   :
    case AIMS_OPT_ULONG  : 
      { ulong tmp;
        char  *e;
        tmp = strtoul(arg,&e,10);
        if (*e) { cerr << "invalid number `" << arg << "'" << endl; exit(99); }
        if (errno == ERANGE                                  || 
            (opt->Type == AIMS_OPT_BYTE   && tmp > UCHAR_MAX)|| 
            (opt->Type == AIMS_OPT_USHORT && tmp > USHRT_MAX)|| 
            (opt->Type == AIMS_OPT_WORD   && tmp >  UINT_MAX)  )
        { cerr << "number `" << arg << "' to `" << AimsOptionString(opt,lng) << 
             "' out of range" << endl;exit(99);}
        if (opt->Type == AIMS_OPT_BYTE)
        { if (opt->Flags & AIMS_OPT_CALLFUNC)
            ((void (*)(byte))opt->Arg)((byte)tmp);
          else
            *((byte *)opt->Arg) = (byte)tmp;
        } 
        else if (opt->Type == AIMS_OPT_USHORT)
        { if (opt->Flags & AIMS_OPT_CALLFUNC)
            ((void (*)(ushort))opt->Arg)((ushort)tmp);
          else
            *((ushort *)opt->Arg) = (ushort)tmp;
        } 
        else if (opt->Type == AIMS_OPT_WORD)
        { if (opt->Flags & AIMS_OPT_CALLFUNC)
            ((void (*)(word))opt->Arg)((word)tmp);
          else
            *((word *)opt->Arg) = (word)tmp;
        } 
        else /* AIMS_OPT_ULONG */ 
        { if (opt->Flags & AIMS_OPT_CALLFUNC)
            ((void (*)(ulong))opt->Arg)(tmp);
          else
            *((ulong *)opt->Arg) = tmp;
        }
        break;
      }
    case AIMS_OPT_FLOAT :
    case AIMS_OPT_DOUBLE :
      { double tmp;
        char  *e;
        tmp = strtod(arg,&e);
        if (*e) { cerr << "invalid number `" << arg << "'" << endl;exit(99); }
        if (opt->Type == AIMS_OPT_FLOAT)
        { if (opt->Flags & AIMS_OPT_CALLFUNC)
            ((void (*)(float))opt->Arg)((float)tmp);
          else
            *((float *)opt->Arg) = (float)tmp;
        } 
        else /* AIMS_OPT_DOUBLE */ 
        { if (opt->Flags & AIMS_OPT_CALLFUNC)
            ((void (*)(double))opt->Arg)(tmp);
          else
            *((double *)opt->Arg) = tmp;
        }
        break;
      }
    default : 
      break;
  }
}


void AimsParseOptions(int *argc,char **argv, const AimsOption *opt,
                      const char **usage,int allowNegNum)
{ int  ai,        // argv index
       optarg,    // argv index of option argument, or -1 if none
       mi,        // Match index in opt
       done;
  char *arg,      // Pointer to argument to an option
       *o,        // pointer to an option character
       *p;
  int  nrequired=0;

  PluginLoader::load();

  // Loop through all arguments.
  for (ai=0;ai<*argc;) 
  { // "--" indicates that the rest of the argv-array does not contain options.
    if (strcmp(argv[ai],"--") == 0) 
    { AimsArgvRemove(argc,argv,ai);
      break;
    }
    if (allowNegNum && argv[ai][0] == '-' && isdigit(argv[ai][1])) 
    { ++ai;
      continue;
    } 
    else if (strncmp(argv[ai],"--",2) == 0) 
    { // long option
      // find matching option
      if ((mi = AimsOptionMatch(opt,argv[ai]+2,1)) < 0)
      { cerr << "unrecognized option `" << argv[ai] << "'" << endl;exit(99);}
      // possibly locate the argument to this option.
      arg = NULL;
      if ((p = strchr(argv[ai],'=')) != NULL) arg = p + 1;
      // does this option take an argument?
      optarg = -1;
      if (AimsOptionNeedsArgument(&opt[mi])) 
      { // option needs an argument. find it.
        if (!arg) 
        { if ((optarg = ai + 1) == *argc)
          { cerr << "option `" << AimsOptionString(&opt[mi],1)
                 << "' requires an argument" << endl;exit(99); }
          arg = argv[optarg];
        }
      } 
      else
      { if (arg)
        { cerr << "option `" << AimsOptionString(&opt[mi],1)
               << "' doesn't allow an argument" << endl;exit(99); }
      }
      // perform the action of this option.
      AimsOptionExecute(&opt[mi],arg,1);
      if(opt[mi].Required == 1) nrequired++;
      // remove option and any argument from the argv-array.
      if (optarg >= 0) AimsArgvRemove(argc,argv,ai);
      AimsArgvRemove(argc,argv,ai);
    } 
    else if (*argv[ai] == '-') 
    { // A dash by itself is not considered an option.
      if (argv[ai][1] == '\0') 
      { ++ai;
        continue;
      }
      // Short option(s) following
      o = argv[ai] + 1;
      done = 0;
      optarg = -1;
      while (*o && !done) 
      { // find matching option
        if ((mi = AimsOptionMatch(opt,o,0)) < 0)
         { cerr << "unrecognized option `-" << *o << "'" << endl;exit(99); }
        //does this option take an argument?
        optarg = -1;
        arg = NULL;
        if (AimsOptionNeedsArgument(&opt[mi]))
        { // option needs an argument. find it.
          arg = o + 1;
          if (!*arg) 
          { if ((optarg = ai + 1) == *argc)
             { cerr << "option `" << AimsOptionString(&opt[mi],0)
                   << "' requires an argument" << endl;exit(99); }
            arg = argv[optarg];
          }
          done = 1;
        }
        // perform the action of this option.
        AimsOptionExecute(&opt[mi],arg,0);
        if (opt[mi].Required == 1) nrequired++;
        ++o;
      }
      // remove option and any argument from the argv-array.
      if (optarg >= 0)
        AimsArgvRemove(argc,argv,ai);
      AimsArgvRemove(argc,argv,ai);
    } 
    else
    { // a non-option argument
      ++ai;
    }
  }
  if (AimsOptionRequiredCount(opt) != nrequired)
  { cerr << argv[0] << " : more options are required" << endl;
    AimsUsage(usage);
  }
  if (*argc != 1)
  { cerr << argv[0] << " : bad argument on command line" << endl;
    AimsUsage(usage);
  }
}


void AimsUsage(const char **usage_text)
{ while (*usage_text) 
  { cerr << endl << *(usage_text++);
  }
  cout << endl;
  exit(1); 
}
