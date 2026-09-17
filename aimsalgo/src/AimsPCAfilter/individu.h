#ifndef INDIVIDU_H
#define INDIVIDU_H
#include <aims/data/data_g.h>
#include <vector>

class Individu
{
 public:
  Individu() { };
  Individu(int nbFrame);
  Individu(const Individu& i);
  ~Individu();
   void cinetique(int nbFrame);

  //private:
  std::vector<float>    myDecours;
  Point3d          myLoc;
  double           myNorm;

  // methode d'acces ...y


};



#endif
