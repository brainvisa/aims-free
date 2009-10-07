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
typessub.update(
				{ 	'Moment<Void>' : \
					{ 'typecode' : 'Moment_VOID',
						'pyFromC' : '',
						'CFromPy' : '',
						'castFromSip' : '',
						'deref' : '*',
						'pyderef' : '*',
						'address' : '&',
						'pyaddress' : '&',
						'defScalar' : '',
						'new' : 'new Moment<Void>',
						'NumType' : 'PyArray_OBJECT',
						'PyType' : 'Moment_VOID',
						'sipClass' : 'Moment_VOID',
						'typeinclude' : \
						'#include <aims/moment/moment.h>',
						'sipinclude' : '#if SIP_VERSION < 0x040700\n'
						'#include "sipaimsalgosipMoment_VOID.h"\n'
						'#endif\n'
						'#ifndef PYAIMSALGO_MOMENT_VOID_DEFINED\n'
						'#define PYAIMSALGO_MOMENT_VOID_DEFINED\n'
						'inline int pyaimsalgoMoment_VOID_Check( PyObject* o )\n'
						'{ return sipCanConvertToInstance( o, sipClass_Moment_VOID, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
						'#endif',
						'module' : 'aimsalgo',
						'testPyType' : 'pyaimsalgoMoment_VOID_Check',
					},
					'Samplable<float,3>' : \
					{ 'typecode' : 'Samplable_FLOAT_3',
						'pyFromC' : '',
						'CFromPy' : '',
						'castFromSip' : '',
						'deref' : '*',
						'pyderef' : '*',
						'address' : '&',
						'pyaddress' : '&',
						'defScalar' : '',
						'new' : '',
						'NumType' : '',
						'PyType' : 'Samplable_FLOAT_3',
						'sipClass' : 'Samplable_FLOAT_3',
						'typeinclude' : \
						'#include <aims/resampling/samplable.h>',
						'sipinclude' : '#if SIP_VERSION < 0x040700\n'
						'#include "sipaimsalgosipSamplable_FLOAT_3.h"\n'
						'#endif\n'
						'#ifndef PYAIMSALGO_SAMPLABLE_FLOAT_3_DEFINED\n'
						'#define PYAIMSALGO_SAMPLABLE_FLOAT_3_DEFINED\n'
						'inline int pyaimsalgoSamplable_FLOAT_3_Check( PyObject* o )\n'
						'{ return sipCanConvertToInstance( o, Samplable_FLOAT_3, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
						'#endif',
						'module' : 'aimsalgo',
						'testPyType' : 'pyaimsalgoSamplable_FLOAT_3_Check',
					},
					'BucketMapSampler<float,3>' : \
					{ 'typecode' : 'BucketMapSampler_FLOAT_3',
						'pyFromC' : '',
						'CFromPy' : '',
						'castFromSip' : '',
						'deref' : '*',
						'pyderef' : '*',
						'address' : '&',
						'pyaddress' : '&',
						'defScalar' : '',
						'new' : '',
						'NumType' : '',
						'PyType' : 'BucketMapSampler_FLOAT_3',
						'sipClass' : 'BucketMapSampler_FLOAT_3',
						'typeinclude' : \
						'#include <aims/resampling/bucketmapsampler.h>',
						'sipinclude' : '#if SIP_VERSION < 0x040700\n'
						'#include "sipaimsalgosipBucketMapSampler_FLOAT_3.h"\n'
						'#endif\n'
						'#ifndef PYAIMSALGO_BUCKETMAPSAMPLER_FLOAT_3_DEFINED\n'
						'#define PYAIMSALGO_BUCKETMAPSAMPLER_FLOAT_3_DEFINED\n'
						'inline int pyaimsalgoBucketMapSampler_FLOAT_3_Check( PyObject* o )\n'
						'{ return sipCanConvertToInstance( o, BucketMapSampler_FLOAT_3, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
						'#endif',
						'module' : 'aimsalgo',
						'testPyType' : 'pyaimsalgoBucketMapSampler_FLOAT_3_Check',
					},
					'GeneralSampler<float,3>' : \
					{ 'typecode' : 'GeneralSampler_FLOAT_3',
						'pyFromC' : '',
						'CFromPy' : '',
						'castFromSip' : '',
						'deref' : '*',
						'pyderef' : '*',
						'address' : '&',
						'pyaddress' : '&',
						'defScalar' : '',
						'new' : '',
						'NumType' : '',
						'PyType' : 'GeneralSampler_FLOAT_3',
						'sipClass' : 'GeneralSampler_FLOAT_3',
						'typeinclude' : \
						'#include <aims/resampling/generalsampler.h>',
						'sipinclude' : '#if SIP_VERSION < 0x040700\n'
						'#include "sipaimsalgosipGeneralSampler_FLOAT_3.h"\n'
						'#endif\n'
						'#ifndef PYAIMSALGO_GENERALSAMPLER_FLOAT_3_DEFINED\n'
						'#define PYAIMSALGO_GENERALSAMPLER_FLOAT_3_DEFINED\n'
						'inline int pyaimsalgoGeneralSampler_FLOAT_3_Check( PyObject* o )\n'
						'{ return sipCanConvertToInstance( o, GeneralSampler_FLOAT_3, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
						'#endif',
						'module' : 'aimsalgo',
						'testPyType' : 'pyaimsalgoGeneralSampler_FLOAT_3_Check',
					},
					'Polynomial<float,3>' : \
					{ 'typecode' : 'Polynomial_FLOAT_3',
						'pyFromC' : '',
						'CFromPy' : '',
						'castFromSip' : '',
						'deref' : '*',
						'pyderef' : '*',
						'address' : '&',
						'pyaddress' : '&',
						'defScalar' : '',
						'new' : '',
						'NumType' : '',
						'PyType' : 'Polynomial_FLOAT_3',
						'sipClass' : 'Polynomial_FLOAT_3',
						'typeinclude' : \
						'#include <aims/resampling/polynomial.h>',
						'sipinclude' : '#if SIP_VERSION < 0x040700\n'
						'#include "sipaimsalgosipPolynomial_FLOAT_3.h"\n'
						'#endif\n'
						'#ifndef PYAIMSALGO_POLYNOMIAL_FLOAT_3_DEFINED\n'
						'#define PYAIMSALGO_POLYNOMIAL_FLOAT_3_DEFINED\n'
						'inline int pyaimsalgoPolynomial_FLOAT_3_Check( PyObject* o )\n'
						'{ return sipCanConvertToInstance( o, Polynomial_FLOAT_3, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
						'#endif',
						'module' : 'aimsalgo',
						'testPyType' : 'pyaimsalgoPolynomial_FLOAT_3_Check',
					},
					'Resampler<int16_t>' : \
					{ 'typecode' : 'Resampler_S16',
						'pyFromC' : '',
						'CFromPy' : '',
						'castFromSip' : '',
						'deref' : '*',
						'pyderef' : '*',
						'address' : '&',
						'pyaddress' : '&',
						'defScalar' : '',
						'new' : '',
						'NumType' : '',
						'PyType' : 'Resampler_S16',
						'sipClass' : 'Resampler_S16',
						'typeinclude' : \
						'#include <aims/resampling/resampler.h>',
						'sipinclude' : '#if SIP_VERSION < 0x040700\n'
						'#include "sipaimsalgosipResampler_S16.h"\n'
						'#endif\n'
						'#ifndef PYAIMSALGO_RESAMPLER_S16_DEFINED\n'
						'#define PYAIMSALGO_RESAMPLER_S16_DEFINED\n'
						'inline int pyaimsalgoResampler_S16_Check( PyObject* o )\n'
						'{ return sipCanConvertToInstance( o, Resampler_S16, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
						'#endif',
						'module' : 'aimsalgo',
						'testPyType' : 'pyaimsalgoResampler_S16_Check',
					},
					'SplineResampler<int16_t>' : \
					{ 'typecode' : 'SplineResampler_S16',
						'pyFromC' : '',
						'CFromPy' : '',
						'castFromSip' : '',
						'deref' : '*',
						'pyderef' : '*',
						'address' : '&',
						'pyaddress' : '&',
						'defScalar' : '',
						'new' : '',
						'NumType' : '',
						'PyType' : 'SplineResampler_S16',
						'sipClass' : 'SplineResampler_S16',
						'typeinclude' : \
						'#include <aims/resampling/splineresampler.h>',
						'sipinclude' : '#if SIP_VERSION < 0x040700\n'
						'#include "sipaimsalgosipSplineResampler_S16.h"\n'
						'#endif\n'
						'#ifndef PYAIMSALGO_SPLINERESAMPLER_S16_DEFINED\n'
						'#define PYAIMSALGO_SPLINERESAMPLER_S16_DEFINED\n'
						'inline int pyaimsalgoSplineResampler_S16_Check( PyObject* o )\n'
						'{ return sipCanConvertToInstance( o, SplineResampler_S16, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
						'#endif',
						'module' : 'aimsalgo',
						'testPyType' : 'pyaimsalgoSplineResampler_S16_Check',
					},
					'MaskLinearResampler<int16_t>' : \
					{ 'typecode' : 'MaskLinearResampler_S16',
						'pyFromC' : '',
						'CFromPy' : '',
						'castFromSip' : '',
						'deref' : '*',
						'pyderef' : '*',
						'address' : '&',
						'pyaddress' : '&',
						'defScalar' : '',
						'new' : '',
						'NumType' : '',
						'PyType' : 'MaskLinearResampler_S16',
						'sipClass' : 'MaskLinearResampler_S16',
						'typeinclude' : \
						'#include <aims/resampling/masklinresampler.h>',
						'sipinclude' : '#if SIP_VERSION < 0x040700\n'
						'#include "sipaimsalgosipMaskLinearResampler_S16.h"\n'
						'#endif\n'
						'#ifndef PYAIMSALGO_MASKLINEARRESAMPLER_S16_DEFINED\n'
						'#define PYAIMSALGO_MASKLINEARRESAMPLER_S16_DEFINED\n'
						'inline int pyaimsalgoMaskLinearResampler_S16_Check( PyObject* o )\n'
						'{ return sipCanConvertToInstance( o, MaskLinearResampler_S16, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
						'#endif',
						'module' : 'aimsalgo',
						'testPyType' : 'pyaimsalgoMaskLinearResampler_S16_Check',
					},
					'NearestNeighborResampler<int16_t>' : \
					{ 'typecode' : 'NearestNeighborResampler_S16',
						'pyFromC' : '',
						'CFromPy' : '',
						'castFromSip' : '',
						'deref' : '*',
						'pyderef' : '*',
						'address' : '&',
						'pyaddress' : '&',
						'defScalar' : '',
						'new' : '',
						'NumType' : '',
						'PyType' : 'NearestNeighborResampler_S16',
						'sipClass' : 'NearestNeighborResampler_S16',
						'typeinclude' : \
						'#include <aims/resampling/nearestneighborresampler.h>',
						'sipinclude' : '#if SIP_VERSION < 0x040700\n'
						'#include "sipaimsalgosipNearestNeighborResampler_S16.h"\n'
						'#endif\n'
						'#ifndef PYAIMSALGO_NEARESTNEIGHBORRESAMPLER_S16_DEFINED\n'
						'#define PYAIMSALGO_NEARESTNEIGHBORRESAMPLER_S16_DEFINED\n'
						'inline int pyaimsalgoNearestNeighborResampler_S16_Check( PyObject* o )\n'
						'{ return sipCanConvertToInstance( o, NearestNeighborResampler_S16, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
						'#endif',
						'module' : 'aimsalgo',
						'testPyType' : 'pyaimsalgoNearestNeighborResampler_S16_Check',
					},
					'CubicResampler<int16_t>' : \
					{ 'typecode' : 'CubicResampler_S16',
						'pyFromC' : '',
						'CFromPy' : '',
						'castFromSip' : '',
						'deref' : '*',
						'pyderef' : '*',
						'address' : '&',
						'pyaddress' : '&',
						'defScalar' : '',
						'new' : '',
						'NumType' : '',
						'PyType' : 'CubicResampler_S16',
						'sipClass' : 'CubicResampler_S16',
						'typeinclude' : \
						'#include <aims/resampling/cubicresampler.h>',
						'sipinclude' : '#if SIP_VERSION < 0x040700\n'
						'#include "sipaimsalgosipCubicResampler_S16.h"\n'
                                                '#endif\n'
						'#ifndef PYAIMSALGO_CUBICRESAMPLER_S16_DEFINED\n'
						'#define PYAIMSALGO_CUBICRESAMPLER_S16_DEFINED\n'
						'inline int pyaimsalgoCubicResampler_S16_Check( PyObject* o )\n'
						'{ return sipCanConvertToInstance( o, CubicResampler_S16, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
						'#endif',
						'module' : 'aimsalgo',
						'testPyType' : 'pyaimsalgoCubicResampler_S16_Check',
					},
					'QuinticResampler<int16_t>' : \
					{ 'typecode' : 'QuinticResampler_S16',
						'pyFromC' : '',
						'CFromPy' : '',
						'castFromSip' : '',
						'deref' : '*',
						'pyderef' : '*',
						'address' : '&',
						'pyaddress' : '&',
						'defScalar' : '',
						'new' : '',
						'NumType' : '',
						'PyType' : 'QuinticResampler_S16',
						'sipClass' : 'QuinticResampler_S16',
						'typeinclude' : \
						'#include <aims/resampling/quinticresampler.h>',
						'sipinclude' : '#if SIP_VERSION < 0x040700\n'
						'#include "sipaimsalgosipQuinticResampler_S16.h"\n'
                                                '#endif\n'
						'#ifndef PYAIMSALGO_QUINTICRESAMPLER_S16_DEFINED\n'
						'#define PYAIMSALGO_QUINTICRESAMPLER_S16_DEFINED\n'
						'inline int pyaimsalgoQuinticResampler_S16_Check( PyObject* o )\n'
						'{ return sipCanConvertToInstance( o, QuinticResampler_S16, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
						'#endif',
						'module' : 'aimsalgo',
						'testPyType' : 'pyaimsalgoQuinticResampler_S16_Check',
					},
					'SixthOrderResampler<int16_t>' : \
					{ 'typecode' : 'SixthOrderResampler_S16',
						'pyFromC' : '',
						'CFromPy' : '',
						'castFromSip' : '',
						'deref' : '*',
						'pyderef' : '*',
						'address' : '&',
						'pyaddress' : '&',
						'defScalar' : '',
						'new' : '',
						'NumType' : '',
						'PyType' : 'SixthOrderResampler_S16',
						'sipClass' : 'SixthOrderResampler_S16',
						'typeinclude' : \
						'#include <aims/resampling/sixthorderresampler.h>',
						'sipinclude' : '#if SIP_VERSION < 0x040700\n'
						'#include "sipaimsalgosipSixthOrderResampler_S16.h"\n'
                                                '#endif\n'
						'#ifndef PYAIMSALGO_SIXTHORDERRESAMPLER_S16_DEFINED\n'
						'#define PYAIMSALGO_SIXTHORDERRESAMPLER_S16_DEFINED\n'
						'inline int pyaimsalgoSixthOrderResampler_S16_Check( PyObject* o )\n'
						'{ return sipCanConvertToInstance( o, SixthOrderResampler_S16, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
						'#endif',
						'module' : 'aimsalgo',
						'testPyType' : 'pyaimsalgoSixthOrderResampler_S16_Check',
					},
					'SeventhOrderResampler<int16_t>' : \
					{ 'typecode' : 'SeventhOrderResampler_S16',
						'pyFromC' : '',
						'CFromPy' : '',
						'castFromSip' : '',
						'deref' : '*',
						'pyderef' : '*',
						'address' : '&',
						'pyaddress' : '&',
						'defScalar' : '',
						'new' : '',
						'NumType' : '',
						'PyType' : 'SeventhOrderResampler_S16',
						'sipClass' : 'SeventhOrderResampler_S16',
						'typeinclude' : \
						'#include <aims/resampling/seventhorderresampler.h>',
						'sipinclude' : '#if SIP_VERSION < 0x040700\n'
						'#include "sipaimsalgosipSeventhOrderResampler_S16.h"\n'
                                                '#endif\n'
						'#ifndef PYAIMSALGO_SEVENTHORDERRESAMPLER_S16_DEFINED\n'
						'#define PYAIMSALGO_SEVENTHORDERRESAMPLER_S16_DEFINED\n'
						'inline int pyaimsalgoSeventhOrderResampler_S16_Check( PyObject* o )\n'
						'{ return sipCanConvertToInstance( o, SeventhOrderResampler_S16, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
						'#endif',
						'module' : 'aimsalgo',
						'testPyType' : 'pyaimsalgoSeventhOrderResampler_S16_Check',
					},
					'LinearResampler<int16_t>' : \
					{ 'typecode' : 'LinearResampler_S16',
						'pyFromC' : '',
						'CFromPy' : '',
						'castFromSip' : '',
						'deref' : '*',
						'pyderef' : '*',
						'address' : '&',
						'pyaddress' : '&',
						'defScalar' : '',
						'new' : '',
						'NumType' : '',
						'PyType' : 'LinearResampler_S16',
						'sipClass' : 'LinearResampler_S16',
						'typeinclude' : \
						'#include <aims/resampling/linearresampler.h>',
						'sipinclude' : '#if SIP_VERSION < 0x040700\n'
						'#include "sipaimsalgosipLinearResampler_S16.h"\n'
                                                '#endif\n'
						'#ifndef PYAIMSALGO_LINEARRESAMPLER_S16_DEFINED\n'
						'#define PYAIMSALGO_LINEARRESAMPLER_S16_DEFINED\n'
						'inline int pyaimsalgoLinearResampler_S16_Check( PyObject* o )\n'
						'{ return sipCanConvertToInstance( o, LinearResampler_S16, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
						'#endif',
						'module' : 'aimsalgo',
						'testPyType' : 'pyaimsalgoLinearResampler_S16_Check',
					},
					'ResamplerFactory<int16_t>' : \
					{ 'typecode' : 'ResamplerFactory_S16',
						'pyFromC' : '',
						'CFromPy' : '',
						'castFromSip' : '',
						'deref' : '*',
						'pyderef' : '*',
						'address' : '&',
						'pyaddress' : '&',
						'defScalar' : '',
						'new' : '',
						'NumType' : '',
						'PyType' : 'ResamplerFactory_S16',
						'sipClass' : 'ResamplerFactory_S16',
						'typeinclude' : \
						'#include <aims/resampling/resamplerfactory.h>',
						'sipinclude' : '#if SIP_VERSION < 0x040700\n'
						'#include "sipaimsalgosipResamplerFactory_S16.h"\n'
                                                '#endif\n'
						'#ifndef PYAIMSALGO_RESAMPLERFACTORY_S16_DEFINED\n'
						'#define PYAIMSALGO_RESAMPLERFACTORY_S16_DEFINED\n'
						'inline int pyaimsalgoResamplerFactory_S16_Check( PyObject* o )\n'
						'{ return sipCanConvertToInstance( o, ResamplerFactory_S16, SIP_NOT_NONE | SIP_NO_CONVERTORS ); }\n'
						'#endif',
						'module' : 'aimsalgo',
						'testPyType' : 'pyaimsalgoResamplerFactory_S16_Check',
					},
		 		}
)
	
completeTypesSub( typessub )
