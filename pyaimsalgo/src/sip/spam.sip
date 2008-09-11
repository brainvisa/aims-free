
namespace aims
{
class SpamBase /Abstract/
{
%TypeHeaderCode
  #include <aims/spam/spam.h>
%End
  public:
    SpamBase();
    virtual ~SpamBase();

    void set_img_density(Volume_FLOAT *img_density);
    void set_bb_talairach_offset(const Point3dd &bb_talairach_offset);
    void set_bb_talairach_size(const Point3dd &bb_talairach_size);

    virtual SIP_PYTUPLE
    prodlikelihoods(const Volume_DOUBLE &X, float shift) = 0 /ReleaseGIL/;
%MethodCode
    std::pair<double, double> res = sipCpp->prodlikelihoods(*a0, a1);
    PyObject *tuple = PyTuple_New(2);
    PyTuple_SetItem(tuple, 0, PyFloat_FromDouble(res.first));
    PyTuple_SetItem(tuple, 1, PyFloat_FromDouble(res.second));
    sipRes = tuple;
%End
};

class SpamFromLikelihood : aims::SpamBase
{
%TypeHeaderCode
  #include <aims/spam/spam.h>
%End
  public:
    SpamFromLikelihood();
    virtual ~SpamFromLikelihood();
};

class Spam : aims::SpamBase
{
%TypeHeaderCode
  #include <aims/spam/spam.h>
%End
  public:
    Spam();
    virtual ~Spam();
};

};
