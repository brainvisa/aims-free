.. toctree::
   :maxdepth: 3


PyAimsAlgo programming documentation
--------------------------------

Overview
++++++++

Many classes of AimsAlgo are actually found in the :aims:`soma.aims module <index.html>`, *after importing soma.aimsalgo* (mainly because of the C++ namespace aims being merged by SIP bindings). The additional `soma.aimsalgo` module contains C++ algorithms which do ne reside inside the aims C++ namespace, and additional python submodules.

(The epydoc version of this doc is :aimsalgoepy:`here <soma.aimsalgo-module.html>`).


Contents
++++++++

In soma.aims module
-------------------

.. py:class:: sima.aims.ResamplerFactory_FLOAT
  :members:

.. py:class:: soma.aims.FastMarching

.. py:class:: soma.aims.FastMarching_BucketMap_S16

.. py:class:: soma.aims.FoldArgOverSegment

.. py:class:: soma.aims.FoldGraphAttributes

.. py:class:: soma.aims.GradientAdvection

.. py:class:: soma.aims.MeshInterpoler

.. py:class:: soma.aims.RegularBinnedHistogram_FLOAT

.. py:function:: soma.aims.meshdistance.MeshDilation

.. py:function:: soma.aims.meshdistance.MeshDistance

.. py:function:: soma.aims.meshdistance.MeshErosion

.. py:function:: soma.aims.meshdistance.MeshVoronoi

.. py:function:: soma.aims.meshdistance.MeshVoronoiStepbyStep

.. py:class:: soma.aims.Spam

.. py:class:: soma.aims.SpamBase

.. py:class:: soma.aims.SpamFromLikelihood


soma.aimsalgo module
--------------------

.. automodule:: soma.aimsalgo
  :members:

  .. py:function:: AimsDistanceFrontPropagation

  .. py:function:: AimsMorphoChamferClosing

  .. py:function:: AimsMorphoChamferDilation

  .. py:function:: AimsMorphoChamferOpening

  .. py:function:: AimsMorphoClosing

  .. py:function:: AimsMorphoDilation

  .. py:function:: AimsMorphoErosion

  .. py:function:: AimsMorphoOpening

  .. py:function:: AimsVoronoiFrontPropagation

  .. py:function:: AimsMeshLabelConnectedComponent

  .. py:class:: DiffusionSmoother_FLOAT

  .. py:class:: Gaussian3DSmoothing_FLOAT

  .. py:class:: GeneralSampler_FLOAT_3

  .. py:class:: GeometricMoment_DOUBLE

  .. py::class:: AimsGradient_FLOAT

  .. py:class:: Histogram_FLOAT

  .. py:class:: Mesher

  .. py:class:: MomentInvariant_FLOAT

  .. py:class:: Moment_FLOAT

  .. py:class:: TriangulationMoment

  .. py:class:: Resampler_FLOAT


soma.wip.aimsalgo module
------------------------

.. automodule:: soma.wip.aimsalgo.foldsgraphthickness
  :members:

  .. py:class:: FoldsGraphThickness

.. automodule:: soma.wip.aimsalgo.moment
  :members:

.. automodule:: soma.wip.aimsalgo.samplables
  :members:

.. automodule:: soma.wip.aimsalgo.transform
  :members:


