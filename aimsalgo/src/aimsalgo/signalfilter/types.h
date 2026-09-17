#ifndef AIMS_SIGNALFILTER_TYPES_H
#define AIMS_SIGNALFILTER_TYPES_H

namespace aims {

  struct FilterType
  {
    enum Direction { Causal, AntiCausal, Symmetrical };
    enum Boundary { Zero, Nearest, Mirror };
    enum CopyType { InPlace, Copy };
  };

} // namespace aims

#endif // AIMS_SIGNALFILTER_TYPES_H
