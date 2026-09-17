#include "individu.h"
#include <aims/io/io_g.h>
#include <iostream>

using namespace std;

// definition des constructeurs et destruteurs de la classe Individu

// Individu :: Individu() : myDecours(0), myLoc(0)
// {}

Individu ::Individu(int nbFrame )
{
  myDecours.resize(nbFrame);
}

Individu ::Individu(const Individu& i) : myDecours(i.myDecours), myLoc(i.myLoc)
{}

Individu ::~Individu()
{}

//definition des methodes de la classe

void Individu :: cinetique(int nbFrame)
{
  for ( int k=0; k<nbFrame ;k++ )
    cout << "frame " << k << ": " << myDecours[k] << endl; 
}
