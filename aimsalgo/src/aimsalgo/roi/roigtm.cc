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


#include <aims/io/io_g.h>
#include <aims/morphology/morphology_g.h>
#include <aims/signalfilter/signalfilter_g.h>
#include <aims/math/gausslu.h>  
#include <aims/roi/roi.h>
#include <aims/roi/roigtm.h>
#include <aims/roi/roiselector.h>
#include <aims/utility/threshold.h>
#include <aims/io/datatypecode.h>
#include <stdio.h>

using namespace aims;
using namespace carto;
using namespace std;


RoiGtm::RoiGtm( RoiSelector* roiSel, float trans, float axial )
  :  Graph(), _roiSel( roiSel ), _trans( trans ), _axial( axial ), _isEditable( false )
{

  sGtm[ "GtmArg" ][ "GtmArg_VERSION" ].type = "string";
  sGtm[ "GtmArg" ][ "GtmArg_VERSION" ].needed = false;

  sGtm[ "GtmArg" ][ "selector_basename" ].type = "string";
  sGtm[ "GtmArg" ][ "selector_basename" ].needed = false;

  sGtm[ "GtmArg" ][ "roi_basename" ].type = "string";
  sGtm[ "GtmArg" ][ "roi_basename" ].needed = false;

  sGtm[ "GtmArg" ][ "gtm_mask_basename" ].type = "string";
  sGtm[ "GtmArg" ][ "gtm_mask_basename" ].needed = false;

  sGtm[ "structure" ][ "surname" ].type = "string";
  sGtm[ "structure" ][ "surname" ].needed = false;

  sGtm[ "structure" ][ "gtm_column" ].type = "float_vector";
  sGtm[ "structure" ][ "gtm_column" ].needed = true;
  
  sGtm[ "structure" ][ "column_number" ].type = "int";
  sGtm[ "structure" ][ "column_number" ].needed = false;


  setSyntax( "GtmArg" );
  setProperty("selector_basename", string( "Unknown" ) );
  setProperty("roi_basename", string( "Unknown" ) );

  if (!roiSel)
    _isComputable = false; else _isComputable = true;
}


RoiGtm::~RoiGtm()
{
}

void RoiGtm::stamp(string &selector_fname, string &roi_fname)
{
  setProperty("selector_basename", selector_fname);
  setProperty("roi_basename", roi_fname);
}


AimsData<float> RoiGtm::getMatrix()
{
  AimsData<float>			outmatrix;
  std::set<Vertex*>				sv = vertices();
  string				surname;
  std::set< Vertex* >::const_iterator	it;

  if ( _isEditable )
    {
      int sx=0, sy=0;
       
      for(it=sv.begin();
	  it != sv.end();
	  it++)
	{
	  if ((*it)->hasProperty("surname"))
	    {
	      vector<float> tmp; (*it)->getProperty("gtm_column", tmp);
	      sy = tmp.size();
	      sx++;
	    }
	}
      
      outmatrix = AimsData<float>( sy, sx );
      vector<string>   entete_trans;

      int isx=0, isy=0;
      string entete;
      for(it=sv.begin();
	  it != sv.end();
	  it++)
	{
	  if ((*it)->hasProperty("surname"))
	    {
	      isx =0;(*it)->getProperty("column_number", isx);
	      vector<float> tmp; (*it)->getProperty("gtm_column", tmp);
	      for(isy=0; isy<sy; isy++)
		{
		  outmatrix(isy, isx) = tmp[isy];
		}
	    }
	}
      return( outmatrix );
    }
  else
    {
    AimsError("RoiGtm object : object is not editable");
    }
  return( outmatrix );
}

AimsData<float> RoiGtm::getInvMatrix()
{
  AimsData<float>	outmatrix;
  if ( _isEditable )
    {
      return( AimsInversionLU( getMatrix() ) );
    }
  else
    {
      AimsError("RoiGtm object : object is not editable");
    }
  return( outmatrix );
}

vector<string>  RoiGtm::getStructNameList()
{
  std::set<Vertex*>            sv = vertices();
  string                  surname;
  int                    sx=0;

  if ( !_isEditable )
    {
          AimsError("RoiGtm object : object is not editable");
	  vector<string> entete;
	  return(entete);
    }

  std::set< Vertex* >::const_iterator it;
  for(it=sv.begin();
      it != sv.end();
      it++)
    {
      if ((*it)->hasProperty("surname")) sx++;
    }

  vector<string> entete(sx);
  for(it=sv.begin();
      it != sv.end();
      it++)
    {
      if ((*it)->hasProperty("surname"))
      {
	int i=0; (*it)->getProperty("column_number",i);
	string stmp; (*it)->getProperty("surname", stmp);
	entete[i] = stmp;
      }
    }

  return(entete);
}

AimsRoi* RoiGtm::doit( AimsRoi &roi, RoiGtm::MaskType m, AimsData<float>* smo )
{
  /*	Denis 10/12/02
	This code is a complete mess - 
	lots of useless things, complex lists are built just to test if they 
	are empty at the end (a single bool would have worked as well), 
	lots of variables allocated inside loops, 
	pseudo-lists built to avoid using the graph structure already 
	containing this info, 
	many values are computed and not used afterwards...

	I think it's mainly a sequence of copy-paste of useless things, 
	just modified a bit so as to compile anyway.

	I'm trying to clean it a little bit, but I'm not sure what it's 
	supposed to do, it's so confused...
   */

  int  indice;
  std::map<int, string> nameList;

  if (!_isComputable ) 
    {
      AimsWarning("No roiSelector specified");
      return(_maskRoi);
    }

  _maskRoi = new AimsRoi;

  // get the maximum bouding box
  vector< int > bbmaxTO; 
  vector< float > vsTO;
  roi.getProperty( "boundingbox_max", bbmaxTO);
  roi.getProperty( "voxel_size", vsTO );
  AimsData<float> imaTmp(bbmaxTO[0],bbmaxTO[1],bbmaxTO[2]);
  imaTmp.setSizeXYZT(vsTO[0],vsTO[1],vsTO[2]);
  if (_trans == -1.) _trans = vsTO[0];
  if (_axial == -1.)   _axial = vsTO[2];

  const list<BaseTree*>		& metaRoiList = _roiSel->children();
  BaseTree::const_iterator	metaRoi;
  int				l;
  rc_ptr<BucketMap<Void> >	bck;
  bool				found;
  Tree				*tmpTree;

  indice = 0;
  if (metaRoiList.empty())
    return(_maskRoi);       // Error Management !!!!

  std::map<BaseTree*, std::map<int, rc_ptr<BucketMap<Void> > > >	corresBuckets;
  std::map<int, rc_ptr<BucketMap<Void> > >::iterator ib, eb;

  // Compute the number of Real Roi submitted to process
  // Conjugate 2 constaints
  //    - Grouping through selector mechanism (inf from file .sel and .hie)
  //    - Availibility of the eligible roi in the data (roi file .arg or .img)
  // Instanciate necessary memory form mask
  for ( metaRoi = metaRoiList.begin();
	metaRoi != metaRoiList.end();
	++metaRoi )
    {
      found = false;
      const list<BaseTree*>	& metaRoiChild = (*metaRoi)->children();
      std::map<int, rc_ptr<BucketMap<Void> > > & cbk = corresBuckets[ *metaRoi ];

      for (BaseTree::const_iterator metaRoiContent = metaRoiChild.begin();
	                            metaRoiContent != metaRoiChild.end();
	                            metaRoiContent++)
	{
	  tmpTree = dynamic_cast< Tree *>( *metaRoiContent ); // Test 
	  string tmp;
	  tmpTree->getProperty("nomenName", tmp);
	  //cout << "DBG " << "Nomen " <<tmp << endl; //==========
	  std::set<Vertex*> roisv = roi.VertByNameAndDescendantName( tmp );

	  for( std::set< Vertex* >::const_iterator j = roisv.begin();
	       !found && j != roisv.end(); ++j )
	    {
	      (*j)->getProperty("roi_label", l);
	      if( (*j)->getProperty( "aims_roi", bck ) )
		// internal desc (roi_label) of value l is cand to
		    // be inserteed in corresBuckets vector
		cbk[ l ] = bck;
	      found = true;
	    }
	}
      if( found ) // i.e. when there is a roi of this type 
	// in the input roi file
	++indice;
    }

  int realMetaRoiNum = indice;
  if (realMetaRoiNum == 0) 
    {
      AimsWarning("No roi selected using RoiFile and SelectorFile");
      return( _maskRoi );     // To be better managed
    }
  AimsData<short> mask(bbmaxTO[0],bbmaxTO[1],bbmaxTO[2], realMetaRoiNum);
  mask.setSizeXYZT(vsTO[0],vsTO[1],vsTO[2],1.0);

  // Now Loop on MetaRoi to create mask
  indice = 0;
  BucketMap<Void>::Bucket::iterator	ibk, ebk;
  string				surname;
  int 					x, y, z;
  Gaussian3DSmoothing< float >		g3ds( _trans,_trans,_axial );

  for ( metaRoi = metaRoiList.begin();
	metaRoi != metaRoiList.end();
	++metaRoi )
    {
      tmpTree = dynamic_cast< Tree *>( *metaRoi ); // Test
      if( !tmpTree->getProperty("surname", surname) )
      	cerr << "No surname attribute" << endl ;
      // Construct the corresBucket
      //cout << "DBG " << "Surname " << surname << endl; //============
      list<BaseTree*> metaRoiChild = (*metaRoi)->children();
      std::map<int, rc_ptr<BucketMap<Void> > > & cbk = corresBuckets[ *metaRoi ];

      if( cbk.size() != 0 )
	{
	  // Images creation
	  switch (m)
	    {
	    case RoiGtm::threshold_on_smoothed:
	      {
		cout << "Mask via smooth and threshold"<< endl;
		imaTmp = 0.0;
		for( ib=cbk.begin(), eb=cbk.end(); ib!=eb; ++ib )
		  for( ibk=ib->second->begin()->second.begin(), 
			 ebk=ib->second->begin()->second.end();
		       ibk!=ebk; ++ibk )
		    imaTmp( ibk->first ) = 1.0;
		AimsData<float> tmpf;
		tmpf = g3ds.doit( imaTmp );
		AimsThreshold<float,short>   thresh( AIMS_GREATER_THAN, 
						     tmpf.maximum() * 50 / 100,
						     0);
		AimsData<short> tmpi = thresh.bin( tmpf );     
		ForEach3d(tmpi, x, y, z)
		  {
		    short tmpival =  tmpi(x,y,z);
		    if (tmpival)
		      //Make label vary
		      mask(x, y, z, indice) = tmpival - indice;
		    // from 32767.... 32767 - indice..
		  }
	      }
	    break;

	    case RoiGtm::threshold_on_closed:
	      {
		cout << "Mask via closing and threshold"<<endl;
		AimsData<short> imaTmpS(bbmaxTO[0],bbmaxTO[1],bbmaxTO[2],1,1);
		imaTmpS.setSizeXYZT(vsTO[0],vsTO[1],vsTO[2]);
		imaTmpS = 0;
		for( ib=cbk.begin(), eb=cbk.end(); ib!=eb; ++ib )
		  for( ibk=ib->second->begin()->second.begin(), 
			 ebk=ib->second->begin()->second.end();
		       ibk!=ebk; ++ibk )
		    imaTmpS( ibk->first ) = 1;
		AimsData<short> clo;
		clo = AimsMorphoChamferClosing( imaTmpS, _trans, 3,3,3,50 );
		ForEach3d(clo, x, y, z)
		  if (clo(x,y,z))
		    mask(x, y, z, indice) = 32767 - indice;//Make label vary
		// from 32767.... 32767 - indice..
	      }
	    break;

	    case RoiGtm::get_from_file:
	      cout << "Mask from file"<<endl;
	      imaTmp = 0.0;
	      for( ib=cbk.begin(), eb=cbk.end(); ib!=eb; ++ib )
		for( ibk=ib->second->begin()->second.begin(), 
		       ebk=ib->second->begin()->second.end();
		     ibk!=ebk; ++ibk )
		  imaTmp( ibk->first ) = 1.0;
	      ForEach3d(imaTmp, x, y, z)
		if (imaTmp(x,y,z))
		  mask(x, y, z, indice) = 32767 - indice; //Make label vary
	      // from 32767.... 32767 - indice..
	      break;
	    }
	
	  nameList[32767 - indice] = surname;
	  indice++;
	  cout << "Indice " << indice << endl ;
	}
    }

  // Mask are available : Now compute gaussian contribution
  
  int             column_cnt=0;
  int             smo_cnt = 0;

  for ( metaRoi = metaRoiList.begin();
	metaRoi != metaRoiList.end();
	metaRoi++)
    {
      tmpTree = dynamic_cast< Tree *>( *metaRoi ); // Test 
      string surname;
      (*tmpTree).getProperty("surname", surname);
      cout << "surname : " << surname << endl ;
      // Construct the corresBucket
      list<BaseTree*> metaRoiChild = (*metaRoi)->children();
      std::map<int, rc_ptr<BucketMap<Void> > > & cbk = corresBuckets[ *metaRoi ];

      if( cbk.size() != 0 )
	{
	  // Images creation
	  imaTmp = 0.0;
	  for( ib=cbk.begin(), eb=cbk.end(); ib!=eb; ++ib )
	    for( ibk=ib->second->begin()->second.begin(), 
		   ebk=ib->second->begin()->second.end();
		 ibk!=ebk; ++ibk )
	      imaTmp( ibk->first ) = 1.0;
	  AimsData<float> tmpf = g3ds.doit( imaTmp );

	  if ( smo )
	    {
	      ForEach3d(tmpf, x, y, z)
		tmpf(x,y,z) = (*smo)(x,y,z,smo_cnt);
	      ++smo_cnt;
	    }

	  vector<float> gtm_col;
	  float cumul;
	  int currentBucketMass;
	  for( indice=0; indice < realMetaRoiNum; ++indice )
	    {
	      cumul = 0.0;
	      currentBucketMass = 0;
	      ForEach3d(imaTmp, x, y, z)
		if ( mask(x, y, z, indice) ) 
		  {
		    cumul += tmpf( x, y, z );
		    ++currentBucketMass;
		  }
	      cout << "DBG " << "cumul " << cumul << " "<< currentBucketMass 
		   << endl; // ====
	      float ncumul = cumul/currentBucketMass;
	      if (ncumul <  - 1e-6 ) // GaussRecursion may have failed
		{
		  cout << "Integral of gaussian queue is : " << ncumul << endl;
		  cout << "Keep going computation with 0.0." << endl;
		}
	      if (ncumul < 0.) ncumul = 0.0;
	      gtm_col.push_back(ncumul);
	    }

	  Vertex *v = addVertex("structure");
	  v->setProperty("surname", surname);
	  v->setProperty("gtm_column", gtm_col);
	  v->setProperty("column_number", column_cnt++);
	}
    }

  AimsData<short> compressedMask(mask.dimX(), mask.dimY(), mask.dimZ()) ;
  compressedMask.setSizeXYZT(mask.sizeX(), mask.sizeY(), mask.sizeZ(), 1.0 ) ;
  if( mask.header() )
  	compressedMask.setHeader( mask.header()->cloneHeader() ) ;
  
  ForEach4d(mask, x, y, z, l ){
      if( mask(x, y, z, l) )
  	compressedMask(x, y, z) = mask(x, y, z, l) ;
  }

  Writer< AimsData<short> > wri("ROIGTMmask.ima" ) ;
  wri.write(compressedMask) ;
  cout << "NbOfRegions : " << mask.dimT() << endl ;
  // On finalise la transformation des mask en AimsRoi _maskRoi.
  
  _maskRoi->setLabel(compressedMask);
  _maskRoi->data2bucket( nameList );
  _isEditable = true;

  return(_maskRoi);
}
 
void RoiGtm::streamout(   )
{
  
  std::set<Vertex*>            sv = vertices();
  string                  surname;

  string tmp;
  if (hasProperty("selector_basename"))
    {
      getProperty("selector_basename", tmp);
      cout << "Selector file basename: " << tmp << endl;
    }
  if (hasProperty("roi_basename"))
    {
      getProperty("roi_basename", tmp);
      cout << "Roi file basename: " << tmp << endl;
    }
    
    
		   
  if ( _isEditable )
    {
      AimsData<float> gtm = getMatrix();
      AimsData<float> gtminv = getInvMatrix();

      vector<string>  entete = getStructNameList();


      int ncol = gtm.dimX();
      int nlig = gtm.dimY();
      
      //entete            
      cout << "=========== Direct GTM matrix" << endl;
      cout << "           ";
      int	i, lig;
      for(i=0;i<ncol;i++)
	{
	  cout << " " << entete[i];
	}
      cout << endl;
      
      // Corps de la matrice
      for(lig=0;lig<nlig;lig++)
	{
	  printf("%10s ", (entete[lig]).c_str() );
	  for(int col=0;col<ncol;col++)
	    {
	      printf(" %1.4f",gtm( lig, col )  );
	    }
	  cout << endl;
	}

      cout << "=========== Inverse GTM matrix" << endl;
      cout << "           ";
      for(i=0;i<ncol;i++)
	{
	  cout << " " << entete[i];
	}
      cout << endl;
      
      // Corps de la matrice
      for(lig=0;lig<nlig;lig++)
	{
	  printf("%10s ", (entete[lig]).c_str() );
	  for(int col=0;col<ncol;col++)
	    {
	      printf(" %1.4f",gtminv( lig, col )  );
	    }
	  cout << endl;
	}

    }		   
  else
    cout << "This roi cannot be edited" << endl;
}
