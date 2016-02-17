/* Copyright (C) 2000-2013 CEA
 *
 * This software and supporting documentation were developed by
 *     bioPICSEL
 *     CEA/DSV/I²BM/MIRCen/LMN, Batiment 61,
 *     18, route du Panorama
 *     92265 Fontenay-aux-Roses
 *     France
 */

#ifndef AIMS_TRANSFORM_LEAST_SQUARE_SESTIMATION_H
#define AIMS_TRANSFORM_LEAST_SQUARE_SESTIMATION_H

#include <aims/vector/vector.h>
#include <aims/data/data.h>
#include <aims/resampling/motion.h>
#include <vector>


namespace aims {
  
  class TransformLeastSquareEstimation {
  public:
    TransformLeastSquareEstimation( const std::vector<Point3df>& from, 
                                    const std::vector<Point3df>& to ) ;
    
    virtual ~TransformLeastSquareEstimation() ;
    
    const DecomposedMotion* motion()
    { 
      if (!_motionCalculated){
        computeMotion() ;
        _motionCalculated = true ;
      }

      return _motion; 
    } 
    
  protected:
    std::vector<Point3df> _pointsFrom ;
    std::vector<Point3df> _pointsTo ;
    
    DecomposedMotion * _motion ;
    bool _motionCalculated ;
    bool _is2D ;

    virtual bool computeMotion() = 0;
  } ;



  class AffineLeastSquareEstimation : public TransformLeastSquareEstimation {
  public:
    AffineLeastSquareEstimation( const std::vector<Point3df>& from, 
                                 const std::vector<Point3df>& to ) ;
    
    virtual ~AffineLeastSquareEstimation() {}
    
    
  protected:
    virtual bool computeMotion() ;
  } ;



  class RigidLeastSquareEstimation : public TransformLeastSquareEstimation {
  public:
    RigidLeastSquareEstimation( const std::vector<Point3df>& from, 
                                const std::vector<Point3df>& to ) ;
    
    virtual ~RigidLeastSquareEstimation() {}

    static AimsData<float> criterionItem( const Point3df& p1, const Point3df& p2, 
                                          const Point3df& gc1, const Point3df& gc2, float weight ) ;
    double error() ;

    
  protected:
    virtual bool computeMotion() {return computeRigidMotion();}
    virtual bool computeRigidMotion() ;
    Point3df _meanX;
    Point3df _meanY;
  } ;
 
  class TranslationLeastSquareEstimation : public TransformLeastSquareEstimation {
  public:
    TranslationLeastSquareEstimation( const std::vector<Point3df>& from, 
                                      const std::vector<Point3df>& to ) ;
    
    virtual ~TranslationLeastSquareEstimation() {}

    static AimsData<float> criterionItem( const Point3df& p1, const Point3df& p2, 
                                          const Point3df& gc1, const Point3df& gc2, float weight ) ;
    double error() ;

    
  protected:
    virtual bool computeMotion() {return computeTranslationMotion();}
    virtual bool computeTranslationMotion() ;

    Point3df _meanX;
    Point3df _meanY;
  } ;
 


  class SimiLeastSquareEstimation : public RigidLeastSquareEstimation {
  public:
    SimiLeastSquareEstimation( const std::vector<Point3df>& from, 
				 const std::vector<Point3df>& to ) ;
    
    virtual ~SimiLeastSquareEstimation() {}

    
  protected:
    virtual bool computeMotion() ;

  } ;


}


#endif
