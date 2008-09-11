
namespace aims
{

  class AimsApplication
  {
%TypeHeaderCode
#include <cartobase/config/verbose.h>
#include <aims/getopt/getopt2.h>
%End

  public:
    AimsApplication( SIP_PYLIST, const std::string & ) 
      [(int &,char **, const std::string &)];
%MethodCode
 // The Python interface is a list of argument strings that is
 // modified.

 int argc;
    char **argv;

    // Convert the list.
    
    if ((argv = pyArgvToC(a0,&argc)) == NULL)
      sipIsErr = 1;
    else
      {
        // Create it now the arguments are right.
        int nargc = argc;

        Py_BEGIN_ALLOW_THREADS
          sipCpp = new aims::AimsApplication(nargc, (const char **) argv, *a1);
        Py_END_ALLOW_THREADS

          // Now modify the original list.
          updatePyArgv(a0,argc,argv);
      }
%End

    void initialize();
    
    int verbose();
%MethodCode
    sipRes = carto::verbose;
%End

  private:

%TypeCode
#include <string.h>


// Convert a Python argv list to a conventional C argc count and argv array.
static char **pyArgvToC(PyObject *argvlist,int *argcp)
{
	int argc;
	char **argv;

	argc = PyList_Size(argvlist);

	// Allocate space for two copies of the argument pointers, plus the
	// terminating NULL.
	if ((argv = (char **)sipMalloc(2 * (argc + 1) * sizeof (char *))) == NULL)
		return NULL;

	// Convert the list.
	for (int a = 0; a < argc; ++a)
	{
		char *arg;

		// Get the argument and allocate memory for it.
		if ((arg = PyString_AsString(PyList_GetItem(argvlist,a))) == NULL ||
		    (argv[a] = (char *)sipMalloc(strlen(arg) + 1)) == NULL)
			return NULL;

		// Copy the argument and save a pointer to it.
		strcpy(argv[a],arg);
		argv[a + argc + 1] = argv[a];
	}

	argv[argc + argc + 1] = argv[argc] = NULL;

	*argcp = argc;

	return argv;
}


// Remove arguments from the Python argv list that have been removed from the
// C argv array.
static void updatePyArgv(PyObject *argvlist,int argc,char **argv)
{
	for (int a = 0, na = 0; a < argc; ++a)
	{
		// See if it was removed.
		if (argv[na] == argv[a + argc + 1])
			++na;
		else
			PyList_SetSlice(argvlist,na,na + 1,NULL);
	}
}
%End

  };

};

