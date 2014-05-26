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

#include <aims/algorithm/imagealgorithm.h>

namespace aims {
  
//   template < class VoxelType >
//   AimsData<VoxelType> ImageAlgorithmSwitch<VoxelType, false>::execute( ImageAlgorithmInterface<VoxelType> & algo, 
//                                                                        const AimsData<VoxelType> & in ) {
// //         std::cout << "VoxelType :" << carto::DataTypeCode<VoxelType>::name() << std::endl
// //                   << "VoxelType::ChannelType :" 
// //                   << carto::toString(carto::DataTypeCode<VoxelType>::name()) << std::endl
// //                   << "ImageAlgorithm is NOT multi channel" << std::endl;
// //         std::cout << "Processing channel 0" << std::endl;
//     return algo.execute( in );
//   }
  
//   template < class VoxelType >
//   AimsData<VoxelType> ImageAlgorithmSwitch<VoxelType, true>::AimsData<VoxelType> execute( 
//                         ImageAlgorithmInterface<typename VoxelType::ChannelType> & algo, 
//                         const AimsData<VoxelType> & in ) {
// //         std::cout << "VoxelType :" << carto::DataTypeCode<VoxelType>::name() << std::endl
// //                   << "VoxelType::ChannelType :" 
// //                   << carto::toString(carto::DataTypeCode<typename VoxelType::ChannelType>::name()) << std::endl
// //                   << "ImageAlgorithm is multi channel" << std::endl;
// 
//     ChannelSelector< AimsData<VoxelType>, AimsData<typename VoxelType::ChannelType> > selector;
//     Point4d dims = algo.getOutputImageDimensions( Point4d( in.dimX(),
//                                                             in.dimY(),
//                                                             in.dimZ(),
//                                                             in.dimT() ) );
// 
//     AimsData<VoxelType> out( dims[0],
//                              dims[1],
//                              dims[2],
//                              dims[3] );
//     if( in.header() ) {
//       out.setHeader( in.header()->cloneHeader( true ) );
//     }
// 
//     for (uint8_t channel = 0; channel < DataTypeInfo< VoxelType >::samples(); channel++) {
//       if (carto::verbose)
//         std::cout << "Processing channel :" << carto::toString(channel) << std::endl;
//       
//       /* Split the data and process filter on each component */ \
//       const AimsData<typename VoxelType::ChannelType> & inChannel = selector.select( in, channel );
//       const AimsData<typename VoxelType::ChannelType> & outChannel = algo.execute( inChannel );
//       selector.set( out, channel, outChannel );
//     }
//     
//     return out;
  }
}
