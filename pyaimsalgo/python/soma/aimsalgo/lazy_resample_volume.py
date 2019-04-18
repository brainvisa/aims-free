
from __future__ import print_function

from soma.aims.lazy_read_data import LazyReadData
from soma import aims, aimsalgo
import six

class LazyResampleVolume(LazyReadData):

    '''
    A specialized version of aims.LazyReadData dedicated to Volumes, which can perform voxel type conversion and resampling to another space when reading data.

    LazyResampleVolume is useful when operations have to be performed on several volumes which are not initially in the same space.

    ::

        image_names = ['image%02d.nii' % i for i in range(10)]
        transf_names = ['transform%02d' % i for i in range(10)]
        rvols = [lazy_resample_volume.LazyResampleVolume(
                    f, transform=t, nops=1, dims=(256, 256, 200, 1),
                    vox_size=(1, 1, 1, 1), dtype='FLOAT')
                  for f, t in zip(image_names, transf_names)]
        res = sum(rvols) / len(rvols)

    '''

    def __init__(self, data_or_filename, allocator_context=None,
                 read_options=None, nops=0, reader=None, dtype=None,
                 transform=None, dims=None, vox_size=(1., 1., 1., 1.),
                 resampling_order=1, default_value=0, **kwargs):
        '''
        Parameters
        ----------
        data_or_filename: see LazyReadData
        allocator_context: see LazyReadData
        read_options: see LazyReadData
        nops: see LazyReadData
        reader: see LazyReadData
        dtype: str or type
            may specify a conversion to a specific voxel type
        transform: str or aims.AffineTransformation3d or list
            Transformations to be applied to the volume when it is read. May
            be an AffineTransformation3d instance, or a filename (.trm file),
            or a list of transformations / filenames to be combined (applied
            rioght to left, thus matrices are multiplied in the left-to-right
            order ).
        dims: list or tuple of int
            resampled volume dimensions in voxels
        vox_size: list or tuple of float
            resampled volume voxel sizes
        resampling_order: int
            interpolation order for the resampling
        default_value: data type
            default background value for the resampled volume
        kwargs: see LazyReadData
        '''
        super(LazyResampleVolume, self).__init__(
            data_or_filename, allocator_context=allocator_context,
            read_options=read_options, nops=nops, reader=reader, **kwargs)
        self.dims = dims
        self.vox_size = vox_size
        self.resampling_order = resampling_order
        self.default_value = default_value
        if dtype is None:
            self.dtype = None
        else:
            self.dtype = aims.typeCode(dtype)
        if isinstance(transform, six.types.StringTypes):
            self.transform_filename = [transform]
            self.transform = None
        elif isinstance(transform, (list, tuple)):
            self.transform_filename = transform
            self.transform = None
        else:
            self.transform_filename = None
            self.transform = transform

    def _lazy_read(self):
        if self.data is None:
            super(LazyResampleVolume, self)._lazy_read()
            if self.dtype is not None \
                    and aims.typeCode(self.data.at(0)) != self.dtype:
                conv = aims.Converter(type(self.data),
                                      type(aims.Volume(dtype=self.dtype)))
                self.data = conv(self.data)
            if self.dims is not None and self.vox_size is not None:
                tmp_trans = False
                if self.transform is None \
                        and self.transform_filename is not None:
                    for tname in self.transform_filename:
                        if isinstance(tname, aims.AffineTransformation3d):
                            trans = tname.__class__(tname)
                        else:
                            trans = aims.read(tname)
                        if self.transform is None:
                            self.transform = trans
                        else:
                            self.transform *= trans
                    tmp_trans = True
                if self.transform is None:
                    self.transform = aims.AffineTransformation3d()
                if not self.transform.isIdentity() \
                        or self.dims != self.data.getSize() \
                        or self.vox_size != self.data.getVoxelSize():
                    resampler = aimsalgo.ResamplerFactory(
                        self.data).getResampler(self.resampling_order)
                    resampler.setDefaultValue(self.default_value)
                    out_vol = type(self.data)(self.dims)
                    out_vol.copyHeaderFrom(self.data.header())
                    out_vol.header()['voxel_size'] = self.vox_size
                    resampler.resample(self.data, self.transform,
                                       self.default_value, out_vol)
                    self.data = out_vol
                if tmp_trans:
                    self.transform = None
        return self.data

