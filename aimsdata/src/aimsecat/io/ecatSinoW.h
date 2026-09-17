/*
 *  VIDA data writer class
 */
#ifndef AIMS_IO_ECATSINOW_H
#define AIMS_IO_ECATSINOW_H

#include <aims/config/aimsdata_config.h>
#include <aims/sino/sino.h>
#include <vector>
#include <string>


/** The class for EcatSino data write operation.
*/
namespace aims
{

  template <class T> class EcatSinoWriter;
  template <class T>
    EcatSinoWriter<T>& operator << ( EcatSinoWriter<T>& writer, Sinogram<T>& thing);


  template<class T>
  class AIMSDATA_API EcatSinoWriter
    { 
public:
  /**@name Constructor and Destructor*/
  //@{
  /** The programmer should give the file name to the writer.
      @param name reference to the file name
  */
  EcatSinoWriter(const std::string& name) : _name(name) { }
  /// Destructor does nothing special
  virtual ~EcatSinoWriter() { }
  //@}
  
  /**@name Methods*/
  //@{
  /** Write the data to disk with "name" file name
      @param thing const reference to the data to write
  */
  //  void write(const Sinogram< T >& thing);
  void write( const std::vector < Sinogram< T > >& thing);
      
  void write( const Sinogram<T>& thing ) ;
  
  std::string removeExtension(const std::string& name);
  //@}
  
    private:
  /**@name Data*/
  //@{
  /// File name
  std::string   _name;
  //@}
    };



template<class T>
EcatSinoWriter<T>&
operator << (EcatSinoWriter<T>& writer,
	      const std::vector < Sinogram<T> >& thing)
{ 
  writer.write(thing);
  return(writer);
}


}


#endif
