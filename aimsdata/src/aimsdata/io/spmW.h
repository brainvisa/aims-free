/*
 *  Spm writer class
 */
#ifndef AIMS_IO_SPMW_H
#define AIMS_IO_SPMW_H

#include <aims/config/aimsdata_config.h>
#include <fstream>
#include <string>
#include <aims/data/data.h>


namespace aims
{
  template <class T> class SpmWriter;

  template <class T>
  SpmWriter<T>& operator << ( SpmWriter<T>& writer, const AimsData<T>& thing );


  /** The template class for SPM write operation.
      The template argument gives the type of the elements stored
      in the container.
  */
  template <class T>
  class SpmWriter
  {
  public:
    SpmWriter( const std::string& name ) 
      : _name( name ) { }
    virtual ~SpmWriter() { }

    /** Write the data with "name" file name to disk
        @param thing const reference to the data to write
    */
    void write( const AimsData<T>& thing );

    /// Return a name without .hdr or .img extension
    inline std::string removeExtension( const std::string& name );

    /// Output stream operator
    friend
    SpmWriter<T>& 
    operator << <> (SpmWriter<T>& writer, const AimsData<T>& thing);

  private:
    std::ofstream _os;
    std::string   _name;
  };


  template <class T>
  inline 
  std::string SpmWriter<T>::removeExtension( const std::string& name )
  {
    std::string res = name;
    std::string ext="";
    if ( res.length() > 4 )
      ext = res.substr( int(res.length() - 4), 4 );
    if (ext == ".hdr" || ext == ".img")
      res = res.substr( 0, res.length() - 4 );
    return res;
  }


  template <class T> inline
  SpmWriter<T>& operator << ( SpmWriter<T>& writer, const AimsData<T>& thing )
  {
    writer.write(thing);
    return writer;
  }

}

#endif


