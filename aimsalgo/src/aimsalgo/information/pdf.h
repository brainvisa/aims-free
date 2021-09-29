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


#ifndef AIMS_INFORMATION_PDF_H
#define AIMS_INFORMATION_PDF_H

#include <aims/config/aimsalgo_config.h>
#include <aims/resampling/sampler.h>
#include <aims/math/knn.h>

namespace carto
{
  template <class T> class Volume;
  template <class T> class VolumeRef;
}


void AimsParzenJointPdf( const carto::rc_ptr<carto::Volume<short> >& data1,
                         const carto::rc_ptr<carto::Volume<short> >& data2,
                         carto::rc_ptr<carto::Volume<float> >& p12,
                         carto::rc_ptr<carto::Volume<float> >& p1,
                         carto::rc_ptr<carto::Volume<float> >& p2 );

void AimsParzenPdf( const carto::rc_ptr<carto::Volume<short> >& data,
                    carto::rc_ptr<carto::Volume<float> >& p );



void AimsWinParzenJointPdf( const carto::rc_ptr<carto::Volume<short> >& data1,
                            const carto::rc_ptr<carto::Volume<short> >& data2,
                            carto::rc_ptr<carto::Volume<float> >& p12,
                            carto::rc_ptr<carto::Volume<float> >& p1,
                            carto::rc_ptr<carto::Volume<float> >& p2,
                            const carto::rc_ptr<carto::Volume<float> >& mask );

void AimsWinParzenPdf( const carto::rc_ptr<carto::Volume<short> >& data,
                       carto::rc_ptr<carto::Volume<float> >& p,
                       const carto::rc_ptr<carto::Volume<float> >& mask );



void AimsJointPdf( const carto::rc_ptr<carto::Volume<short> >& data1,
                   const carto::rc_ptr<carto::Volume<short> >& data2,
                   carto::rc_ptr<carto::Volume<float> >& p12,
                   carto::rc_ptr<carto::Volume<float> >& p1,
                   carto::rc_ptr<carto::Volume<float> >& p2 );

void AimsPdf( const carto::rc_ptr<carto::Volume<short> >& data,
              carto::rc_ptr<carto::Volume<float> >& p );


void AimsJointMaskPdf( const carto::rc_ptr<carto::Volume<short> >& data1,
		       const carto::rc_ptr<carto::Volume<short> >& data2,
		       carto::rc_ptr<carto::Volume<float> >& p12,
		       carto::rc_ptr<carto::Volume<float> >& p1,
                       carto::rc_ptr<carto::Volume<float> >& p );

void AimsJointPVPdf( const carto::rc_ptr<carto::Volume<short> >& data1,
		     const carto::rc_ptr<carto::Volume<short> >& data2,
		     const carto::rc_ptr<carto::Volume<PVItem> >& comb,
		     carto::rc_ptr<carto::Volume<float> >& p12,
		     carto::rc_ptr<carto::Volume<float> >& p1,
                     carto::rc_ptr<carto::Volume<float> >& p2 );

void AimsJointPVPdf( const aims::BucketMap<short>&         data1, 
		     const carto::rc_ptr<carto::Volume<short> >& data2,
		     const std::vector< PVVectorItem >& comb,
		     carto::rc_ptr<carto::Volume<float> >& p12,
		     carto::rc_ptr<carto::Volume<float> >& p1,
                     carto::rc_ptr<carto::Volume<float> >& p2 );

/** Compute Knn density estimation.
    img : output pdf
    db : input database (order of dots may be modified)
    k : distance to the kth nearest neighbours is used to set parzen
        window width
 */
void	AimsKnnPdf( aims::knn::Database &db,
                    carto::rc_ptr<carto::Volume<float> > &pdf,
                    unsigned int k );

/** Compute Generalized Knn Parzen density estimation.
    img : output pdf
    db : input database (order of dots may be modified)
    k : distance to the kth nearest neighbours is used to set parzen
        window width
 */
void AimsGeneralizedKnnParzenPdf( aims::knn::Database &db,
                                  carto::rc_ptr<carto::Volume<float> > &pdf,
                                  unsigned int k );

#endif
