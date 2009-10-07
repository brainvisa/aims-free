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

#include <aims/roi/roi.h>
#include <aims/roi/roistat.h>
#include <aims/roi/roiselector.h>
#include <aims/roi/roigtm.h>
#include <aims/data/pheader.h>
#include <aims/resampling/resampler.h>
#include <aims/math/mathelem.h>
#include <float.h>
#include <iomanip>

using namespace aims;
using namespace carto;
using namespace std;


RoiStats::RoiStats( Resampler< float >* interpolator , RoiSelector* roiSel)
  :  Graph(), _interpolator( interpolator ), _roiSel( roiSel ), _isGtmCorrected(false)
{
}


RoiStats::~RoiStats()
{
}




vector<float> RoiStats::tacByStructName(string s)
{
  vector<float> vout;

  std::set<Vertex*>    setV = vertices();

  bool found = false;
  for(std::set< Vertex* >::const_iterator it=setV.begin();
      it != setV.end();
      it++)
    {
      string tmp; (*it)->getProperty("surname", tmp);
      found = (string(tmp) == string(s) );
      if (found) 
	{
	  (*it)->getProperty("mean_ac", vout);
	  break;
	}
    }
  return(vout);
}


void RoiStats::applyGtm( RoiGtm& rgtm )
{
  // Check compatiblilty of rgtm with internal description
  std::set<Vertex*>    setV = vertices();
  vector<string>  vecS = rgtm.getStructNameList();
  int		  i;
  for(i=0; i < (int) vecS.size(); i++)
    {
      bool found = false;
      for(std::set< Vertex* >::const_iterator it=setV.begin();
	  it != setV.end();
	  it++)
	{
	  string tmp; (*it)->getProperty("surname", tmp);
	  found = (string(tmp) == string(vecS[i]));
	  if (found) break;
	}
      if (! found ) return;
    }
  //Loop on gtmcolumn item to create tac matrices
  ASSERT( hasProperty("start_time") );     // Checking
  vector<int> start_time; int dT;
  getProperty("start_time", start_time);
  dT = start_time.size();
  AimsData<float> StructTac( vecS.size(), dT );
  for(i=0; i < (int)vecS.size(); i++)
    {
      vector<float> tmp = tacByStructName(vecS[i]);
      for(int j=0; j < dT; j++)
	{
	  StructTac(i,j) = tmp[j];
	}
    }
  AimsData<float> invgtm = rgtm.getInvMatrix();
  //Perform processing and store results
  AimsData<float> CorrTac = invgtm.cross(StructTac);
  for(i=0; i < (int) vecS.size(); i++)
    {  
      bool found = false;
      for(set< Vertex* >::const_iterator it=setV.begin();
	  it != setV.end();
	  it++)
	{
	  string tmp; (*it)->getProperty("surname", tmp);
	  found = (string(tmp) == string(vecS[i]));
	  if (found)
	    {
	      vector<float> gtmac;
	      for(int j=0; j < dT; j++)
		{
		  gtmac.push_back( CorrTac(i, j) );
		}
	      (*it)->setProperty( "gtm_ac", gtmac );
	      break;
	    }
	}
      // Case not found impossible since checking performed at first in this func
    }

  setGtmCorrected();
}

void RoiStats::streamout( char *out )
{
  vector<int>     start_time, duration_time;
  vector<float>   taCurv, stdCurv, minCurv, maxCurv, gtm_ac;
  string          surname;
  int             nbVox;
  float           volVox;
  ofstream        *os = NULL;

  if (out)
    os = new ofstream(out, ios::app);

  ASSERT( hasProperty("start_time") );     // Checking
  getProperty("start_time", start_time);
  ASSERT( hasProperty("duration_time") );  // Checking
  getProperty("duration_time", duration_time);
  ASSERT( hasProperty("last_image_stated") );     // Checking
  ASSERT( hasProperty("voxel_volume") );
  getProperty("voxel_volume", volVox);

  std::set<Vertex*>    sv = vertices();
  
  ASSERT( sv.begin() != sv.end() );         // There at least one frame !!!
  (*sv.begin())->getProperty("mean_ac", taCurv);
  ASSERT(start_time.size() == taCurv.size()); //Checking

  std::map< string, Vertex *> reorderedVertices;
  
  // Streaming out yet...
  string separ = "|";
  if (os)
    *os << setw(15) << "RoiName" << separ  
	<< setw(7) << "StTime" << separ 
	<< setw(7) <<"DuTime" << separ 
	<< setw(8) << "Volume" << separ
	<< setw(10) << "Mean" << separ 
	<< setw(10) << "StdDev" << separ 
	<< setw(10) << "Min" << separ 
	<< setw(10) << "Max";
  else
    cout << setw(15) << "RoiName" << separ  
	<< setw(7) << "StTime" << separ 
	<< setw(7) <<"DuTime" << separ 
	<< setw(8) << "Volume" << separ
	<< setw(10) << "Mean" << separ 
	<< setw(10) << "StdDev" << separ 
	<< setw(10) << "Min" << separ 
	<< setw(10) << "Max";
  if ( isGtmCorrected() )
    {
      if (os)
	*os << setw(10) << separ << "GtmCorr" << endl;
      else
	cout << setw(10) <<separ << "GtmCorr" << endl;
    }
  else
    {
      if (os)
	*os << endl;
      else
	cout << endl;
    }
  for(set< Vertex* >::const_iterator it=sv.begin();
      it != sv.end();
      it++)
    {
      (*it)->getProperty("surname", surname);
      reorderedVertices[surname] = *it ;
    }
  
  for(map< string, Vertex* >::const_iterator 
	it=reorderedVertices.begin();
      it != reorderedVertices.end();
      it++)
    {
      it->second->getProperty("voxel_number", nbVox);
      it->second->getProperty("mean_ac", taCurv);
      it->second->getProperty("std_ac", stdCurv);
      it->second->getProperty("min_ac", minCurv);
      it->second->getProperty("max_ac", maxCurv);
      it->second->getProperty("surname", surname);
      if ( isGtmCorrected() ) 
	{
	  it->second->getProperty("gtm_ac", gtm_ac);
	}

      int endT = taCurv.size();
      for(int t=0;t<endT;t++)
	{
	  if (os)
	    *os << setw(15) << surname.c_str() << separ 
		<< setw(7) << start_time[t] << separ
		<< setw(7) << duration_time[t] << separ
		<< setw(8) << nbVox*volVox << separ
		<< setw(10) << taCurv[t] << separ
		<< setw(10) << stdCurv[t] << separ
		<< setw(10) << minCurv[t] << separ
	        << setw(10) << maxCurv[t];
	  else
	    cout << setw(15) <<surname.c_str() << separ
               <<  setw(7) << start_time[t] << separ
               <<  setw(7) << duration_time[t] << separ
	       <<  setw(8) << nbVox*volVox << separ
               <<  setw(10) << taCurv[t] << separ
               <<  setw(10) << stdCurv[t] << separ
               <<  setw(10) << minCurv[t] << separ
               <<  setw(10) << maxCurv[t];
	    if ( isGtmCorrected() )
	      {
		if (os)
		  *os << separ <<  setw(10) << gtm_ac[t] << endl;
		else
		  cout << separ << setw(10) << gtm_ac[t] << endl;
	      }
	    else
	      {
		if (os)
		  *os << endl;
		else
		  cout << endl;
	      }
	}
    }
  os->close();
  delete(os);
}

