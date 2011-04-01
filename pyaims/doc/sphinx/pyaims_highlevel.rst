
PyAims high-level functions and classes
=======================================

This section is the "useful" part, what you need to know first.


IO system
+++++++++

.. autofunction:: soma.aims.read

.. autofunction:: soma.aims.write


Neuroimaging data structures
++++++++++++++++++++++++++++

.. autofunction:: soma.aims.Volume

.. autofunction:: soma.aims.AimsData

.. autofunction:: soma.aims.TimeTexture

.. autofunction:: soma.aims.Texture

.. autofunction:: soma.aims.BucketMap

.. autofunction:: soma.aims.AimsTimeSurface


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

