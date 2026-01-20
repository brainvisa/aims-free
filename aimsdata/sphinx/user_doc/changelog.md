# Changelog

## [Unreleased]

### Changed

- Volume view transform to parent is now an option in `Volume` view constructor
- `AimsSubVolume` has now an option (`-p` / `--parent`) to optionally add the transformation from the extracted sub-volume to its parent space. It used to be the mandatory behavior in 6.0.0 but appeared to be confusing for external (but popular) software making too many assumptions on transformations target spaces.


## [6.6.0] 2026-01-12

### Added

- Referential and axes manipulation in Volume: axes orientation is no longer fixed to LPI orientation but may be queried, and changed. Transformations are updated when axes orientation is changed.
- Support for MIF (MRTrix volumes) format reading
- pyaims functions to handle FSL warp fields
- Complete rework of Transformation classes: N-dimensional transforms are supported, non-linear ones (warp fields as FFD) etc.
- Support for reading .xfm (Freesurfer affine normalization transformations) files format
- Experimental support for reading .lta (Freesurfer affine transformations) files format
- Support for reading Freesurfer surface files format (meshes, such as lh.white, lh.pial etc)
- Support for reading Freesurfer texture files format (curvcature, such as lh.curv)
- Support for reading Freesurfer annotation files format (label textures, such as lh.aparc.annot)
- New module `soma.aims.demotools` in `pyaims`, providing functions to list and install demo and test datasets from the BrainVisa server.
- Transformation graphs implementation and IO
- FFD transformations composition. Transformation chains may also compose FFD fields (they did not, before). AimsComposeTransformation thus now also supports non-linear FFD fields.
- Volume views transformations are now autmatically adapted from the parent ones.
- Several API additions to make things easier, such as `Volume::allocateBorder()`.

### Changed

- Performance improvements in conversion, copy and assignation of pyaims vector classes. They used to be versy slow, they are now fast.
- Improvements and optimizations in GLTF IO (gloablly more than 10 times faster).
- `AimsApplyTransform` has been reworked and based on transformation graphs. It simplifies the way referentials and transformations are addressed, and in many cases also avoid the needs to supply a transformation file when all info is in the objects headers. Transformations from/to disk space is also supported. Output space can be completely specified also.
- Many other bug fixes

### Removed

- The old command `AimsResample`, obsolete since 5.0.0 and replaced with `AimsApplyTransform`, has been removed.

```{raw} html
:file: ../../doc/html/en/changelog.html
```

