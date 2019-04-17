
from __future__ import print_function

from soma.aims.lazy_read_data import LazyReadData
from soma import aims, aimsalgo
import six

class LazyResampleVolume(LazyReadData):
    def __init__(self, data_or_filename, allocator_context=None,
                 read_options=None, nops=0, reader=None, dtype=None,
                 transform=None, dims=None, vox_size=(1., 1., 1., 1.),
                 resampling_order=1, default_value=0, **kwargs):
        '''
        Parameters
        ----------
        dtype: str or type
            may specify a conversion to a specific voxel type
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

