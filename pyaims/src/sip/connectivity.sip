
namespace aims
{

  class Connectivity
  {
%TypeHeaderCode
#include <aims/connectivity/connectivity.h>
%End

  public:
    /// The different kinds of connectivity
    enum Type
      {
        CONNECTIVITY_4_XY,
        CONNECTIVITY_4_XZ,
        CONNECTIVITY_4_YZ,
        CONNECTIVITY_6_XYZ,
        CONNECTIVITY_8_XY,
        CONNECTIVITY_8_XZ,
        CONNECTIVITY_8_YZ,
        CONNECTIVITY_18_XYZ,
        CONNECTIVITY_26_XYZ,
        CONNECTIVITY_9_XY_Zminus,
        CONNECTIVITY_9_XY_Zplus,
        CONNECTIVITY_9_XZ_Yminus,
        CONNECTIVITY_9_XZ_Yplus,
        CONNECTIVITY_9_YZ_Xminus,
        CONNECTIVITY_9_YZ_Xplus,
        CONNECTIVITY_5_XminusY,
        CONNECTIVITY_5_XplusY,
        CONNECTIVITY_5_XYminus,
        CONNECTIVITY_5_XYplus,
        CONNECTIVITY_5_XminusZ,
        CONNECTIVITY_5_XplusZ,
        CONNECTIVITY_5_XZminus,
        CONNECTIVITY_5_XZplus,
        CONNECTIVITY_5_YminusZ,
        CONNECTIVITY_5_YplusZ,
        CONNECTIVITY_5_YZminus,
        CONNECTIVITY_5_YZplus,
        CONNECTIVITY_8_XYZ,
        CONNECTIVITY_4_XYdiag,
        CONNECTIVITY_4_XZdiag,
        CONNECTIVITY_4_YZdiag,
      };

    static aims::Connectivity::Type type_from_string(const std::string &);
    static std::string type_to_string(aims::Connectivity::Type);

    Connectivity( int oline, int oslice, aims::Connectivity::Type type);
    virtual ~Connectivity();

    aims::Connectivity::Type type() const;
    std::string type_string() const;
    int nbNeighbors() const;
    int offset( int n ) const;
    const Point3d& xyzOffset( int n ) const;
    const Point3df& dir( int n ) const;
  };
};

