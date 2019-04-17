from __future__ import print_function

from soma import aims
import six


class LazyReadData(object):

    '''
    LazyReadData is a data class proxy, which loads the underlying data when
    used, and is also able to unload it after a given number of operations to
    release memory.

    If the data is used again after release, then it is loaded again.

    The aim of this proxy is to carry data references in complex expressions or
    formulas, while allowing to lower the amount of memory needed to process
    the expression.

    Ex: if we need to add 100 Volumes, the easy way to write it is:

    ::

        volumes = [aims.read(f) for f in filenames]
        res = sum(volumes)

    This expression, ``sum(volumes)`` uses a complete list of volumes, and thus
    needs the 100 volumes to be physically in mempry before the sum operation
    actually begins. However as the sum is performed sequentially, it should be
    possible to perform the same operation using only memory for 2 volumes.

    One solution would use iterators and yield to read data during the for
    loop, but it would not work in a more "hand-made" expression like this one:

    ::

        res = vol1 + vol2 + vol3 - vol4 * vol5 + vol6  # etc.

    LazyReadData offers a solution to process these expressions:

    ::

        volumes = [LazyReadData(f, nops=1) for f in filenames]
        res = sum(volumes).data

        vol1 = LazyReadData(filenames[0], nops=1)
        # ...
        vol6 = LazyReadData(filename[5], nops=1)
        # etc.
        res = vol1 + vol2 + vol3 - vol4 * vol5 + vol6  # etc.
        res = res.data  # get actual Volume object

    LazyReadData loads the underlying data from its filename whenever any
    attribute or method of the proxy is queried in the underlying data. Reading
    is done using aims.read(), thus only AIMS objects are supported, but on the
    other hand, all kinds of AIMS objects can work this way: volumes, meshes,
    textures, graphs, transformations, etc.

    Without specifying the *nops* parameter, LazyReadData does not save so much
    memory: it just loads data whenever needed, but from this moment, keeps it
    in memory until the proxy is actually deleted. *nops* tells the proxy that,
    after this number of *operations*, the data will be released.

    *operations* in this context are arithmetic operators (+, -, *, /, pow).
    Other method calls are not counted.

    Thus in order to optimize things, *nops* should be set to the number of
    times the object will be used in an expression. A kind of pre-parsing of
    the expression may be needed in order to automatize this.
    '''

    def __init__(self, data_or_filename, allocator_context=None,
                 read_options=None, nops=0, reader=None, **kwargs):
        '''
        Parameters
        ----------
        data_or_filename: str, Aims object, or LazyReadData
            a LazyReadData can be built from another one (copying its data,
            filename and other internals), or from a filename, or from an
            existing AIMS object.
        allocator_context: AllocatorContext
            passed to aims.read() when data is read.
        read_options: dict
            passed to aims.read() when data is read
        nops: int
            number of operations before data is unloaded. 0 means never
            released.
        reader: aims.Reader
            pre-built Reader instance, used when more specific reader options
            are needed. Otherwise a standard reader will be used.
        kwargs: dict
            if data is an AIMS object, kwargs may include an additional
            'filename' argument. The rest is passed to aims.read() when data is
            read.
        '''
        # print('init', self)
        if isinstance(data_or_filename, six.types.StringTypes):
            self.filename = data_or_filename
            self.data = None
        elif isinstance(data_or_filename, LazyReadData):
            self.filename = data_or_filename.filename
            self.data = data_or_filename.data
            self.allocator_context = data_or_filename.allocator_context
            self.read_options = data_or_filename.read_options
            self.kwargs = data_or_filename.kwargs
            self.nops = data_or_filename.nops
            self.reader = data_or_filename.reader
            return
        else:
            self.data = data_or_filename
            if 'filename' in kwargs:
                self.filename = kwargs['filename']
                kwargs = dict(kwargs)
                del kwargs['filename']
            else:
                self.filename = None
        self.allocator_context = allocator_context
        self.read_options = read_options
        self.kwargs = kwargs
        self.nops = nops
        self.reader = reader

    def _lazy_read(self):
        if self.data is None:
            if self.reader is not None:
                self.data = self.reader.read(self.filename, **self.kwargs)
            else:
                self.data = aims.read(self.filename,
                                      allocmode=self.allocator_context,
                                      options=self.read_options, **self.kwargs)
            # print('read', self, ':', self.data)
        return self.data

    def _dec_release(self):
        if self.nops > 0:
            self.nops -= 1
            if self.nops == 0:
                # count reach 0, erase data to free memory
                # print('release', self, self.data)
                self.data = None
                # allow one additional operation
                self.nops = 1

    #def write(self):
        #if self.data is not None:
            #aims.write(self.data, self.filename)

    def __getattr__(self, name):
        self._lazy_read()
        return getattr(self.data, name)

    #def __del__(self):
        #if self.data is not None:
            #print('del', self, ':', self.data)

    def __add__(self, d):
        self._lazy_read()
        ld = None
        if isinstance(d, LazyReadData):
            d._lazy_read()
            ld = d
            d = d.data
        res = LazyReadData(self.data.__add__(d), filename=self.filename)
        self._dec_release()
        if ld is not None:
            ld._dec_release()
        return res

    def __radd__(self, d):
        self._lazy_read()
        ld = None
        if isinstance(d, LazyReadData):
            d._lazy_read()
            ld = d
            filename = d.filename
            d = d.data
        else:
            filename = None
        res = LazyReadData(self.data.__radd__(d), filename=filename)
        self._dec_release()
        if ld is not None:
            ld._dec_release()
        return res

    def __iadd__(self, d):
        self._lazy_read()
        ld = None
        if isinstance(d, LazyReadData):
            d._lazy_read()
            d = d.data
        self.data.__iadd__(d)
        if ld is not None:
            ld._dec_release()

    def __sub__(self, d):
        self._lazy_read()
        ld = None
        if isinstance(d, LazyReadData):
            d._lazy_read()
            ld = d
            d = d.data
        res = LazyReadData(self.data.__sub__(d), filename=self.filename)
        self._dec_release()
        if ld is not None:
            ld._dec_release()
        return res

    def __rsub__(self, d):
        self._lazy_read()
        ld = None
        if isinstance(d, LazyReadData):
            d._lazy_read()
            ld = d
            filename = d.filename
            d = d.data
        else:
            filename = None
        res = LazyReadData(self.data.__rsub__(d), filename=filename)
        self._dec_release()
        if ld is not None:
            ld._dec_release()
        return res

    def __isub__(self, d):
        self._lazy_read()
        ld = None
        if isinstance(d, LazyReadData):
            d._lazy_read()
            d = d.data
        self.data.__isub__(d)
        if ld is not None:
            ld._dec_release()

    def __mul__(self, d):
        self._lazy_read()
        ld = None
        if isinstance(d, LazyReadData):
            d._lazy_read()
            ld = d
            d = d.data
        res = LazyReadData(self.data.__mul__(d), filename=self.filename)
        self._dec_release()
        if ld is not None:
            ld._dec_release()
        return res

    def __rmul__(self, d):
        self._lazy_read()
        ld = None
        if isinstance(d, LazyReadData):
            d._lazy_read()
            ld = d
            filename = d.filename
            d = d.data
        else:
            filename = None
        res = LazyReadData(self.data.__rmul__(d), filename=filename)
        self._dec_release()
        if ld is not None:
            ld._dec_release()
        return res

    def __imul__(self, d):
        self._lazy_read()
        ld = None
        if isinstance(d, LazyReadData):
            d._lazy_read()
            d = d.data
        self.data.__imul__(d)
        if ld is not None:
            ld._dec_release()

    def __div__(self, d):
        self._lazy_read()
        ld = None
        if isinstance(d, LazyReadData):
            d._lazy_read()
            ld = d
            d = d.data
        res = LazyReadData(self.data.__div__(d), filename=self.filename)
        self._dec_release()
        if ld is not None:
            ld._dec_release()
        return res

    def __rdiv__(self, d):
        self._lazy_read()
        ld = None
        if isinstance(d, LazyReadData):
            d._lazy_read()
            ld = d
            filename = d.filename
            d = d.data
        else:
            filename = None
        res = LazyReadData(self.data.__rdiv__(d), filename=filename)
        self._dec_release()
        if ld is not None:
            ld._dec_release()
        return res

    def __idiv__(self, d):
        self._lazy_read()
        ld = None
        if isinstance(d, LazyReadData):
            d._lazy_read()
            d = d.data
        self.data.__idiv__(d)
        if ld is not None:
            ld._dec_release()

    def __pow__(self, d):
        self._lazy_read()
        ld = None
        res = LazyReadData(self.data.__pow__(d), filename=self.filename)
        self._dec_release()
        return res

    def __ipow__(self, d):
        self._lazy_read()
        self.data.__ipow__(d)

    def __neg__(self):
        self._lazy_read()
        return LazyReadData(-self.data, filename=filename)

    def __abs__(self):
        self._lazy_read()
        return LazyReadData(self.data.__abs__(), filename=filename)

