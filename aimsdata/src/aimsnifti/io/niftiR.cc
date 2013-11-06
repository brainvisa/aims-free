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

#include <aims/io/niftiR_d.h>

using namespace aims ;

template class NiftiReader<int8_t> ;
template class NiftiReader<uint8_t> ;
template class NiftiReader<int16_t> ;
template class NiftiReader<uint16_t> ;
template class NiftiReader<int32_t> ;
template class NiftiReader<uint32_t> ;
template class NiftiReader<int64_t> ;
template class NiftiReader<uint64_t> ;
template class NiftiReader<float> ;
template class NiftiReader<double> ;
template class NiftiReader<AimsRGB>;
template class NiftiReader<AimsRGBA>;

// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template class NiftiReader<char> ;
#endif
#ifndef __LP64__
template class NiftiReader<long> ;
template class NiftiReader<unsigned long> ;
#endif

