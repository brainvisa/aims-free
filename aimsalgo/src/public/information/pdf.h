/* Copyright (c) 1995-2007 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */


#ifndef AIMS_INFORMATION_PDF_H
#define AIMS_INFORMATION_PDF_H

#include <aims/config/aimsalgopub_config.h>
#include <aims/resampling/sampler.h>
#include <aims/math/knn.h>

template <class T> class AimsData;


AIMSALGOPUB_API 
void AimsParzenJointPdf( const AimsData<short>& data1,
                         const AimsData<short>& data2,
                         AimsData<float>& p12, 
                         AimsData<float>& p1, AimsData<float>& p2 );

AIMSALGOPUB_API 
void AimsParzenPdf( const AimsData<short>& data,
                    AimsData<float>& p );



AIMSALGOPUB_API 
void AimsWinParzenJointPdf( const AimsData<short>& data1,
                            const AimsData<short>& data2,
                            AimsData<float>& p12, 
                            AimsData<float>& p1, AimsData<float>& p2,
                            const AimsData<float>& mask );

AIMSALGOPUB_API 
void AimsWinParzenPdf( const AimsData<short>& data,
                       AimsData<float>& p, const AimsData<float>& mask );



AIMSALGOPUB_API 
void AimsJointPdf( const AimsData<short>& data1,
                   const AimsData<short>& data2,
                   AimsData<float>& p12, 
                   AimsData<float>& p1, AimsData<float>& p2 );

AIMSALGOPUB_API 
void AimsPdf( const AimsData<short>& data, AimsData<float>& p );


AIMSALGOPUB_API 
void AimsJointMaskPdf( const AimsData<short>& data1,
		       const AimsData<short>& data2,
		       AimsData<float>& p12, 
		       AimsData<float>& p1, AimsData<float>& p );

AIMSALGOPUB_API 
void AimsJointPVPdf( const AimsData<short>& data1,
		     const AimsData<short>& data2,
		     const AimsData<PVItem>& comb,
		     AimsData<float>& p12, 
		     AimsData<float>& p1, AimsData<float>& p2 );

AIMSALGOPUB_API 
void AimsJointPVPdf( const aims::BucketMap<short>&         data1, 
		     const AimsData<short>&                data2,
		     const std::vector< PVVectorItem >& comb,
		     AimsData<float>& p12, 
		     AimsData<float>& p1, AimsData<float>& p2 );

/** Compute Knn density estimation.
    img : output pdf
    db : input database (order of dots may be modified)
    k : distance to the kth nearest neighbours is used to set parzen
        window width
 */
AIMSALGOPUB_API
void	AimsKnnPdf(aims::knn::Database &db,
			AimsData<float> &pdf, unsigned int k);

/** Compute Generalized Knn Parzen density estimation.
    img : output pdf
    db : input database (order of dots may be modified)
    k : distance to the kth nearest neighbours is used to set parzen
        window width
 */
AIMSALGOPUB_API
void	AimsGeneralizedKnnParzenPdf(aims::knn::Database &db,
			AimsData<float> &pdf, unsigned int k);

#endif
