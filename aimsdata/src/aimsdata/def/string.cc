// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

/*
 *  General String operations
 */
#include <cstdlib>
#include <aims/def/general.h>
#include <cstdlib>
#include <iostream>
#include <stdio.h>

using namespace std;


void AimsScanNflush(char **str,char *strRef)
{
  sscanf(*str,"%s",strRef);
  while (**str!=*strRef) (*str)++;
  while (*(*str)++ == *strRef++) {}
  while (**str == ' ' || **str == '\t' || **str == '\n') (*str)++;
}


void AimsError(const string& message)
{
  cerr << "\n-------------------------------------------"
       << "------------------------------------\n";
  cerr << "######################### A.I.M.S. syntax error !!!"
       << "###########################\n";
  cerr << message << "\n";
  cerr << "----------------------------------------"
       << "---------------------------------------\n";
  exit(EXIT_FAILURE);
}


void AimsWarning(const string& message)
{
  cerr << "\n-------------------------------------------"
       << "------------------------------------\n";
  cerr << "########################### A.I.M.S. warning !!!"
       << "##############################\n";
  cerr << message << "\n";
  cerr << "---------------------------------------------"
       << "----------------------------------\n";
}



