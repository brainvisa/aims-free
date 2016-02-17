/* Copyright (C) 2000-2013 CEA
 *
 * This software and supporting documentation were developed by
 *     bioPICSEL
 *     CEA/DSV/I²BM/MIRCen/LMN, Batiment 61,
 *     18, route du Panorama
 *     92265 Fontenay-aux-Roses
 *     France
 */

#ifndef AIMS_REGISTRATION_BLOCK_MATCHING_H
#define AIMS_REGISTRATION_BLOCK_MATCHING_H

#include <aims/data/data.h>
namespace aims
{
  class AffineTransrformation3d;
}

template<class T>
class BlockMatching
{
public:
  enum TypeSeuil {
    REF_LOW_THRES,
    REF_HIGH_THRES,
    TEST_LOW_THRES,
    TEST_HIGH_THRES
  };
  BlockMatching(); 
  virtual ~BlockMatching() {}

  aims::AffineTransformation3d doit(AimsData<T>& ref,
                                    const AimsData<T>& test_orig);
  void setinitialisation(const aims::AffineTransformation3d& m)
  {_initialisation = m;}
  void setlevel_start(int lsta) {_level_start=lsta;}
  void setlevel_stop(int lsto) {_level_stop=lsto;}
  void settransfo(int t) {_transfo=t;}
  void settailleBloc(int nx, int ny, int nz) {_tailleBloc=Point3d(nx,ny,nz);}
  void setcutVar(double cv) {_cutVar=cv;}
  void setstopVar(double sv) {_stopVar=sv;}
  void setPkept(double pk) {_Pkept=pk;}
  void setseuilCorrel(double sc) {_seuilCorrel=sc;}
  void setseuils(T sbr, T shr, T sbt, T sht) {
    _seuils[0]=sbr;
    _seuils[1]=shr;
    _seuils[2]=sbt;
    _seuils[3]=sht;
  }
  void setitermax(int itm) {_itermax=itm;}
  void setinfo(bool info) {_info=info;}

  AimsData<T> getresult() {return _result;}
  

private:
  AimsData<T> _result;
  bool _done;
  aims::AffineTransformation3d _initialisation;
  int _level_start;
  int _level_stop;
  int _transfo;
  Point3d _tailleBloc;
  double _cutVar;
  double _stopVar;
  double _Pkept;
  double _seuilCorrel;
  T _seuils[4];
  int _itermax;
  bool _info;
};
#endif
