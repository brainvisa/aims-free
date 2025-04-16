#  This software and supporting documentation are distributed by
#      Institut Federatif de Recherche 49
#      CEA/NeuroSpin, Batiment 145,
#      91191 Gif-sur-Yvette cedex
#      France
#
# This software is governed by the CeCILL-B license under
# French law and abiding by the rules of distribution of free software.
# You can  use, modify and/or redistribute the software under the
# terms of the CeCILL-B license as circulated by CEA, CNRS
# and INRIA at the following URL "http://www.cecill.info".
#
# As a counterpart to the access to the source code and  rights to copy,
# modify and redistribute granted by the license, users are provided only
# with a limited warranty  and the software's author,  the holder of the
# economic rights,  and the successive licensors  have only  limited
# liability.
#
# In this respect, the user's attention is drawn to the risks associated
# with loading,  using,  modifying and/or developing or reproducing the
# software by the user in light of its specific status of free software,
# that may mean  that it is complicated to manipulate,  and  that  also
# therefore means  that it is reserved for developers  and  experienced
# professionals having in-depth computer knowledge. Users are therefore
# encouraged to load and test the software's suitability as regards their
# requirements in conditions enabling the security of their systems and/or
# data to be ensured and,  more generally, to use and operate it in the
# same conditions as regards security.
#
# The fact that you are presently reading this means that you have had
# knowledge of the CeCILL-B license and that you accept its terms.

import numpy
import sys


aims = sys.modules['soma.aims']

__all__ = []


def _init_cache(self):
    self._cache = {}
    self._cache_tree(self, [])


def _cache_tree(self, tree, parents):
    for t in tree.children():
        p = [t] + list(parents)
        try:
            name = t['name']
            self._cache[name] = p
        except KeyError:
            pass
        self._cache_tree(t, p)


def find_color(self, name, default_color=KeyError):
    try:
        stack = self.find(name)
    except KeyError:
        if default_color is KeyError:
            raise
        return default_color
    if stack is None:
        raise KeyError("unknown label '%s'" % name)

    for t in stack:
        try:
            color = numpy.array(t['color'], dtype='f')
            color /= 255.
            return color
        except KeyError:
            continue
    if default_color is KeyError:
        raise KeyError('label %s has no color in the nomenclature' % name)
    return default_color


def find(self, name):
    try:
        c = self._cache
    except AttributeError:
        self._init_cache()
        return self.find(name)
    try:
        return c[name]
    except KeyError:
        return None


aims.Hierarchy._cache_tree = _cache_tree
aims.Hierarchy._init_cache = _init_cache
aims.Hierarchy.find_color = find_color
aims.Hierarchy.find = find
