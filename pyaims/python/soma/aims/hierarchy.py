import exceptions, numpy, sys
aims = sys.modules[ 'soma.aims']

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
		except exceptions.KeyError: pass
		self._cache_tree(t, p)

def find_color(self, name):
	try:
		stack = self.find(name)
	except exceptions.KeyError:
		return [1, 1, 1, 0.1]
	if stack is None: raise KeyError("unknown label '%s'" % name)
	
	for t in stack:
		try:
			color = numpy.array(t['color'], dtype='f')
			color /= 255.
			return color
		except exceptions.KeyError: continue
	return [1, 1, 1, 0.1]

def find(self, name):
	try:
		c = self._cache
	except exceptions.AttributeError:
		self._init_cache()
		return self.find(name)
	try: return c[name]
	except exceptions.KeyError: return None
	

aims.Hierarchy._cache_tree = _cache_tree
aims.Hierarchy._init_cache = _init_cache
aims.Hierarchy.find_color = find_color
aims.Hierarchy.find = find
