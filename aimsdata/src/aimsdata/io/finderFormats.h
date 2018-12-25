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

/*
 *  Finder class
 */

#ifndef AIMS_IO_FINDERFORMATS_H
#define AIMS_IO_FINDERFORMATS_H


#include <aims/config/aimsdata_config.h>
#include <aims/io/fileFormat.h>


namespace aims
{

  class FinderSpmFormat : public FinderFormat
  {
  public:
    virtual ~FinderSpmFormat() {}
    virtual bool check( const std::string & filename, Finder & f ) const;
  };

#ifdef VIDA_LIB
  class FinderVidaFormat : public FinderFormat
  {
  public:
    virtual ~FinderVidaFormat() {}
    virtual bool check( const std::string & filename, Finder & f ) const;
  };
#endif

#ifdef DICOM_LIB
  class FinderDicomFormat : public FinderFormat
  {
  public:
    virtual ~FinderDicomFormat() {}
    virtual bool check( const std::string & filename, Finder & f ) const;
  };
#endif

#ifdef ECAT_LIB
  class FinderEcatFormat : public FinderFormat
  {
  public:
    virtual ~FinderEcatFormat() {}
    virtual bool check( const std::string & filename, Finder & f ) const;
  };
#endif

#ifdef AIMS_JPEG
  class FinderJpegFormat : public FinderFormat
  {
  public:
    virtual ~FinderJpegFormat() {}
    virtual bool check( const std::string & filename, Finder & f ) const;
  };
#endif

  class FinderGenesisFormat : public FinderFormat
  {
  public:
    virtual ~FinderGenesisFormat() {}
    virtual bool check( const std::string & filename, Finder & f ) const;
  };

  class FinderMeshFormat : public FinderFormat
  {
  public:
    virtual ~FinderMeshFormat() {}
    virtual bool check( const std::string & filename, Finder & f ) const;
  };

  class FinderTriFormat : public FinderFormat
  {
  public:
    virtual ~FinderTriFormat() {}
    virtual bool check( const std::string & filename, Finder & f ) const;
  };

  class FinderMniObjFormat : public FinderFormat
  {
  public:
    virtual ~FinderMniObjFormat() {}
    virtual bool check( const std::string & filename, Finder & f ) const;
  };

  class FinderWavefrontFormat : public FinderFormat
  {
  public:
    virtual ~FinderWavefrontFormat() {}
    virtual bool check( const std::string & filename, Finder & f ) const;
  };

  class FinderTexFormat : public FinderFormat
  {
  public:
    virtual ~FinderTexFormat() {}
    virtual bool check( const std::string & filename, Finder & f ) const;
  };

  class FinderBckFormat : public FinderFormat
  {
  public:
    virtual ~FinderBckFormat() {}
    virtual bool check( const std::string & filename, Finder & f ) const;
  };

  class FinderGraphFormat : public FinderFormat
  {
  public:
    virtual ~FinderGraphFormat() {}
    virtual bool check( const std::string & filename, Finder & f ) const;
  };

  class FinderTrmFormat : public FinderFormat
  {
  public:
    virtual ~FinderTrmFormat() {}
    virtual bool check( const std::string & filename, Finder & f ) const;
  };

  class FinderHieFormat : public FinderFormat
  {
    public:
      virtual ~FinderHieFormat() {}
      virtual bool check( const std::string & filename, Finder & f ) const;
  };

  class FinderImasFormat : public FinderFormat
  {
    public:
      virtual ~FinderImasFormat() {}
      virtual bool check( const std::string & filename, Finder & f ) const;
  };

}


#endif


