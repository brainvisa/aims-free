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
#include <pyaims/init.h>
#include <Python.h>
#define PY_ARRAY_UNIQUE_SYMBOL PyAims_NumpyAPI
#include <numpy/arrayobject.h>

namespace
{
  /* import_array() is a macro which defines a block of code, which
     includes a conditional return statement. This return statement, in
     python 2, does not return any value, but in python 3, returns NULL.
     So we must use it as part of a function declaration, which conforms to
     this return policy.
   */
#if PY_VERSION_HEX >= 0x03000000
  void *npy_import_array()
  {
    import_array();
    return NULL;
  }
#else
  void npy_import_array()
  {
    import_array();
  }
#endif
}

namespace carto
{
  void pyaims_init(void)
  {
    npy_import_array();
  }
}

#if PY_VERSION_HEX < 0x02070000
/* in python 2.6, PyErr_NewExceptionWithDoc does not exist.
   Some modules (Pandas for Mac + python2.6) need it. Pandas seems obviously
   compiled for python 2.7 but easy_install actually installs it this way.
   However with this trick, it seems to work.
*/
extern "C"
{
  PyObject* PyErr_NewExceptionWithDoc(char *name, char *doc, PyObject *base, PyObject *dict)
  {
    return PyErr_NewException( name, base, dict );
  }
}
#endif

