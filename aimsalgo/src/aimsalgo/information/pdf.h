
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
