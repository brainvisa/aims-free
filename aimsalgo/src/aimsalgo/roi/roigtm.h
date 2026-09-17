
#ifndef AIMS_ROI_ROIGTM_H
#define AIMS_ROI_ROIGTM_H


#include <aims/config/aimsalgo_config.h>
#include <cartodata/volume/volume.h>
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
                   carto::rc_ptr<carto::Volume<float> > smo
                   = carto::rc_ptr<carto::Volume<float> >( 0 ) );
    void streamout(   );
  
    carto::VolumeRef<float> getMatrix();
    carto::VolumeRef<float> getInvMatrix();
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
