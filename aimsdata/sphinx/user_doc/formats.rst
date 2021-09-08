
==========================
AIMS-specific file formats
==========================

.. _gis_format:

``.ima`` GIS volume format
==========================

.. todo::

  to be continued...


.. _mesh_format:

``.mesh`` surfacic mesh format
==============================

1. Introduction
---------------

This document describes the main format used by *BrainVISA* and *Anatomist* to represent surfaces (composed of polygons such as triangles) and segment set. The aimed audience is programmers who wish to generate surfaces and/or segments with their own software and use them with *BrainVISA / Anatomist*.


2. Syntax
---------

The format description is written with the following elements. A *field* (written in bold) represent an element that can be split in other elements. All fields are described in :ref:`section 4 <mesh_fields_descr>`. In *ascii* mode fields can be separated by one or more **space**.

Characters strings are represented in verbatim between quotes. For example, ``’string’`` represent six *ascii* characters (each one coded on one byte).


3. Format description
---------------------

A mesh file contains the following fields:

#. **mode**
#. **textureType**
#. **polygonDimension**
#. **numberOfTimeSteps**
#. **timeSteps**


.. _mesh_fields_descr:

4. Fields description
---------------------

**mode**: The format can be written either as an ascii text file or as a binary file. The **mode** is used to identify the representation it can have three values:

* ``’ascii’``: the file is in text format.
* ``’binarABCD’``: the file is in binary format and uses big-endian byte order for numbers (such as Motorola or Sun processors for example).
* ``’binarDCBA’``: the file is in binary format and uses little-endian byte order for numbers (such as Intel processors for example).

**textureType**: The file format was created with the possibility to include a texture. But this is never used since textures are represented in a separate format. In *ascii* mode his field should always contain ``’VOID’`` or, in binary mode, a **U32** containing ``4`` (which is the size of the following string) followed by the four characters ``’VOID’``.

**polygonDimension**: This field is a **U32** containing the number of points of each polygon. The following values are supported in Anatomist and Aims:

* ``3``: Polygons are composed of three points (they are triangles). This is the recomanded value for surfaces because other values may not be supported by all BrainVISA processing tools.
* ``4``: Polygon are composed of four points. This is supported in Anatomist but may not be supported in every BrainVISA processing tools.
* ``2``: This value is used to represent segments in a 3D space. Each ”polygon” is composed of two 3D points.

**numberOfTimeSteps**: The mesh format can represent several meshes at different time steps. This is a *U32* representing the number of time steps.

**timeSteps**: This field contains numberOfTimeSteps times the following structure:

* **instant**: a **U32** representing a time instant.
* **vectorOf<vertex>**: contains all the vertices which are used to build polygons.
* **vectorOf<normal>**: contains the normals of the surface at each vertex. It must have the same size as **vectorOf<vertex>** or be empty.
* **vectorOf<texture>**: must be an empty vector (i.e a **U32** containing ``0``).
* **vectorOf<polygon>**: contains the polygons which represent the surface.

**vertex**: is 3D a point.

* In *ascii* mode it has the following syntax: ``’(’`` **FLOAT** ``’,’`` **FLOAT** ``’,’`` **FLOAT** ``’)’``.
* In binary mode it is represented by three **FLOAT**.

**normal**: is a normalized vector.

* In *ascii* mode it has the following syntax: ``’(’`` **FLOAT** ``’,’`` **FLOAT** ``’,’`` **FLOAT** ``’)’``.
* In binary mode it is represented by three **FLOAT**.

**polygon**: is a set of **polygonDimension** points. Each point is represented by a **U32** which is an index in **vectorOf<vertex>**. The first vertex index is *zero*, the second is *one*, etc.

* In *ascii* mode it has the following syntax: ``’(’`` **U32** ``’,’`` **U32** ``’,’`` ... ``’,’`` **U32** ``’)’``.
* In binary mode it is represented by a series of **polygonDimension** elements of type **U32**.

**U32**: A 32 bits wide unsigned integer (between 0 and 4294967295).

* In *ascii* mode it is written as a decimal number.
* In binary mode it is represented on four bytes with the chosen byte order (see **mode** above).

**FLOAT**: A 32 bits wide real number (maximum 3.40282347e+38).

* In *ascii* mode it is written as a decimal number.
* In binary mode it is represented on four bytes with the chosen byte order (see mode above).

**vectorOf<field>**: where **field** is a field type. It represents a fixed length vector of elements of type **field**. It contains the size of the vector (i.e. the number of elements) as a **U32** followed by the elements.

**space**: A byte with one of the *ascii* value for a space, a tabulation or a carriage-return.


5. Examples
-----------

Here is an example of an ascii mesh file containing a tetrahedron.

::

  ascii
  VOID
  3
  1
  0
  4 (-0.8,0.8,0) (0.8,8e-1,0) (-1,-1,0) (0,0,1)
  4 (-0.8,0.8,0) (0.8,8e-1,0) (-1,-1,0) (0,0,1)
  0
  4 (0,1,2) (0,3,1) (1,3,2) (2,3,0)

Here is an example of an ascii mesh file containing a linear spiral.

::

  ascii
  VOID
  2
  1
  0
  16
  (10, 0, 0) (7.07, 7.07, 0.4) (0, 10, 0.8)
  (-7.07, 7.07, 1.2) (-10, 0, 1.6) (-7.07, -7.07, 2.0)
  (0, -10, 2.4) (7.07, -7.07, 2.8) (10, 0, 3.2)
  (7.07, 7.07, 3.6) (0, 10, 4.0) (-7.07, 7.07, 4.4)
  (-10, 0, 4.8) (-7.07, -7.07, 5.2) (0, -10, 5.6)
  (7.07, -7.07, 6.0)
  0
  0
  15
  (0,1) (1,2) (2,3) (3,4) (4,5) (5,6) (6,7) (7,8) (8,9)
  (9,10) (10,11) (11,12) (12,13) (13,14) (14,15)

:documents:`Additional example data can be found here <formats/mesh_examples.tar.gz>`.


.. _tex_format:

``.tex`` texture format
=======================

1. Syntax
---------

The format description is written with the following elements. A field (written in bold) represent an element that can be split in other elements. All fields are described in :ref:`section 3 <tex_fields_descr>`. In *ascii* mode fields can be separated by one or more **space**.

Characters strings are represented in verbatim between quotes. For example,
``’string’`` represent six ascii characters (each one coded on one byte).

2. Format description
---------------------

A tex file contains the following *fields*:

#. **mode**
#. **textureType**
#. **numberOfTimeSteps**
#. **timeSteps**


.. _tex_fields_descr:

3. Fields description
---------------------

**mode**: The format can be written either as an *ascii* text file or as a binary file. The **mode** is used to identify the representation it can have three values :

* ``’ascii’``: the file is in text format.
* ``’binarABCD’``: the file is in binary format and uses big-endian byte order for numbers (such as Motorola or Sun processors for example).
* ``’binarDCBA’``: the file is in binary format and uses little-endian byte order for numbers (such as Intel processors for example).

**textureType**:

* In *ascii* mode his field should contain the name of the texture type (``’FLOAT’``, ``’S16’``, ``’U32’``, ``’POINT2DF’``),
* In binary mode, a **U32** containing the size of the typename string followed by the string itself (``’FLOAT’`` etc).

**numberOfTimeSteps**: The texture format can represent several textures at different time steps. This is a **U32** representing the number of time steps.

**timeSteps**: This field contains **numberOfTimeSteps** times the following structure :

* **instant**: a **U32** representing a time instant.
* **vectorOf<texture>**: contains all the texture values, each associated to a mesh vertex (in the same order).

**U32**: A 32 bits wide unsigned integer (between 0 and 4294967295).

* In *ascii* mode it is written as a decimal number.
* In binary mode it is represented on four bytes with the chosen byte order (see mode above).

**S16**: A 16 bits wide signed integer (between -32768 and 32767).

* In *ascii* mode it is written as a decimal number. In binary mode it is represented on two bytes with the chosen byte order (see mode above).

**FLOAT**: A 32 bits wide real number (maximum 3.40282347e+38).

* In *ascii* mode it is written as a decimal number.
* In binary mode it is represented on four bytes with the chosen byte order (see mode above).

**vectorOf<field>**: where **field** is a field type. It represents a fixed length vector of elements of type **field**. It contains the size of the vector (i.e. the number of elements) as a **U32** followed by the elements.

**POINT2DF**: A **POINT2DF** is a couple of floating point numbers, ie 2 texture coords for a 2D texture. It is coded as a vector of two 32 bits wide real (**FLOAT**) numbers.

4. Example
----------

Here is an example of an ascii tex file containing a 2D texture of 2 timesteps, for a mesh of 4 vertices.

::

  ascii
  POINT2DF
  2
  0
  4 (-0.2,0.8) (0.8,8e-1) (-1,0) (0,0)
  1
  4 (-0.8,0.7) (0.7,-0.3) (-0.9,0.1) (0.2,0.3)


.. _bck_format:

``.bck`` "bucket" (voxels list) format
======================================

1. Introduction
---------------

This document describes the main format used by *BrainVISA* and *Anatomist* to represent a series of 3D voxel coordinates along with corresponding values. For historical reason this format is called *bucket* format. The aimed audience is programmers who wish to read or generate buckets files with their own software.


2. Syntax
---------

The format description is written with the following elements. A *field* (written in bold) represent an element that can be splitted in other elements. All fields are described in :ref:`section 4 <bck_fields_descr>`. In ascii mode fields can be separated by spaces.

Characters strings are represented in verbatim between quotes. For example, ``’string’`` represent six *ascii* characters (each one coded on one byte).

3. Format description
---------------------

A bucket file contains the following fields:

* **mode**
* **textureType**
* **voxelSize**
* **numberOfTimeSteps**
* **timeSteps**


.. _bck_fields_descr:

4. Fields description
---------------------

**mode**: The format can be written either as an *ascii* text file or as a binary file. The mode is used to identify the representation it can have three values:

* ``’ascii’``: the file is in text format.
* ``’binarABCD’``: the file is in binary format and uses big-endian byte order for numbers (such as Motorola or Sun processors for example).
* ``’binarDCBA’``: the file is in binary format and uses little-endian byte order for numbers (such as Intel processors for example).

**dataType**: The file format was created with the possibility to store several data types. However, in a single bucket file, all values must be of the same type. Recognized types are described below. The **dataType** field defines the data type.

* In *ascii* mode his field contains ``’-type’`` followed by **spaces** and the data type name (see below).
* In binary mode, it is a string containing the data type name.

**voxelSize**: Contains x, y, z and t voxel sizes.

* In ascii it is ``’-dx’`` **FLOAT** ``’-dy’`` **FLOAT** ``’-dz’`` **FLOAT** ``’-dt’`` **FLOAT**.
* In binary, it is four **FLOAT**.

**numberOfTimeSteps**: The bucket format can represent several series of coordinate/value at different time steps. **numberOfTimeSteps** contains the number of time steps.

* In *ascii* mode, it is ``’-dimt’`` **U32**
* In binary mode, it is a **U32**

**timeSteps**: This field contains **numberOfTimeSteps** times the following structure:

* **instant**: A **U32** representing a time instant.

* In *ascii*: ``’-time’`` **U32**.
* In binary: a **U32**

**numberOfPoints**: Number of *(coordinate, value)* pairs.

* In *ascii*: ``’-dim’`` **U32**.
* In binary: a **U32**

**pointsAndValues**: a series containing **numberOfPoints** times a **coordinate value** pair (**space**-separated in *ascii* mode).

**coordinate**: A 3D coordinate.

* In *ascii* mode it has the following syntax: ``’(’`` **S32** ``’,’`` **S32** ``’,’`` **S32** ``’)’``.
* In binary it is three **S32**.

**value**: The content of this field depends on the value of dataType. See below.

**U32**: A 32 bits wide unsigned integer (between 0 and 4294967295).

* In ascii mode it is written as a decimal number.
* In binary mode it is represented on four bytes with the chosen byte order (see **mode** above).

**S32**: A 32 bits wide signed integer (between -2147483648 and 2147483647).

* In ascii mode it is written as a decimal number.
* In binary mode it is represented on four bytes with the chosen byte order (see **mode** above).

**FLOAT**: A 32 bits wide real number (maximum 3.40282347e+38).

* In *ascii* mode it is written as a decimal number.
* In binary mode it is represented on four bytes with the chosen byte order (see **mode** above).

**string**:
* In *ascii* mode it is a serie of non-space *ascii* bytes
* in binary mode it is a **U32** containing the string size followed by the string content (*ascii* bytes series).

**vectorOf<field>**: where **field** is a field type. It represents a fixed length vector of elements of type **field**. It contains the size of the vector (i.e. the number of elements) as a **U32** followed by the elements.

**spaces**: Valid only in *ascii* mode (in binary, a space can be considered as an empty string). One or more byte with one of the *ascii* value for a space, a tabulation or a carriage-return.


5. Data types
-------------

The field **dataType** defines the type of value associated to each **coordinate**. Here is a list of the possible values for **dataType** and the corresponding value structure.

* ``VOID``
* ``FLOAT``
* ``DOUBLE``
* ``U32``
* ``S32``
* ``U16``
* ``S16``
* ``POINT2DF``


6. Examples
-----------

Here should be an example of an *ascii* bucket file.

.. todo::

  To be continued...


.. _arg_format:

``.arg`` graph format
=====================

.. todo::

  To be continued...


.. _hie_format:

``.hie`` tree format
====================

.. todo::

  To be continued...


.. _bundles_format:

``.bundles`` bundles / fibers format
====================================

Definition of curve bundles format (``*.bundles``) used in Brainvisa

1. Introduction
---------------

This document describes the main format used by *brainvisa* and *anatomist* to represent named sets of curves.


2. Format description
---------------------

The bundle format is composed of two files. One human readable file with the extension ``.bundles`` and one data file with the extension ``.bundlesdata``. The ``*.bundles`` file contains a generic header with the same format as ``*.minf`` files of *brainvisa*.

A ``*.bundles`` file contains the following fields :

**format:** (required)
    Format identifier. A format identifier uses the following pattern: ``<format_name>.<major_version>.<minor_version>``. To date, ``bundles_1.0`` value is required.

**space_dimension:** (optional, default = 3)
    Space dimension. Must be 3 for ``bundles_1.0``.

**curves_count:** (required)
    Number of fibers in data file (required). Must be > 0 for ``bundles_1.0``.

**data_file_name:** (optional, default = ``*.bundlesdata``)
    File where to find the data. Must be ``*.bundlesdata`` for ``bundles_1.0``.

A star (``*``) in the file name is replaced by the header file name without its extension. This file contains a series of curves. Each curve is a series of points. Each point is a series of ``space_dimension`` coordinates.

In ascii mode:

* *coordinate* is a decimal number.

* A *point* is ``space_dimension`` space-separated *coordinates*.

* A *curve* is a comma separated series of *points*.

* There is one and only one *curve* per line.

In binary mode:

* A *coordinate* is a 8 bytes floating point number (a *double* in C or C++).

* A *point* is the concatenation of ``space_dimension`` *coordinates*.

* A *curve* is a 4 bytes integer representing the number of points and a series of points.


