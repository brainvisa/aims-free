
#ifndef AIMS_IO_GIFTIHEADER_H
#define AIMS_IO_GIFTIHEADER_H

#include <aims/config/aimsdata_config.h>
#include <aims/data/pheader.h>
#include <aims/io/gifti.h>


template <typename T> class TimeTexture;

namespace carto
{
  class Mutex;
}

namespace aims
{

  /**   GIFTI Header class.
  */
  class GiftiHeader : public PythonHeader
  {
  public:
    GiftiHeader( const std::string & name );
    /*GiftiHeader( int dimx, int dimy, int dimz, int dimt, float sx, float sy,
                 float sz, float st, const std::string & name );*/
    virtual ~GiftiHeader();

    const std::string& name() const;
    void setName( const std::string & fname ) { _name = fname; }

    const carto::Object options() const { return _options; }
    void setOptions( carto::Object opt ) { _options = opt; }

    virtual std::string extension() const;

    bool read();
    static std::string niftiRefFromAimsString( const std::string & space );
    static carto::Mutex & giftiMutex();

  private:
    template <int D, typename T>
    friend class GiftiMeshFormat;
    template <typename T>
    friend class GiftiTextureFormat;

    gifti_image* giftiImageBase();
    void giftiAddExternalTextures( gifti_image *gim, int & hdrtexda,
                                   carto::Object da_info );
    void giftiAddLabelTable( gifti_image *gim );
    static carto::Object giftiFindHdrDA( int & nda, carto::Object dainfo,
                                         const std::string & intent );
    static void giftiCopyMetaToGii( carto::Object dainf, giiDataArray *da );
    static void giftiSetTransformations( carto::Object cs, giiDataArray *da );
    template <typename T>
    void giftiAddTexture( gifti_image* gim, const std::vector<T> & tex );
    template <typename T>
    void giftiAddTexture( gifti_image* gim, const TimeTexture<T> & texture );

    //void giftiAddTexture( gifti_image* gim, const std::vector<Void> & tex );
    template <typename T>
    void giftiAddTextureObject( gifti_image* gim, carto::Object texture );

    std::string _name;
    carto::Object _options;
   };

}

#endif
