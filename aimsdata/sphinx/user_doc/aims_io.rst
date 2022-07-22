==============
AIMS IO system
==============

The Aims contains a generic IO system which allows to read and write different kinds of objects, supporting a large set of file formats, in a consistent API available in C++ and Python languages. All Aims commandlines are using this IO system, so support the same formats and options.

Actually there are two different IO systems: an older one (that we call the "aims IO system"), and a newer (well, not so new) called "Soma-IO". There are bridges between them so it should not really matter for the end user, but IO options are richer for the Soma-IO system.

Some formats are defined within the Soma-IO API, and others in the old Aims IO API. Aims Readers are using the Soma-IO system first, and if a file cannot be read using soma-io, then the old Aims system is used.


Soma-IO
=======

:somaio:`Soma-IO <index.html>` has been built as a replacement for the Aims IO system, with a more generic API and in particular an abstraction for volume (or let's say, array) structures in order to be usable for CartoData/Aims or orther libraries which define their own volume structures (this feature has never been used, however).

A number of volume formats are implemented in Soma-IO API, in the :cartoddox:`CartoData <index.html>` library which is under Aims and defines the Volume structures for Aims. See also :cartoddox:`this doc <cartovolumes_io.html>`.

IO may support options like partial reading, multi-resolution selection and others. Options may be passed either as an options dictionary in the programming API, or using URL-like requests syntax appended to the filename::

    filename?option=x&other_option=y

This way, any program using the IO system will be able to receive and use transparently options from the user.


The older AIMS IO system
========================

The older system is more limited, and doesn't allow the filename-URL options, but the API support an options dictionary.


.minf meta-information files
============================

Aims and BrainVisa are using an optional "meta-information file" (``.minf`` extensioin) which is read (if it exists) and written next to every data file supported by the IO system. This file is *not* part of "official" formats (like Nifti or Gifti) but is an addition introduced by Aims and BrainVisa. The file stores additional metadata which may or may not be supported by individual formats, in the most possible format-neutral way. Some info is already stored in formats and duplicated in this header, but makes it easier to read in a format-independent way.

It stores for instance volumes sizes and voxel sizes, coordinates systems, objects UUIDs when they have one, and possibly other information. The meta-information is a dictionary ((string key: value) set). The format is extensible and is very similar to JSON, except that it begins with a variable name ``attributes = {...}`` and quotes may be single or double quotes.

Values may be strings, numbers, lists or dictionaries.


Why not use BIDS JSON files ?
-----------------------------

``minf`` did exist long before the `BIDS <https://bids.neuroimaging.io/>`_ specification. BIDs specifies data directory trees organization, and the use of JSON files containing metadata next to data files. The idea is thus pretty similar. However the format (JSON) is not exactly the same, and the metadata are not standardized the same way, they use different names etc. Too bad they are not the same, but when we designed the ``minf`` format, even JSON did not exist, and we could not guess the BIDS conventions that would be taken in the future. And, of course, no wonder BIDS did not reuse our conventions: it had to be *new*...

Then Aims/BrainVisa should move on and also read BIDS files instead or in addition to ``minf``. This should be done in the future, but is not implemented yet.


Objects types, formats and and options
======================================

To list information about supported data types and formats, you can use the following command::

    AimsFileInfo --info

* Volumes: see :somaio:`Soma-IO <index.html>` for generic options.

  NIFTI-1 and NIFTI-2 formats: ``.nii``, ``.nii.gz``, Soma-IO API
      support partial IO (``ox``, ``oy``, ``oz``, ``ot``, ``sx``, ``sy``, ``sz``, ``st``) opions
  GIS: ``.ima``, ``.dim``, Soma-IO API
      the old french volume format. Merely used today, as it has been supplanted by Nifti. Supports partial IO (``ox``, ``oy``, ``oz``, ``ot``, ``sx``, ``sy``, ``sz``, ``st`` options), memory mapping.
  DICOM: Soma-IO API
      maybe partially supported as output.
  MINC and FREESURFER: ``.mnc``, ``.mgz``, ``.mgh``, Soma-IO API
      Freesurfer files are supported through recent libminc. Supports partial IO (``ox``, ``oy``, ``oz``, ``ot``, ``sx``, ``sy``, ``sz``, ``st`` options).
  OpenSlide: Soma-IO API
      supports a variety of formats, like Zeiss ``.czi`` files. Supports partial IO (``ox``, ``oy``, ``oz``, ``ot``, ``sx``, ``sy``, ``sz``, ``st`` options), and multi-resolution selection (``resolution_level`` option).
  TIFF: ``.tiff``, ``.tif``, Soma-IO API
      2D image format. Supports partial IO (``ox``, ``oy``, ``oz``, ``ot``, ``sx``, ``sy``, ``sz``, ``st`` options). N-D volumes can be rebuilt from a series of files using ``.minf`` header information.
  SPM/Analyze: ``.img``, ``.hdr``, Aims API
      Older Analyze format. Obsolete, it has flaws and missing information: please never use it. It has been replaced with NIFTI.
  VIDA: ``.vimg``, ``.vinfo``, ``.vhdr``, Aims API
      Old format. Obsolete. Please don't use it.
  ECAT: ``.v``, ``.i``, Aims API
      Nuclear imaging format.
  2D image formats: ``.jpg``, ``.gif``, ``.bmp``, ``.ppm``, ``.svg`` and others, mostly supported through Qt library, Aims API
      N-D volumes can be rebuilt from a series of files using ``.minf`` header information.
  GENESIS: Aims API
      Old General Electric MRI format. Obsolete. Supported only for reading old files. It has been replaced with DICOM.

* Meshes

  A mesh can be made of triangles (3 elements per polygon), segments (3 elements per polygon), quads (4 elements per polygon). Not all formats support segments or quads. Actually only MESH and WAVEFRONT do support other than triangles.

  GIFTI: ``.gii``, Aims API
      Now the neuroimaging community standard.
  MESH: ``.mesh``, Aims API
      The Aims format. Can be binary or ASCII.
  WAVEFRONT: ``.obj``, Aims API
      Wavefront format is used to convert mesh data with other communities. It is recognized by several software such as Blender. It may contain meshes and textures, and other material information (colors...).
  MNI_OBJ: ``.obj``, Aims API
      The McGill/MNI mesh format. Same extension as WAVEFRONT, be careful, thus if you need to save as ``.obj``, you'd better specify the output format as a writer option.
  PLY: ``.ply``, Aims API
      Another ASCII exchange format. May contain meshes and textures.
  POV: ``.pov``, Aims API
      Pov-Ray ray-tracer format. Used only for writing as an exchange format. Not supported for reading.
  VRML1: ``.vrml``, Aims API
      VRML 1 exchange format. Used only for writing as an exchange format. Not supported for reading.
  TRI: ``.tri``, Aims API
      Our Aims first mesh format, obsolete and not used any longer. Don't use it. It has been replaced with MESH later, and not most the community is using GIFTI (unless meshes are not triangles).

* Affine transformation matrices

  TRM: ``.trm``, Soma-IO API
      Format designed for Aims. Supports options:

      ``inv``: bool (0, 1)
          if true, get the inverse transformation instead of the specified one
  TRMHEADER: ``.trmhdr``, Soma-IO API
      Fake format: the ``.trmhdr`` file does not actually exist, but this extension indicates to the format reader to read transformation information from another object data file, like a NIFTI volume, a GIFTI mesh, or any other data file with a ``.minf`` meta-header containing transformation information. Ex::

          filename.nii.gz.trmhdr?idex=1&inv=1

      It supports the following options:

      ``inv``: bool (0, 1)
          if true, get the inverse transformation instead of the specified one
      ``index``: int
          several transformations may be contained in a data file. This indicates which one to be taken (default is 0, the first).
      ``target``: string
          instead of ``index``, the transformation may be specified by its target referential name. It should be listed in the ``referentials`` field in the ``.minf`` header or using the ``AimsFileInfo`` command.
  TRMCOMPOSE: ``.trmc``, Soma-IO API
            Fake format: the ``.trmc`` file does not actually exist, but this extension indicates to the format reader to read several transformation files, and combine them using the mathematical matrix composition operator. Filenames are separated with a ``*`` sign, and composed in the order they are given in matrix operations. Ex::

                file1.trm?inv=1*file2.nii.trmhdr?index=1.trmc?

            Here, the tailing ``?`` is needed to avoid a parsing error: if we don't supply it, the IO system will parse the filename as ``file1.trm?inv=1*file2.nii.trmhdr`` with options ``{"index": "1.trmc"}``, which is not what we want.

* Graphs (sulci, ROIs)

  ARG: ``.arg``, ``.data``, Aims API
      Aims format.

      read options:

      ``subobjectsfilter``: int
          read only selected sub-objects (buckets, meshes...). Values are:

          - 0: none
          - 1: nodes
          - 2: relations
          - 3: both

      write options:

      ``save_only_modified``: bool
          if true (the default), elements (buckets, meshes) not marked as modified will not be saved back if output files already exist. You must set it to false to force actual output, if needed.
      ``saving_mode``: string
          ``local`` (save one bucket/mesh file per element), ``global`` (concatenate buckets or meshes in the same file for all vertices), ``keep`` (don't change previous mode)
      ``force_global``: bool
          force ``global`` mode

* Fibers tractographies

    Tractographies are read by default (in Aims/Anatomist) as graphs. They can however be read as *streams*, using ``BundleReader``, and ``BundleListener``/``BundleProducer`` APIs.

    BUNDLES: graph format, Aims API

        supported sub-formats:

        BUNDLES

        TRK: ``.trk``
            Trackviz format

        TCK: ``.tck``
            MRTrix format

* Buckets (voxels lists)

  BCK: ``.bck``, Aims API
      The Aims bucket format.

* Textures

  GIFTI: ``.gii``, Aims API
      Now the neuroimaging community standard.
  TEX: ``.tex``, Aims API
      The Aims texture format.

* Sparse matrix

  IMASPARSE: ``.imas``, Aims API
      Aims format for sparse/dense matrices.

  CIFTI-2: ``.nii`` with Cifti extension, Aims API
      Should be a community standard, but is not widely used today. Support is only for reading, and probably partial.

  NUMPY: ``.npy``, Aims/Python API
      Only available when python extensions are enabled (but this is normally OK).

      Numpy arrays reading as (dense) matrices.

* Hierarchy

  HIE: ``.hie``, Aims API
      Aims format.

* GenericObject (dictionaries, lists... containing other generic objects of arbitrary types, a bit like Python language objects)

  PYTHON: ``.minf``, Soma-IO API
      Aims format
  JSON: ``.json``, Soma-IO API
      Reads standard JSON files
  XML: ``.xml``, Soma-IO API
      Reads subsets or specialized XML files as genric objects


See also
========

* Documentation about :doc:`some of the AIMS specific data formats <formats>`
* :doc:`Old notes about SPM/Analyze format handling <spm_format>` across AIMS history
