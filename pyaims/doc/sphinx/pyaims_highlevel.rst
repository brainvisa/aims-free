
.. default-domain:: py

.. highlight: py

.. currentmodule:: soma.aims

PyAims high-level functions and classes
=======================================

This section is the "useful" part, what you need to know first.


IO system
+++++++++

.. autofunction:: read

.. autofunction:: write


Neuroimaging data structures
++++++++++++++++++++++++++++

.. autofunction:: soma.aims.Volume

  Volume classes bindings are instantiated on a number of types: U8, S16, U16, S32, U32, FLOAT, DOUBLE, RGB, RGBA. For an example of the volumes API, see the :py:class:`soma.aims.Volume_FLOAT` class.

.. autofunction:: soma.aims.AimsTimeSurface

  For an example of mesh surface classes, see the :py:class:`soma.aims.AimsTimeSurface_3_VOID` class.

.. autofunction:: soma.aims.TimeTexture

  TimeTexture classes bindings are instantiated on a number of types: S16, S32, U32, FLOAT, POINT2DF. For an example of the TimeTexture API, see the :py:class:`soma.aims.TimeTexture_FLOAT` class.

.. autofunction:: soma.aims.Texture

  Texture classes bindings are instantiated on a number of types: S16, S32, U32, FLOAT, POINT2DF. For an example of the Texture API, see the :py:class:`soma.aims.Texture_FLOAT` class.
  
  .. note::
    
    You normally seldom use the Texture classes directly: most of the time they are (and should be) used through the higher-level :py:func:`soma.aims.TimeTexture` function and classes.

.. autofunction:: soma.aims.BucketMap

  For an example of bucket classes, see the :py:class:`soma.aims.BucketMap_VOID` class, which is actually the only instantiation bound to Python, because textured buckets are not used directly.


Transformations and coordinates systems
+++++++++++++++++++++++++++++++++++++++

.. autoclass:: soma.aims.AffineTransformation3d
  :members:

  See the :aimsdox:`AffineTransformation3d C++ documentation <classaims_1_1AffineTransformation3d.html>`

.. autoclass:: soma.aims.Quaternion
  :members:

.. autoclass:: soma.aims.StandardReferentials
  :members:


General purpose C++ containers
++++++++++++++++++++++++++++++

.. autoclass:: soma.aims.Object
  :members:

  See the :cartobdox:`Object C++ documentation <classcarto_1_1Object.html>`

.. autofunction:: soma.aims.AimsVector

.. autofunction:: soma.aims.stdVector

.. autofunction:: soma.aims.stdSet

.. autofunction:: soma.aims.stdList

.. autofunction:: soma.aims.rc_ptr


Conversion classes and functions
++++++++++++++++++++++++++++++++

.. autofunction:: soma.aims.Converter

.. autofunction:: soma.aims.ShallowConverter


Graphs and ROIs
+++++++++++++++

.. autoclass:: GraphManip
  :members:

  Graph manipulation, which make things easier than the direct use of :class:`Graph`.
  See the :aimsdox:`GraphManip C++ documentation <classaims_1_1GraphManip.html>`

.. autoclass:: soma.aims.RoiIterator

.. autoclass:: soma.aims.MaskIterator


.. _numpy_bindings:

Precisions about aims / numpy bindings
++++++++++++++++++++++++++++++++++++++

Several data structures are "array containers", arrays of items packed in memory: :class:`vectors <soma.aims.vector_FLOAT>` and :class:`texture wrappers <soma.aims.TimeTexture_FLOAT>`, :class:`AimsVectors <soma.aims.AimsVector_FLOAT_3>`, :class:`volumes <soma.aims.Volume_FLOAT>` etc. They may be 1 dimensional (vectors) or multi-dimensional (volumes). Many of such data structures offer bindings to the `numpy <http://numpy.org>`_ library.

Such bindings are normally accessed by using the numpy :func:`numpy.asarray` or :func:`numpy.array` functions on the aims object.

The array returned is a reference to the actual data block, so any
modification to its contents also affect the Volume contents, so it is
generally an easy way of manipulating array items because all the power of
the numpy module can be used there.

Arrays of non-scalar items
~~~~~~~~~~~~~~~~~~~~~~~~~~

Since PyAims 4.7 the numpy arrays bindings have notably improved, and are now able to bind arrays to voxels types which are not scalar numeric types. Volumes or vectors of RGB, RGBA, HSV, or Point3df now have numpy bindings. The bound object have generally a "numpy struct" binding, that is not the usual C++/python object binding, but instead a structure managed by numpy which also supports indexing. For most objects we are using, they also have an array stucture (a RGB is an array with 3 int8 items), and are bound under a sturcture with a unique field, named "v" (for "vector").

However we chose not to implement these array items the same way in Volume or in vector containers. In Volumes, we are actually using the structure elements:

    >>> v = aims.Volume('RGB', 100, 100, 10)
    >>> numpy.asarray(v)[0, 0, 0, 0]
    (0, 0, 0)

Such an array may be indexed by the field name, which returns another array with scalar values and additional dimensions:

    >>> numpy.asarray(v)['v']
    >>> numpy.asarray(v)['v'].dtype
    uint8
    >>> numpy.asarray(v)['v'].shape
    (100, 100, 100, 10, 3)

Both arrays share their memory with the aims volume.

In vectors however (vector or AimsVector) which are 1D array containers, we found more convenient to directly wrap the 'v' field when possible, so as to obtain a regular 2D array, which is what is expected most of the time:

    >>> v = aims.vector_POINT3DF([(1.5, 2., 3), (4,5,.6), (7, 8., 9.1)])
    >>> numpy.asarray(v)
    array([[1.5, 2. , 3. ],
          [4. , 5. , 0.6],
          [7. , 8. , 9.1]], dtype=float32)

Note that the same code in PyAims 4.6 or earlier will look exactly the same, except that:

* in pyaims 4.6 the array conversion is very slow, because numpy iterates over each element of each item of the array (probably using a python loop), since it does not know about its packed array structure in memory. In 4.7 it is essentially immediate.
* in pyaims 4.6 the array obtained this way is a *copy*, and thus its modification does not affect the original vector. In pyaims 4.7 the memory is shared.

If one actually wants to get the array of structure objects as for Volumes, then the :meth:`~soma.aims.vector_FLOAT.array_struct` method is here:

    >>> v.array_struct()
    array([([1.5, 2. , 3. ],), ([4. , 5. , 0.6],), ([7. , 8. , 9.1],)],
          dtype=[('v', '<f4', (3,))])


