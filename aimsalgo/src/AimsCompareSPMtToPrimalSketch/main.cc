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


#include <cstdlib>
#include <aims/getopt/getopt2.h>
#include <aims/data/data.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/io/process.h>
#include <aims/io/finder.h>
#include <aims/texdata/texdata.h>
#include <aims/primalsketch/greyLevelBlob.h>
#include <aims/mesh/surface.h>
#include <aims/mesh/texture.h>
#include <aims/primalsketch/scalespace.h>
#include <aims/signalfilter/diffusionSmoother.h>
#include <aims/primalsketch/dericheSmoother_d.h>
#include <aims/primalsketch/primalSketch.h>
#include <aims/primalsketch/primalSketch2graph.h>
#include <aims/primalsketch/primalSketchUtil.h>
#include <iostream>

using namespace aims;
using namespace carto;
using namespace std;

int main(int argc, const char **argv)
{
  try
    {
      Reader<Graph> rg;
      Reader<AimsData<short> > rspm;

      AimsApplication     app( argc, argv, "Select blobs in a PS that are the closest to selected activations in a SPMt - EXPERIMENTAL. Blobs that are selected are given the activation label in their 'name' attribute");

      app.addOption( rg, "-p", "PS graph");
      app.alias( "--primal", "-p" );
      app.addOption( rspm, "-s", "spmt image");
      app.alias( "--spmt", "-s" );
      app.initialize();

      cout << "reading " << rg.fileName() << endl;
      Graph              *primal = rg.read();
      cout << "reading " << rspm.fileName() << endl;
      AimsData<short> spmt;
      rspm.read(spmt);

      std::map<int, std::vector<float> > cc_spmt;
      // on met dans la map la bounding box de chaque cc sous forme d'un vecteur de
      // coordonnées boundboxmin(x y z) boundboxmax(x y z)
      int x, y, z, sx, sy, sz;
      float xf, yf, zf;
      sx=spmt.dimX(); sy=spmt.dimY(); sz=spmt.dimZ();
      int cc;

      cout << "Itemizing connected components in spmt map" << endl;
      for (z=0; z<sz; z++)
          for (y=0; y<sy; y++)
               for (x=0; x< sx; x++)
               {
                    xf=float(x);
                    yf=float(y);
                    zf=float(z);
                    if (spmt(x,y,z)!=0)
                    {
                         std::vector<float> bb(6);
                         cc=spmt(x,y,z);
                         if (cc_spmt.find(cc) != cc_spmt.end())
                         {
                              if (xf < (cc_spmt[cc])[0]) (cc_spmt[cc])[0]=xf;
                              else if (xf > (cc_spmt[cc])[3]) (cc_spmt[cc])[3]=xf;
                              if (yf < (cc_spmt[cc])[1]) (cc_spmt[cc])[1]=yf;
                              else if (yf > (cc_spmt[cc])[4]) (cc_spmt[cc])[4]=yf;
                              if (zf < (cc_spmt[cc])[2]) (cc_spmt[cc])[2]=zf;
                              else if (zf > (cc_spmt[cc])[5]) (cc_spmt[cc])[5]=zf;
                         }
                         else
                         {
                              cout << "New entry in CC map : key=" << cc << endl;
                              bb[0]=xf; bb[3]=xf;
                              bb[1]=yf; bb[4]=yf;
                              bb[2]=zf; bb[5]=zf;
                              cc_spmt.insert(std::pair< int, std::vector<float> >(cc, bb));
                         }
                    }
               }
     // comparaison avec le graphe :

     cout << "Getting the best blob for each of the connected components" << endl;
     std::map<int, vector<float> >::iterator itCC=cc_spmt.begin();
     Graph::iterator itG, itM;
     double rec, recmax, overlap_x, overlap_y, overlap_z;
     std::vector<float>  bbmin_1(3), bbmin_2(3), bbmax_1(3), bbmax_2(3);
     for ( ; itCC != cc_spmt.end() ; ++itCC)
     {
          recmax=0;
          itM=primal->begin();
          cc=(*itCC).first;
          std::vector<float> bb=(*itCC).second;
          bbmin_1[0]=bb[0];bbmin_1[1]=bb[1];bbmin_1[2]=bb[2];
          bbmax_1[0]=bb[3];bbmax_1[1]=bb[4];bbmax_1[2]=bb[5];
          int no_overlap;
          cout << "Processing CC no. " << cc << endl;
          for (itG=primal->begin() ; itG!=primal->end(); ++itG)
          {
               no_overlap=0; rec=0;
               (*itG)->getProperty( "boundingbox_max", bbmax_2);
               (*itG)->getProperty( "boundingbox_min", bbmin_2);
               if (bbmin_1[0]<=bbmin_2[0])
                    if (bbmax_1[0]<bbmin_2[0]) no_overlap=1;
                    else overlap_x= (bbmax_2[0] < bbmax_1[0] ? bbmax_2[0] : bbmax_1[0]) - bbmin_2[0] +1;
               else
                    if (bbmax_2[0]<bbmin_1[0]) no_overlap=1;
                    else overlap_x= (bbmax_1[0] < bbmax_2[0] ? bbmax_1[0] : bbmax_2[0]) - bbmin_1[0] +1;
               if (no_overlap==0)
               {
                    if (bbmin_1[1]<=bbmin_2[1])
                         if (bbmax_1[1]<bbmin_2[1]) no_overlap=1;
                         else overlap_y= (bbmax_2[1] < bbmax_1[1] ? bbmax_2[1] : bbmax_1[1]) - bbmin_2[1] +1;
                    else
                         if (bbmax_2[1]<bbmin_1[1]) no_overlap=1;
                         else overlap_y= (bbmax_1[1] < bbmax_2[1] ? bbmax_1[1] : bbmax_2[1]) - bbmin_1[1] +1;
                    if (no_overlap==0)
                    {
                         if (bbmin_1[2]<=bbmin_2[2])
                              if (bbmax_1[2]<bbmin_2[2]) no_overlap=1;
                              else overlap_z= (bbmax_2[2] < bbmax_1[2] ? bbmax_2[2] : bbmax_1[2]) - bbmin_2[2] +1;
                         else
                              if (bbmax_2[2]<bbmin_1[2]) no_overlap=1;
                              else overlap_z= (bbmax_1[2] < bbmax_2[2] ? bbmax_1[2] : bbmax_2[2]) - bbmin_1[2] +1;
                         if (no_overlap==0)
                         {
                              rec=overlap_x*overlap_y*overlap_z;
                              double div=( ((bbmax_1[0]-bbmin_1[0])*(bbmax_1[1]-bbmin_1[1])*(bbmax_1[2]-bbmin_1[2]) +1)
                              + ((bbmax_2[0]-bbmin_2[0])*(bbmax_2[1]-bbmin_2[1])*(bbmax_2[2]-bbmin_2[2]) +1));
                              rec=2 * rec / div;
                         }
                    }
               }
               if (rec>recmax)
               {
                    recmax=rec;
                    itM=itG;
               }
          }
          (*itM)->setProperty("activation", cc);
          (*itM)->setProperty("name", "activated");
     }

     FILE *fileActive, *fileNoise;
     fileActive=fopen("/home/olivier/statActivations.txt", "a");
     fileNoise=fopen("/home/olivier/statNoise.txt", "a");

     float lT, maxC, maxI, meanC, meanI, tVal;
     std::vector<float> AmaxI, AmeanI, AmaxC, AmeanC, AlT, AtV;
     std::vector<float> NmaxI, NmeanI, NmaxC, NmeanC, NlT, NtV;
     float AmaxIM=0.0, AmeanIM=0.0, AmaxCM=0.0, AmeanCM=0.0, AlTM=0.0, AtVM=0.0;
     float NmaxIM=0.0, NmeanIM=0.0, NmaxCM=0.0, NmeanCM=0.0, NlTM=0.0, NtVM=0.0;
     for (itG=primal->begin() ; itG!=primal->end(); ++itG)
     {
          (*itG)->getProperty("lifeTime", lT);
          (*itG)->getProperty("maxContrast", maxC);
          (*itG)->getProperty("maxIntensity", maxI);
          (*itG)->getProperty("meanContrast", meanC);
          (*itG)->getProperty("meanIntensity", meanI);
          (*itG)->getProperty("tValue", tVal);
          if ((*itG)->getProperty( "activation", cc))
          {
               fprintf(fileActive, "%.4f   %.4f   %.4f   %.4f   %.4f   %.4f\n", tVal, maxI, meanI, maxC, meanC, lT);
               AmaxI.push_back(maxI); AmaxIM+=maxI;
               AmeanI.push_back(meanI); AmeanIM+=meanI;
               AmaxC.push_back(maxC); AmaxCM+=maxC;
               AmeanC.push_back(meanC); AmeanCM+=meanC;
               AlT.push_back(lT); AlTM+=lT;
               AtV.push_back(tVal); AtVM+=tVal;
          }
          else
          {
               fprintf(fileNoise, "%.4f   %.4f   %.4f   %.4f   %.4f   %.4f\n", tVal, maxI, meanI, maxC, meanC, lT);
               NmaxI.push_back(maxI); NmaxIM+=maxI;
               NmeanI.push_back(meanI); NmeanIM+=meanI;
               NmaxC.push_back(maxC); NmaxCM+=maxC;
               NmeanC.push_back(meanC); NmeanCM+=meanC;
               NlT.push_back(lT); NlTM+=lT;
               NtV.push_back(tVal); NtVM+=tVal;
          }
     }
     fclose(fileActive); fclose(fileNoise);
     cout << "Writing graph with connected components set as 'name' of corresponding blobs" << endl;
     Writer<Graph > gW(rg.fileName());
     gW.write(*primal);
     cout << "Computing stats" << endl;
     int nA, nN;
     float AmaxIS=0.0, AmeanIS=0.0, AmaxCS=0.0, AmeanCS=0.0, AlTS=0.0, AtVS=0.0;
     float NmaxIS=0.0, NmeanIS=0.0, NmaxCS=0.0, NmeanCS=0.0, NlTS=0.0, NtVS=0.0;
     nA=AmaxI.size(); nN=NmaxI.size();
     AmaxIM/=float(nA);AmeanIM/=float(nA);AmaxCM/=float(nA);AmeanCM/=float(nA);AlTM/=float(nA);AtVM/=float(nA);
     NmaxIM/=float(nN);NmeanIM/=float(nN);NmaxCM/=float(nN);NmeanCM/=float(nN);NlTM/=float(nN);NtVM/=float(nN);
     int i;
     for (i=0; i<nA; i++)
     {
          AmaxIS+=(AmaxI[i]-AmaxIM)*(AmaxI[i]-AmaxIM);
          AmeanIS+=(AmeanI[i]-AmeanIM)*(AmeanI[i]-AmeanIM);
          AmaxCS+=(AmaxC[i]-AmaxCM)*(AmaxC[i]-AmaxCM);
          AmeanCS+=(AmeanC[i]-AmeanCM)*(AmeanC[i]-AmeanCM);
          AlTS+=(AlT[i]-AlTM)*(AlT[i]-AlTM);
          AtVS+=(AtV[i]-AtVM)*(AtV[i]-AtVM);
     }
     AmaxIS=sqrt(AmaxIS/float(nA-1));
     AmeanIS=sqrt(AmeanIS/float(nA-1));
     AmaxCS=sqrt(AmaxCS/float(nA-1));
     AmeanCS=sqrt(AmeanCS/float(nA-1));
     AlTS=sqrt(AlTS/float(nA-1));
     AtVS=sqrt(AtVS/float(nA-1));
     for (i=0; i<nN; i++)
     {
          NmaxIS+=(NmaxI[i]-NmaxIM)*(NmaxI[i]-NmaxIM);
          NmeanIS+=(NmeanI[i]-NmeanIM)*(NmeanI[i]-NmeanIM);
          NmaxCS+=(NmaxC[i]-NmaxCM)*(NmaxC[i]-NmaxCM);
          NmeanCS+=(AmeanC[i]-NmeanCM)*(NmeanC[i]-NmeanCM);
          NlTS+=(NlT[i]-NlTM)*(NlT[i]-NlTM);
          NtVS+=(NtV[i]-NtVM)*(NtV[i]-NtVM);
     }
     NmaxIS=sqrt(NmaxIS/float(nN-1));
     NmeanIS=sqrt(NmeanIS/float(nN-1));
     NmaxCS=sqrt(NmaxCS/float(nN-1));
     NmeanCS=sqrt(NmeanCS/float(nN-1));
     NlTS=sqrt(NlTS/float(nN-1));
     NtVS=sqrt(NtVS/float(nN-1));
     FILE *fileReport;
     fileReport=fopen("/home/olivier/reportBlobs.txt", "a");
     fprintf(fileReport, "Blobs stats : report\n");
     fprintf(fileReport, "--------------------\n\n");
     fprintf(fileReport, "Activations :\n");
     fprintf(fileReport, "maxI : \tmoy: %.4f\tsd : %.4f\n", AmaxIM, AmaxIS);
     fprintf(fileReport, "meanI : \tmoy: %.4f\tsd : %.4f\n", AmeanIM, AmeanIS);
     fprintf(fileReport, "maxC : \tmoy: %.4f\tsd : %.4f\n", AmaxCM, AmaxCS);
     fprintf(fileReport, "meanC : \tmoy: %.4f\tsd : %.4f\n", AmeanCM, AmeanCS);
     fprintf(fileReport, "lT : \tmoy: %.4f\tsd : %.4f\n", AlTM, AlTS);
     fprintf(fileReport, "tV : \tmoy: %.4f\tsd : %.4f\n", AtVM, AtVS);
     fprintf(fileReport, "\nNoise :\n");
     fprintf(fileReport, "maxI : \tmoy: %.4f\tsd : %.4f\n", NmaxIM, NmaxIS);
     fprintf(fileReport, "meanI : \tmoy: %.4f\tsd : %.4f\n", NmeanIM, NmeanIS);
     fprintf(fileReport, "maxC : \tmoy: %.4f\tsd : %.4f\n", NmaxCM, NmaxCS);
     fprintf(fileReport, "meanC : \tmoy: %.4f\tsd : %.4f\n", NmeanCM, NmeanCS);
     fprintf(fileReport, "lT : \tmoy: %.4f\tsd : %.4f\n", NlTM, NlTS);
     fprintf(fileReport, "tV : \tmoy: %.4f\tsd : %.4f\n", NtVM, NtVS);
     fprintf(fileReport, "\n\n\n");
     fclose(fileReport);
     cout << "Finished" << endl;
    }
  catch( user_interruption & )
    {
    }
  catch( exception & e )
    {
      cerr << e.what() << endl;
      return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
