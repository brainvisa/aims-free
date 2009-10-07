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

#include <aims/math/knn.h>
#include <assert.h>

namespace aims
{

namespace knn
{

/*****************************************************************************/
Database::Database() {};

Database::Database(double *data, unsigned int size, unsigned int dim) :
	_data(data), _size(size), _dim(dim), _holes(size, false) {};

void Database::init(double *data, unsigned int size, unsigned int dim)
{
	_data = data;
       	_size = size;
	_dim = dim;
	_holes = std::vector<bool>(size, false);
}

void	Database::sort(unsigned int dim)
{
	iterator	i = begin();
	iterator	e = end();
	std::sort(i, e, DatabaseCompare(dim));
}

double	Database::variance_along_dim(unsigned int dim) const
{
	double	sd = 0., mean = 0., val;

	for (unsigned int i = 0; i < _size; ++i)
	{
		val = (*this)(i, dim);
		mean += val;
		sd += val * val;
	}
	mean /= _size;
	sd /= _size;
	sd -= mean * mean;
	return sd;
}


int Database::search(const std::vector<double> &v,
					unsigned int dim) const
{
	int             g = 0;
	int             d = size() - 1;
	int             m = (d + g) / 2;

	while (m != g && m != d)
	{
		if ((*this)(m, dim) > v[dim])
			d = m;
		else    g = m;
		m = (d + g) / 2;
	}

	return g;
}


int Database::search_with_hole(const std::vector<double> &v,
					unsigned int dim) const
{
	int             g = 0;
	int             d = size() - 1;
	int             m;

	while (_holes[g] && g < d) g++;
	while (_holes[d] && d > g) d--;
	/** si le tableau est rempli de trous */
	if (g >= d) return -1;
	m = (d + g) / 2;
	while (_holes[m]) m++;

	while (m != g && m != d)
	{
		/** cette boucle est censé s'arrêter au pire pour (m == d). */
		if ((*this)(m, dim) > v[dim])
			d = m;
		else    g = m;
		m = (d + g) / 2;
		while (_holes[m]) m++;
	}
	return g;
}

/*****************************************************************************/
void	Heap::exchange(unsigned int a, unsigned int b)
{
	assert((a < _n + 1) and (b < _n + 1));
	MiniHeap	&ha = _list[a];
	MiniHeap	&hb = _list[b];
	MiniHeap	ht = ha;
	ha = hb;
	hb = ht;
};

void	Heap::erase_worst(void)
{
	unsigned int	f;

	_n--;
	_list[0] = _list[_n];

	for (unsigned int i = 0; i < _n / 2;)
	{
		if ((i * 2 + 1 == _n - 1) or
			(_list[i * 2 + 1] > _list[i * 2 + 2]))
			f = i * 2 + 1;
		else	f = i * 2 + 2;
		if (_list[f] > _list[i])
		{
			exchange(f, i);
			i = f;
		}
		else break;
	}
}

void	Heap::insert(double key, const void *value)
{
	_list[_n] = MiniHeap(key, value);

	for (unsigned int i = _n; i > 0;)
	{
		unsigned int	p = (i - 1) / 2;
		if (_list[p] < _list[i])
		{
			exchange(p, i);
			i = p;
		}
		else break;
	}
	_n++;
	if (_n > _limit) erase_worst();
}

std::pair<std::vector<unsigned int>, std::vector<double> >
Heap::toVectors(const Database &_db)
{
	std::vector<unsigned int>	v(_n);
	std::vector<double>		d(_n);

	Heap	tmp(*this);
	for (unsigned int i = 0; i < _n; ++i)
	{
		const MiniHeap	&mh = tmp[0];
		v[i] = ((const double *) mh.value() - _db[0]) /_db.dim();
		d[i] = mh.key();
		tmp.erase_worst();
	}
	return std::pair<std::vector<unsigned int>,std::vector<double> >(v, d);
}

/*****************************************************************************/
std::pair<std::vector<unsigned int>, std::vector<double> >
KnnBruteForce::find(const std::vector<double> &v)
{
	unsigned int	dim = _db.dim();
	double		d;
	unsigned int	i;
	const double	*v2 = &v[0];
	Heap		heap(_k);

	for (i = 0; i < _k; i++)
	{
		d = (*_distance)(v2, _db[i], dim);
		heap.insert(d, _db[i]);
	}
	for (; i < _db.size(); i++)
	{
		d = (*_distance)(v2, _db[i], dim);
		if (d < heap.get_worst_value())
			heap.insert(d, _db[i]);
	}
	_distance_n = _db.size();
	return heap.toVectors(_db);
}

void KnnGlobalFriedman::precompute(void)
{
	unsigned int		_dim = _db.dim();
	unsigned int		best_dim = 0;
	unsigned int		i;
	std::vector<double>	sd(_dim);

	/** Calcul de la dispersion des données sur chacune des dimensions. */
	for (i = 0; i < _dim; i++)
		sd[i] = _db.variance_along_dim(i);
	for (i = 1; i < _dim; i++)
		if(sd[i] > sd[best_dim])
			best_dim = i;

	/** Trie de la base de données en fonction de la dimension de plus
	 *          * grande dispersion. */
	_db.sort(best_dim);
	_best_dim = best_dim;
}

std::pair<std::vector<unsigned int>, std::vector<double> >
KnnGlobalFriedman::find(const std::vector<double> &v)
{
	unsigned int	dim = _db.dim();
	unsigned int	size = _db.size();
	const double	*v2 = &v[0];
	double		rd;
	unsigned int	g, c, d, id;
	bool		is_squared_dist = (_distance->name()
					== "squared_euclidian");
	const std::vector<bool> &holes = _db.holes();
	std::vector<double>	v3(dim);
	std::vector<double>	dist(size, -1.);
	Heap		heap(_k);

	_distance_n = 0;
	
	for (unsigned int j = 0; j < _k; j++)
	{
		c = _db.search_with_hole(v, _best_dim);
		rd = (*_distance)(v2, _db[c], dim);
		if (is_squared_dist) rd = sqrt(rd);

		d = c + 1;
		while (holes[d] && d < size) d++;
		if (d < size)
		{
			double rd2 = (*_distance)(v2, _db[d], dim);
			if (is_squared_dist) rd2 = sqrt(rd2);
			if (rd2 < rd)
			{
				rd = rd2;
				c = d;
			}
		}
		dist[c] = rd;
		v3[_best_dim] = v[_best_dim] - rd;
		g = _db.search_with_hole(v3, _best_dim);
		v3[_best_dim] = v[_best_dim] + rd;
		d = _db.search_with_hole(v3, _best_dim) + 2;
		if (d >= size) d = size - 1;
		id = c;

		for (unsigned int i = g; i < d; i++)
		{
			//pour sauter les trous qu'on a fait dans la base.
			if (holes[i]) continue;
			if (dist[i] == -1)
			{
				dist[i] = (*_distance)(v2, _db[i], dim);
				if (is_squared_dist) dist[i] = sqrt(dist[i]);
				_distance_n++;
			}
			if (dist[i] < dist[id])
				id = i;
		}
		if (is_squared_dist)
			heap.insert(dist[id] * dist[id], _db[id]);
		else	heap.insert(dist[id], _db[id]);
		_db.setHole(id, true);
	}
	_db.removeHoles();
	return heap.toVectors(_db);
}

}; /* end of namespace knn */
}; /* end of namespace aims */
