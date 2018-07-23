%ModuleHeaderCode
#include <aims/information/pdf.h>
%End

void    AimsParzenJointPdf(const AimsData_S16& data1,
                           const AimsData_S16& data2,
                           AimsData_FLOAT& p12, 
                           AimsData_FLOAT& p1, AimsData_FLOAT& p2 );

void    AimsParzenPdf(const AimsData_S16& data,
                      AimsData_FLOAT& p );



void    AimsWinParzenJointPdf(const AimsData_S16& data1,
                              const AimsData_S16& data2,
                              AimsData_FLOAT& p12, 
                              AimsData_FLOAT& p1, 
                              AimsData_FLOAT& p2,
                              const AimsData_FLOAT& mask );

void    AimsWinParzenPdf(const AimsData_S16& data,
                         AimsData_FLOAT& p, 
                         const AimsData_FLOAT& mask );



void    AimsJointPdf(const AimsData_S16& data1,
                     const AimsData_S16& data2,
                     AimsData_FLOAT& p12, 
                     AimsData_FLOAT& p1,
                     AimsData_FLOAT& p2);

void    AimsPdf(const AimsData_S16& data,
                AimsData_FLOAT& p);

void    AimsJointMaskPdf(const AimsData_S16& data1,
                         const AimsData_S16& data2,
                         AimsData_FLOAT& p12, 
                         AimsData_FLOAT& p1, 
                         AimsData_FLOAT& p2);

void    AimsGeneralizedKnnParzenPdf(aims::knn::Database &db,
                        AimsData_FLOAT &pdf, unsigned int k);

void    AimsKnnPdf(aims::knn::Database &db,
                        AimsData_FLOAT &pdf, unsigned int k);
