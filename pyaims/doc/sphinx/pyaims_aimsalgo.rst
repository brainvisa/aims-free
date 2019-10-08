************************************
PyAimsAlgo programming documentation
************************************

Overview
++++++++

Many classes of AimsAlgo are actually found in the :aims:`soma.aims module <index.html>`, *after importing soma.aimsalgo* (mainly because of the C++ namespace aims being merged by SIP bindings). The additional `soma.aimsalgo` module contains C++ algorithms which do ne reside inside the aims C++ namespace, and additional python submodules.

A more complete API is in :doc:`pyaims_api_aimsalgo`.

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

.. autofunction:: soma.aims.meshdistance.MeshDilation

.. autofunction:: soma.aims.meshdistance.MeshDistance

.. autofunction:: soma.aims.meshdistance.MeshErosion

.. autofunction:: soma.aims.meshdistance.MeshVoronoi

.. autofunction:: soma.aims.meshdistance.MeshVoronoiStepbyStep

.. autoclass:: soma.aims.GeodesicPath
  :members:

.. autoclass:: soma.aims.Spam
  :members:

.. autoclass:: soma.aims.SpamBase
  :members:

.. autoclass:: soma.aims.SpamFromLikelihood
  :members:

.. autoclass:: soma.aims.SplineFfd
  :members:

.. autoclass:: soma.aims.TrilinearFfd
  :members:

.. autoclass:: BundleTransformer
  :members:


soma.aimsalgo module
--------------------

.. automodule:: soma.aimsalgo
  :members:

  .. autofunction:: AimsDistanceFrontPropagation

  .. autofunction:: AimsMorphoChamferClosing

      Please use preferably MorphoGreyLevel_* classes.

  .. autofunction:: AimsMorphoChamferDilation

      Please use preferably MorphoGreyLevel_* classes.

  .. autofunction:: AimsMorphoChamferErosion

      Please use preferably MorphoGreyLevel_* classes.

  .. autofunction:: AimsMorphoChamferOpening

      Please use preferably MorphoGreyLevel_* classes.

  .. autofunction:: AimsMorphoClosing

  .. autofunction:: AimsMorphoDilation

  .. autofunction:: AimsMorphoErosion

  .. autofunction:: AimsMorphoOpening

  .. autofunction:: AimsVoronoiFrontPropagation

  .. autofunction:: AimsMeshLabelConnectedComponent

  .. autoclass:: DiffusionSmoother_S16
    :members:

  .. autoclass:: DiffusionSmoother_FLOAT

  .. autoclass:: Gaussian2DSmoothing_DOUBLE
    :members:

  .. autoclass:: Gaussian2DSmoothing_FLOAT

  .. autoclass:: Gaussian2DSmoothing_S16

  .. autoclass:: Gaussian2DSmoothing_S32

  .. autoclass:: Gaussian2DSmoothing_U16

  .. autoclass:: Gaussian2DSmoothing_U32

  .. autoclass:: Gaussian2DSmoothing_U8

  .. autoclass:: Gaussian3DSmoothing_DOUBLE
    :members:

  .. autoclass:: Gaussian3DSmoothing_FLOAT

  .. autoclass:: Gaussian3DSmoothing_S16

  .. autoclass:: Gaussian3DSmoothing_S32

  .. autoclass:: Gaussian3DSmoothing_U16

  .. autoclass:: Gaussian3DSmoothing_U32

  .. autoclass:: Gaussian3DSmoothing_U8

  .. autoclass:: GeneralSampler_FLOAT_3
    :members:

  .. autoclass:: GeometricMoment_DOUBLE
    :members:

  .. autoclass:: AimsGradient_FLOAT
    :members:

  .. autoclass:: Histogram_FLOAT
    :members:

  .. autoclass:: MedianSmoothing_DOUBLE
    :members:

  .. autoclass:: MedianSmoothing_FLOAT

  .. autoclass:: MedianSmoothing_S16

  .. autoclass:: MedianSmoothing_S32

  .. autoclass:: MedianSmoothing_U16

  .. autoclass:: MedianSmoothing_U32

  .. autoclass:: MedianSmoothing_U8

  .. autoclass:: Mesher
    :members:

  .. autoclass:: MomentInvariant_FLOAT
    :members:

  .. autoclass:: Moment_FLOAT
    :members:

  .. autoclass:: TriangulationMoment
    :members:

  .. autofunction:: transformGraph

  .. autofunction:: transformMesh

  .. autofunction:: transformBucketDirect

  .. autofunction:: resampleBucket


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
    :members:

.. automodule:: soma.wip.aimsalgo.samplables
  :members:

.. autoclass:: SuperQuadricSamplable
    :members:

.. automodule:: soma.wip.aimsalgo.transform
  :members:

.. autoclass:: BendingTransform
  :members:

.. autoclass:: TaperingTransform
  :members:


