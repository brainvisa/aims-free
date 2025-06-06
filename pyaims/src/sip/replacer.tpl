 

class Replacer_%Template1typecode%
{
%TypeHeaderCode
#include <aims/utility/linearcomb.h>
%Template1typeinclude%
#ifndef PYAIMS_REPLACER_%Template1typecode%_DEFINED
#define PYAIMS_REPLACER_%Template1typecode%_DEFINED
typedef aims::Replacer<%Template1% > Replacer_%Template1typecode%;
#endif
%End

%Docstring
      Replacement of (label) values in a volume.

      Values are replaced at once, thus it does not suffer from overlapping
      values if ivol is ovol (which would happen for instance in a loop
      replacing one value after another).
%End

public:
  Replacer_%Template1typecode%();
  ~Replacer_%Template1typecode%();

  static void replace( const Volume_%Template1typecode% & ivol,
                       Volume_%Template1typecode% & ovol,
                       const std::map<%Template1%, %Template1%> & repl );
%Docstring
    replace(ivol, ovol, repl)

    Replace values from "repl" keys, from the input volume ivol, to the
    corresponding repl values in the output volume ovol.

    Only actually replaced values will be written in ovol, other will be
    left untouched.

    It is OK to specify the same volume as ivol and ovol, then it will be
    replaced in-place.
%End
%MethodCode
    // aims::Replacer map type has not the same comparator type, 
    // it is necessary to copy in a new std::map with the correct
    // comparator type
    aims::Replacer<%Template1% >::MapType r;
    std::map<%Template1%, %Template1%>::const_iterator it, ie = a2->end();
    for (it=a2->begin(); it!=ie; ++it)
      r[it->first] = it->second;
    aims::Replacer<%Template1% >::replace(*a0, *a1, r);
%End

};

