namespace aims
{

namespace knn
{

class Database
{
%TypeHeaderCode
  #include <aims/math/knn.h>
%End
public:
  Database();
  ~Database();

void init(SIP_PYOBJECT);
%MethodCode
	PyArrayObject	*a = NULL;
	
	if ((a0 == Py_None) or !PyArray_Check(a0))
	{
		sipIsErr = 1;
		PyErr_SetString(PyExc_TypeError, "wrong argument type for x");
	} else
	{	
		a = (PyArrayObject *) a0;
		if (!PyArray_ISCONTIGUOUS(a))
		{
			sipIsErr = 1;
			PyErr_SetString( PyExc_RuntimeError,
				"x must be a contiguous array");
      		} else {
			double	*x = NULL;
			int size = PyArray_DIMS( a )[0];
			int dim = PyArray_DIMS( a )[1];
    			x = (double *) PyArray_DATA( a );
			sipCpp->init(x, size, dim);
		}
	}
%End


};


class Knn /Abstract/
{
%TypeHeaderCode
  #include <aims/math/knn.h>
  #ifndef PYAIMSSIP_VECTOR_U32_DEFINED
  #define PYAIMSSIP_VECTOR_U32_DEFINED
  typedef std::vector<unsigned int> vector_U32;
  #endif
  #ifndef PYAIMSSIP_VECTOR_DOUBLE_DEFINED
  #define PYAIMSSIP_VECTOR_DOUBLE_DEFINED
  typedef std::vector<double> vector_DOUBLE;
  #endif
%End

  public:
    Knn(aims::knn::Database &db, unsigned int k);
    virtual ~Knn();

    SIP_PYTUPLE find(vector_DOUBLE &v) = 0 /ReleaseGIL/;
%MethodCode
    std::pair<std::vector<unsigned int>, std::vector<double> > res;
    res = sipCpp->find(*a0);
    std::vector<unsigned int> *ind = new std::vector<unsigned int>(res.first);
    std::vector<double> *dist = new std::vector<double>(res.second);
    PyObject *tuple = PyTuple_New(2);
    PyTuple_SetItem(tuple, 0, sipConvertFromType(ind,
		sipType_vector_U32, Py_None));
    PyTuple_SetItem(tuple, 1, sipConvertFromType(dist,
		sipType_vector_DOUBLE, Py_None));
    sipRes = tuple;
%End

};

class KnnBruteForce : aims::knn::Knn
{
%TypeHeaderCode
  #include <aims/math/knn.h>
%End

  public:
    KnnBruteForce(aims::knn::Database &db, unsigned int k);
    virtual ~KnnBruteForce();
};

class KnnFriedman : aims::knn::Knn /Abstract/
{
%TypeHeaderCode
  #include <aims/math/knn.h>
%End

  public:
    KnnFriedman(aims::knn::Database &db, unsigned int k);
    virtual ~KnnFriedman();

    virtual void precompute() = 0;
};

class KnnGlobalFriedman : aims::knn::Knn
{
%TypeHeaderCode
  #include <aims/math/knn.h>
%End

  public:
    KnnGlobalFriedman(aims::knn::Database &db, unsigned int k);
    virtual ~KnnGlobalFriedman();

    void precompute();
};


};

};
