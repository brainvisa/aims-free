
==================
AIMS documentation
==================

AIMS meant something like *"Analysis of Images and Signals"* but nobody really knows what it exactly means now...

Anyway it is the neuroimaging structures manipulation library of the `BrainVISA project <https://brainvisa.info>`_.

General
=======

* What's new ? :doc:`See the changelog <changelog>`

* :doc:`AIMS configuration file documentation <aims_config>`

* :cmds:`List of all executable programs with help <>`

Tutorial
========

:doc:`tutorial`

Developers section
==================

C++ API docs
------------

:aims:`Beginning of programming tutorial <dev_doc/index.html>`

:aims:`AIMS library C++ API <doxygen/index.html>`

AIMS is also composed of several sub-projects:

* :cartoddox:`Carto data <index.html>`: neuroimaging data structures and IO implementations
* :aimsdata:`AIMS data <user_doc/index.html>`: image processing basics (this doc)
* :aimsalgo:`AIMS algo <index.html>`: algorithms for AIMS
* :graphdox:`Graph <index.html>`: high-level graphs library
* :aimsgui:`AIMS GUI <index.html>`: `Qt-based <http://qt-project.org>`_ GUI classes for AIMS
* :aimstil:`AIMS Til <index.html>`: Mainly meshes manipulation tools for AIMS
* **Soma-IO**: C++ general IO libraries, containing these sub-projects:

  * :cartobdox:`Carto base <index.html>`: C++ general usage library
  * :somaio:`Soma-IO <index.html>`: IO libraries


PyAIMS: in Python
-----------------

:pyaimsdev:`PyAIMS documentation <index.html>`


Sources repository
==================

The main repository is now on gitHub: https://github.com/brainvisa/aims-free.


Building sources and using AIMS for custom programs
===================================================

Projects are mainly in C++ and Python language. As BrainVISA is modular, it is made of multiple projects, which are separated by scope, aim, maintainers, language, and licence. To build the whole thing, we needed a tool to build an aggregation of the various projects. This tool is based on `CMake <http://cmake.org>`_ but needed a higher-level tool to glue the various projects. This is the role of the `brainvisa-cmake <https://github.com/brainvisa/brainvisa-cmake>`_ project of BrainVisa, and its main program, **bv_maker**.

Instructions on how to use it are available on the BrainVISa web server:

  https://brainvisa.info/brainvisa-cmake/index.html


Miscelaneous
============

* Documentation about :doc:`some of the AIMS specific data formats <formats>`
* :doc:`Old notes about SPM/Analyze format handling <spm_format>` across AIMS history

.. toctree::
  :hidden:

  changelog
  aims_config
  tutorial
  formats
  spm_format

