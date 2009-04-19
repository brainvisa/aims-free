%ModuleHeaderCode
#include <aims/information/pdf.h>
%End

void    AimsGeneralizedKnnParzenPdf(aims::knn::Database &db,
                        AimsData_FLOAT &pdf, unsigned int k);

void    AimsKnnPdf(aims::knn::Database &db,
                        AimsData_FLOAT &pdf, unsigned int k);
