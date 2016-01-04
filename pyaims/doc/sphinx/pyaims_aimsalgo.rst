************************************
PyAimsAlgo programming documentation
************************************

Overview
++++++++

Many classes of AimsAlgo are actually found in the :aims:`soma.aims module <index.html>`, *after importing soma.aimsalgo* (mainly because of the C++ namespace aims being merged by SIP bindings). The additional `soma.aimsalgo` module contains C++ algorithms which do ne reside inside the aims C++ namespace, and additional python submodules.


Contents
++++++++

In soma.aims module
-------------------

.. autoclass:: soma.aims.ResamplerFactory_FLOAT
  :members:

.. autoclass:: soma.aims.FastMarching
  :members:

.. autoclass:: soma.aims.FastMarching_BucketMap_S16
  :members:

.. autoclass:: soma.aims.FoldArgOverSegment
  :members:

.. autoclass:: soma.aims.FoldGraphAttributes
  :members:

.. autoclass:: soma.aims.GradientAdvection
  :members:

.. autoclass:: soma.aims.MeshInterpoler
  :members:

.. autoclass:: soma.aims.RegularBinnedHistogram_FLOAT
  :members:

.. py:function:: soma.aims.meshdistance.MeshDilation

.. py:function:: soma.aims.meshdistance.MeshDistance

.. py:function:: soma.aims.meshdistance.MeshErosion

.. py:function:: soma.aims.meshdistance.MeshVoronoi

.. py:function:: soma.aims.meshdistance.MeshVoronoiStepbyStep

.. autoclass:: soma.aims.Spam
  :members:

.. autoclass:: soma.aims.SpamBase
  :members:

.. autoclass:: soma.aims.SpamFromLikelihood
  :members:


soma.aimsalgo module
--------------------

.. automodule:: soma.aimsalgo
  :members:

  .. py:function:: AimsDistanceFrontPropagation

  .. py:function:: AimsMorphoChamferClosing

      Please use preferably MorphoGreyLevel_* classes.

  .. py:function:: AimsMorphoChamferDilation

      Please use preferably MorphoGreyLevel_* classes.

  .. py:function:: AimsMorphoChamferErosion

      Please use preferably MorphoGreyLevel_* classes.

  .. py:function:: AimsMorphoChamferOpening

      Please use preferably MorphoGreyLevel_* classes.

  .. py:function:: AimsMorphoClosing

  .. py:function:: AimsMorphoDilation

  .. py:function:: AimsMorphoErosion

  .. py:function:: AimsMorphoOpening

  .. py:function:: AimsVoronoiFrontPropagation

  .. py:function:: AimsMeshLabelConnectedComponent

  .. autoclass:: DiffusionSmoother_S16

  .. autoclass:: DiffusionSmoother_FLOAT

  .. autoclass:: Gaussian2DSmoothing_DOUBLE

  .. autoclass:: Gaussian2DSmoothing_FLOAT

  .. autoclass:: Gaussian2DSmoothing_S16

  .. autoclass:: Gaussian2DSmoothing_S32

  .. autoclass:: Gaussian2DSmoothing_U16

  .. autoclass:: Gaussian2DSmoothing_U32

  .. autoclass:: Gaussian2DSmoothing_U8

  .. autoclass:: Gaussian3DSmoothing_DOUBLE

  .. autoclass:: Gaussian3DSmoothing_FLOAT

  .. autoclass:: Gaussian3DSmoothing_S16

  .. autoclass:: Gaussian3DSmoothing_S32

  .. autoclass:: Gaussian3DSmoothing_U16

  .. autoclass:: Gaussian3DSmoothing_U32

  .. autoclass:: Gaussian3DSmoothing_U8

  .. autoclass:: GeneralSampler_FLOAT_3

  .. autoclass:: GeometricMoment_DOUBLE

  .. autoclass:: AimsGradient_FLOAT

  .. autoclass:: Histogram_FLOAT

  .. autoclass:: MedianSmoothing_DOUBLE

  .. autoclass:: MedianSmoothing_FLOAT

  .. autoclass:: MedianSmoothing_S16

  .. autoclass:: MedianSmoothing_S32

  .. autoclass:: MedianSmoothing_U16

  .. autoclass:: MedianSmoothing_U32

  .. autoclass:: MedianSmoothing_U8

  .. autoclass:: Mesher

  .. autoclass:: MomentInvariant_FLOAT

  .. autoclass:: Moment_FLOAT

  .. autoclass:: TriangulationMoment


soma.aimsalgo.t1mapping module
------------------------------

.. automodule:: soma.aimsalgo.t1mapping
  :members:


soma.wip.aimsalgo module
------------------------

.. automodule:: soma.wip.aimsalgo.foldsgraphthickness
  :members:

  .. autoclass:: FoldsGraphThickness

.. automodule:: soma.wip.aimsalgo.moment
  :members:

  .. autoclass:: Moment

.. automodule:: soma.wip.aimsalgo.samplables
  :members:

  .. autoclass:: SuperQuadricSamplable

.. automodule:: soma.wip.aimsalgo.transform
  :members:

  .. autoclass:: BendingTransform

  .. autoclass:: TaperingTransform


