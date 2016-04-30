# -*- coding: utf-8 -*-

#  This software and supporting documentation are distributed by
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
numtypes = ['unsigned char', 'short', 'unsigned short', 'int',
            'unsigned', 'float', 'double']
basetypes = numtypes + ['AimsRGB', 'AimsRGBA', 'AimsHSV']
matrix = []
for x in basetypes:
    matrix += [(x, y) for y in basetypes]

todo = {'system': ['Void'],
        'volume': basetypes + ['cfloat', 'cdouble', 'Point3df'],
        'aimsdata': basetypes + ['cfloat', 'cdouble', 'Point3df'],
        'vector': ['unsigned char', 'short', 'unsigned short', 'int',
                   'unsigned', 'unsigned long', 'float',
                   'double', 'Point2df', 'Point3df', 'Point3dd', 'Point3d',
                   'std::string', 'AimsVector<unsigned,2>',
                   'AimsVector<unsigned,3>', 'AimsVector<unsigned,4>',
                   'std::set<unsigned>', 'Void'],
        'list':   ['int', 'unsigned', 'float', 'double', 'std::string',
                   'carto::PluginLoader::PluginFile', 'Graph *',
                   'carto::rc_ptr<Graph>', 'Point3d', 'Point3df'],
        'set':    ['short', 'int', 'unsigned', 'float', 'double',
                   'std::string',
                   'Vertex *', 'Edge *'],
        'map':    [('std::string', x)
                   for x in basetypes +
                   ['std::string', 'carto::Semantic']] +
                   [('int', 'std::string'),
                    ('std::string', 'carto::Syntax'),
                    ('unsigned', 'std::set<unsigned>')],
        'rcptr': ['AimsSurfaceTriangle', 'carto::GenericObject',
                  'AimsData<uint8_t>', 'AimsData<int16_t>',
                  'AimsData<uint16_t>', 'AimsData<int32_t>',
                  'AimsData<uint32_t>', 'AimsData<float>',
                  'AimsData<double>', 'AimsData<AimsRGB>',
                  'AimsData<AimsRGBA>', 'AimsData<AimsHSV>',
                  'AimsData<cfloat>',
                  'AimsData<cdouble>', 'AimsData<Point3df>',
                  'carto::Volume<uint8_t>',
                  'carto::Volume<int16_t>', 'carto::Volume<uint16_t>',
                  'carto::Volume<int32_t>', 'carto::Volume<uint32_t>',
                  'carto::Volume<float>', 'carto::Volume<double>',
                  'carto::Volume<cfloat>', 'carto::Volume<cdouble>',
                  'carto::Volume<AimsRGB>', 'carto::Volume<AimsRGBA>',
                  'carto::Volume<AimsHSV>', 'carto::Volume<Point3df>',
                  'AimsTimeSurface<2,Void>',
                  'AimsTimeSurface<3,Void>',
                  'AimsTimeSurface<4,Void>',
                  'AimsTimeSurface<2,float>',
                  'AimsTimeSurface<3,float>',
                  'AimsTimeSurface<4,float>',
                  'AimsTimeSurface<2,Point2df>',
                  'AimsTimeSurface<3,Point2df>',
                  'AimsTimeSurface<4,Point2df>',
                  'TimeTexture<float>', 'TimeTexture<int16_t>',
                  'TimeTexture<int32_t>', 'TimeTexture<uint32_t>',
                  'TimeTexture<Point2df>', 'TimeTexture<double>',
                  'aims::BucketMap<Void>',
                  'aims::BucketMap<int16_t>', 'aims::BucketMap<uint16_t>',
                  'aims::BucketMap<int32_t>', 'aims::BucketMap<uint32_t>',
                  'aims::BucketMap<float>', 'aims::BucketMap<double>',
                  'aims::AffineTransformation3d',
                  'carto::Site',
                  'carto::SiteIterator',
                  'aims::Interpolator',
                  'Graph', 'Tree', 'aims::SparseMatrix',
                  'aims::SparseOrDenseMatrix',
                  'soma::DataSource',
                  'soma::DataSourceInfo',
                  ],
        'converter_Volume': matrix,
        'converter': [('AimsData<uint8_t>', 'aims::BucketMap<Void>'),
                      ('AimsData<uint8_t>', 'aims::BucketMap<int16_t>'),
                      ('AimsData<uint8_t>', 'aims::BucketMap<uint16_t>'),
                      ('AimsData<uint8_t>', 'aims::BucketMap<int32_t>'),
                      ('AimsData<uint8_t>', 'aims::BucketMap<uint32_t>'),
                      ('AimsData<uint8_t>', 'aims::BucketMap<float>'),
                      ('AimsData<uint8_t>', 'aims::BucketMap<double>'),
                      ('AimsData<uint16_t>', 'aims::BucketMap<Void>'),
                      ('AimsData<uint16_t>', 'aims::BucketMap<int16_t>'),
                      ('AimsData<uint16_t>', 'aims::BucketMap<uint16_t>'),
                      ('AimsData<uint16_t>', 'aims::BucketMap<int32_t>'),
                      ('AimsData<uint16_t>', 'aims::BucketMap<uint32_t>'),
                      ('AimsData<uint16_t>', 'aims::BucketMap<float>'),
                      ('AimsData<uint16_t>', 'aims::BucketMap<double>'),
                      ('AimsData<int16_t>', 'aims::BucketMap<Void>'),
                      ('AimsData<int16_t>', 'aims::BucketMap<int16_t>'),
                      ('AimsData<int16_t>', 'aims::BucketMap<uint16_t>'),
                      ('AimsData<int16_t>', 'aims::BucketMap<int32_t>'),
                      ('AimsData<int16_t>', 'aims::BucketMap<uint32_t>'),
                      ('AimsData<int16_t>', 'aims::BucketMap<float>'),
                      ('AimsData<int16_t>', 'aims::BucketMap<double>'),
                      ('AimsData<uint32_t>', 'aims::BucketMap<Void>'),
                      ('AimsData<uint32_t>', 'aims::BucketMap<int16_t>'),
                      ('AimsData<uint32_t>', 'aims::BucketMap<uint16_t>'),
                      ('AimsData<uint32_t>', 'aims::BucketMap<int32_t>'),
                      ('AimsData<uint32_t>', 'aims::BucketMap<uint32_t>'),
                      ('AimsData<uint32_t>', 'aims::BucketMap<float>'),
                      ('AimsData<uint32_t>', 'aims::BucketMap<double>'),
                      ('AimsData<int32_t>', 'aims::BucketMap<Void>'),
                      ('AimsData<int32_t>', 'aims::BucketMap<int16_t>'),
                      ('AimsData<int32_t>', 'aims::BucketMap<uint16_t>'),
                      ('AimsData<int32_t>', 'aims::BucketMap<int32_t>'),
                      ('AimsData<int32_t>', 'aims::BucketMap<uint32_t>'),
                      ('AimsData<int32_t>', 'aims::BucketMap<float>'),
                      ('AimsData<int32_t>', 'aims::BucketMap<double>'),
                      ('AimsData<float>', 'aims::BucketMap<Void>'),
                      ('AimsData<float>', 'aims::BucketMap<int16_t>'),
                      ('AimsData<float>', 'aims::BucketMap<uint16_t>'),
                      ('AimsData<float>', 'aims::BucketMap<int32_t>'),
                      ('AimsData<float>', 'aims::BucketMap<uint32_t>'),
                      ('AimsData<float>', 'aims::BucketMap<float>'),
                      ('AimsData<float>', 'aims::BucketMap<double>'),
                      ('AimsData<double>', 'aims::BucketMap<Void>'),
                      ('AimsData<double>', 'aims::BucketMap<int16_t>'),
                      ('AimsData<double>', 'aims::BucketMap<uint16_t>'),
                      ('AimsData<double>', 'aims::BucketMap<int32_t>'),
                      ('AimsData<double>', 'aims::BucketMap<uint32_t>'),
                      ('AimsData<double>', 'aims::BucketMap<float>'),
                      ('AimsData<double>', 'aims::BucketMap<double>'),
                      ('aims::BucketMap<Void>', 'AimsData<uint8_t>'),
                      ('aims::BucketMap<Void>', 'AimsData<uint16_t>'),
                      ('aims::BucketMap<Void>', 'AimsData<int16_t>'),
                      ('aims::BucketMap<Void>', 'AimsData<uint32_t>'),
                      ('aims::BucketMap<Void>', 'AimsData<int32_t>'),
                      ('aims::BucketMap<Void>', 'AimsData<float>'),
                      ('aims::BucketMap<Void>', 'AimsData<double>'),
                      ('aims::BucketMap<int16_t>', 'AimsData<uint8_t>'),
                      ('aims::BucketMap<int16_t>', 'AimsData<uint16_t>'),
                      ('aims::BucketMap<int16_t>', 'AimsData<int16_t>'),
                      ('aims::BucketMap<int16_t>', 'AimsData<uint32_t>'),
                      ('aims::BucketMap<int16_t>', 'AimsData<int32_t>'),
                      ('aims::BucketMap<int16_t>', 'AimsData<float>'),
                      ('aims::BucketMap<int16_t>', 'AimsData<double>'),
                      ('aims::BucketMap<uint16_t>', 'AimsData<uint8_t>'),
                      ('aims::BucketMap<uint16_t>', 'AimsData<uint16_t>'),
                      ('aims::BucketMap<uint16_t>', 'AimsData<int16_t>'),
                      ('aims::BucketMap<uint16_t>', 'AimsData<uint32_t>'),
                      ('aims::BucketMap<uint16_t>', 'AimsData<int32_t>'),
                      ('aims::BucketMap<uint16_t>', 'AimsData<float>'),
                      ('aims::BucketMap<uint16_t>', 'AimsData<double>'),
                      ('aims::BucketMap<int32_t>', 'AimsData<uint8_t>'),
                      ('aims::BucketMap<int32_t>', 'AimsData<uint16_t>'),
                      ('aims::BucketMap<int32_t>', 'AimsData<int16_t>'),
                      ('aims::BucketMap<int32_t>', 'AimsData<uint32_t>'),
                      ('aims::BucketMap<int32_t>', 'AimsData<int32_t>'),
                      ('aims::BucketMap<int32_t>', 'AimsData<float>'),
                      ('aims::BucketMap<int32_t>', 'AimsData<double>'),
                      ('aims::BucketMap<uint32_t>', 'AimsData<uint8_t>'),
                      ('aims::BucketMap<uint32_t>', 'AimsData<uint16_t>'),
                      ('aims::BucketMap<uint32_t>', 'AimsData<int16_t>'),
                      ('aims::BucketMap<uint32_t>', 'AimsData<uint32_t>'),
                      ('aims::BucketMap<uint32_t>', 'AimsData<int32_t>'),
                      ('aims::BucketMap<uint32_t>', 'AimsData<float>'),
                      ('aims::BucketMap<uint32_t>', 'AimsData<double>'),
                      ('aims::BucketMap<float>', 'AimsData<uint8_t>'),
                      ('aims::BucketMap<float>', 'AimsData<uint16_t>'),
                      ('aims::BucketMap<float>', 'AimsData<int16_t>'),
                      ('aims::BucketMap<float>', 'AimsData<uint32_t>'),
                      ('aims::BucketMap<float>', 'AimsData<int32_t>'),
                      ('aims::BucketMap<float>', 'AimsData<float>'),
                      ('aims::BucketMap<float>', 'AimsData<double>'),
                      ('aims::BucketMap<double>', 'AimsData<uint8_t>'),
                      ('aims::BucketMap<double>', 'AimsData<uint16_t>'),
                      ('aims::BucketMap<double>', 'AimsData<int16_t>'),
                      ('aims::BucketMap<double>', 'AimsData<uint32_t>'),
                      ('aims::BucketMap<double>', 'AimsData<int32_t>'),
                      ('aims::BucketMap<double>', 'AimsData<float>'),
                      ('aims::BucketMap<double>', 'AimsData<double>'),
                      ],
        'rawconverter_bucket': ['short'],
        'reader': ['Graph', 'AimsSurfaceTriangle',
                   'AimsData<uint8_t>', 'AimsData<int16_t>',
                   'AimsData<uint16_t>', 'AimsData<int32_t>',
                   'AimsData<uint32_t>', 'AimsData<float>',
                   'AimsData<double>', 'AimsData<AimsRGB>',
                   'AimsData<AimsRGBA>', 'AimsData<AimsHSV>',
                   'AimsData<cfloat>',
                   'AimsData<cdouble>', 'AimsData<Point3df>',
                   'carto::Volume<uint8_t>', 'carto::Volume<int16_t>',
                   'carto::Volume<uint16_t>', 'carto::Volume<int32_t>',
                   'carto::Volume<uint32_t>', 'carto::Volume<float>',
                   'carto::Volume<double>', 'carto::Volume<AimsRGB>',
                   'carto::Volume<AimsRGBA>', 'carto::Volume<AimsHSV>',
                   'carto::Volume<cfloat>', 'carto::Volume<cdouble>',
                   'carto::Volume<Point3df>',
                   'AimsTimeSurface<2,Void>',
                   'AimsTimeSurface<3,Void>',
                   'AimsTimeSurface<4,Void>',
                   'AimsTimeSurface<2,float>',
                   'AimsTimeSurface<3,float>',
                   'AimsTimeSurface<4,float>',
                   'AimsTimeSurface<2,Point2df>',
                   'AimsTimeSurface<3,Point2df>',
                   'AimsTimeSurface<4,Point2df>',
                   'TimeTexture<float>', 'TimeTexture<int16_t>',
                   'TimeTexture<int32_t>', 'TimeTexture<uint32_t>',
                   'TimeTexture<Point2df>', 'TimeTexture<double>',
                   'aims::BucketMap<Void>', 'aims::BucketMap<int16_t>',
                   'aims::BucketMap<uint16_t>', 'aims::BucketMap<int32_t>',
                   'aims::BucketMap<uint32_t>', 'aims::BucketMap<float>',
                   'aims::BucketMap<double>',
                   'aims::AffineTransformation3d',
                   'aims::Hierarchy', 'carto::Object',
                   'aims::SparseMatrix', 'aims::SparseOrDenseMatrix',
                   ],

        'soma_reader': [
                   'carto::Volume<uint8_t>', 'carto::Volume<int16_t>',
                   'carto::Volume<uint16_t>', 'carto::Volume<int32_t>',
                   'carto::Volume<uint32_t>', 'carto::Volume<float>',
                   'carto::Volume<double>', 'carto::Volume<AimsRGB>',
                   'carto::Volume<AimsRGBA>', 'carto::Volume<AimsHSV>',
                   'carto::Volume<cfloat>', 'carto::Volume<cdouble>',
                   'carto::Volume<Point3df>',
                   'carto::Object',
                   ],

        'aimstimesurface': [('2', 'Void'), ('3', 'Void'), ('4', 'Void'),
                            ('2', 'float'), ('3', 'float'), ('4', 'float'),
                            ('2', 'Point2df'), ('3', 'Point2df'),
                            ('4', 'Point2df')],
        'timetexture': ['float', 'short', 'int', 'unsigned', 'Point2df',
                        'double'],
        'bucketmap': ['Void', 'short', 'unsigned short', 'int',
                      'unsigned', 'float', 'double'],
        'aimsvector': [('float', '2'), ('float', '3'), ('float', '4'),
                       ('double', '2'), ('double', '3'),
                       ('double', '4'),
                       ('short', '2'), ('short', '3'), ('short', '4'),
                       ('int', '2'), ('int', '3'), ('int', '4'),
                       ('unsigned', '2'), ('unsigned', '3'),
                       ('unsigned', '4'),
                       ],
        'threshold': [('float', 'short'), ('short', 'short')],
        'merge': [('short', 'short'), ],
        'soma': ['Void'],
        }
