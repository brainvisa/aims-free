%ModuleHeaderCode
#include <aims/information/pdf.h>
%End

void    AimsParzenJointPdf(const rc_ptr_Volume_S16& data1,
                           const rc_ptr_Volume_S16& data2,
                           rc_ptr_Volume_FLOAT& p12,
                           rc_ptr_Volume_FLOAT& p1, rc_ptr_Volume_FLOAT& p2 );

void    AimsParzenPdf(const rc_ptr_Volume_S16& data,
                      rc_ptr_Volume_FLOAT& p );



void    AimsWinParzenJointPdf(const rc_ptr_Volume_S16& data1,
                              const rc_ptr_Volume_S16& data2,
                              rc_ptr_Volume_FLOAT& p12,
                              rc_ptr_Volume_FLOAT& p1,
                              rc_ptr_Volume_FLOAT& p2,
                              const rc_ptr_Volume_FLOAT& mask );

void    AimsWinParzenPdf(const rc_ptr_Volume_S16& data,
                         rc_ptr_Volume_FLOAT& p,
                         const rc_ptr_Volume_FLOAT& mask );



void    AimsJointPdf(const rc_ptr_Volume_S16& data1,
                     const rc_ptr_Volume_S16& data2,
                     rc_ptr_Volume_FLOAT& p12,
                     rc_ptr_Volume_FLOAT& p1,
                     rc_ptr_Volume_FLOAT& p2);

void    AimsPdf(const rc_ptr_Volume_S16& data,
                rc_ptr_Volume_FLOAT& p);

void    AimsJointMaskPdf(const rc_ptr_Volume_S16& data1,
                         const rc_ptr_Volume_S16& data2,
                         rc_ptr_Volume_FLOAT& p12,
                         rc_ptr_Volume_FLOAT& p1,
                         rc_ptr_Volume_FLOAT& p2);

void    AimsGeneralizedKnnParzenPdf(aims::knn::Database &db,
                        rc_ptr_Volume_FLOAT &pdf, unsigned int k);

void    AimsKnnPdf(aims::knn::Database &db,
                        rc_ptr_Volume_FLOAT &pdf, unsigned int k);
