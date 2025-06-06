set( _headers
    bucket/bucket_g.h
    data/data_g.h
    border/borderfiller.h
    border/borderiterator.h
    bucket/bucket.h
    bucket/bucketMap.h
    bucket/bucketutil.h
    bucket/bucketutil_d.h
    bucket/item.h
    bucket/sparsevolume.h
    bucket/bucketgen.h
    color/colormap.h
    config/aimsdata_config.h
    connectivity/connectivity.h
    connectivity/connectivity_g.h
    connectivity/component.h
    connectivity/component_d.h
    connectivity/structuring_element.h
    curve/curve.h
    data/cartodatavolume.h
    data/data.h
    data/fastAllocationData.h
    data/header.h
    data/headerutil.h
    data/pheader.h
    data/pheader_doc.h
    data/sliceformatheader.h
    data/volumemanip.h
    data/volumeoperators_aims.h
    def/def_g.h
    def/assert.h
    def/general.h
    def/info.h
    def/path.h
    def/settings.h
    doc/aimsdata_doc.h
    features/features.h
    features/scalar_set_features.h
    features/point_set_features.h
    features/roi_features.h
    fibers/bundles.h
    fibers/bundles_features.h
    fibers/bundlesFusion.h
    fibers/bundleRegroup.h
    fibers/bundleSampler.h
    fibers/bundlesSlicer.h
    fibers/fiberOversampler.h
    fibers/mrtrixbundlereader.h
    fibers/trackvisbundlereader.h
    getopt/getopt2.h
    getopt/getoptProcess.h
    graph/graphmanip.h
    graph/graphmanip_d.h
    hsv/hsv.h
    io/dbh.h
    io/aimsGraphR.h
    io/aimsGraphR_d.h
    io/aimsGraphW.h
    io/aimsGraphW_d.h
    io/apcreader.h
    io/argheader.h
    io/argR.h
    io/argW.h
    io/baseFormats_bucket.h
    io/baseFormats_bucket_d.h
    io/baseFormats_cartovolume.h
    io/baseFormats_genericobject.h
    io/baseFormats_graph.h
    io/baseFormats_hierarchy.h
    io/baseFormats_mesh.h
    io/baseFormats_mesh_d.h
    io/baseFormats_sparsematrix.h
    io/baseFormats_syntax.h
    io/baseFormats_texture.h
    io/baseFormats_texture_d.h
    io/baseFormats_volume.h
    io/baseFormats_volume_d.h
    io/bckheader.h
    io/bckItemR.h
    io/bckItemW.h
    io/bckR.h
    io/bckW.h
    io/bckMapR.h
    io/bckMapW.h
    io/byteswap.h
    io/channelR.h
    io/coarseR.h
    io/cmapR.h
    io/cmapW.h
    io/defaultItemR.h
    io/defaultItemW.h
    io/directory.h
    io/dtitensorItemR.h
    io/dtitensorItemW.h
    io/finder.h
    io/finderFormats.h
    io/fileFormat.h
    io/fileFormat_d.h
    io/fileFormat_cartovolume_d.h
    io/ge_imageFileOffsets.h
    io/ge_imghdr.h
    io/genesisheader.h
    io/genesisR.h
    io/graphBundlesFormat.h
    io/hieR.h
    io/imasparseheader.h
    io/io_g.h
    io/iooptions.h
    io/itemR.h
    io/itemR_stl.h
    io/itemW.h
    io/itemW_stl.h
    io/labelImage.h
    io/listItemR.h
    io/listItemW.h
    io/macosxbugs.h
    io/mniobjheader.h
    io/povW.h
    io/scaledcoding.h
    io/selectionr.h
    io/selectionw.h
    io/spmR.h
    io/spmR_d.h
    io/spmheader.h
    io/spmW.h
    io/spmW_d.h
    io/spmattribconverter.h
    io/triederItemR.h
    io/triederItemW.h
    io/reader.h
    io/reader_d.h
    io/readerasobject.h
    io/writer.h
    io/writer_d.h
    io/meshheader.h
    io/meshR.h
    io/meshW.h
    io/roiR.h
    io/tensorItemR.h
    io/tensorItemW.h
    io/texheader.h
    io/texR.h
    io/texW.h
    io/triheader.h
    io/triR.h
    io/triW.h
    io/process.h
    io/datatypecode.h
    io/wavefrontheader.h
    io/wavefrontmeshW.h
    io/wavefrontmeshR.h
    io_soma/fsannotformatchecker.h
    io_soma/fsannotformatreader.h
    io_soma/fsannotformatreader_d.h
    io_soma/fscurvformatchecker.h
    io_soma/fscurvformatreader.h
    io_soma/fscurvformatreader_d.h
    io_soma/fssurfformatchecker.h
    io_soma/fssurfformatreader.h
    io_soma/fssurfformatreader_d.h
    io_soma/ltaformatchecker.h
    io_soma/ltaformatreader.h
    io_soma/trmformatchecker.h
    io_soma/trmformatwriter.h
    io_soma/trmformatreader.h
    io_soma/trmgraphformatchecker.h
    io_soma/trmgraphformatreader.h
    io_soma/trmgraphformatwriter.h
    io_soma/trmingraphformatchecker.h
    io_soma/trmingraphformatreader.h
    io_soma/trm_compose_formatchecker.h
    io_soma/trm_compose_formatreader.h
    io_soma/trm_header_formatchecker.h
    io_soma/trm_header_formatreader.h
    io_soma/xfmformatchecker.h
    io_soma/xfmformatreader.h
    io_soma/xfmformatwriter.h
    math/dtitensor.h
    math/gausslu.h
    math/integrator.h
    math/lagrange.h
    math/mathelem.h
    math/mathelemrgb.h
    math/romberg.h
    math/rombergitg.h
    math/tensor.h
    math/trapezeitg.h
    math/trieder.h
    memmap/allocator.h
    mesh/cutmesh.h
    mesh/cutmesh_d.h
    mesh/mesh_rasterization_d.h
    mesh/surface_g.h
    mesh/surface.h
    mesh/surfacegen.h
    mesh/surfacemanip.h
    mesh/surfacemanip_d.h
    mesh/surfaceOperation.h
    mesh/texture.h
    mesh/texturetools.h
    mesh/tex2graph.h
    mesh/tex2graph_d.h
    meshprojection/meshprojection.h
    meshprojection/meshprojection_d.h
    resampling/mask.h
    resampling/mask_d.h
    resampling/motion.h
    resampling/linearInterpolator.h
    resampling/quaternion.h
    resampling/standardreferentials.h
    rgb/rgb.h
    rgb/rgbnorm.h
    roi/hie.h
    roi/roi_g.h
    roi/fill.h
    roi/fill_d.h
    roi/maskIterator.h
    roi/roiIterator.h
    roi/roi.h
    roi/roi_diff.h
    roi/voxelSampling.h
    selection/selection.h
    selection/selector.h
    sparsematrix/ciftitools.h
    sparsematrix/sparseMatrix.h
    sparsematrix/sparseMatrixItemR.h
    sparsematrix/sparseMatrixItemW.h
    sparsematrix/sparsematrixutil.h
    sparsematrix/sparseordensematrix.h
    texdata/texdata.h
    transformation/affinetransformation3d.h
    transformation/transformation.h
    transformation/transformation_chain.h
    transformation/transformationgraph3d.h
    utility/anytype_reader.h
    utility/anytype_reader_details.h
    utility/bininverse.h
    utility/boundingbox.h
    utility/channel.h
    utility/converter_g.h
    utility/converter_bucket.h
    utility/converter_hsv.h
    utility/converter_rgb.h
    utility/converter_texture.h
    utility/converter_volume.h
    utility/clock.h
    utility/cyclic_iterator.h
    utility/flip.h
    utility/linearcomb.h
    utility/masscenter.h
    utility/merge.h
    utility/minmax.h
    utility/progress.h
    utility/stl_conversion.h
    utility/threshold.h
    utility/utility_g.h
    vector/vector.h
)

# It is necessary to split aims libraries to reduce
# sizes, especially for Windows OS. Symbols seems
# to be undefined when library exceed size limit
# (size limit is probably 64MB, but it is not )
set( _aimsdata_0_sources
#     vector/vector2d.cc
#     vector/vector3d.cc
#     vector/vector4d.cc
)

set(_aimsdata_sources
    bucket/bucketMap.cc
    bucket/bucketgen.cc
    bucket/bucketutil.cc
    color/colormap.cc
    connectivity/component.cc
    connectivity/connectivity.cc
    connectivity/structuring_element.cc
    curve/curve.cc
    data/cartodatavolume.cc
    data/data.cc
    data/fastAllocationData.cc
    data/header.cc
    data/headerutil.cc
    data/pheader.cc
    data/sliceformatheader.cc
    def/filedim.cc
    def/general.cc
    def/info.cc
    def/path.cc
    def/settings.cc
    def/string.cc
    features/scalar_set_features.cc
    features/point_set_features.cc
    features/roi_features.cc
    fibers/bundles.cc
    fibers/bundles_features.cc
    fibers/bundlesFusion.cc
    fibers/bundleRegroup.cc
    fibers/bundleSampler.cc
    fibers/bundlesSlicer.cc
    fibers/fiberOversampler.cc
    fibers/mrtrixbundlereader.cc
    fibers/trackvisbundlereader.cc
    getopt/getopt2.cc
    graph/graphmanip.cc
    io/aimsGraphR.cc
    io/aimsGraphW.cc
    io/apcreader.cc
    io/argheader.cc
    io/argR.cc
    io/argW.cc
    io/baseFormats_genericobject.cc
    io/baseFormats_graph.cc
    io/baseFormats_hierarchy.cc
    io/baseFormats_mesh.cc
    io/baseFormats_sparsematrix.cc
    io/baseFormats_syntax.cc
    io/bckheader.cc
    io/channelR.cc
    io/coarseR.cc
    io/cmapR.cc
    io/cmapW.cc
    io/dtitensorItemR.cc
    io/dtitensorItemW.cc
    io/fileFormat.cc
    io/fileFormat_genericobject.cc
    io/fileFormat_mesh.cc
    io/fileFormat_motion.cc
    io/fileFormat_texture.cc
    io/fileFormat_bucket.cc
    io/fileFormat_graph.cc
    io/fileFormat_hierarchy.cc
    io/fileFormat_sparsematrix.cc
    io/fileFormat_volume.cc
    io/fileFormat_volumeVec.cc
    io/finder.cc
    io/finderFormats.cc
    io/genesisheader.cc
    io/graphBundlesFormat.cc
    io/hieR.cc
    io/iooptions.cc
    io/imasparseheader.cc
    io/labelR.cc
    io/meshheader.cc
    io/mniobjformat.cc
    io/mniobjheader.cc
    io/process.cc
    io/reader_bucket.cc
    io/reader_cartovolume.cc
    io/reader_genericobject.cc
    io/reader_graph.cc
    io/reader_hierarchy.cc
    io/reader_mesh.cc
    io/reader_motion.cc
    io/reader_sparsematrix.cc
    io/reader_texture.cc
    io/reader_volume.cc
    io/readerasobject.cc
    io/roiR.cc
    io/scaledcoding.cc
    io/selectionr.cc
    io/selectionw.cc
    io/spmheader.cc
    io/spmR.cc
    io/spmW.cc
    io/spmattribconverter.cc
    io/tensorItemR.cc
    io/tensorItemW.cc
    io/texheader.cc
    io/triederItemR.cc
    io/triederItemW.cc
    io/triheader.cc
    io/triR.cc
    io/triW.cc
    io/wavefrontheader.cc
    io/writer_bucket.cc
    io/writer_cartovolume.cc
    io/writer_genericobject.cc
    io/writer_graph.cc
    io/writer_hierarchy.cc
    io/writer_mesh.cc
    io/writer_motion.cc
    io/writer_sparsematrix.cc
    io/writer_texture.cc
    io/writer_volume.cc
    io_soma/fsannotformatchecker.cc
    io_soma/fsannotformatreader.cc
    io_soma/fscurvformatchecker.cc
    io_soma/fscurvformatreader.cc
    io_soma/fssurfformatchecker.cc
    io_soma/fssurfformatreader.cc
    io_soma/ltaformatchecker.cc
    io_soma/ltaformatreader.cc
    io_soma/trmformatchecker.cc
    io_soma/trmformatreader.cc
    io_soma/trmformatwriter.cc
    io_soma/trmgraphformatchecker.cc
    io_soma/trmgraphformatreader.cc
    io_soma/trmgraphformatwriter.cc
    io_soma/trmingraphformatchecker.cc
    io_soma/trmingraphformatreader.cc
    io_soma/trm_compose_formatchecker.cc
    io_soma/trm_compose_formatreader.cc
    io_soma/trm_header_formatchecker.cc
    io_soma/trm_header_formatreader.cc
    io_soma/xfmformatchecker.cc
    io_soma/xfmformatreader.cc
    io_soma/xfmformatwriter.cc
    math/gausslu.cc
    math/mathelem.cc
    math/romberg.cc
    math/trapezeitg.cc
    math/triederRot.cc
    math/gccbuiltin.c
    memmap/allocator.cc
    mesh/cutmesh.cc
    mesh/mesh_rasterization_d.cc
    mesh/surface.cc
    mesh/surfacegen.cc
    mesh/surfacemanip.cc
    mesh/tex2graph.cc
    mesh/texture.cc
    mesh/texturetools.cc
    meshprojection/meshprojection.cc
    resampling/mask.cc
    resampling/motion.cc
    resampling/linearInterpolator.cc
    resampling/quaternion.cc
    resampling/standardreferentials.cc
    roi/fill.cc
    roi/hie.cc
    roi/maskIterator.cc
    roi/roiIterator.cc
    roi/roi.cc
    roi/roi_diff.cc
    roi/voxelSampling.cc
    selection/selection.cc
    selection/selector.cc
    sparsematrix/ciftitools.cc
    sparsematrix/sparseMatrix.cc
    sparsematrix/sparseMatrixItemR.cc
    sparsematrix/sparseMatrixItemW.cc
    sparsematrix/sparsematrixutil.cc
    sparsematrix/sparseordensematrix.cc
    transformation/affinetransformation3d.cc
    transformation/transformation_chain.cc
    transformation/transformationgraph3d.cc
    utility/boundingbox.cc
    utility/clock.cc
    utility/masscenter.cc
    utility/progress.cc
)

BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers aims headersTarget )
if( NOT LIBKDTREE_FOUND )
  # copy internal headers for libkdtree+
  set( _kdtreeHeaders kdtree++/allocator.hpp
    kdtree++/function.hpp
    kdtree++/iterator.hpp
    kdtree++/kdtree.hpp
    kdtree++/node.hpp
    kdtree++/region.hpp
    )
  BRAINVISA_COPY_AND_INSTALL_HEADERS( _kdtreeHeaders "" _kdtreeHeadersTarget )
endif()
set(AIMSDATA_HEADERS ${_headers} PARENT_SCOPE)

# add_library( aimsdata_0 SHARED ${_aimsdata_0_sources} )
# add_dependencies( aimsdata_0 ${headersTarget} )
# set_property( TARGET aimsdata_0 PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )

add_library( aimsdata SHARED ${_aimsdata_sources} )
add_dependencies( aimsdata ${headersTarget} )
set_property( TARGET aimsdata PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )

if( NOT LIBKDTREE_FOUND )
#   add_dependencies( aimsdata_0 ${_kdtreeHeadersTarget} )
  add_dependencies( aimsdata ${_kdtreeHeadersTarget} )
endif()

# target_link_libraries( aimsdata_0 graph cartodata )
# target_link_libraries( aimsdata aimsdata_0 )
target_link_libraries( aimsdata graph cartodata )

if( WIN32 AND CMAKE_CROSSCOMPILING )
#   target_link_libraries( aimsdata_0 gnurx )
#   target_link_libraries( aimsdata )
  target_link_libraries( aimsdata gnurx )
endif( WIN32 AND CMAKE_CROSSCOMPILING )
BRAINVISA_INSTALL( TARGETS aimsdata # aimsdata_0
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )
