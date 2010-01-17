
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
numtypes = [ 'unsigned char', 'short', 'unsigned short', 'int',
             'unsigned', 'float', 'double' ]
basetypes = numtypes + [ 'AimsRGB', 'AimsRGBA' ]
matrix = []
for z in [ map( lambda y: (x,y), basetypes ) for x in basetypes ]:
  matrix += z

todo = { 'system' : [ 'Void' ], 
         'volume' : basetypes,
         'aimsdata' : basetypes,
         'vector' : [ 'unsigned char', 'short', 'unsigned short', 'int',
                      'unsigned', 'unsigned long', 'float',
                      'double', 'Point2df', 'Point3df', 'Point3dd', 'Point3d',
                      'std::string', 'AimsVector<unsigned,2>',
                      'AimsVector<unsigned,3>', 'AimsVector<unsigned,4>',
                      'std::set<unsigned>', ],
         'list' :   [ 'int', 'unsigned', 'float', 'double', 'std::string' ],
         'set' :    [ 'short', 'int', 'unsigned', 'float', 'double',
                      'std::string',
                      'Vertex *', 'Edge *' ],
         'map' :    [ ( 'std::string', x ) \
                      for x in basetypes + \
                      [ 'std::string', 'carto::Semantic' ]] +
                      [('int', 'std::string')],
         'rcptr' : [ 'AimsSurfaceTriangle', 'carto::GenericObject',
                     'AimsData<uint8_t>', 'AimsData<int16_t>',
                     'AimsData<uint16_t>', 'AimsData<int32_t>',
                     'AimsData<uint32_t>', 'AimsData<float>',
                     'AimsData<double>', 'AimsData<AimsRGB>',
                     'AimsData<AimsRGBA>',
                     'carto::Volume<uint8_t>', 
                     'carto::Volume<int16_t>', 'carto::Volume<uint16_t>', 
                     'carto::Volume<int32_t>', 'carto::Volume<uint32_t>', 
                     'carto::Volume<float>', 'carto::Volume<double>', 
                     'carto::Volume<AimsRGB>', 'carto::Volume<AimsRGBA>', 
                     'AimsTimeSurface<2,Void>',
                     'AimsTimeSurface<3,Void>', 'AimsTimeSurface<4,Void>',
                     'TimeTexture<float>', 'TimeTexture<int16_t>',
                     'TimeTexture<int32_t>', 'TimeTexture<uint32_t>',
                     'TimeTexture<Point2df>', 'aims::BucketMap<Void>',
                     'aims::BucketMap<int16_t>', 'aims::BucketMap<float>',
                     'Motion',
                     'carto::Site', 
                     'carto::SiteIterator',
                     'aims::Interpolator',
                     'Graph', 'Tree',
                     ], 
         'converter_Volume' : matrix,
         'converter' : [ ( 'AimsData<uint8_t>', 'aims::BucketMap<Void>' ),
                         ( 'AimsData<uint8_t>', 'aims::BucketMap<float>' ),
                         ( 'AimsData<uint16_t>', 'aims::BucketMap<Void>' ),
                         ( 'AimsData<uint16_t>', 'aims::BucketMap<float>' ),
                         ( 'AimsData<int16_t>', 'aims::BucketMap<Void>' ),
                         ( 'AimsData<int16_t>', 'aims::BucketMap<int16_t>' ),
                         ( 'AimsData<int16_t>', 'aims::BucketMap<float>' ),
                         ( 'AimsData<uint32_t>', 'aims::BucketMap<Void>' ),
                         ( 'AimsData<uint32_t>', 'aims::BucketMap<float>' ),
                         ( 'AimsData<int32_t>', 'aims::BucketMap<Void>' ),
                         ( 'AimsData<int32_t>', 'aims::BucketMap<float>' ),
                         ( 'AimsData<float>', 'aims::BucketMap<Void>' ),
                         ( 'AimsData<float>', 'aims::BucketMap<float>' ),
                         ( 'AimsData<double>', 'aims::BucketMap<Void>' ),
                         ( 'AimsData<double>', 'aims::BucketMap<float>' ),
                         ( 'aims::BucketMap<Void>', 'AimsData<uint8_t>' ),
                         ( 'aims::BucketMap<Void>', 'AimsData<uint16_t>' ),
                         ( 'aims::BucketMap<Void>', 'AimsData<int16_t>' ),
                         ( 'aims::BucketMap<Void>', 'AimsData<uint32_t>' ),
                         ( 'aims::BucketMap<Void>', 'AimsData<int32_t>' ),
                         ( 'aims::BucketMap<Void>', 'AimsData<float>' ),
                         ( 'aims::BucketMap<Void>', 'AimsData<double>' ),
                         ( 'aims::BucketMap<int16_t>', 'AimsData<uint8_t>' ),
                         ( 'aims::BucketMap<int16_t>', 'AimsData<uint16_t>' ),
                         ( 'aims::BucketMap<int16_t>', 'AimsData<int16_t>' ),
                         ( 'aims::BucketMap<int16_t>', 'AimsData<uint32_t>' ),
                         ( 'aims::BucketMap<int16_t>', 'AimsData<int32_t>' ),
                         ( 'aims::BucketMap<int16_t>', 'AimsData<float>' ),
                         ( 'aims::BucketMap<int16_t>', 'AimsData<double>' ),
                         ( 'aims::BucketMap<float>', 'AimsData<uint8_t>' ),
                         ( 'aims::BucketMap<float>', 'AimsData<uint16_t>' ),
                         ( 'aims::BucketMap<float>', 'AimsData<int16_t>' ),
                         ( 'aims::BucketMap<float>', 'AimsData<uint32_t>' ),
                         ( 'aims::BucketMap<float>', 'AimsData<int32_t>' ),
                         ( 'aims::BucketMap<float>', 'AimsData<float>' ),
                         ( 'aims::BucketMap<float>', 'AimsData<double>' ),
                       ],
         'rawconverter_bucket' : [ 'short' ],
         'reader' : [ 'Graph', 'AimsSurfaceTriangle',
                      'AimsData<uint8_t>', 'AimsData<int16_t>',
                      'AimsData<uint16_t>', 'AimsData<int32_t>',
                      'AimsData<uint32_t>', 'AimsData<float>',
                      'AimsData<double>', 'AimsData<AimsRGB>',
                      'AimsData<AimsRGBA>', 
                      'carto::Volume<uint8_t>', 'carto::Volume<int16_t>', 
                      'carto::Volume<uint16_t>', 'carto::Volume<int32_t>', 
                      'carto::Volume<uint32_t>', 'carto::Volume<float>', 
                      'carto::Volume<double>', 'carto::Volume<AimsRGB>', 
                      'carto::Volume<AimsRGBA>', 'AimsTimeSurface<2,Void>', 
                      'AimsTimeSurface<3,Void>', 'AimsTimeSurface<4,Void>',
                      'TimeTexture<float>', 'TimeTexture<int16_t>',
                      'TimeTexture<int32_t>', 'TimeTexture<uint32_t>',
                      'TimeTexture<Point2df>',
                      'aims::BucketMap<Void>', 'aims::BucketMap<int16_t>',
                      'aims::BucketMap<float>',
                      'Motion', 'aims::Hierarchy', 'carto::Object',
                      ],
         'aimstimesurface' : [ '2', '3', '4', ],
         'timetexture' : [ 'float', 'short', 'int', 'unsigned', 'Point2df' ],
         'bucketmap' : [ 'Void', 'short', 'float' ],
         'aimsvector' : [ ( 'float', '2' ), ( 'float', '3' ),
                          ( 'float', '4' ), ( 'double', '3' ),
                          ( 'short', '3' ),
                          ( 'unsigned', '2' ), ( 'unsigned', '3' ),
                          ( 'unsigned', '4' ),
                          ], 
         'threshold' : [ ( 'float', 'short' ), ( 'short', 'short') ],
         'merge' : [ ( 'short', 'short'), ],
         }

