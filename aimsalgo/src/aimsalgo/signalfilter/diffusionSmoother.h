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


#ifndef AIMS_PRIMALSKETCH_DIFFUSIONSMOOTHER_H
#define AIMS_PRIMALSKETCH_DIFFUSIONSMOOTHER_H

#include <cstdlib>
#include <aims/primalsketch/imageSmoother.h>

namespace aims
{

/** Base class for Heat diffusion */
template<typename T>
class BaseDiffusionSmoother
  : public Smoother<carto::VolumeRef<T>, carto::VolumeRef<T> >
{
public:
  BaseDiffusionSmoother(float delta_t) {this->SetDt(delta_t);}
  ~BaseDiffusionSmoother() {}
public:
  void check(int maxiter);
  bool optimal() {return true;}
  void SetDt(float Delta_t) {
    if (Delta_t<=0.25) _dt=Delta_t;
    else {
      std::cerr << "Diffusion Smoother : dt must be <= 0.25"
        << std::endl; exit(EXIT_FAILURE);
    }
  }
  virtual carto::VolumeRef<T> doSmoothing(const carto::VolumeRef<T> & ima,
      int maxiter, bool verbose=false) = 0;

  float dt() {return _dt;}

public:
  static carto::VolumeRef<float> init_laplacian(void);
  static carto::VolumeRef<float> laplacian;

protected:
  float _dt;
};


/** Heat diffusion with a volume of given datasource (Dirichlet conditions)
*/
template<typename T>
class DiffusionSmoother : public BaseDiffusionSmoother<T>
{
public: 
  DiffusionSmoother(float delta_t) : BaseDiffusionSmoother<T>(delta_t),
            _hasConstantSources( false ) {}
  ~DiffusionSmoother() {}

public:
  virtual carto::VolumeRef<T> doSmoothing(const carto::VolumeRef<T> & ima,
        int maxiter, bool verbose=false);
  void setConstantSources( const carto::VolumeRef<T> &, const T & background );
  void removeConstantSources();

private:
  carto::VolumeRef<T> _constantSources;
  bool _hasConstantSources;
  T _constantSourcesBackground;
};


template<typename T>
class BaseMaskedDiffusionSmoother : public BaseDiffusionSmoother<T>
{
public: 
  BaseMaskedDiffusionSmoother(float delta_t, bool safe=true) :
    BaseDiffusionSmoother<T>(delta_t), _safe(safe)
  { _mask.reset( 0 ); }
  ~BaseMaskedDiffusionSmoother() {}

public:
  virtual carto::VolumeRef<T> doSmoothing(const carto::VolumeRef<T> & ima,
        int maxiter, bool verbose=false) = 0;
  void setMask( const carto::rc_ptr<carto::Volume<short> > & mask,
                short background=0)
  {
    _mask = mask;
    _background = background;
  }

protected:
  carto::VolumeRef<short>    _mask;
  bool      _safe;
  short      _background;
};



template<typename T, typename N=std::vector<Point3df> >
class MaskedDiffusionSmoother : public BaseMaskedDiffusionSmoother<T>
{
public: 
  MaskedDiffusionSmoother(float delta_t, bool safe=true) :
    BaseMaskedDiffusionSmoother<T>(delta_t, safe) {}
  ~MaskedDiffusionSmoother() {}

public:
  carto::VolumeRef<T> doSmoothing(const carto::VolumeRef<T> & ima,
      int maxiter, bool verbose=false);
};

/** Heat diffusion, with Dirichlet and Neumann border conditions.
    - Dirichlet conditions (data source, 1st order) are stored in the input
    volume.
    - Neumann conditions (null derivative condition along border normals) can
    be specified also (see add_neumann_condition).
    Conditions must be masked. Neumann ones must have 2 voxel width if there
    are surrounded by unmasked values (because values from unmasked neighbours
    are copied to borders satisfying Neumann conditions).
 */
template<typename T>
class MaskedDiffusionSmoother<T, std::vector<Point3df> > :
public BaseMaskedDiffusionSmoother<T>
{
public: 
  MaskedDiffusionSmoother(float delta_t, bool safe=true) :
    BaseMaskedDiffusionSmoother<T>(delta_t, safe),
           _has_neumann_condition(false) {}
  ~MaskedDiffusionSmoother() {}

public:
  virtual carto::VolumeRef<T> doSmoothing(const carto::VolumeRef<T> & ima,
        int maxiter, bool verbose=false);
  void add_neumann_condition(const Point3df &p);

private :
  void update_neumann_conditions(carto::VolumeRef<float> &ima);

private :
  bool      _has_neumann_condition;
  std::vector<Point3df>  _neumann_conditions;
};



/** Heat diffusion, with Dirichlet and Neumann border conditions.
    - Dirichlet conditions (data source, 1st order) are stored in the input
        volume.
    - Neumann conditions (null derivative condition along border normals) are
    specified in the mask (use set_neumann_value).
    Conditions must be masked. In this version Neumann conditions can be
    isolated voxels.
  */
template<typename T>
class MaskedDiffusionSmoother<T, carto::VolumeRef<short> > :
public BaseMaskedDiffusionSmoother<T>
{
public: 
  MaskedDiffusionSmoother(float delta_t, bool safe=true) :
    BaseMaskedDiffusionSmoother<T>(delta_t, safe),
           _neumann_value(1) {}
  ~MaskedDiffusionSmoother() {}

public:
  virtual carto::VolumeRef<T> doSmoothing(const carto::VolumeRef<T> & ima,
        int maxiter, bool verbose=false);
  void  set_neumann_value(short value) {
    _neumann_value = value;
  }

  void convolution(const carto::VolumeRef<float> &ima1,
      carto::VolumeRef<float> &ima2) const;
private:
  short  _neumann_value;
};

}

#endif
