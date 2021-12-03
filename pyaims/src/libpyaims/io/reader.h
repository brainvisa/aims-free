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

#ifndef PYAIMS_IO_READER_H
#define PYAIMS_IO_READER_H

#include <aims/io/reader.h>
#include <aims/data/data.h>
#include <aims/rgb/rgb.h>

typedef aims::Reader<AimsData<int8_t> > Reader_AimsData_S8;
typedef aims::Reader<AimsData<uint8_t> > Reader_AimsData_U8;
typedef aims::Reader<AimsData<int16_t> > Reader_AimsData_S16;
typedef aims::Reader<AimsData<uint16_t> > Reader_AimsData_U16;
typedef aims::Reader<AimsData<int32_t> > Reader_AimsData_S32;
typedef aims::Reader<AimsData<uint32_t> > Reader_AimsData_U32;
typedef aims::Reader<AimsData<float> > Reader_AimsData_FLOAT;
typedef aims::Reader<AimsData<double> > Reader_AimsData_DOUBLE;
typedef aims::Reader<AimsData<AimsRGB> > Reader_AimsData_RGB;
typedef aims::Reader<AimsData<AimsRGBA> > Reader_AimsData_RGBA;

typedef aims::Reader<carto::Volume<int8_t> > Reader_Volume_S8;
typedef aims::Reader<carto::Volume<uint8_t> > Reader_Volume_U8;
typedef aims::Reader<carto::Volume<int16_t> > Reader_Volume_S16;
typedef aims::Reader<carto::Volume<uint16_t> > Reader_Volume_U16;
typedef aims::Reader<carto::Volume<int32_t> > Reader_Volume_S32;
typedef aims::Reader<carto::Volume<uint32_t> > Reader_Volume_U32;
typedef aims::Reader<carto::Volume<float> > Reader_Volume_FLOAT;
typedef aims::Reader<carto::Volume<double> > Reader_Volume_DOUBLE;
typedef aims::Reader<carto::Volume<AimsRGB> > Reader_Volume_RGB;
typedef aims::Reader<carto::Volume<AimsRGBA> > Reader_Volume_RGBA;

#endif
