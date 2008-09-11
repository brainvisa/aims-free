/* Copyright (c) 1995-2007 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */


#ifndef AIMS_ROI_ROIGTM_H
#define AIMS_ROI_ROIGTM_H


#include <aims/config/aimsalgo_config.h>
#include <aims/data/data.h>
#include <aims/bucket/bucket.h>
#include <aims/roi/roi.h>
#include <aims/roi/roiselector.h>
#include <graph/graph/graph.h>
#include <vector>


namespace aims
{

  class AIMSALGO_API RoiGtm : public Graph
  {

  public:


    enum MaskType
      {
	threshold_on_closed,
	threshold_on_smoothed,
	get_from_file
      };


    RoiGtm( RoiSelector* roiSel=NULL, float trans=-1, float axial=-1);
    virtual ~RoiGtm( );


    void stamp(std::string &selector_fname, std::string &roi_fname);
    AimsRoi* doit( AimsRoi &roi,
		   MaskType m = threshold_on_smoothed,
		   AimsData<float>* smo = NULL );
    void streamout(   );
  
    AimsData<float> getMatrix();
    AimsData<float> getInvMatrix();
    std::vector<std::string>  getStructNameList();
    AimsRoi&        getMaskRoi(){return *_maskRoi;}
    void            setMaskRoi(AimsRoi* roi){ _maskRoi = roi;}


    void editionSet() {_isEditable = true;}
    void computationSet() {_isComputable = true;}

    carto::SyntaxSet& syntax() { return sGtm; }


  protected:
    carto::SyntaxSet             sGtm;

  private:
    RoiSelector*          _roiSel;
    float                 _trans, _axial;
    AimsRoi*              _maskRoi;
    bool                  _isEditable;
    bool                  _isComputable;
  };

}

#endif
