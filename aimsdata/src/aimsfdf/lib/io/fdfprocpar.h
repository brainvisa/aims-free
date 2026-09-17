#ifndef AIMS_IO_FDFPROCPAR_H
#define AIMS_IO_FDFPROCPAR_H

#include <aims/def/general.h>
#include <string>
#include <vector>

namespace aims
{
    template< class T >
    class AIMSDATA_API FdfParType
    {
        private:
            FdfParType() {}

        public:
            virtual ~FdfParType() {}

            static inline int getFdfType();
            static inline T getFdfDefaultValue();
            static inline char getFdfSeparator();
    };

    class AIMSDATA_API FdfProcPar
    {
        public:
        
            FdfProcPar( const std::string & name ) :
                _name(name)
            { }
            virtual ~FdfProcPar() { }
        
            /// Get the file name of the header
            std::string name() const { return _name; }
        
            std::string search( std::ifstream & file, std::string param );
            int search( std::ifstream & file, std::string param, int type, int active );

            template< class T >
            std::vector<T> values(std::string param);

            template< class T >
            T value(std::string param);

            template< class T >
            T value(std::string param, T defaultvalue);
        
        private:
            std::string _name;
        
    };
}

#endif

