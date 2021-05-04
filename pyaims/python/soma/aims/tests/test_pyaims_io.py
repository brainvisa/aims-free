# -*- coding: utf-8 -*-
from __future__ import print_function

from __future__ import absolute_import
import glob
import cmath
import tempfile
import os
import sys
import unittest
import shutil
import six
from soma import aims
from soma.aims import soma
import numpy as np
import glob
from soma.aims.volumetools import compare_images
from six.moves import range


FLOAT_DATA_TYPES = ("FLOAT", "CFLOAT", "DOUBLE", "CDOUBLE")
NO_NAN_FORMATS = ("DICOM", )
NO_INF_FORMATS = ["MINC", "FREESURFER (MINC)"] # Minc supports NaN but not inf

class TestPyaimsIO(unittest.TestCase):

    verbose = False
    debug = False
    default_epsilon = 1e-6
    # ecat scaling is far from exact...
    epsilon = {'.v': {'S16': 1e-6, 'FLOAT': (6e-5, True)}}


    def setUp(self):
        self.work_dir = tempfile.mkdtemp(prefix='test_pyaims')
        if self.verbose or self.debug:
            print('work directory:', self.work_dir)

        if self.debug:
            aims.carto.setDebugMessageLevel(100)

        if self.verbose:
            aims.carto.setVerbose(1)

    def check_open_files(self, fnames):
        proc_dir = '/proc/self/fd'
        failing = []
        if os.path.exists(proc_dir):
            for fd in os.listdir(proc_dir):
                fdpath = os.path.join(proc_dir, fd)
                if os.path.islink(fdpath):
                    fpath = os.readlink(fdpath)
                    if fpath in fnames:
                        failing.append((fd, fpath))
        if self.verbose and failing:
            print('    ! remaining open files:', failing)
        return failing


    def use_type(self, dtype):
        formats = [('.nii', {'max_dims': 7, 'write_unallocated': True}),
                   ('.nii', {'max_dims': 4}), # to use scale factor
                   ('.nii.gz', {'max_dims': 7, 'write_unallocated': True}),
                   ('.ima', {'write_unallocated': True}),
                   ('.tiff', {'max_dims': 4}),
                   # minc 4D is OK except voxel size
                   ('.mnc', {'max_dims': 3}),
                   ('.v', {'max_dims': 3})] # ecat >= 4D unsupported

        # create test volume for 8D IO
        vol = aims.Volume((9, 8, 7, 6, 5, 4, 3, 3), dtype=dtype)
        vol.header()['voxel_size'] = [0.5, 0.6, 0.7, 0.8, 0.9, 1., 1.1, 1.2]
        view = ((2, 3, 2, 1, 1, 1, 1, 1), (7, 5, 2, 2, 3, 2, 2, 2),
                (1, 2, 3, 3, 2, 0, 0, 0), (7, 5, 2, 2, 3, 2, 2, 2))
        vol.arraydata()[:] \
            = np.ogrid[0:vol.arraydata().ravel().shape[0]].reshape(
                vol.arraydata().shape)
        failing_files = set()
        for format in formats:
            options = {}
            if isinstance(format, tuple):
                format, options = format
            failing_files.update(self.use_format(vol, format, view, options))
        return failing_files


    def file_name(self, vol, dtype, format):
        def sliced_suffix(vol):
            if np.any(np.array(vol.getSize()[3:]) - 1 != 0):
                return '_t0000_s0'
            else:
                return '_0000'
        suffixes = {'.dcm': '*', '.tiff': sliced_suffix, '.jpg': sliced_suffix,
                    '.bmp': sliced_suffix}
        suffix = suffixes.get(format, '')
        if not isinstance(suffix, str):
            suffix = suffix(vol)
        fname = os.path.join(
            self.work_dir, 'vol_%s%s%s'
            % (dtype, suffix, format))
        if '*' in suffix:
            # wildcard in suffix: use glob to get the filename
            fname = glob.glob(fname)[0]
        return fname


    def get_format_cmp_epsilon(self, format, dtype):
        thresh = self.epsilon.get(format, self.default_epsilon)
        rel_thresh = False
        if isinstance(thresh, dict):
            thresh = thresh.get(dtype, self.default_epsilon)
        if isinstance(thresh, tuple):
            thresh, rel_thresh = thresh
        return thresh, rel_thresh


    def use_format(self, vol, format, view, options={}):
        partial_read = ['.nii', '.nii.gz', '.ima', '.mnc', '.mgh', '.mgz'] #, '.dcm']
        partial_write = ['.nii', '.ima']
        dtype = aims.typeCode(np.asarray(vol).dtype)

        ndim = len(vol.getSize())
        if 'max_dims' in options:
            ndim = options['max_dims']
            vol = aims.VolumeView(vol, [0] * ndim, vol.getSize()[:ndim])
            view = [x[:ndim] for x in view]
            for i in range(ndim, len(vol.header()['voxel_size'])):
                vol.header()['voxel_size'][i] = 1.

        if self.verbose:
            print('testing type: %s, format: %s, dims: %d'
                  % (dtype, format, ndim))
        # write volume
        fname = os.path.join(
            self.work_dir, 'vol_%s%s' % (dtype, format))
        w_fname = fname # write under this file name
        aims.write(vol, fname)
        vol1_name = os.path.basename(fname) + ' (written)'

        # re-read it
        fname = self.file_name(vol, dtype, format)
        #print('re-read:', fname, file=sys.stderr)
        vol2_name = os.path.basename(fname) + ' (re-read)'
        vol2 = aims.read(fname)

        # ensure we get the same
        thresh, rel_thresh = self.get_format_cmp_epsilon(format, dtype)
        self.assertTrue(compare_images(vol, vol2, vol1_name, vol2_name, thresh,
                            rel_thresh))
        del vol2

        # test native file without minf
        minf_fname = fname + '.minf'
        if os.path.exists(minf_fname):
            minf = aims.read(minf_fname)
            if 'filenames' not in minf:
                os.unlink(minf_fname)
                vol3_name = os.path.basename(fname) \
                    + ' (re-read without .minf)'
                vol3 = aims.read(fname)
                self.assertTrue(compare_images(vol, vol3, vol1_name, vol3_name,
                                               thresh, rel_thresh))
        else:
            minf = {} # no .minf file (dicom)
        
        view_pos1, view_size1, view_pos2, view_size2 = view
        if format in partial_read:
            if self.verbose:
                print('    testing partial reading:', format)
            url_ext = '&&'.join(['ox%d=%d' % (i + 1, n)
                                 for i, n in enumerate(view_pos1)])
            url_ext += '&&' + '&&'.join(['sx%d=%d' % (i + 1, n)
                                         for i, n in enumerate(view_size1)])
            vol3 = aims.read(fname + '?%s' % url_ext)
            volsize = list(vol3.getSize())
            if len(volsize) > len(view_size1):
              view_size1 = view_size1 + (1, ) * (len(volsize)
                                                 - len(view_size1))
            self.assertEqual(
                vol3.getSize(), view_size1,
                msg='partial read test for %s, size differ: %s instead of %s'
                % (format, repr(list(vol3.getSize())), repr(view_size1)))
            vol4 = aims.VolumeView(vol, view_pos1, view_size1)
            self.assertTrue(compare_images(vol4, vol3,
                                           'sub-volume %s (write, format %s)'
                                           % (aims.typeCode(vol), format),
                                           'patially read', thresh,
                                           rel_thresh))
        if format in partial_write:
            if self.verbose:
                print('    testing partial writing:', format)
            vol2 = aims.VolumeView(vol, view_pos2, view_size2)
            url_ext = '&&'.join(['ox%d=%d' % (i + 1, n)
                                 for i, n in enumerate(view_pos2)])
            aims.write(vol2, fname + '?partial_writing=1&&%s' % url_ext)
            vol3 = aims.read(fname)
            self.assertEqual(vol3.getSize(), vol.getSize())
            vol4 = aims.VolumeView(vol3, view_pos2, view_size2)
            # compare the written view
            self.assertTrue(compare_images(vol4, vol2,
                                           'sub-volume %s (write, format %s)'
                                           % (aims.typeCode(vol), format),
                                           'patially written', thresh,
                                           rel_thresh))
            # compare a part of the original volume
            view_size = [min(x, s) for x, s in zip((10, 10, 5, 1), vol.shape)]
            vol2 = aims.VolumeView(vol, (0, 0, 0, 0), view_size)
            vol4 = aims.VolumeView(vol3, (0, 0, 0, 0), view_size)
            self.assertTrue(compare_images(vol4, vol2,
                                           'sub-volume %s (write, format %s)'
                                           % (aims.typeCode(vol), format),
                                           'original part', thresh,
                                           rel_thresh))

        if options.get('write_unallocated', False):
            if self.verbose:
                print('    testing unallocated volume writing')
            # create an unallocated volume
            vol5 = aims.Volume(
                vol.getSize(),
                aims.carto.AllocatorContext(
                    aims.carto.AllocatorStrategy.InternalModif),
                False, dtype=aims.typeCode(np.asarray(vol).dtype))
            self.assertFalse(vol5.allocatorContext().isAllocated())
            vol5.copyHeaderFrom(vol.header())
            os.unlink(fname)
            aims.write(vol5, fname)
            vol3 = aims.read(fname)
            self.assertEqual(vol3.getSize(), vol.getSize())
            if format in partial_write:
                vol2 = aims.VolumeView(vol, view_pos2, view_size2)
                url_ext = '&&'.join(['ox%d=%d' % (i + 1, n)
                                    for i, n in enumerate(view_pos2)])
                aims.write(vol2, fname + '?partial_writing=1&&%s' % url_ext)
                vol3 = aims.read(fname)
                self.assertEqual(vol3.getSize(), vol.getSize())
                vol4 = aims.VolumeView(vol3, view_pos2, view_size2)
                # compare the written view
                self.assertTrue(compare_images(vol4, vol2,
                                    'sub-volume %s (write, format %s)'
                                    % (aims.typeCode(vol), format),
                                    'patially written', thresh, rel_thresh))

        # disable this test for now.
        # test IO for VolumeView
        if 'filenames' in minf:
            for f in minf['filenames']:
                os.unlink(os.path.join(os.path.dirname(fname), f))
        else:
            os.unlink(fname)
        if os.path.exists(minf_fname):
            os.unlink(minf_fname)
        pos = [max(min(x, s - 2), 0) for x, s in zip((2, 3, 2, 0), vol.shape)]
        view_size = [min(x, s-p)
                     for x, s, p in zip((7, 5, 4, 1), vol.shape, pos)]
        pos = tuple(pos)
        view_size = tuple(view_size)
        vol2 = aims.VolumeView(vol, pos, view_size)
        aims.write(vol2, w_fname)
        fname = self.file_name(vol2, dtype, format)
        vol3 = aims.read(fname)
        self.assertEqual(tuple(vol3.getSize()), view_size)
        self.assertTrue(compare_images(vol2, vol3, 'volume view',
                                       're-read volume view', thresh,
                                       rel_thresh))

        # check if files remain open
        failing_files = self.check_open_files([fname, minf_fname])
        return failing_files
    
    def test_pyaims_io(self):
        types = ['S16', 'FLOAT']
        failing_files = set()
        for dtype in types:
            failing_files.update(self.use_type(dtype))

        if failing_files:
            raise RuntimeError('There are still open files: %s'
                % repr(failing_files) )

    def test_soma_io(self):
        def parses_type_code(type_code):
            return type_code.split(' of ')

        def is_volume_type(object_type):
            return object_type in ('VolumeRef', 'carto_volume')

        def get_type_info(type_code):
            lt = parses_type_code(type_code)
            o, d = (None, None)
            if len(lt) > 0:
                o = lt[0]
                if len(lt) > 1:
                    d = ' of '.join(lt[1:])

            return (o, d)

        def get_python_type(type_code):
            if six.PY2:
                longtype = long
            else:
                longtype = int
            tmap = {'U8': int, 'S8': int,
                    'U16': int, 'S16': int,
                    'U32': int, 'S32': int,
                    'U64': longtype, 'S64': longtype,
                    'FLOAT': float, 'DOUBLE': float,
                    'CFLOAT': complex, 'CDOUBLE': complex,
                    'RGB': aims.AimsRGB, 'RGBA': aims.AimsRGBA,
                    'HSV': aims.AimsHSV, #'HSVA': aims.AimsHSVA,
                    'POINT2D': aims.Point2d, 'POINT2DF': aims.Point2df,
                    'POINT2DD': aims.Point2d, 'POINT2DU': aims.Point2du,
                    'POINT3D': aims.Point3d, 'POINT3DF': aims.Point3df,
                    'POINT3DD': aims.Point3d, 'POINT3DU': aims.Point3du,
                    'POINT4D': aims.Point4d, 'POINT4DF': aims.Point4df,
                    'POINT4DD': aims.Point4d, 'POINT4DU': aims.Point4du}
            t = tmap.get(type_code)
            if t is None:
                t = getattr(aims, type_code, None)

                if t is None:
                    raise TypeError('Unknown type %s' % type_code)

            return t

        def is_complex_type(data_type):
            return data_type in ('CFLOAT', 'CDOUBLE')

        def is_vector_type(data_type):

            if data_type is None:
                return False

            return data_type in ('RGB', 'RGBA', 'HSV', 'HSVA') or \
                   data_type.startswith('VECTOR_OF') or \
                   data_type.startswith('POINT')

        def get_vector_type_info(data_type):
            l, t = (None, None)
            if data_type in ('RGB', 'RGBA', 'HSV', 'HSVA'):
                l, t = (len(data_type), 'U8')
            elif data_type.startswith('VECTOR_OF'):
                v = data_type.split('_')
                if len(v) > 2:
                    l = int(v[2])
                    if len(v) > 3:
                        t = v[3]
            elif data_type.startswith('POINT'):
                v = data_type.lstrip('POINT').split('D', 1)
                if len(v) > 0:
                    l = int(v[0])
                    if len(v) > 1:
                        if v[1] == 'F':
                            t = 'FLOAT'
                        elif v[1] == 'D':
                            t = 'DOUBLE'
                        elif v[1] == 'U':
                            t = 'U32'
                        else:
                            raise TypeError('Unknown %s type' % data_type)
                    else:
                        # TODO: check that Point3d type is S16 or S32
                        t = 'S32'
            else:
                raise TypeError('Unknown type %s' % data_type)

            return (l, t)

        def get_vectorized_value(type_code, value):
            l, t = get_vector_type_info(type_code)
            if l is None or t is None:
                raise RuntimeError('Unable to vectorize value %s for type %s' \
                                   % (str(value), type_code))
            python_type = get_python_type(type_code)
            python_data_type = get_python_type(t)
            v = [python_data_type(value)] * l
            return python_type(*v)

        if self.verbose:
            print('SOMA-IO')
            print('- write format')

        # Some exceptions are necessary to make tests passing
        # now but they should be avoided in a near future
        format_exceptions = { 'DICOM': {'read_pattern': 'Volume_%s_%d_%s*%s'} }

        dsil = soma.DataSourceInfoLoader()
        for t, lf in \
                six.iteritems(aims.carto.IOObjectTypesDictionary.writeTypes()):
            object_type, data_type = get_type_info(t)
            if is_volume_type(object_type):
                try:
                    volume_type = getattr(aims, 'Volume_' + data_type)
                except:
                    print('WARNING: volume type', 'Volume_' + data_type, 
                          'exists in C++ objects but is not supported by',
                          'python bindings, so it will not be tested.',
                          file = sys.stderr)
                    continue

                volumes_info = [{ 'dims': [1,1,1], 'value': 0 },
                                { 'dims': [10,10,10], 'value': 1 }]
                if data_type in FLOAT_DATA_TYPES:
                    # Test non-normal forms of IEEE 754 floats. Some libs
                    # (including unpatched niftilib) tend to silently discard
                    # such values upon reading, but only higher-level
                    # algorithmic code can decide how to handle them properly,
                    # so the reader should let them pass through.
                    # NOTE: a test for -0.0 is missing.
                    volumes_info += [{ 'dims': [1,1,1], 'value': float("NaN") },
                                     { 'dims': [1,1,1], 'value': float("Inf") }]
                    # Test denormalized floats
                    if data_type.endswith("FLOAT"):
                        volumes_info.append({ 'dims': [1,1,1],
                                              'value': 1.40129846e-45 })
                    if data_type.endswith("DOUBLE"):
                        volumes_info.append({ 'dims': [1,1,1],
                                              'value': 4.9406564584124654e-324 })
                volumes = []   
                for vinfo in volumes_info:
                    # Get a default value
                    if is_vector_type(data_type):
                        v = get_vectorized_value(data_type, 
                                                 vinfo.get('value', 0))
                    else:
                        v = get_python_type(data_type)(vinfo.get('value', 0))

                    # Create new volume initialized
                    volume = volume_type(*vinfo.get('dims', []))
                    volume.fill(v)
                    volumes.append(volume)

                for f in lf:
                    # TODO: Check that format is also readable
                    fe = format_exceptions.get(f, dict())

                    emsg = 'testing dtype %s, format %s:' % (data_type, f)
 
                    exts = dsil.extensions(f)
                    #print('ext for write type', f, ':', exts)
                    for e in exts:
                        write_pattern = fe.get('write_pattern', 
                                               'Volume_%s_%d_%s%s')
                        read_pattern = fe.get('read_pattern', 
                                              'Volume_%s_%d_%s%s')

                        for i in range(len(volumes)):
                            if data_type in FLOAT_DATA_TYPES \
                                    and (cmath.isnan(volumes[i].at(0))
                                         or cmath.isinf(volumes[i].at(0))) \
                                    and f in NO_NAN_FORMATS:
                                continue
                            if data_type in FLOAT_DATA_TYPES \
                                    and cmath.isinf(volumes[i].at(0)) \
                                    and f in NO_INF_FORMATS:
                                continue
                            fl = os.path.join(self.work_dir, 
                                            write_pattern % (
                                                data_type, i, f, 
                                                '.' + e if len(e) > 0 else ''))
                            if self.verbose:
                                print('writing', 'Volume_' + data_type, 
                                      list(volumes[i].header()['volume_dimension']),
                                      'in', f, 'to file ', fl )

                            # Due to some ambiguities (NIFTI-1 .img and 
                            # ANALYZE .img) it is necessary to force format
                            aims.write(volumes[i], fl, format = f, 
                                       options = {'force_disk_data_type': True})

                            # Search for the written file
                            fl = os.path.join(self.work_dir, 
                                            read_pattern % (data_type, i, f, 
                                            ('.' + e if len(e) > 0 else '')))
                            found_files = glob.glob(fl)

                            self.assertNotEqual(len(found_files), 0, emsg)
                            fl = found_files[0]

                            if self.verbose:
                                print('reading', 'Volume_' + data_type, 'in',
                                      f, 'from file ', fl)

                            volume_read_back = aims.read(fl)
                            if (data_type in FLOAT_DATA_TYPES
                                and (cmath.isnan(volumes[i].at(0))
                                     or cmath.isinf(volumes[i].at(0)))):
                                if cmath.isnan(volumes[i].at(0)):
                                    self.assertTrue(
                                        cmath.isnan(volume_read_back.at(0)),
                                        msg='value %s should be nan'
                                        % repr(volume_read_back.at(0)))
                                else:
                                    self.assertTrue(
                                        cmath.isinf(volume_read_back.at(0)),
                                        msg='%s value %s should be inf'
                                        % (emsg, repr(volume_read_back.at(0))))
                            else:
                                #self.assertTrue( (volumes[i] == volume_read_back).all() )
                                thresh, rel_thresh \
                                      = self.get_format_cmp_epsilon(f,
                                                                    data_type)
                                self.assertTrue(
                                    compare_images(
                                        volumes[i], volume_read_back,
                                        'Volume_' + data_type,
                                        'Volume_' + data_type,
                                        thresh, rel_thresh), emsg)

        # TODO: Add tests for aims format and read-only formats
        #print('- read format')
        #print(aims.carto.IOObjectTypesDictionary.readTypes())

    def test_minf_uuid(self):
        '''
            check that uuid in minf is rightly written and conserved 
            by writters for existing formats.
            To summarize behaviour:
            - when output .minf file does not exists, writers must
              remove uuid property in given meta information, 
              except when specific option is given. This is because
              we want to avoid uuid copy when meta informations are 
              copied.
            - when output .minf file exists, writers must keep uuid
              existing uuid of the .minf file except when specific 
              option is given.
        '''
        from soma import aims, uuid

        import six
        import os

        def write_minf(attributes, filename, options={}):
            import os
            
            if options.get('update_minf', False):
                if os.path.exists(filename):
                    # Read minf first if it exists:
                    read_attributes = aims.read(filename, options=options)
                    read_attributes.update(attributes)
                    attributes = read_attributes
                
            #print('attributes=', repr(attributes))
            with open(filename, 'w+') as f:
                print('attributes=', repr(attributes), file=f)

        def check_meta_write_rewrite(f, format):

            def write_and_read_with_meta(meta, f, format=None, options=dict()):
                # Write image with uuid stored in .minf
                
                if self.verbose:
                    print('Writing file (%s) with updated meta-informations' %f)
                
                vol1 = aims.Volume_S16()
                h = vol1.header()
                h.update(meta)
                aims.write(vol1, f, format=format, options=options)
                
                if self.verbose:
                    print('Written header:', h)

                if self.verbose:
                    print('Re-reading file (%s)' % f)
                
                vol2 = aims.read(f)
                h = vol2.header()        
                #h = aims.read(f + '.minf')
                if self.verbose:
                    print('Re-read header:', h)
                    
                for k, v in six.iteritems(meta):
                    if self.verbose:
                        print('Re-read', k, '(%s)' % str(h.get(k)), 
                            '==', 'written', k, '(%s)' % str(v), ':', 
                            (str(v) == str(h.get(k))))
                
                return dict(h)

            #def remove_file(filename):
                ##print('checking file:', filename)
                #if os.path.exists(filename):
                    #dsil = aims.soma.DataSourceInfoLoader()
                    ##print('check file name:', filename)
                    ##sys.stdout.flush()
                    #try:
                        #dsi = dsil.check(filename)
                        
                        #print(dsi)
                        #sys.stdout.flush()
                        #return
                        #dsl = dsi.list()
                        
                        #for t in dsl.types():
                            #dsts = dsl.size(t)
                            ##print('type:', t, 'size:', dsts)
                            
                            #if t != 'default':
                                #for i in range(dsts):
                                    #ds = dsl.dataSource(t, i)
                                    
                                    ##print('url:', ds.url())
                                    #if os.path.exists(ds.url()):
                                        #os.unlink(ds.url())
                    #except:
                        #print('Unable to delete file', filename)
                        
            def remove_file(filename):
                if os.path.exists(filename):
                    os.unlink(filename)
                    
                if os.path.exists(filename + '.minf'):
                    os.unlink(filename + '.minf')
                
            # Check uuid write / rewrite
            if self.verbose:
                print('Check written uuid for a new file:', f)
            
            remove_file(f)
            
            # Output .minf file does not exists so re-read meta
            # must not contain uuid, even though it was given
            # in meta
            meta = {'uuid': str(uuid.Uuid())}
            written_meta = write_and_read_with_meta(meta, f, format)
            self.assertEqual(written_meta.get('uuid'), None)
            
            if self.verbose:
                print()

            # Force minf file update with uuid
            if self.verbose:
                print('Force minf file', f + '.minf', 'update with uuid:', meta['uuid'])
            write_minf(meta, f + '.minf', options={'update_minf': True})
                
            if self.verbose:
                print('Check rewritten uuid for an existing file:', f)

            # Once .minf file have uuid, it must be preserved by 
            # writters during rewriting
            new_meta = {'uuid': str(uuid.Uuid())}            
            rewritten_meta = write_and_read_with_meta(new_meta, f, format)
            self.assertEqual(rewritten_meta.get('uuid'), meta.get('uuid'))
            
            if self.verbose:
                print()
                
            if not self.debug:
                remove_file(f)


        #aims.carto.setVerbose(1)
        #aims.carto.setDebugMessageLevel(4)

        
        #read_only_formats = (('FDF', '.fdf'), 
                             #('GENESIS', '.hdr'),
                             #('GIF', '.gif'),
                             #('MNG', '.mng'),
                             #('OPENSLIDE', '.czi'),
                             #('OPENSLIDE', '.svs'),
                             #('SPM', '.spm'),
                             #('SVG', '.svg'),
                             #('SVGZ', '.svgz'),
                             #('TGA', '.tga'),
                             #('VIDA', '.vida'))

        read_write_formats = (('BMP', '.bmp'), 
                              ('ECAT', '.v'),  
                              ('GIS', '.ima'),
                              ('ICO', '.ico'), 
                              ('JPEG(Qt)', '.jpeg'),
                              ('JPG', '.jpg'), 
                              ('NIFTI-1', '.nii'),
                              ('NIFTI-2', '.nii'),
                              ('PBM', '.pbm'),
                              ('PGM', '.pgm'),
                              ('PNG', '.png'),
                              ('PPM', '.ppm'),
                              ('TIFF', '.tiff'),
                              ('TIFF(Qt)', '.tiff'),
                              ('XBM', '.xbm'),
                              ('XPM', '.xpm'))
                
        for s, e in read_write_formats:
            
            if self.verbose:
                print('== Check meta informations for format', s)
                
            # Check write/rewrite meta informations for a format
            f = os.path.join(self.work_dir, 'test_uuid_%s%s' % (s, e))
            check_meta_write_rewrite(f, s)
                
            if self.verbose:
                print()

    def test_io_with_strides(self):
        #formats = ['.nii', '.ima', '.tiff', '.mnc', '.v', '.jpg', '.bmp']
        formats = ['.nii', '.ima', '.mnc', '.tiff']
        failing_files = set()
        view = ((3, 1, 0, 0), (3, 3, 1, 1),
                (4, 2, 1, 0), (3, 3, 1, 1))
        options = {}

        for format in formats:
            arr = np.arange(100, dtype=np.int16).reshape((10, 5, 2, 1))
            vol = aims.Volume(arr)
            vol.header()['voxel_size'] = [0.8, 0.7, 0.6, 1.]
            failing_files.update(self.use_format(vol, format, view, options))
        if failing_files:
            raise RuntimeError('There are still open files: %s'
                % repr(failing_files) )

    def tearDown(self):
        if self.debug:
            print('leaving files in', self.work_dir)
        else:
            shutil.rmtree(self.work_dir)


def test_suite():
    if '-v' in sys.argv[1:] or '--verbose' in sys.argv[1:]:
        TestPyaimsIO.verbose = True
    return unittest.TestLoader().loadTestsFromTestCase(TestPyaimsIO)


if __name__ == '__main__':
    if '-d' in sys.argv[1:] or '--debug' in sys.argv[1:]:
        TestPyaimsIO.debug = True
        try:
            i = sys.argv.index('-d')
        except:
            i = sys.argv.index('--debug')
        del sys.argv[i]
    unittest.main(defaultTest='test_suite')

