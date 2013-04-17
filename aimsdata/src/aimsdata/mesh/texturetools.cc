#include <aims/mesh/texturetools.h>

using namespace std;
using namespace aims;

namespace aims
{

  std::size_t textureMax( const TimeTexture<short> & intex )
  {
    const Texture<short> & intex0 = intex.begin()->second;
    unsigned n_intexvertex = intex0.nItem();
    int max = 0;
    for (unsigned i = 0; i < n_intexvertex; ++i)
    {
      int current_val = intex0[i];
      if (current_val > max)
      {
        max = current_val;
      }
    }
    return std::size_t(max);
  }//textureMax


  std::vector< std::size_t > * labelsHistogram(
    const TimeTexture<short> & intex, std::size_t labels_nb, bool verbose )
  {
    std::vector< std::size_t > * labelsHisto_ptr = new std::vector<std::size_t>( labels_nb + 1, 0 );
    std::vector< std::size_t > & labels_histo = *labelsHisto_ptr;
    if (verbose)
      std::cout << "Reading Region (Gyrus) Labels... " << std::endl;
    std::size_t countLabel = 0;
    std::size_t label;
    std::size_t extra_labels = 0;
    std::size_t background_labels = 0;
    const Texture<short> & intex0 = intex.begin()->second;
    for (std::size_t i = 0; i < intex0.nItem(); ++i)
    {
      label = intex0.item(i); 
      if (label <= labels_nb && 1 <= label)
      {
        labels_histo[label]++;
      }
      else
      {
        if (label > labels_nb)
          extra_labels++;  // count labels greater than NUM_LABELS
        else
        {
          background_labels++; // count labels lower than 1 : background labels
        }
      }
    }
    for (std::size_t i = 0; i < labels_histo.size(); ++i)
    {
      if (labels_histo[i] != 0) countLabel++;
    }
  
    if (verbose)
    {
      std::cout << "Number of not-empty labels:" << countLabel << std::endl;
      std::cout << "Number of bad labels:   #labels > " << labels_nb << ": " << extra_labels << std::endl;
      std::cout << "Number of bad labels:   #labels < 1 : " << background_labels << std::endl;
    }
    if (verbose) std::cout << "OK" << std::endl;
    return labelsHisto_ptr;
  }//labelsHistogram

}//namespace aims

