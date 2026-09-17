#ifndef PYAIMS_DATA_DATA_H
#define PYAIMS_DATA_DATA_H

#include <cartodata/volume/volume.h>
#include <aims/rgb/rgb.h>
#include <aims/vector/vector.h>

typedef carto::Volume<int8_t> Volume_S8;
typedef carto::Volume<uint8_t> Volume_U8;
typedef carto::Volume<int16_t> Volume_S16;
typedef carto::Volume<uint16_t> Volume_U16;
typedef carto::Volume<int32_t> Volume_S32;
typedef carto::Volume<uint32_t> Volume_U32;
typedef carto::Volume<float> Volume_FLOAT;
typedef carto::Volume<double> Volume_DOUBLE;
typedef carto::Volume<AimsRGB> Volume_RGB;
typedef carto::Volume<AimsRGBA> Volume_RGBA;
typedef carto::Volume<Point3df> Volume_POINT3DF;

typedef carto::rc_ptr<carto::Volume<int8_t> > rc_ptr_Volume_S8;
typedef carto::rc_ptr<carto::Volume<uint8_t> > rc_ptr_Volume_U8;
typedef carto::rc_ptr<carto::Volume<int16_t> > rc_ptr_Volume_S16;
typedef carto::rc_ptr<carto::Volume<uint16_t> > rc_ptr_Volume_U16;
typedef carto::rc_ptr<carto::Volume<int32_t> > rc_ptr_Volume_S32;
typedef carto::rc_ptr<carto::Volume<uint32_t> > rc_ptr_Volume_U32;
typedef carto::rc_ptr<carto::Volume<float> > rc_ptr_Volume_FLOAT;
typedef carto::rc_ptr<carto::Volume<double> > rc_ptr_Volume_DOUBLE;
typedef carto::rc_ptr<carto::Volume<AimsRGB> > rc_ptr_Volume_RGB;
typedef carto::rc_ptr<carto::Volume<AimsRGBA> > rc_ptr_Volume_RGBA;
typedef carto::rc_ptr<carto::Volume<Point3df> > rc_ptr_Volume_POINT3DF;

#endif

