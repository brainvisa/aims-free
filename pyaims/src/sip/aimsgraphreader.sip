namespace aims
{

  class AimsGraphReader /* : aims::Process */
  {
%TypeHeaderCode
#include <aims/io/aimsGraphR.h>
%End

  public:
    AimsGraphReader( const std::string & filename );
    virtual ~AimsGraphReader();
    virtual void readElements( Graph & g, int mask = 1 );
    void setReadFilter( const set_STRING & toread );
    void setExcludeFilter( const set_STRING & toexclude );
  };


  class AimsGraphWriter /* : aims::Process */
  {
%TypeHeaderCode
#include <aims/io/aimsGraphW.h>
%End

  public:
    enum SavingMode
    {
      Keep,
      Global,
      Local
    };

    AimsGraphWriter( const std::string & filename );
    virtual ~AimsGraphWriter();
    virtual void writeElements( Graph & g, SavingMode newmode = Keep,
                                SavingMode oldmode = aims::AimsGraphWriter::Keep,
                                bool saveOnlyModified = false );
  };

};

