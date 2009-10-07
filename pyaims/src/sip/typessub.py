# Copyright IFR 49 (1995-2009)
#
#  This software and supporting documentation were developed by
#      Institut Federatif de Recherche 49
#      CEA/NeuroSpin, Batiment 145,
#      91191 Gif-sur-Yvette cedex
#      France
#
# This software is governed by the CeCILL-B license under
# French law and abiding by the rules of distribution of free software.
# You can  use, modify and/or redistribute the software under the 
# terms of the CeCILL-B license as circulated by CEA, CNRS
# and INRIA at the following URL "http://www.cecill.info". 
# 
# As a counterpart to the access to the source code and  rights to copy,
# modify and redistribute granted by the license, users are provided only
# with a limited warranty  and the software's author,  the holder of the
# economic rights,  and the successive licensors  have only  limited
# liability. 
# 
# In this respect, the user's attention is drawn to the risks associated
# with loading,  using,  modifying and/or developing or reproducing the
# software by the user in light of its specific status of free software,
# that may mean  that it is complicated to manipulate,  and  that  also
# therefore means  that it is reserved for developers  and  experienced
# professionals having in-depth computer knowledge. Users are therefore
# encouraged to load and test the software's suitability as regards their
# requirements in conditions enabling the security of their systems and/or 
# data to be ensured and,  more generally, to use and operate it in the 
# same conditions as regards security. 
# 
# The fact that you are presently reading this means that you have had
# knowledge of the CeCILL-B license and that you accept its terms.
def classInNamespace( include, cls, namespace ):
  return { 'typecode' : cls,
    'pyFromC' : 'pyaimsConvertFrom_' + namespace + '_' + cls,
    'CFromPy' : 'pyaimsConvertTo_' + namespace + '_' + cls,
    'castFromSip' : '(' + namespace + '::' + cls + ' *)',
    'deref' : '*',
    'pyderef' : '*',
    'address' : '&', 
    'pyaddress' : '&', 
    'defScalar' : '',
    'new' : 'new ' + namespace + '::' + cls, 
    'NumType' : 'PyArray_OBJECT', 
    'PyType' : namespace + '::' + cls,
    'sipClass' : namespace + '_' + cls,
    'typeinclude' : \
    '#include <' + include + '>', 
    'sipinclude' : '#ifndef PYAIMS_' + namespace.upper() + '_' + cls.upper() + '_CHECK_DEFINED\n'
    '#define PYAIMS_' + namespace.upper() + '_' + cls.upper() + '_CHECK_DEFINED\n'
    'inline int pyaims' + cls + '_Check( PyObject* o )\n'
    '{ return sipCanConvertToInstance( o, sipClass_' + namespace + '_' + cls + ', SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
    '#endif\n'
    '#ifndef PYAIMS_' + namespace.upper() + '_' + cls.upper() + '_CONVERT_DEFINED\n'
    '#define PYAIMS_' + namespace.upper() + '_' + cls.upper() + '_CONVERT_DEFINED\n'
    'inline void* pyaimsConvertTo_' + namespace + '_' + cls + '( PyObject* o )\n'
    '{ int iserr = 0;\n'
    '  void *ptr = sipForceConvertToInstance( o, sipClass_' + namespace + '_' + cls + ', 0, 0, 0, &iserr );\n'
    '  if( iserr ) return 0;\n'
    '  return ptr;\n}\n'
    'inline PyObject* pyaimsConvertFrom_' + namespace + '_' + cls + '( void * a )\n'
    '{ return sipConvertFromInstance( a, sipClass_' + namespace + '_' + cls + ', 0 ); }\n'
    '#endif\n', 
    'module' : 'aims', 
    'testPyType' : 'pyaims' + cls + '_Check',
    'compareElement' : '&',
    }

def classInCartoNamespace( include, cls ):
  return classInNamespace( include, cls, 'carto' )

def classInAimsNamespace( include, cls ):
  return classInNamespace( include, cls, 'aims' )


def completeTypesSub( typessub ):
  for x,y in typessub.items():
    if not y.has_key( 'compareElement' ):
      if( x.endswith( '*' ) ):
        y[ 'compareElement' ] = ''
      else:
        y[ 'compareElement' ] = '&'


typessub = { 'signed char' : \
             { 'typecode' : 'S8',
               'pyFromC' : 'PyInt_FromLong',
               'CFromPy' : 'PyInt_AsLong',
               'castFromSip' : '',
               'deref' : '', 
               'pyderef' : '',
               'address' : '', 
               'pyaddress' : '', 
               'defScalar' : '#define PYAIMS_SCALAR',
               'new' : '',
               'NumType' : 'PyArray_SBYTE',
               'PyType' : 'short',
               'sipClass' : '',
               'typeinclude' : '', 
               'sipinclude' : '', 
               'module' : 'aims',
               'testPyType' : 'PyNumber_Check',
               'compareElement' : '',
             },
             'unsigned char' : \
             { 'typecode' : 'U8',
               'pyFromC' : 'PyInt_FromLong',
               'CFromPy' : 'PyInt_AsLong',
               'castFromSip' : '',
               'deref' : '',
               'pyderef' : '',
               'address' : '', 
               'pyaddress' : '', 
               'defScalar' : '#define PYAIMS_SCALAR',
               'new' : '', 
               'NumType' : 'PyArray_UBYTE', 
               'PyType' : 'unsigned short',
               'sipClass' : '',
               'typeinclude' : '', 
               'sipinclude' : '', 
               'module' : 'aims', 
               'testPyType' : 'PyNumber_Check',
               'compareElement' : '',
             },
             'short' : \
             { 'typecode' : 'S16',
               'pyFromC' : 'PyInt_FromLong',
               'CFromPy' : 'PyInt_AsLong',
               'castFromSip' : '',
               'deref' : '',
               'pyderef' : '',
               'address' : '', 
               'pyaddress' : '', 
               'defScalar' : '#define PYAIMS_SCALAR',
               'new' : '', 
               'NumType' : 'PyArray_SHORT', 
               'PyType' : 'short',
               'sipClass' : '',
               'typeinclude' : '', 
               'sipinclude' : '', 
               'module' : 'aims', 
               'testPyType' : 'PyNumber_Check', 
               'compareElement' : '',
             },
             'unsigned short' : \
             { 'typecode' : 'U16',
               'pyFromC' : 'PyInt_FromLong',
               'CFromPy' : 'PyInt_AsLong',
               'castFromSip' : '',
               'deref' : '',
               'pyderef' : '',
               'address' : '', 
               'pyaddress' : '', 
               'defScalar' : '#define PYAIMS_SCALAR',
               'new' : '', 
               'NumType' : 'PyArray_USHORT', 
               'PyType' : 'unsigned short',
               'sipClass' : '',
               'typeinclude' : '', 
               'sipinclude' : '', 
               'module' : 'aims', 
               'testPyType' : 'PyNumber_Check', 
               'compareElement' : '',
               },
             'int' : \
             { 'typecode' : 'S32',
               'pyFromC' : 'PyInt_FromLong',
               'CFromPy' : 'PyInt_AsLong',
               'castFromSip' : '',
               'deref' : '',
               'pyderef' : '',
               'address' : '', 
               'pyaddress' : '', 
               'defScalar' : '#define PYAIMS_SCALAR',
               'new' : '', 
               'NumType' : 'PyArray_INT', 
               'PyType' : 'int',
               'sipClass' : '',
               'typeinclude' : '', 
               'sipinclude' : '', 
               'module' : 'aims', 
               'testPyType' : 'PyNumber_Check', 
               'compareElement' : '',
             },
             'unsigned' : \
             { 'typecode' : 'U32',
               'pyFromC' : 'PyInt_FromLong',
               'CFromPy' : 'PyLong_AsUnsignedLong',
               'castFromSip' : '',
               'deref' : '',
               'pyderef' : '',
               'address' : '', 
               'pyaddress' : '', 
               'defScalar' : '#define PYAIMS_SCALAR',
               'new' : '', 
               'NumType' : 'PyArray_UINT', 
               'PyType' : 'unsigned',
               'sipClass' : '',
               'typeinclude' : '', 
               'sipinclude' : '', 
               'module' : 'aims', 
               'testPyType' : 'PyNumber_Check', 
               'compareElement' : '',
             },
             'unsigned long' : \
             { 'typecode' : 'ULONG',
               'pyFromC' : 'PyInt_FromLong',
               'CFromPy' : 'PyLong_AsUnsignedLong',
               'castFromSip' : '',
               'deref' : '',
               'pyderef' : '',
               'address' : '', 
               'pyaddress' : '', 
               'defScalar' : '#define PYAIMS_SCALAR',
               'new' : '', 
               'NumType' : 'PyArray_UINT', 
               'PyType' : 'unsigned long',
               'sipClass' : '',
               'typeinclude' : '', 
               'sipinclude' : '', 
               'module' : 'aims', 
               'testPyType' : 'PyNumber_Check', 
               'compareElement' : '',
             },
             'float' : \
             { 'typecode' : 'FLOAT',
               'pyFromC' : 'PyFloat_FromDouble',
               'CFromPy' : 'PyFloat_AsDouble',
               'castFromSip' : '',
               'deref' : '',
               'pyderef' : '',
               'address' : '', 
               'pyaddress' : '', 
               'defScalar' : '#define PYAIMS_SCALAR',
               'new' : '', 
               'NumType' : 'PyArray_FLOAT', 
               'PyType' : 'float',
               'sipClass' : '',
               'typeinclude' : '', 
               'sipinclude' : '', 
               'module' : 'aims', 
               'testPyType' : 'PyNumber_Check', 
               'compareElement' : '',
             },
             'double' : \
             { 'typecode' : 'DOUBLE',
               'pyFromC' : 'PyFloat_FromDouble',
               'CFromPy' : 'PyFloat_AsDouble',
               'castFromSip' : '',
               'deref' : '',
               'pyderef' : '',
               'address' : '', 
               'pyaddress' : '', 
               'defScalar' : '#define PYAIMS_SCALAR',
               'new' : '', 
               'NumType' : 'PyArray_DOUBLE', 
               'PyType' : 'double',
               'sipClass' : '',
               'typeinclude' : '', 
               'sipinclude' : '', 
               'module' : 'aims', 
               'testPyType' : 'PyNumber_Check', 
               'compareElement' : '',
             },
             'std::string' : \
             { 'typecode' : 'STRING',
               'pyFromC' : 'PyString_FromStdString',
               'CFromPy' : 'PyString_ToStdString',
               'castFromSip' : '(std::string *)',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '', 
               'defScalar' : '',
               'new' : 'new std::string', 
               'NumType' : 'PyArray_STRING', 
               'PyType' : 'std::string',
               'sipClass' : '',
               'typeinclude' : '#include <string>', 
               'sipinclude' : '#ifndef PYSTDSTRING\n'
               '#define PYSTDSTRING\n'
               'inline PyObject*\n'
               'PyString_FromStdString( const std::string * x )\n'
               '{ return PyString_FromString( x->c_str() ); }\n'
               'inline std::string *\n'
               'PyString_ToStdString( PyObject * x )\n'
               '{ return new std::string(PyString_AsString(x)); }\n'
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'PyString_Check', 
               'compareElement' : '',
             },
             'Void' : \
             { 'typecode' : 'VOID',
               'pyFromC' : 'PyNone',
               'CFromPy' : '',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new Void', 
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'Void',
               'sipClass' : 'Void',
               'typeinclude' : '#include <cartobase/type/types.h>',
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipVoid.h"\n' 
               '#endif\n'
               '#ifndef PYAIMS_VOID_DEFINED\n' 
               '#define PYAIMS_VOID_DEFINED\n' 
               'inline int pyaimsVoid_Ckeck( PyObject *o )\n'
               '{ return o == PyNone; }\n'
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsVoid_Check', 
               'compareElement' : '',
             },
             'AimsRGB' : \
             { 'typecode' : 'RGB',
               'pyFromC' : '',
               'CFromPy' : '',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new AimsRGB', 
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'AimsRGB',
               'sipClass' : 'AimsRGB',
               'typeinclude' : '#include <aims/rgb/rgb.h>', 
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipAimsRGB.h"\n'
               '#endif\n'
               '#ifndef PYAIMS_RGB_DEFINED\n' 
               '#define PYAIMS_RGB_DEFINED\n'
               'inline int pyaimsRGB_Check( PyObject* o )\n'
               '{ return sipCanConvertToInstance( o, sipClass_AimsRGB, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsRGB_Check', 
               'compareElement' : '',
               },
             'AimsRGBA' : \
             { 'typecode' : 'RGBA',
               'pyFromC' : '',
               'CFromPy' : '',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new AimsRGBA', 
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'AimsRGBA',
               'sipClass' : 'AimsRGBA',
               'typeinclude' : '#include <aims/rgb/rgb.h>', 
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipAimsRGBA.h"\n' 
               '#endif\n'
               '#ifndef PYAIMS_RGBA_DEFINED\n' 
               '#define PYAIMS_RGBA_DEFINED\n'
               'inline int pyaimsRGBA_Check( PyObject* o )\n'
               '{ return sipCanConvertToInstance( o, sipClass_AimsRGBA, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsRGBA_Check', 
               'compareElement' : '',
               },

             'Point2df' : \
             { 'typecode' : 'POINT2DF',
               'pyFromC' : 'pyaimsConvertFrom_AimsVector',
               'CFromPy' : 'pyaimsConvertTo_AimsVector<float,2>',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new Point2df', 
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'Point2df',
               'sipClass' : 'Point2df',
               'typeinclude' : '#include <pyaims/vector/vector.h>', 
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipAimsVector_FLOAT_2.h"\n' 
               '#endif\n'
               '#include <pyaims/vector/sipvector.h>\n'
               '#ifndef PYAIMS_WRAPPER_AIMSVECTOR_FLOAT_2\n'
               '#define PYAIMS_WRAPPER_AIMSVECTOR_FLOAT_2\n'
               'template <> inline sipWrapperType*\n'
               'sipClass_AimsVector<float,2>()\n'
               '{ return sipClass_AimsVector_FLOAT_2; }\n' 
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsCheck_AimsVector<float,2>', 
               'compareElement' : '',
               },

             'Point3df' : \
             { 'typecode' : 'POINT3DF',
               'pyFromC' : 'pyaimsConvertFrom_AimsVector',
               'CFromPy' : 'pyaimsConvertTo_AimsVector<float,3>',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new Point3df', 
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'Point3df',
               'sipClass' : 'Point3df',
               'typeinclude' : '#include <pyaims/vector/vector.h>', 
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipAimsVector_FLOAT_3.h"\n' 
               '#endif\n'
               '#include <pyaims/vector/sipvector.h>\n'
               '#ifndef PYAIMS_WRAPPER_AIMSVECTOR_FLOAT_3\n'
               '#define PYAIMS_WRAPPER_AIMSVECTOR_FLOAT_3\n'
               'template <> inline sipWrapperType*\n'
               'sipClass_AimsVector<float,3>()\n'
               '{ return sipClass_AimsVector_FLOAT_3; }\n' 
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsCheck_AimsVector<float,3>', 
               'compareElement' : '',
               },

             'Point3dd' : \
             { 'typecode' : 'POINT3DD',
               'pyFromC' : 'pyaimsConvertFrom_AimsVector',
               'CFromPy' : 'pyaimsConvertTo_AimsVector<double,3>',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new Point3dd', 
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'Point3dd',
               'sipClass' : 'Point3dd',
               'typeinclude' : '#include <pyaims/vector/vector.h>', 
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipAimsVector_DOUBLE_3.h"\n' 
               '#endif\n'
               '#include <pyaims/vector/sipvector.h>\n'
               '#ifndef PYAIMS_WRAPPER_AIMSVECTOR_DOUBLE_3\n'
               '#define PYAIMS_WRAPPER_AIMSVECTOR_DOUBLE_3\n'
               'template <> inline sipWrapperType*\n'
               'sipClass_AimsVector<double,3>()\n'
               '{ return sipClass_AimsVector_DOUBLE_3; }\n' 
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsCheck_AimsVector<double,3>', 
               'compareElement' : '',
               },

             'Point3d' : \
             { 'typecode' : 'POINT3D',
               'pyFromC' : 'pyaimsConvertFrom_AimsVector',
               'CFromPy' : 'pyaimsConvertTo_AimsVector<int16_t,3>',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new Point3d', 
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'Point3d',
               'sipClass' : 'Point3d',
               'typeinclude' : '#include <pyaims/vector/vector.h>', 
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipAimsVector_S16_3.h"\n' 
               '#endif\n'
               '#include <pyaims/vector/sipvector.h>\n'
               '#ifndef PYAIMS_WRAPPER_AIMSVECTOR_S16_3\n'
               '#define PYAIMS_WRAPPER_AIMSVECTOR_S16_3\n'
               'template <> inline sipWrapperType*\n'
               'sipClass_AimsVector<int16_t,3>()\n'
               '{ return sipClass_AimsVector_S16_3; }\n' 
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsCheck_AimsVector<int16_t,3>', 
               'compareElement' : '',
               },

             'AimsVector<unsigned,2>' : \
             { 'typecode' : 'AimsVector_U32_2',
               'pyFromC' : 'pyaimsConvertFrom_AimsVector',
               'CFromPy' : 'pyaimsConvertTo_AimsVector<uint32_t,2>',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new AimsVector_U32_2', 
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'AimsVector_U32_2',
               'sipClass' : 'AimsVector_U32_2',
               'typeinclude' : '#include <pyaims/vector/vector.h>', 
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipAimsVector_U32_2.h"\n' 
               '#endif\n'
               '#include <pyaims/vector/sipvector.h>\n'
               '#ifndef PYAIMS_WRAPPER_AIMSVECTOR_U32_2\n'
               '#define PYAIMS_WRAPPER_AIMSVECTOR_U32_2\n'
               'template <> inline sipWrapperType*\n'
               'sipClass_AimsVector<uint32_t,2>()\n'
               '{ return sipClass_AimsVector_U32_2; }\n' 
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsCheck_AimsVector<uint32_t,2>', 
               'compareElement' : '',
               },

             'AimsVector<unsigned,3>' : \
             { 'typecode' : 'AimsVector_U32_3',
               'pyFromC' : 'pyaimsConvertFrom_AimsVector',
               'CFromPy' : 'pyaimsConvertTo_AimsVector<uint32_t,3>',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new AimsVector_U32_3', 
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'AimsVector_U32_3',
               'sipClass' : 'AimsVector_U32_3',
               'typeinclude' : '#include <pyaims/vector/vector.h>', 
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipAimsVector_U32_3.h"\n' 
               '#endif\n'
               '#include <pyaims/vector/sipvector.h>\n'
               '#ifndef PYAIMS_WRAPPER_AIMSVECTOR_U32_3\n'
               '#define PYAIMS_WRAPPER_AIMSVECTOR_U32_3\n'
               'template <> inline sipWrapperType*\n'
               'sipClass_AimsVector<uint32_t,3>()\n'
               '{ return sipClass_AimsVector_U32_3; }\n' 
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsCheck_AimsVector<uint32_t,3>', 
               'compareElement' : '',
               },

             'AimsVector<unsigned,4>' : \
             { 'typecode' : 'AimsVector_U32_4',
               'pyFromC' : 'pyaimsConvertFrom_AimsVector',
               'CFromPy' : 'pyaimsConvertTo_AimsVector<uint32_t,4>',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new AimsVector_U32_4', 
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'AimsVector_U32_4',
               'sipClass' : 'AimsVector_U32_4',
               'typeinclude' : '#include <pyaims/vector/vector.h>', 
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipAimsVector_U32_4.h"\n'
               '#endif\n'
               '#include <pyaims/vector/sipvector.h>\n'
               '#ifndef PYAIMS_WRAPPER_AIMSVECTOR_U32_4\n'
               '#define PYAIMS_WRAPPER_AIMSVECTOR_U32_4\n'
               'template <> inline sipWrapperType*\n'
               'sipClass_AimsVector<uint32_t,4>()\n'
               '{ return sipClass_AimsVector_U32_4; }\n' 
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsCheck_AimsVector<uint32_t,4>', 
               'compareElement' : '',
               },

             'carto::Semantic' : \
                classInCartoNamespace( 'cartobase/object/syntax.h',
                'Semantic' ),
             'Tree' : \
             { 'typecode' : 'Tree',
               'pyFromC' : '',
               'CFromPy' : '',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new Tree',
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'Tree',
               'sipClass' : 'Tree',
               'typeinclude' : '#include <graph/tree/tree.h>',
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipTree.h"\n'
               '#endif\n'
               '#ifndef PYAIMS_TREE_DEFINED\n'
               '#define PYAIMS_TREE_DEFINED\n'
               'inline int pyaimsTree_Check( PyObject* o )\n'
               '{ return sipCanConvertToInstance( o, sipClass_Tree, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsTree_Check',
               },
             'aims::Hierarchy' : \
             { 'typecode' : 'Hierarchy',
               'pyFromC' : '',
               'CFromPy' : '',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new aims::Hierarchy',
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'aims::Hierarchy',
               'sipClass' : 'aims_Hierarchy',
               'typeinclude' : '#include <aims/roi/hie.h>',
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipaimsHierarchy.h"\n'
               '#endif\n'
               '#ifndef PYAIMS_HIERARCHY_DEFINED\n'
               '#define PYAIMS_HIERARCHY_DEFINED\n'
               'inline int pyaimsHierarchy_Check( PyObject* o )\n'
               '{ return sipCanConvertToInstance( o, sipClass_aims_Hierarchy, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsHierarchy_Check',
               },
             'Graph' : \
             { 'typecode' : 'Graph',
               'pyFromC' : '',
               'CFromPy' : '',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new Graph', 
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'Graph',
               'sipClass' : 'Graph',
               'typeinclude' : '#include <graph/graph/graph.h>', 
               'sipinclude' : '#ifndef PYAIMS_GRAPH_DEFINED\n'
               '#define PYAIMS_GRAPH_DEFINED\n'
               'inline int pyaimsGraph_Check( PyObject* o )\n'
               '{ return sipCanConvertToInstance( o, sipClass_Graph, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsGraph_Check', 
               },
             'carto::Syntax' : \
                classInCartoNamespace( 'cartobase/object/syntax.h',
                'Syntax' ),
             'AimsSurfaceTriangle' : \
             { 'typecode' : 'AimsSurfaceTriangle',
               'pyFromC' : '',
               'CFromPy' : '',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new AimsSurfaceTriangle', 
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'AimsSurfaceTriangle',
               'sipClass' : 'AimsSurfaceTriangle',
               'typeinclude' : '#include <aims/mesh/surface.h>', 
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipAimsSurfaceTriangle.h"\n' 
               '#endif\n'
               '#ifndef PYAIMS_SURFACETRIANGLE_DEFINED\n' 
               '#define PYAIMS_SURFACETRIANGLE_DEFINED\n'
               'inline int pyaimsSurfaceTriangle_Check( PyObject* o )\n'
               '{ return sipCanConvertToInstance( o, sipClass_AimsSurfaceTriangle, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsSurfaceTriangle_Check', 
               },
             'carto::GenericObject' : \
             { 'typecode' : 'GenericObject',
               'pyFromC' : '',
               'CFromPy' : '',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new carto::GenericObject', 
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'carto::GenericObject',
               'sipClass' : 'carto_GenericObject',
               'typeinclude' : '#include <cartobase/object/object.h>', 
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipcartoGenericObject.h"\n' 
               '#endif\n'
               '#ifndef PYAIMS_GENERICOBJECT_DEFINED\n' 
               '#define PYAIMS_GENERICOBJECT_DEFINED\n'
               'inline int pyaimsGenericObject_Check( PyObject* o )\n'
               '{ return sipCanConvertToInstance( o, sipClass_carto_GenericObject, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsGenericObject_Check', 
               },

             'AimsData<int8_t>' : \
             { 'typecode' : 'AimsData_S8',
               'pyFromC' : '',
               'CFromPy' : '',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new AimsData<int8_t>', 
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'AimsData_S8',
               'sipClass' : 'AimsData_S8',
               'typeinclude' : '#include <aims/data/data.h>', 
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipAimsData_S8.h"\n' 
               '#endif\n'
               '#ifndef PYAIMS_AIMSDATA_S8_CHECK_DEFINED\n' 
               '#define PYAIMS_AIMSDATA_S8_CHECK_DEFINED\n'
               'inline int pyaimsAimsData_S8_Check( PyObject* o )\n'
               '{ return sipCanConvertToInstance( o, sipClass_AimsData_S8, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsAimsData_S8_Check', 
               },
             'AimsData<uint8_t>' : \
             { 'typecode' : 'AimsData_U8',
               'pyFromC' : '',
               'CFromPy' : '',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new AimsData<uint8_t>', 
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'AimsData_U8',
               'sipClass' : 'AimsData_U8',
               'typeinclude' : '#include <aims/data/data.h>', 
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipAimsData_U8.h"\n' 
               '#endif\n'
               '#ifndef PYAIMS_AIMSDATA_U8_CHECK_DEFINED\n' 
               '#define PYAIMS_AIMSDATA_U8_CHECK_DEFINED\n'
               'inline int pyaimsAimsData_U8_Check( PyObject* o )\n'
               '{ return sipCanConvertToInstance( o, sipClass_AimsData_U8, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsAimsData_U8_Check', 
               },
             'AimsData<int16_t>' : \
             { 'typecode' : 'AimsData_S16',
               'pyFromC' : '',
               'CFromPy' : '',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new AimsData<int16_t>', 
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'AimsData_S16',
               'sipClass' : 'AimsData_S16',
               'typeinclude' : '#include <aims/data/data.h>', 
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipAimsData_S16.h"\n'
               '#endif\n'
               '#ifndef PYAIMS_AIMSDATA_S16_CHECK_DEFINED\n'
               '#define PYAIMS_AIMSDATA_S16_CHECK_DEFINED\n'
               'inline int pyaimsAimsData_S16_Check( PyObject* o )\n'
               '{ return sipCanConvertToInstance( o, sipClass_AimsData_S16, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsAimsData_S16_Check', 
               },
             'AimsData<uint16_t>' : \
             { 'typecode' : 'AimsData_U16',
               'pyFromC' : '',
               'CFromPy' : '',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new AimsData<uint16_t>', 
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'AimsData_U16',
               'sipClass' : 'AimsData_U16',
               'typeinclude' : '#include <aims/data/data.h>', 
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipAimsData_U16.h"\n'
               '#endif\n'
               '#ifndef PYAIMS_AIMSDATA_U16_CHECK_DEFINED\n'
               '#define PYAIMS_AIMSDATA_U16_CHECK_DEFINED\n'
               'inline int pyaimsAimsData_U16_Check( PyObject* o )\n'
               '{ return sipCanConvertToInstance( o, sipClass_AimsData_U16, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsAimsData_U16_Check', 
               },
             'AimsData<int32_t>' : \
             { 'typecode' : 'AimsData_S32',
               'pyFromC' : '',
               'CFromPy' : '',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new AimsData<int32_t>', 
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'AimsData_S32',
               'sipClass' : 'AimsData_S32',
               'typeinclude' : '#include <aims/data/data.h>', 
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipAimsData_S32.h"\n'
               '#endif\n'
               '#ifndef PYAIMS_AIMSDATA_S32_CHECK_DEFINED\n'
               '#define PYAIMS_AIMSDATA_S32_CHECK_DEFINED\n'
               'inline int pyaimsAimsData_S32_Check( PyObject* o )\n'
               '{ return sipCanConvertToInstance( o, sipClass_AimsData_S32, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsAimsData_S32_Check', 
               },
             'AimsData<uint32_t>' : \
             { 'typecode' : 'AimsData_U32',
               'pyFromC' : '',
               'CFromPy' : '',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new AimsData<uint32_t>', 
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'AimsData_U32',
               'sipClass' : 'AimsData_U32',
               'typeinclude' : '#include <aims/data/data.h>', 
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipAimsData_U32.h"\n'
               '#endif\n'
               '#ifndef PYAIMS_AIMSDATA_U32_CHECK_DEFINED\n'
               '#define PYAIMS_AIMSDATA_U32_CHECK_DEFINED\n'
               'inline int pyaimsAimsData_U32_Check( PyObject* o )\n'
               '{ return sipCanConvertToInstance( o, sipClass_AimsData_U32, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsAimsData_U32_Check', 
               },
             'AimsData<float>' : \
             { 'typecode' : 'AimsData_FLOAT',
               'pyFromC' : '',
               'CFromPy' : '',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new AimsData<float>', 
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'AimsData_FLOAT',
               'sipClass' : 'AimsData_FLOAT',
               'typeinclude' : '#include <aims/data/data.h>', 
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipAimsData_FLOAT.h"\n'
               '#endif\n'
               '#ifndef PYAIMS_AIMSDATA_FLOAT_CHECK_DEFINED\n'
               '#define PYAIMS_AIMSDATA_FLOAT_CHECK_DEFINED\n'
               'inline int pyaimsAimsData_FLOAT_Check( PyObject* o )\n'
               '{ return sipCanConvertToInstance( o, sipClass_AimsData_FLOAT, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsAimsData_FLOAT_Check', 
               },
             'AimsData<double>' : \
             { 'typecode' : 'AimsData_DOUBLE',
               'pyFromC' : '',
               'CFromPy' : '',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new AimsData<double>', 
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'AimsData_DOUBLE',
               'sipClass' : 'AimsData_DOUBLE',
               'typeinclude' : '#include <aims/data/data.h>', 
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipAimsData_DOUBLE.h"\n'
               '#endif\n'
               '#ifndef PYAIMS_AIMSDATA_DOUBLE_CHECK_DEFINED\n'
               '#define PYAIMS_AIMSDATA_DOUBLE_CHECK_DEFINED\n'
               'inline int pyaimsAimsData_DOUBLE_Check( PyObject* o )\n'
               '{ return sipCanConvertToInstance( o, sipClass_AimsData_DOUBLE, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsAimsData_DOUBLE_Check', 
               },
             'AimsData<AimsRGB>' : \
             { 'typecode' : 'AimsData_RGB',
               'pyFromC' : '',
               'CFromPy' : '',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new AimsData<AimsRGB>', 
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'AimsData_RGB',
               'sipClass' : 'AimsData_RGB',
               'typeinclude' : \
               '#include <aims/data/data.h>\n#include <aims/rgb/rgb.h>', 
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipAimsData_RGB.h"\n'
               '#endif\n'
               '#ifndef PYAIMS_AIMSDATA_RGB_CHECK_DEFINED\n'
               '#define PYAIMS_AIMSDATA_RGB_CHECK_DEFINED\n'
               'inline int pyaimsAimsData_RGB_Check( PyObject* o )\n'
               '{ return sipCanConvertToInstance( o, sipClass_AimsData_RGB, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsAimsData_RGB_Check', 
               },
             'AimsData<AimsRGBA>' : \
             { 'typecode' : 'AimsData_RGBA',
               'pyFromC' : '',
               'CFromPy' : '',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new AimsData<AimsRGBA>', 
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'AimsData_RGBA',
               'sipClass' : 'AimsData_RGBA',
               'typeinclude' : \
               '#include <aims/data/data.h>\n#include <aims/rgb/rgb.h>', 
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipAimsData_RGBA.h"\n'
               '#endif\n'
               '#ifndef PYAIMS_AIMSDATA_RGBA_CHECK_DEFINED\n'
               '#define PYAIMS_AIMSDATA_RGBA_CHECK_DEFINED\n'
               'inline int pyaimsAimsData_RGBA_Check( PyObject* o )\n'
               '{ return sipCanConvertToInstance( o, sipClass_AimsData_RGBA, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsAimsData_RGBA_Check', 
               },

             'carto::Volume<int8_t>' : \
             { 'typecode' : 'Volume_S8',
               'pyFromC' : '',
               'CFromPy' : '',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new carto::Volumea<int8_t>', 
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'Volume_S8',
               'sipClass' : 'Volume_S8',
               'typeinclude' : '#include <cartodata/volume/volume.h>', 
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipVolume_S8.h"\n'
               '#endif\n'
               '#ifndef PYAIMS_VOLUME_S8_CHECK_DEFINED\n'
               '#define PYAIMS_VOLUME_S8_CHECK_DEFINED\n'
               'inline int pyaimsVolume_S8_Check( PyObject* o )\n'
               '{ return sipCanConvertToInstance( o, sipClass_Volume_S8, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsVolume_S8_Check', 
               },
             'carto::Volume<uint8_t>' : \
             { 'typecode' : 'Volume_U8',
               'pyFromC' : '',
               'CFromPy' : '',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new carto::Volumea<uint8_t>', 
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'Volume_U8',
               'sipClass' : 'Volume_U8',
               'typeinclude' : '#include <cartodata/volume/volume.h>', 
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipVolume_U8.h"\n'
               '#endif\n'
               '#ifndef PYAIMS_VOLUME_U8_CHECK_DEFINED\n'
               '#define PYAIMS_VOLUME_U8_CHECK_DEFINED\n'
               'inline int pyaimsVolume_U8_Check( PyObject* o )\n'
               '{ return sipCanConvertToInstance( o, sipClass_Volume_U8, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsVolume_U8_Check', 
               },
             'carto::Volume<int16_t>' : \
             { 'typecode' : 'Volume_S16',
               'pyFromC' : 'sipConvertFrom_Volume_S16',
               'CFromPy' : 'pyaimsConvertTo_Volume_S16',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new carto::Volumea<int16_t>', 
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'Volume_S16',
               'sipClass' : 'Volume_S16',
               'typeinclude' : '#include <cartodata/volume/volume.h>', 
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipVolume_S16.h"\n'
               '#endif\n'
               '#ifndef PYAIMS_VOLUME_S16_CHECK_DEFINED\n'
               '#define PYAIMS_VOLUME_S16_CHECK_DEFINED\n'
               'inline int pyaimsVolume_S16_Check( PyObject* o )\n'
               '{ return sipCanConvertToInstance( o, sipClass_Volume_S16, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsVolume_S16_Check', 
               },
             'carto::Volume<uint16_t>' : \
             { 'typecode' : 'Volume_U16',
               'pyFromC' : 'sipConvertFrom_Volume_U16',
               'CFromPy' : '',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new carto::Volumea<uint16_t>', 
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'Volume_U16',
               'sipClass' : 'Volume_U16',
               'typeinclude' : '#include <cartodata/volume/volume.h>', 
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipVolume_U16.h"\n'
               '#endif\n'
               '#ifndef PYAIMS_VOLUME_U16_CHECK_DEFINED\n'
               '#define PYAIMS_VOLUME_U16_CHECK_DEFINED\n'
               'inline int pyaimsVolume_U16_Check( PyObject* o )\n'
               '{ return sipCanConvertToInstance( o, sipClass_Volume_U16, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsVolume_U16_Check', 
               },
             'carto::Volume<int32_t>' : \
             { 'typecode' : 'Volume_S32',
               'pyFromC' : '',
               'CFromPy' : '',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new carto::Volumea<int32_t>', 
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'Volume_S32',
               'sipClass' : 'Volume_S32',
               'typeinclude' : '#include <cartodata/volume/volume.h>', 
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipVolume_S32.h"\n'
               '#endif\n'
               '#ifndef PYAIMS_VOLUME_S32_CHECK_DEFINED\n'
               '#define PYAIMS_VOLUME_S32_CHECK_DEFINED\n'
               'inline int pyaimsVolume_S32_Check( PyObject* o )\n'
               '{ return sipCanConvertToInstance( o, sipClass_Volume_S32, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsVolume_S32_Check', 
               },
             'carto::Volume<uint32_t>' : \
             { 'typecode' : 'Volume_U32',
               'pyFromC' : '',
               'CFromPy' : '',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new carto::Volumea<uint32_t>', 
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'Volume_U32',
               'sipClass' : 'Volume_U32',
               'typeinclude' : '#include <cartodata/volume/volume.h>', 
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipVolume_U32.h"\n'
               '#endif\n'
               '#ifndef PYAIMS_VOLUME_U32_CHECK_DEFINED\n'
               '#define PYAIMS_VOLUME_U32_CHECK_DEFINED\n'
               'inline int pyaimsVolume_U32_Check( PyObject* o )\n'
               '{ return sipCanConvertToInstance( o, sipClass_Volume_U32, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsVolume_U32_Check', 
               },
             'carto::Volume<float>' : \
             { 'typecode' : 'Volume_FLOAT',
               'pyFromC' : '',
               'CFromPy' : '',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new carto::Volumea<float>', 
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'Volume_FLOAT',
               'sipClass' : 'Volume_FLOAT',
               'typeinclude' : '#include <cartodata/volume/volume.h>', 
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipVolume_FLOAT.h"\n'
               '#endif\n'
               '#ifndef PYAIMS_VOLUME_FLOAT_CHECK_DEFINED\n'
               '#define PYAIMS_VOLUME_FLOAT_CHECK_DEFINED\n'
               'inline int pyaimsVolume_FLOAT_Check( PyObject* o )\n'
               '{ return sipCanConvertToInstance( o, sipClass_Volume_FLOAT, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsVolume_FLOAT_Check', 
               },
             'carto::Volume<double>' : \
             { 'typecode' : 'Volume_DOUBLE',
               'pyFromC' : '',
               'CFromPy' : '',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new carto::Volumea<double>', 
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'Volume_DOUBLE',
               'sipClass' : 'Volume_DOUBLE',
               'typeinclude' : '#include <cartodata/volume/volume.h>', 
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipVolume_DOUBLE.h"\n'
               '#endif\n'
               '#ifndef PYAIMS_VOLUME_DOUBLE_CHECK_DEFINED\n'
               '#define PYAIMS_VOLUME_DOUBLE_CHECK_DEFINED\n'
               'inline int pyaimsVolume_DOUBLE_Check( PyObject* o )\n'
               '{ return sipCanConvertToInstance( o, sipClass_Volume_DOUBLE, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsVolume_DOUBLE_Check', 
               },
             'carto::Volume<AimsRGB>' : \
             { 'typecode' : 'Volume_RGB',
               'pyFromC' : '',
               'CFromPy' : '',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new carto::Volumea<AimsRGB>', 
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'Volume_RGB',
               'sipClass' : 'Volume_RGB',
               'typeinclude' : \
               '#include <cartodata/volume/volume.h>\n' \
               '#include <aims/rgb/rgb.h>', 
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipVolume_RGB.h"\n'
               '#endif\n'
               '#ifndef PYAIMS_VOLUME_RGB_CHECK_DEFINED\n'
               '#define PYAIMS_VOLUME_RGB_CHECK_DEFINED\n'
               'inline int pyaimsVolume_RGB_Check( PyObject* o )\n'
               '{ return sipCanConvertToInstance( o, sipClass_Volume_RGB, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsVolume_RGB_Check', 
               },
             'carto::Volume<AimsRGBA>' : \
             { 'typecode' : 'Volume_RGBA',
               'pyFromC' : '',
               'CFromPy' : '',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new carto::Volumea<AimsRGBA>', 
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'Volume_RGBA',
               'sipClass' : 'Volume_RGBA',
               'typeinclude' : \
               '#include <cartodata/volume/volume.h>\n' \
               '#include <aims/rgb/rgb.h>', 
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipVolume_RGBA.h"\n'
               '#endif\n'
               '#ifndef PYAIMS_VOLUME_RGBA_CHECK_DEFINED\n'
               '#define PYAIMS_VOLUME_RGBA_CHECK_DEFINED\n'
               'inline int pyaimsVolume_RGBA_Check( PyObject* o )\n'
               '{ return sipCanConvertToInstance( o, sipClass_Volume_RGBA, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsVolume_RGBA_Check', 
               },

             '1' : \
             { 'typecode' : '1',
               'pyFromC' : '',
               'CFromPy' : '',
               'castFromSip' : '',
               'deref' : '',
               'pyderef' : '',
               'address' : '', 
               'pyaddress' : '', 
               'defScalar' : '',
               'new' : '', 
               'NumType' : '', 
               'PyType' : '',
               'sipClass' : '',
               'typeinclude' : '', 
               'sipinclude' : '', 
               },
             '2' : \
             { 'typecode' : '2',
               'pyFromC' : '',
               'CFromPy' : '',
               'castFromSip' : '',
               'deref' : '',
               'pyderef' : '',
               'address' : '', 
               'pyaddress' : '', 
               'defScalar' : '',
               'new' : '', 
               'NumType' : '', 
               'PyType' : '',
               'sipClass' : '',
               'typeinclude' : '', 
               'sipinclude' : '', 
               },
             '3' : \
             { 'typecode' : '3',
               'pyFromC' : '',
               'CFromPy' : '',
               'castFromSip' : '',
               'deref' : '',
               'pyderef' : '',
               'address' : '', 
               'pyaddress' : '', 
               'defScalar' : '',
               'new' : '', 
               'NumType' : '', 
               'PyType' : '',
               'sipClass' : '',
               'typeinclude' : '', 
               'sipinclude' : '', 
               },
             '4' : \
             { 'typecode' : '4',
               'pyFromC' : '',
               'CFromPy' : '',
               'castFromSip' : '',
               'deref' : '',
               'pyderef' : '',
               'address' : '', 
               'pyaddress' : '', 
               'defScalar' : '',
               'new' : '', 
               'NumType' : '', 
               'PyType' : '',
               'sipClass' : '',
               'typeinclude' : '', 
               'sipinclude' : '', 
               },

             'AimsTimeSurface<2,Void>' : \
             { 'typecode' : 'AimsTimeSurface_2',
               'pyFromC' : '',
               'CFromPy' : '',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new AimsTimeSurface<2,Void>', 
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'AimsTimeSurface_2',
               'sipClass' : 'AimsTimeSurface_2',
               'typeinclude' : \
               '#include <aims/mesh/surface.h>', 
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipAimsTimeSurface_2.h"\n'
               '#endif\n'
               '#ifndef PYAIMS_TIMESURFACE_2_CHECK_DEFINED\n'
               '#define PYAIMS_TIMESURFACE_2_CHECK_DEFINED\n'
               'inline int pyaimsTimeSurface_2_Check( PyObject* o )\n'
               '{ return sipCanConvertToInstance( o, sipClass_AimsTimeSurface_2, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsTimeSurface_2_Check', 
               },
             'AimsTimeSurface<3,Void>' : \
             { 'typecode' : 'AimsTimeSurface_3',
               'pyFromC' : '',
               'CFromPy' : '',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new AimsTimeSurface<3,Void>', 
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'AimsTimeSurface_3',
               'sipClass' : 'AimsTimeSurface_3',
               'typeinclude' : \
               '#include <aims/mesh/surface.h>', 
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipAimsTimeSurface_3.h"\n'
               '#endif\n'
               '#ifndef PYAIMS_TIMESURFACE_3_CHECK_DEFINED\n'
               '#define PYAIMS_TIMESURFACE_3_CHECK_DEFINED\n'
               'inline int pyaimsTimeSurface_3_Check( PyObject* o )\n'
               '{ return sipCanConvertToInstance( o, sipClass_AimsTimeSurface_3, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsTimeSurface_3_Check', 
               },
             'AimsTimeSurface<4,Void>' : \
             { 'typecode' : 'AimsTimeSurface_4',
               'pyFromC' : '',
               'CFromPy' : '',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new AimsTimeSurface<4,Void>', 
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'AimsTimeSurface_4',
               'sipClass' : 'AimsTimeSurface_4',
               'typeinclude' : \
               '#include <aims/mesh/surface.h>', 
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipAimsTimeSurface_4.h"\n'
               '#endif\n'
               '#ifndef PYAIMS_TIMESURFACE_4_CHECK_DEFINED\n'
               '#define PYAIMS_TIMESURFACE_4_CHECK_DEFINED\n'
               'inline int pyaimsTimeSurface_4_Check( PyObject* o )\n'
               '{ return sipCanConvertToInstance( o, sipClass_AimsTimeSurface_4, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsTimeSurface_4_Check', 
               },

             'Texture<float>' : \
             { 'typecode' : 'Texture_FLOAT',
               'pyFromC' : '',
               'CFromPy' : '',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new Texture<float>', 
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'Texture_FLOAT',
               'sipClass' : 'Texture_FLOAT',
               'typeinclude' : \
               '#include <aims/mesh/texture.h>', 
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipTexture_FLOAT.h"\n'
               '#endif\n'
               '#ifndef PYAIMS_TEXTURE_FLOAT_CHECK_DEFINED\n'
               '#define PYAIMS_TEXTURE_FLOAT_CHECK_DEFINED\n'
               'inline int pyaimsTexture_FLOAT_Check( PyObject* o )\n'
               '{ return sipCanConvertToInstance( o, sipClass_Texture_FLOAT, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsTexture_FLOAT_Check', 
               },
             'Texture<int16_t>' : \
             { 'typecode' : 'Texture_S16',
               'pyFromC' : '',
               'CFromPy' : '',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new Texture<int16_t>', 
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'Texture_S16',
               'sipClass' : 'Texture_S16',
               'typeinclude' : \
               '#include <aims/mesh/texture.h>', 
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipTexture_S16.h"\n'
               '#endif\n'
               '#ifndef PYAIMS_TEXTURE_S16_CHECK_DEFINED\n'
               '#define PYAIMS_TEXTURE_S16_CHECK_DEFINED\n'
               'inline int pyaimsTexture_S16_Check( PyObject* o )\n'
               '{ return sipCanConvertToInstance( o, sipClass_Texture_S16, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsTexture_S16_Check', 
               },
             'Texture<int32_t>' : \
             { 'typecode' : 'Texture_S32',
               'pyFromC' : '',
               'CFromPy' : '',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new Texture<int32_t>', 
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'Texture_S32',
               'sipClass' : 'Texture_S32',
               'typeinclude' : \
               '#include <aims/mesh/texture.h>', 
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipTexture_S32.h"\n'
               '#endif\n'
               '#ifndef PYAIMS_TEXTURE_S32_CHECK_DEFINED\n'
               '#define PYAIMS_TEXTURE_S32_CHECK_DEFINED\n'
               'inline int pyaimsTexture_S32_Check( PyObject* o )\n'
               '{ return sipCanConvertToInstance( o, sipClass_Texture_S32, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsTexture_FLOAT_Check', 
               },
             'Texture<uint32_t>' : \
             { 'typecode' : 'Texture_U32',
               'pyFromC' : '',
               'CFromPy' : '',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new Texture<uint32_t>', 
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'Texture_U32',
               'sipClass' : 'Texture_U32',
               'typeinclude' : \
               '#include <aims/mesh/texture.h>', 
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipTexture_U32.h"\n'
               '#endif\n'
               '#ifndef PYAIMS_TEXTURE_U32_CHECK_DEFINED\n'
               '#define PYAIMS_TEXTURE_U32_CHECK_DEFINED\n'
               'inline int pyaimsTexture_U32_Check( PyObject* o )\n'
               '{ return sipCanConvertToInstance( o, sipClass_Texture_U32, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsTexture_FLOAT_Check', 
               },
             'Texture<Point2df>' : \
             { 'typecode' : 'Texture_POINT2DF',
               'pyFromC' : '',
               'CFromPy' : '',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new Texture<Point2df>', 
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'Texture_POINT2DF',
               'sipClass' : 'Texture_POINT2DF',
               'typeinclude' : \
               '#include <aims/mesh/texture.h>\n' \
               '#include <aims/vector/vector.h>', 
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipTexture_POINT2DF.h"\n'
               '#endif\n'
               '#ifndef PYAIMS_TEXTURE_POINT2DF_CHECK_DEFINED\n'
               '#define PYAIMS_TEXTURE_POINT2DF_CHECK_DEFINED\n'
               'inline int pyaimsTexture_POINT2DF_Check( PyObject* o )\n'
               '{ return sipCanConvertToInstance( o, sipClass_Texture_POINT2DF, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsTexture_POINT2DF_Check', 
               },

             'TimeTexture<float>' : \
             { 'typecode' : 'TimeTexture_FLOAT',
               'pyFromC' : '',
               'CFromPy' : '',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new TimeTexture<float>', 
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'TimeTexture_FLOAT',
               'sipClass' : 'TimeTexture_FLOAT',
               'typeinclude' : \
               '#include <aims/mesh/texture.h>', 
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipTimeTexture_FLOAT.h"\n'
               '#endif\n'
               '#ifndef PYAIMS_TIMETEXTURE_FLOAT_CHECK_DEFINED\n'
               '#define PYAIMS_TIMETEXTURE_FLOAT_CHECK_DEFINED\n'
               'inline int pyaimsTimeTexture_FLOAT_Check( PyObject* o )\n'
               '{ return sipCanConvertToInstance( o, sipClass_TimeTexture_FLOAT, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsTimeTexture_FLOAT_Check', 
               },
             'TimeTexture<int16_t>' : \
             { 'typecode' : 'TimeTexture_S16',
               'pyFromC' : '',
               'CFromPy' : '',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new TimeTexture<int16_t>', 
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'TimeTexture_S16',
               'sipClass' : 'TimeTexture_S16',
               'typeinclude' : \
               '#include <aims/mesh/texture.h>', 
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipTimeTexture_S16.h"\n'
               '#endif\n'
               '#ifndef PYAIMS_TIMETEXTURE_S16_CHECK_DEFINED\n'
               '#define PYAIMS_TIMETEXTURE_S16_CHECK_DEFINED\n'
               'inline int pyaimsTimeTexture_S16_Check( PyObject* o )\n'
               '{ return sipCanConvertToInstance( o, sipClass_TimeTexture_S16, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsTimeTexture_S16_Check', 
               },
             'TimeTexture<int32_t>' : \
             { 'typecode' : 'TimeTexture_S32',
               'pyFromC' : '',
               'CFromPy' : '',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new TimeTexture<int32_t>', 
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'TimeTexture_S32',
               'sipClass' : 'TimeTexture_S32',
               'typeinclude' : \
               '#include <aims/mesh/texture.h>', 
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipTimeTexture_S32.h"\n'
               '#endif\n'
               '#ifndef PYAIMS_TIMETEXTURE_S32_CHECK_DEFINED\n'
               '#define PYAIMS_TIMETEXTURE_S32_CHECK_DEFINED\n'
               'inline int pyaimsTimeTexture_S32_Check( PyObject* o )\n'
               '{ return sipCanConvertToInstance( o, sipClass_TimeTexture_S32, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsTimeTexture_S32_Check', 
               },
             'TimeTexture<uint32_t>' : \
             { 'typecode' : 'TimeTexture_U32',
               'pyFromC' : '',
               'CFromPy' : '',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new TimeTexture<uint32_t>', 
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'TimeTexture_U32',
               'sipClass' : 'TimeTexture_U32',
               'typeinclude' : \
               '#include <aims/mesh/texture.h>', 
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipTimeTexture_U32.h"\n'
               '#endif\n'
               '#ifndef PYAIMS_TIMETEXTURE_U32_CHECK_DEFINED\n'
               '#define PYAIMS_TIMETEXTURE_U32_CHECK_DEFINED\n'
               'inline int pyaimsTimeTexture_U32_Check( PyObject* o )\n'
               '{ return sipCanConvertToInstance( o, sipClass_TimeTexture_U32, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsTimeTexture_U32_Check', 
               },
             'TimeTexture<Point2df>' : \
             { 'typecode' : 'TimeTexture_POINT2DF',
               'pyFromC' : '',
               'CFromPy' : '',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new TimeTexture<Point2df>', 
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'TimeTexture_POINT2DF',
               'sipClass' : 'TimeTexture_POINT2DF',
               'typeinclude' : \
               '#include <aims/mesh/texture.h>\n' \
               '#include <aims/vector/vector.h>', 
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipTimeTexture_POINT2DF.h"\n'
               '#endif\n'
               '#ifndef PYAIMS_TIMETEXTURE_POINT2DF_CHECK_DEFINED\n'
               '#define PYAIMS_TIMETEXTURE_POINT2DF_CHECK_DEFINED\n'
               'inline int pyaimsTimeTexture_POINT2DF_Check( PyObject* o )\n'
               '{ return sipCanConvertToInstance( o, sipClass_TimeTexture_POINT2DF, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsTimeTexture_POINT2DF_Check', 
               },

             'aims::BucketMap<Void>' : \
             { 'typecode' : 'BucketMap_VOID',
               'pyFromC' : '',
               'CFromPy' : '',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new aims::BucketMap<Void>', 
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'BucketMap_VOID',
               'sipClass' : 'BucketMap_VOID',
               'typeinclude' : \
               '#include <aims/bucket/bucket.h>\n' \
               '#include <cartobase/type/types.h>', 
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipBucketMap_VOID.h"\n'
               '#endif\n'
               '#ifndef PYAIMS_BUCKETMAP_VOID_CHECK_DEFINED\n'
               '#define PYAIMS_BUCKETMAP_VOID_CHECK_DEFINED\n'
               'inline int pyaimsBucketMap_VOID_Check( PyObject* o )\n'
               '{ return sipCanConvertToInstance( o, sipClass_BucketMap_VOID, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsBucketMap_VOID_Check', 
               },

             'aims::BucketMap<int16_t>' : \
             { 'typecode' : 'BucketMap_S16',
               'pyFromC' : '',
               'CFromPy' : '',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new aims::BucketMap<short>', 
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'BucketMap_S16',
               'sipClass' : 'BucketMap_S16',
               'typeinclude' : \
               '#include <aims/bucket/bucket.h>\n' \
               '#include <cartobase/type/types.h>', 
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipBucketMap_S16.h"\n'
               '#endif\n'
               '#ifndef PYAIMS_BUCKETMAP_S16_CHECK_DEFINED\n'
               '#define PYAIMS_BUCKETMAP_S16_CHECK_DEFINED\n'
               'inline int pyaimsBucketMap_S16_Check( PyObject* o )\n'
               '{ return sipCanConvertToInstance( o, sipClass_BucketMap_S16, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsBucketMap_S16_Check', 
               },


             'aims::BucketMap<float>' : \
             { 'typecode' : 'BucketMap_FLOAT',
               'pyFromC' : '',
               'CFromPy' : '',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new aims::BucketMap<float>', 
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'BucketMap_FLOAT',
               'sipClass' : 'BucketMap_FLOAT',
               'typeinclude' : \
               '#include <aims/bucket/bucket.h>\n' \
               '#include <cartobase/type/types.h>', 
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipBucketMap_FLOAT.h"\n'
               '#endif\n'
               '#ifndef PYAIMS_BUCKETMAP_FLOAT_CHECK_DEFINED\n'
               '#define PYAIMS_BUCKETMAP_FLOAT_CHECK_DEFINED\n'
               'inline int pyaimsBucketMap_FLOAT_Check( PyObject* o )\n'
               '{ return sipCanConvertToInstance( o, sipClass_BucketMap_FLOAT, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsBucketMap_FLOAT_Check', 
               },

             'Motion' : \
             { 'typecode' : 'Motion',
               'pyFromC' : '',
               'CFromPy' : '',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new Motion', 
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'Motion',
               'sipClass' : 'Motion',
               'typeinclude' : \
               '#include <aims/resampling/motion.h>', 
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipMotion.h"\n'
               '#endif\n'
               '#ifndef PYAIMS_MOTION_DEFINED\n'
               '#define PYAIMS_MOTION_DEFINED\n'
               'inline int pyaimsMotion_Check( PyObject* o )\n'
               '{ return sipCanConvertToInstance( o, sipClass_Motion, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsMotion_Check', 
               'compareElement' : '',
               },

             'carto::Site' : \
               classInCartoNamespace( 'cartodata/wip/roi/siteIterator.h',
               'Site' ),
             'carto::SiteIterator' : \
               classInCartoNamespace( 'cartodata/wip/roi/siteIterator.h',
               'SiteIterator' ),

             'aims::Interpolator' : \
               classInAimsNamespace( 'aims/resampling/linearInterpolator.h',
               'Interpolator' ),

             'carto::Object' : \
               classInCartoNamespace( 'cartobase/object/object.h',
               'Object' ),

             'Vertex *' : \
             { 'typecode' : 'VertexPtr',
               'pyFromC' : 'pyaimsConvertFrom_VertexP',
               'CFromPy' : 'pyaimsConvertTo_VertexP',
               'castFromSip' : '(Vertex *)',
               'deref' : '',
               'pyderef' : '',
               'address' : '', 
               'pyaddress' : '', 
               'defScalar' : '',
               'new' : 'new Vertex *',
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'Vertex *',
               'sipClass' : 'Vertex *',
               'typeinclude' : '#include <graph/graph/vertex.h>', 
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipVertex.h"\n'
               '#endif\n'
               '#include <pyaims/graph.h>',
               'module' : 'aims',
               'testPyType' : 'pyaimsVertexP_Check',
               'compareElement' : '',
              },

             'Edge *' : \
             { 'typecode' : 'EdgePtr',
               'pyFromC' : 'pyaimsConvertFrom_EdgeP',
               'CFromPy' : 'pyaimsConvertTo_EdgeP',
               'castFromSip' : '(Edge *)',
               'deref' : '',
               'pyderef' : '',
               'address' : '', 
               'pyaddress' : '', 
               'defScalar' : '',
               'new' : 'new Edge *',
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'Edge *',
               'sipClass' : 'Edge *',
               'typeinclude' : '#include <graph/graph/edge.h>', 
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipEdge.h"\n'
               '#endif\n'
               '#include <pyaims/graph.h>',
               'module' : 'aims',
               'testPyType' : 'pyaimsEdgeP_Check',
               'compareElement' : '',
              },

             'carto::Info' : \
               classInCartoNamespace( 'cartobase/config/info.h',
               'Info' ),

             'carto::Info *' : \
             { 'typecode' : 'InfoPtr',
               'pyFromC' : 'pyaimsConvertFrom_InfoP',
               'CFromPy' : 'pyaimsConvertTo_InfoP',
               'castFromSip' : '(carto::Info *)',
               'deref' : '',
               'pyderef' : '',
               'address' : '', 
               'pyaddress' : '', 
               'defScalar' : '',
               'defNoObject' : '#define PYAIMS_NOOBJECT',
               'new' : 'new carto::Info *',
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'carto::Info *',
               'sipClass' : 'carto::Info *',
               'typeinclude' : '#include <cartobase/config/info.h>',
               'sipinclude' : '#if SIP_VERSION < 0x040700\n'
               '#include "sipaimssipInfo.h"\n'
               '#endif\n'
               '#include <pyaims/info.h>',
               'module' : 'aims',
               'testPyType' : 'pyaimsInfoP_Check',
               'compareElement' : '',
              },

             'std::set<unsigned>' : \
             { 'typecode' : 'set_U32',
               'pyFromC' : 'pyaimsConvertFrom_set',
               'CFromPy' : 'pyaimsConvertTo_set<unsigned>',
               'castFromSip' : '',
               'deref' : '*',
               'pyderef' : '*',
               'address' : '&', 
               'pyaddress' : '&', 
               'defScalar' : '',
               'new' : 'new set_U32',
               'NumType' : 'PyArray_OBJECT', 
               'PyType' : 'set_U32',
               'sipClass' : 'set_U32',
               'typeinclude' : '#include <set>',
               'sipinclude' : '#include <pyaims/vector/sipset.h>\n'
               '#ifndef PYAIMS_WRAPPER_SET_U32\n'
               '#define PYAIMS_WRAPPER_SET_U32\n'
               'template <> inline sipWrapperType*\n'
               'sipClass_set<unsigned>()\n'
               '{ return sipClass_set_U32; }\n'
               '#endif', 
               'module' : 'aims', 
               'testPyType' : 'pyaimsCheck_set<unsigned>',
               'compareElement' : '',
               },

             }

completeTypesSub( typessub )
