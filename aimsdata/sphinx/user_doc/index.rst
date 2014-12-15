
==================
AIMS documentation
==================

AIMS meant something like *"Analysis of Images and Signals"* but nobody really knows what it exactly means now...

Anyway it is the neuroimaging structures manipulation library of the `BrainVISA project <http://brainvisa.info>`_.

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
  * :somaio:`Soma-IO <doxygen/index.html>`: IO libraries


PyAIMS: in Python
-----------------

:pyaimsdev:`PyAIMS documentation <index.html>`


Sources repository
==================

The main repository is on https://bioproj.extra.cea.fr/neurosvn/brainvisa/, and the web interface: https://bioproj.extra.cea.fr/redmine/projects/brainvisa

The server here is operated by the CEA institution, and has access rules that we cannot decide to change: it requires a login and password, which is somewhat annoying for open-source software.

However there is a "public" login/password to access the open-source code for reading:

::

  login: brainvisa
  password: Soma2009

So it is normally possible to access the repository using this kind of link:

* https://brainvisa:Soma2009@bioproj.extra.cea.fr/neurosvn/brainvisa/
* https://brainvisa:Soma2009@bioproj.extra.cea.fr/redmine/projects/brainvisa


Building sources and using AIMS for custom programs
===================================================

Projects are mainly in C++ and Python language. As BrainVISA is modular, it is made of multiple projects, which are separated by scope, aim, maintainers, language, and licence. To build the whole thing, we needed a tool to build an aggregation of the various projects. This tool is based on `CMake <http://cmake.org>`_ but needed a higher-level tool to glue the various projects. This is the role of the **brainvisa-cmake** project of BrainVisa, and its main program, **bv_maker**.

Instructions on how to use it are available on the sources server wen interface:

  https://brainvisa:Soma2009@bioproj.extra.cea.fr/redmine/projects/brainvisa-devel/wiki


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

