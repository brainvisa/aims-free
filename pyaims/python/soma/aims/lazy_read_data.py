'''
This module provides wrappers for Aims readable data types which lazily load when they are used, and can release memory after they are used: :class:`LazyReadData`

Specialized iterators can help parallelizing reading opertions, and perform it earlier (before they are really used) in an iteration: :class:`PreloadIterator`, :class:`PreloadList`.

A specialized version in aimsalgo handles resampling while loading: :class:`~soma.aimsalgo.lazy_resample_volume.LazyResampleVolume`.

'''

from soma import aims
import threading
import itertools
import multiprocessing
import time


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
    needs the 100 volumes to be physically in memory before the sum operation
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

    Loading is done in a thread-safe manner (using a lock) so that two (or
    more) threads accessing data will not trigger several loads.

    **Specializing**

    Subclasses may override the :meth:`_lazy_read` method to implement a
    different behavior or load additional data. This method should set
    self.data with the loaded data. This method returns the loaded data.

    Another way of specializing the load behavior is to provide a Reader object
    which could also be a specialized version of :class:`soma.aims.Reader`.

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
        self._loaded = False
        if isinstance(data_or_filename, str):
            self.filename = data_or_filename
            self.data = None
        elif isinstance(data_or_filename, LazyReadData):
            self.filename = data_or_filename.filename
            with data_or_filename._lock:
                self.data = data_or_filename.data
                self._loaded = data_or_filename._loaded
            self.allocator_context = data_or_filename.allocator_context
            self.read_options = data_or_filename.read_options
            self.kwargs = data_or_filename.kwargs
            self.nops = data_or_filename.nops
            self.reader = data_or_filename.reader
            return
        else:
            self.data = data_or_filename
            self._loaded = True
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
        self._lock = threading.RLock()
        self._loading = False

    def _lazy_read(self):
        '''
        Internal mechanism of data reading. It may be called from a non-
        principal thread when used in a threaded context such as in
        :class:`PreloadIterator`.
        '''
        with self._lock:
            if self._loaded:
                return self.data
        with self._lock:
            self._loading = True
        try:
            self.load_data()
        finally:
            with self._lock:
                self._loading = False
                self._loaded = True
        # print('read done:', self, ':', self.data)
        return self.data

    def load_data(self):
        '''
        Implements actual data reading. The default implementation calls
        self.reader.read() if a Reader instance has been provided, or aims.read
        otherwise. You normally do not call this directly, but it will be
        triggered, either by a threaded preloading, or via get_data().
        '''
        if self.reader is not None:
            self.data = self.reader.read(self.filename, **self.kwargs)
        else:
            self.data = aims.read(self.filename,
                                  allocmode=self.allocator_context,
                                  options=self.read_options,
                                  **self.kwargs)

    def get_data(self):
        '''
        Get the underlying data object, and load it beforehand if not already
        done, in a thread-safe way.
        '''
        loading = False
        with self._lock:
            if self._loaded:
                return self.data
            loading = self._loading
            # reading in another thread
        if loading:
            while loading:
                time.sleep(0.01)
                with self._lock:
                    loading = self._loading
            return self.data
        # read it (in current thread, possibly main thread)
        return self._lazy_read()

    def _dec_release(self):
        with self._lock:
            if self.nops > 0:
                self.nops -= 1
                if self.nops == 0:
                    # count reach 0, erase data to free memory
                    # print('release', self, self.data)
                    self.data = None
                    # allow one additional operation
                    self.nops = 1
                    with self._lock:
                        self._loading = False
                        self._loaded = False

    def preloading(self):
        ''' If a threaded load operation has been started ("preloading"), then
        this method returns True as soon as the operation has started. It still
        returns True as long as the data is in memory. Its goal is to tell that
        another load operation is not needed.
        '''
        with self._lock:
            return self._loading

    def loaded(self):
        with self._lock:
            return self._loaded

    def not_started(self):
        with self._lock:
            return not self._loaded and not self._loading

    #def write(self):
        #if self.data is not None:
            #aims.write(self.data, self.filename)

    def __getattr__(self, name):
        self.get_data()
        return getattr(self.data, name)

    #def __del__(self):
        #if self.data is not None:
            #print('del', self, ':', self.data)

    def __add__(self, d):
        self.get_data()
        ld = None
        if isinstance(d, LazyReadData):
            d.get_data()
            ld = d
            d = d.data
        res = LazyReadData(self.data.__add__(d), filename=self.filename)
        self._dec_release()
        if ld is not None:
            ld._dec_release()
        return res

    def __radd__(self, d):
        self.get_data()
        ld = None
        if isinstance(d, LazyReadData):
            d.get_data()
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
        self.get_data()
        ld = None
        if isinstance(d, LazyReadData):
            d.get_data()
            d = d.data
        self.data.__iadd__(d)
        if ld is not None:
            ld._dec_release()
        return self

    def __sub__(self, d):
        self.get_data()
        ld = None
        if isinstance(d, LazyReadData):
            d.get_data()
            ld = d
            d = d.data
        res = LazyReadData(self.data.__sub__(d), filename=self.filename)
        self._dec_release()
        if ld is not None:
            ld._dec_release()
        return res

    def __rsub__(self, d):
        self.get_data()
        ld = None
        if isinstance(d, LazyReadData):
            d.get_data()
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
        self.get_data()
        ld = None
        if isinstance(d, LazyReadData):
            d.get_data()
            d = d.data
        self.data.__isub__(d)
        if ld is not None:
            ld._dec_release()
        return self

    def __mul__(self, d):
        self.get_data()
        ld = None
        if isinstance(d, LazyReadData):
            d.get_data()
            ld = d
            d = d.data
        res = LazyReadData(self.data.__mul__(d), filename=self.filename)
        self._dec_release()
        if ld is not None:
            ld._dec_release()
        return res

    def __rmul__(self, d):
        self.get_data()
        ld = None
        if isinstance(d, LazyReadData):
            d.get_data()
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
        self.get_data()
        ld = None
        if isinstance(d, LazyReadData):
            d.get_data()
            d = d.data
        self.data.__imul__(d)
        if ld is not None:
            ld._dec_release()
        return self

    def __div__(self, d):
        self.get_data()
        ld = None
        if isinstance(d, LazyReadData):
            d.get_data()
            ld = d
            d = d.data
        res = LazyReadData(self.data.__div__(d), filename=self.filename)
        self._dec_release()
        if ld is not None:
            ld._dec_release()
        return res

    def __rdiv__(self, d):
        self.get_data()
        ld = None
        if isinstance(d, LazyReadData):
            d.get_data()
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
        self.get_data()
        ld = None
        if isinstance(d, LazyReadData):
            d.get_data()
            d = d.data
        self.data.__idiv__(d)
        if ld is not None:
            ld._dec_release()
        return self

    def __pow__(self, d):
        self.get_data()
        res = LazyReadData(self.data.__pow__(d), filename=self.filename)
        self._dec_release()
        return res

    def __ipow__(self, d):
        self.get_data()
        self.data.__ipow__(d)
        return self

    def __neg__(self):
        self.get_data()
        return LazyReadData(-self.data, filename=self.filename)

    def __abs__(self):
        self.get_data()
        return LazyReadData(self.data.__abs__(), filename=self.filename)


class PreloadIterator(object):
    '''
    An iterator intended to be used to iterate over sequences of LazyReadData,
    which performs pre-iterations and pre-loads data before they get used in an
    actual iteration.

    **Idea:**

    When iterating over a list of LazyReadData, data is loaded when accessed,
    thus at the last moment, sequentially. As data loading can be efficiently
    threaded, the idea is to use threads to start preloading of a number of
    data which will be used later in the loop. This parallel loading idea is
    somewhat antinomic with the lazy loading data principle, so the
    PreloadIterator mixes both approaches. The number of preloaded data can be
    specified, the default is the number of processors in the machine. Each
    preload operation will run in a separate thread.

    ::

        volumes = [LazyReadData(f, nops=1) for f in filenames]
        res = sum(PreloadIterator(volumes, npreload=8))

    In the above example, 8 threads will be used to preload the next 8 items in
    the list from the current iterator position. As the iterator advances, more
    data preloads will be triggered.

    '''

    def __init__(self, iterable, npreload=multiprocessing.cpu_count()):
        '''
        Parameters
        ----------
        iterable: iterable
            the iterable can be a list, a generator, or an iterator. It should
            iterate over items which are LazyReadData instances, because it
            will use their lazy loading mechanism and their threading locks.
        npreload: number of preloaded data / number of threads used to preload
        '''
        self.iter = iter(iterable)
        self.npreload = npreload
        self.preload()

    def __iter__(self):
        return self

    def __next__(self):
        self.preload()
        item = next(self.iter)
        return item

    def preload(self):
        self.iter, iter = itertools.tee(self.iter)
        for i in range(self.npreload):
            try:
                item = next(iter)
                if hasattr(item, 'not_started') and item.not_started():
                    with item._lock:
                        item._loading = True
                    th = threading.Thread(target=item._lazy_read)
                    th.start()
            except StopIteration:
                break


class PreloadList(list):
    '''
    A list which provides a PreloadIterator to iterate over it.

    ::

        volumes = PreloadList((LazyReadData(f, nops=1) for f in filenames), npreload=8)
        res = sum(volumes)

    equivalent to:

    ::

        volumes = [LazyReadData(f, nops=1) for f in filenames]
        res = sum(PreloadIterator(volumes, npreload=8))
    '''

    def __init__(self, iterable=None, npreload=multiprocessing.cpu_count()):
        if isinstance(iterable, PreloadList):
            super().__init__(list.__iter__(iterable))
        super().__init__(iterable)
        self.npreload = npreload

    def __iter__(self):
        return PreloadIterator(super(PreloadList, self).__iter__(),
                               npreload=self.npreload)
