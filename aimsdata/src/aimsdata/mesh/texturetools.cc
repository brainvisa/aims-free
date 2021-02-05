#include <aims/mesh/texturetools.h>

using namespace std;
using namespace aims;
using namespace carto;

namespace aims {

  //--------------
  //  textureMax
  //--------------
  int textureMax(const TimeTexture<short> & intex) {

    const Texture<short> & intex0 = intex.begin()->second;
    // Compute the vertices number in the texture
    unsigned n_intexvertex = intex0.nItem();

    // Search the largest label number
    int max = 0;
    for (unsigned i = 0; i < n_intexvertex; ++i) {
      int current_val = intex0[i];
      if (current_val > max) max = current_val;
    }

    return max;
  }

  //--------------
  //  TextureMin
  //--------------
  int textureMin(const TimeTexture<short> & intex) {

    const Texture<short> & intex0 = intex.begin()->second;
    // Compute the vertices number in the texture
    unsigned n_intexvertex = intex0.nItem();

    // Search the smallest label number
    int min = 0;
    for (unsigned i = 0; i < n_intexvertex; ++i) {
      int current_val = intex0[i];
      if (current_val < min) min = current_val;
    }

    return min;
  }

  //-------------------
  //  labelsHistogram
  //-------------------
  map<short, size_t> *labelsHistogram(const TimeTexture<short> &intex,
                                  int maxlabel,
                                  int minlabel,
                                  bool verbose) {

    map<short, size_t> *labelsHisto_ptr = new map<short, size_t>;

    size_t background_labels = 0;
    size_t extra_toplabels   = 0;
    size_t extra_lowlabels   = 0;
    int label                = 0;

    const Texture<short> & intex0 = intex.begin()->second;
 
    for (size_t i = 0; i < intex0.nItem(); ++i) {
      label = intex0.item(i);
      if (label == 0) {
        background_labels++;
      } else if (label <= maxlabel && label >= minlabel) {
        (*labelsHisto_ptr)[label]++;
      } else if (label > maxlabel) {
        extra_toplabels++;
      } else if (label < minlabel) {
        extra_lowlabels++;
      }
    }

    if (verbose) {
      cout << "Number of bad labels: #labels > " << maxlabel
        << ": " << extra_toplabels << endl;
      cout << "Number of bad labels: #labels < " <<  minlabel
        << ": " << extra_lowlabels << endl;
      cout << "Number of non identified labels (background): "
        << background_labels << endl;
    }

    return labelsHisto_ptr;
  }

  //-----------------
  //  giftiColormap
  //-----------------
  VolumeRef<AimsRGBA> giftiColormap( const carto::Object header )
  {
    VolumeRef<AimsRGBA> vol;
    Object labels_table;
    try
    {
      vector<string> labels;
      labels_table = header->getProperty( "GIFTI_labels_table" );

      // labels_table is a dictionary-like object with int keys.
      set<int32_t> keys;
      Object it = labels_table->objectIterator();
      for( ; it->isValid(); it->next() )
        keys.insert( it->intKey() );

      if( keys.empty() )
        return vol;

      set<int32_t>::const_iterator il, el = keys.end();
      int32_t tmin = *keys.begin();
      size_t i, j, n = *keys.rbegin() - tmin + 1;
      vol->reallocate( n );
      labels.resize( n );
      for( il=keys.begin(); il!=el; ++il )
        try
        {
          i = *il;
          Object label_map = labels_table->getArrayItem( i );
          if( label_map.isNull() )
            continue;

          string label = label_map->getProperty( "Label" )->getString();
          Object color = label_map->getProperty( "RGB" );
          labels[i - tmin] = label;
          AimsRGBA rgba;
          rgba[3] = 255;
          Object cit = color->objectIterator();
          for( j=0; cit->isValid() && j<4; ++j, cit->next() )
            rgba[j] = uint8_t( rint( cit->currentValue()->getScalar()
                                    * 255.9 ) );
          vol->at( i - tmin ) = rgba;
        }
        catch( ... )
        {
        }
      vol->header().setProperty( "labels", labels );

    }
    catch( ... )
    {
    }
    return vol;
  }

}//namespace aims

