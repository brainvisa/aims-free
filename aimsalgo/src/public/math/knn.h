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

#ifndef AIMS_MATH_KNN_H
#define AIMS_MATH_KNN_H

#include <cstdlib>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <string.h>

namespace aims
{

namespace knn
{

class Distance
{
public:
	Distance() {};
	virtual ~Distance() {};

	virtual inline const std::string &name(void) const
	{
		return _name;
	}

	virtual double	operator()(const double *v1,
		const double *v2, unsigned int dim) const = 0;

	virtual double	operator()(const std::vector<double> &v1,
			const std::vector<double> &v2) const
	{
		return (*this)(&v1[0], &v2[0], v1.size());
	}
protected:
	std::string	_name;
};

class EuclidianDistance : public Distance
{
public:
	EuclidianDistance()
	{
		_name = "euclidian";
	}
	virtual ~EuclidianDistance() {};

	virtual double	operator()(const double *v1,
		const double *v2, unsigned int dim) const
	{
		double	d, s = 0.;

		for (unsigned int i = 0; i < dim; ++i)
		{
			d = v1[i] - v2[i];
			s += d * d;
		}
		return sqrt(s);
	}
};

class SquaredEuclidianDistance : public Distance
{
public:
	SquaredEuclidianDistance()
	{
		_name = "squared_euclidian";
	}
	virtual ~SquaredEuclidianDistance() {};

	virtual double	operator()(const double *v1,
		const double *v2, unsigned int dim) const
	{
		double	d, s = 0.;

		for (unsigned int i = 0; i < dim; ++i)
		{
			d = v1[i] - v2[i];
			s += d * d;
		}
		return s;
	}
};

class ManhattanDistance : public Distance
{
public:
	ManhattanDistance()
	{
		_name = "manhattan";
	}
	virtual ~ManhattanDistance() {};

	virtual double	operator()(const double *v1,
		const double *v2, unsigned int dim) const
	{
		double	s = 0.;

		for (unsigned int i = 0; i < dim; ++i)
			s += fabs(v1[i] - v2[i]);
		return s;
	}
};


class TchebychevDistance : public Distance
{
public:
	TchebychevDistance()
	{
		_name = "tchebychev";
	}
	virtual ~TchebychevDistance() {};

	virtual double	operator()(const double *v1,
		const double *v2, unsigned int dim) const
	{
		double	d = 0., max = 0.;

		for (unsigned int i = 0; i < dim; ++i)
		{
			d = fabs(v1[i] - v2[i]);
			if (d  > max) max = d;
		}
		return max;
	}
};


class Database
{
public:
	typedef double *	value_type;
	typedef double **	pointer;
	typedef double *&	reference;


	Database();
	Database(double *data, unsigned int size, unsigned int dim);
	virtual ~Database() {};

	void init(double *data, unsigned int size, unsigned int dim);
	int search(const std::vector<double> &v,
					unsigned int dim) const;
	int search_with_hole(const std::vector<double> &v,
					unsigned int dim) const;


	class Vector
	{
	public:
		Vector(Database *db, unsigned int ind)
		{
			init(db, ind);
		}
		Vector(const Vector &v)
		{
			_dim = v._dim;
			_data = (double *) malloc(sizeof(double) * _dim);
			memcpy(_data, v._data, sizeof(double) * _dim);
			_own_data = true;
		}

		double	*operator*(void)
		{
			return _data;
		};

		const double	*operator*(void) const
		{
			return _data;
		};

		virtual ~Vector()
		{
			this->free();
		}

		inline void init(Database *db, unsigned int ind)
		{
			if (db != NULL)
			{
				_data = (*db)[ind];
				_dim = db->dim();
			}
			else
			{
				_data = NULL;
				_dim = 0;
			}
			_own_data = false;
		}

		inline void free()
		{
			if (_own_data) ::free(_data);
		}

		inline double operator()(unsigned int dim) const
		{
			return _data[dim];
		}

		inline void update(const Vector &v)
		{	
			_data = v._data;
			_dim = v._dim;
			_own_data = false;
		}

		inline void update(Database *db, unsigned int ind)
		{	
			this->free();
			init(db, ind);
		}

		inline Vector &operator =(const Vector &v)
		{	
			_dim = v._dim;
			if (_data == NULL)
			{
				_data = (double *)malloc(sizeof(double)* _dim);
				_own_data = true;
			}
			memcpy(_data, v._data, sizeof(double) * _dim);
			return *this;
		}
	private:
		bool			_own_data;
		double			*_data;
		unsigned int		_dim;
	};

	class DatabaseCompare
	{
	public:
		DatabaseCompare(unsigned int dim) : _dim(dim) {};

		bool operator()(const Vector &v1, const Vector &v2) const
		{
			return v1(_dim) < v2(_dim);
		}

	private:
		unsigned int _dim;
	};

	class iterator
	{
	public:
		typedef std::ptrdiff_t			difference_type;
		typedef std::bidirectional_iterator_tag	iterator_category;
		typedef Vector				value_type;
		typedef Vector*				pointer;
		typedef Vector&				reference;



		iterator() : _db(NULL), _ind(0), _vector(Vector(NULL, 0)) {}
		iterator(Database *db, unsigned int ind=0) :
			_db(db), _ind(ind), _vector(Vector(db, _ind)){}
		iterator(const iterator &it) : _db(it._db), _ind(it._ind),
			_vector(it._vector) {};
		~iterator() {};

		inline iterator &operator =(const iterator &it)
		{
			_ind = it._ind;
			_db = it._db;
			return *this;
		}

		bool operator!=(const iterator & it)
		{
			return _ind != it._ind;
		};

		bool operator==(const iterator & it)
		{
			return _ind == it._ind;
		};

		iterator operator++(void)
		{
			if (_ind < _db->size()) ++_ind;
			return *this;
		};

		iterator operator++(int)
		{
			const iterator tmp = *this;
			if (_ind < _db->size()) _ind++;
			return tmp;
		};

		iterator operator--(void)
		{
			--_ind;
			return *this;
		};

		iterator operator--(int)
		{
			const iterator tmp = *this;
			_ind--;
			return tmp;
		};

		int operator-(const iterator &it)
		{
			return _ind - it._ind;
		};

		int operator+(const iterator &it)
		{
			return _ind + it._ind;
		};

		iterator operator-(unsigned int ind)
		{
			return iterator(_db, _ind - ind);
		};

		iterator operator+(unsigned int ind)
		{
			return iterator(_db, _ind + ind);
		};

		Vector	&operator*(void)
		{
			_vector.update(_db, _ind);
			return _vector;
		};

		bool	operator<(const iterator &it)
		{
			return _ind < it._ind;
		};

	private:
		Database	*_db;
		unsigned int	_ind;
		Vector		_vector;
	};

	iterator	begin(void)
	{
		return iterator(this, 0);
	}

	iterator	end(void)
	{
		return iterator(this, _size);
	}

	inline unsigned int	size(void) const
	{
		return _size;
	}

	inline unsigned int	dim(void) const
	{
		return _dim;
	}

	inline double	operator()(unsigned int  x, unsigned int y) const
	{
		return _data[x * _dim + y];
	}

	inline double *operator[](unsigned int ind)
	{
		return &_data[ind * _dim];
	}

	inline const double *operator[](unsigned int ind) const
	{
		return &_data[ind * _dim];
	}

	double	variance_along_dim(unsigned int dim) const;

	void	sort(unsigned int dim);

	inline const	std::vector<bool> &holes() const
	{
		return _holes;
	}

	inline void setHole(unsigned int ind, bool status)
	{
		_holes[ind] = status;
	}

	inline void removeHoles(void)
	{
		_holes = std::vector<bool>(_size, false);
	}

protected:
	double			*_data;
	unsigned int		_size;
	unsigned int		_dim;
	std::vector<bool>	_holes;
};


class MultiDatabase : public Database
{
public:
	MultiDatabase(double *data, unsigned int n, unsigned int dim) :
		Database(data, n, dim) {};
};

class Heap
{
public:
	class MiniHeap
	{
	public:
		MiniHeap(double key, const void *value) :
			_key(key), _value(value) {};
		virtual ~MiniHeap() {};

		MiniHeap(const MiniHeap &h) : _key(h._key), _value(h._value) {}

		inline MiniHeap &operator =(const MiniHeap &h)
		{
			_key = h._key;
			_value = h._value;
			return *this;
		}

		inline double key() const
		{
			return _key;
		}

		inline const void *value() const
		{
			return _value;
		}

		inline bool operator < (const MiniHeap &h) const
		{
			return _key < h._key;
		}

		inline bool operator > (const MiniHeap &h) const
		{
			return _key > h._key;
		}

	private:
		double		_key;
		const void	*_value;
	};

	Heap(unsigned int n) : _n(0), _limit(n),
		_list(std::vector<MiniHeap>(n + 1,
			MiniHeap(HUGE_VAL, NULL))) {};
	virtual ~Heap() {};

	void	exchange(unsigned int a, unsigned int b);
	void	erase_worst(void);
	void	insert(double key, const void *value);
	std::pair<std::vector<unsigned int>, std::vector<double> >
	toVectors(const Database &_db);

	const MiniHeap &operator[](unsigned int ind) const
	{
		return _list[ind];
	}

	inline double	get_worst_value()
	{
		return _list[0].key();
	}

private:
	unsigned int		_n;
	unsigned int		_limit;
	std::vector<MiniHeap>	_list;
};

class Knn
{
public:
	Knn(Database &db, unsigned int k,
		Distance *distance = new SquaredEuclidianDistance()) :
		_db(db), _k(k), _distance(distance), _distance_n(-1) {};
	virtual ~Knn() {};

	virtual std::pair<std::vector<unsigned int>, std::vector<double> >
	find(const std::vector<double> &v) = 0;

protected:
	/** database wrapper of data */
	Database	&_db;
	/** k : number of nearest neighbours */	
	unsigned int	_k;
	/** distance used in nearest neighbours computations */
	Distance  *_distance;
	/** number of computed distance */
	int		_distance_n;
};

class KnnBruteForce : public Knn
{
public:
	KnnBruteForce(Database &db, unsigned int k,
		Distance *distance = new SquaredEuclidianDistance()) :
		Knn(db, k, distance) {};
	virtual ~KnnBruteForce() {};

	virtual std::pair<std::vector<unsigned int>, std::vector<double> >
	find(const std::vector<double> &v);
};

class KnnFriedman : public Knn
{
public:
	KnnFriedman(Database &db, unsigned int k,
		Distance *distance = new SquaredEuclidianDistance()) :
		Knn(db, k, distance) {};
	virtual ~KnnFriedman() {};

	virtual void precompute(void) = 0;
	virtual std::pair<std::vector<unsigned int>, std::vector<double> >
	find(const std::vector<double> &v) = 0;
};

class KnnGlobalFriedman : public KnnFriedman
{
public:
	KnnGlobalFriedman(Database &db, unsigned int k,
		Distance *distance = new SquaredEuclidianDistance()) :
		KnnFriedman(db, k, distance) {};
	virtual ~KnnGlobalFriedman() {};

	virtual void precompute(void);
	std::pair<std::vector<unsigned int>, std::vector<double> >
	find(const std::vector<double> &v);

private:
	unsigned int _best_dim;
};

}; /* end of namespace knn */
}; /* end of namespace aims */

#endif
