
import unittest
# import sys
import shutil
import uuid
import numpy as np
import os
import os.path as osp
import tempfile
import subprocess
import yaml

from soma import aims


class AimsApplyTransformTestCase(unittest.TestCase):
    base_work_dir = None

    def setUp(self):
        self.tmp = []
        if self.base_work_dir is None:
            t = tempfile.mkdtemp(prefix='aims_test_applytrans_')
            self.tmp.append(t)
            self.work_dir = t
        else:
            self.work_dir = self.base_work_dir
            if not osp.exists(self.work_dir):
                os.makedirs(self.work_dir)

    def tearDown(self):
        del self.work_dir
        for t in self.tmp:
            shutil.rmtree(t)
        self.tmp = []

    def _get_test_vol(self):
        vol = aims.Volume(10, 15, 20, dtype='S16')
        vol.setVoxelSize([0.8, 1.1, 1.3])
        vol.fill(0)
        vol[3:6, 4:12, 5:13, 0] = 100
        vol[4, 8, 9] = 200
        return vol

    def get_test_vol(self, with_ref=False, ref=None):
        vol = self._get_test_vol()
        h = vol.header()
        if ref is not None:
            h['referential'] = ref
        if with_ref:
            if ref is None:
                ref = str(uuid.uuid4())
                h['referential'] = ref
            h['referentials'] = [
                'A',
                aims.StandardReferentials.mniTemplateReferential(),
                aims.StandardReferentials.commonScannerBasedReferential()]

            q1 = aims.Quaternion()
            q1.fromAxis((0, 0, 1), 5. / 180. * np.pi)
            t1 = aims.AffineTransformation3d(q1)
            t1.setTranslation((3.57, 8.4, 9.1))

            t2 = aims.AffineTransformation3d()
            t2.affine()[:, :, 0, 0] = np.diag((1.2, 1.1, 0.9, 1.))
            t2.setTranslation((-3.36, -8.46, -11.7))
            ti = aims.AffineTransformation3d()
            ti.affine()[:3, :3, 0, 0] = np.diag((-1, -1, -1))
            t2 = ti * t2

            q3 = aims.Quaternion()
            q3.fromAxis((1, 0, 0), 7. / 180. * np.pi)
            t3 = aims.AffineTransformation3d(q3)
            t3.setTranslation((3.57, 8.4, 9.1))
            t3 = ti * t3

            h['transformations'] = [t1.toVector(), t2.toVector(),
                                    t3.toVector()]
        return vol

    def _get_ref_vol(self):
        vol = aims.Volume(14, 19, 27, dtype='S16')
        vol.setVoxelSize([0.5, 0.7, 0.9])
        vol.fill(0)
        return vol

    def get_ref_vol(self, with_ref=False, with_common_ref=True, ref=None):
        vol = self._get_ref_vol()
        h = vol.header()
        if ref is not None:
            h['referential'] = ref
        if with_ref:
            if ref is None:
                ref = str(uuid.uuid4())
                h['referential'] = ref
            h['referentials'] = [
                'R',
                aims.StandardReferentials.commonScannerBasedReferential()]

            q1 = aims.Quaternion()
            q1.fromAxis((0, 1, 1), 3. / 180. * np.pi)
            t1 = aims.AffineTransformation3d(q1)
            t1.setTranslation((-3.3441, -6.1327, -10.8))

            ti = aims.AffineTransformation3d()
            ti.affine()[:3, :3, 0, 0] = np.diag((-1, -1, -1))

            q2 = aims.Quaternion()
            q2.fromAxis((0, 0, 1), 7. / 180. * np.pi)
            t2 = aims.AffineTransformation3d(q2)
            t2.setTranslation((-3.3441, -6.1327, -10.8))
            t2 = ti * t2

            h['transformations'] = [t1.toVector(), t2.toVector()]

            if with_common_ref:
                q3 = aims.Quaternion()
                q3.fromAxis((0, 0, 1), -4 / 180. * np.pi)
                t3 = aims.AffineTransformation3d()
                t3.affine()[:, :, 0, 0] = np.diag((0.95, 1.05, 1.1, 1.))
                t3.setTranslation((-3.40684, -5.63082, -10.8))
                t3 = ti * aims.AffineTransformation3d(q3) * t3
                h['referentials'].append(
                    aims.StandardReferentials.mniTemplateReferential())
                h['transformations'] = [t1.toVector(), t2.toVector(),
                                        t3.toVector()]

        return vol

    def get_vols(self, with_in_ref, with_out_ref, with_common_ref=True,
                 refs=None):
        if refs is None:
            refs = (None, None)
        vol = self.get_test_vol(with_in_ref, ref=refs[0])
        rvol = self.get_ref_vol(with_out_ref, with_common_ref, ref=refs[1])
        return vol, rvol

    def prepare_vols(self, with_in_ref, with_out_ref, with_common_ref=True,
                     refs=None):
        test = osp.join(self.work_dir, 'test.nii')
        ref = osp.join(self.work_dir, 'ref.nii')
        vol, rvol = self.get_vols(with_in_ref, with_out_ref, with_common_ref,
                                  refs=refs)
        out = osp.join(self.work_dir, 'out.nii')
        aims.write(vol, test)
        aims.write(rvol, ref)
        return test, ref, out

    def prepare_graph(self, vols):
        graph = aims.TransformationGraph3d()
        refs = []
        for vol in vols:
            refs.append(graph.updateFromObjectHeader(vol.header()))
        return graph, refs

    def test_01_simple_id(self):
        test, ref, out = self.prepare_vols(False, False)
        cmd = ['AimsApplyTransform', '-t', 'n', '-i', test, '-r', ref,
               '-o', out]
        subprocess.check_call(cmd)
        ovol = aims.read(out)
        self.assertEqual(list(ovol.getSize()), [14, 19, 27, 1])
        self.assertEqual(ovol[4, 6, 7, 0], 100)
        self.assertEqual(ovol[8, 18, 17, 0], 100)
        self.assertEqual(ovol[9, 18, 17, 0], 0)
        self.assertEqual(ovol[7, 13, 13, 0], 200)

    def test_02_simple_translation(self):
        test, ref, out = self.prepare_vols(False, False)
        tr = aims.AffineTransformation3d()
        tr.setTranslation([2.-0.15, -1.-0.2, 1.-0.2])
        trm = osp.join(self.work_dir, 'trans.trm')
        aims.write(tr, trm)
        cmd = ['AimsApplyTransform', '-t', 'n', '-i', test, '-r', ref,
               '-o', out, '-d', trm]
        subprocess.check_call(cmd)
        ovol = aims.read(out)
        self.assertEqual(list(ovol.getSize()), [14, 19, 27, 1])
        self.assertEqual(ovol[8, 4, 8, 0], 100)
        self.assertEqual(ovol[7, 4, 8, 0], 0)
        self.assertEqual(ovol[12, 16, 18, 0], 100)
        self.assertEqual(ovol[11, 15, 18, 0], 100)
        self.assertEqual(ovol[10, 11, 14, 0], 200)

    def test_03_via_mni(self):
        test, ref, out = self.prepare_vols(True, True, True)
        cmd = ['AimsApplyTransform', '-t', 'n', '-i', test, '-r', ref,
               '-o', out]
        subprocess.check_call(cmd)
        ovol = aims.read(out)
        self.assertEqual(list(ovol.getSize()), [14, 19, 27, 1])
        self.assertEqual(ovol[6, 2, 5, 0], 100)
        self.assertEqual(ovol[5, 2, 5, 0], 0)
        self.assertEqual(ovol[10, 15, 13, 0], 100)
        self.assertEqual(ovol[8, 9, 10, 0], 200)

    def test_04_via_mni_broken(self):
        test, ref, out = self.prepare_vols(True, True, False)
        cmd = ['AimsApplyTransform', '-t', 'n', '-i', test, '-r', ref,
               '-o', out]
        subprocess.check_call(cmd)
        ovol = aims.read(out)
        self.assertEqual(list(ovol.getSize()), [14, 19, 27, 1])
        # here the ID transform has been applied because there is no path
        # between test and ref, as in test 01
        self.assertEqual(ovol[4, 6, 7, 0], 100)
        self.assertEqual(ovol[8, 18, 17, 0], 100)
        self.assertEqual(ovol[9, 18, 17, 0], 0)
        self.assertEqual(ovol[7, 13, 13, 0], 200)

    def test_05_trm_from_mni(self):
        test, ref, out = self.prepare_vols(True, True, False)
        tr = aims.AffineTransformation3d()
        tr.setTranslation([2.-0.15, -1.-0.2, 1.-0.2])
        trm = osp.join(self.work_dir, 'trans.trm')
        aims.write(tr, trm)
        cmd = ['AimsApplyTransform', '-t', 'n', '-i', test, '-r', ref,
               '-o', out, '-d', trm, '--input-coords', 'mni']
        subprocess.check_call(cmd)
        ovol = aims.read(out)
        self.assertEqual(list(ovol.getSize()), [14, 19, 27, 1])
        self.assertEqual(ovol[5, 4, 6, 0], 100)
        self.assertEqual(ovol[0, 0, 0, 0], 100)
        self.assertEqual(ovol[6, 0, 2, 0], 0)

    def test_06_trm_from_mni_forced(self):
        # ref has a transform to MNI, but we force another one.
        # we test that the forced one is used in priority,
        # but the output header is not updated, for now.
        # what is the good behavior ?
        test, ref, out = self.prepare_vols(True, True, True)
        tr = aims.AffineTransformation3d()
        tr.setTranslation([2.-0.15, -1.-0.2, 1.-0.2])
        trm = osp.join(self.work_dir, 'trans.trm')
        aims.write(tr, trm)
        cmd = ['AimsApplyTransform', '-t', 'n', '-i', test, '-r', ref,
               '-o', out, '-d', trm, '--input-coords', 'mni']
        subprocess.check_call(cmd)
        ovol = aims.read(out)
        self.assertEqual(list(ovol.getSize()), [14, 19, 27, 1])
        self.assertEqual(ovol[5, 4, 6, 0], 100)
        self.assertEqual(ovol[0, 0, 0, 0], 100)
        self.assertEqual(ovol[6, 0, 2, 0], 0)

    def test_07_trm_to_other(self):
        test, ref, out = self.prepare_vols(True, True, False)
        tr = aims.AffineTransformation3d()
        tr.setTranslation([-5., -15., -20.])
        trm = osp.join(self.work_dir, 'trans.trm')
        ograph = osp.join(self.work_dir, 'out_graph.yaml')
        aims.write(tr, trm)
        cmd = ['AimsApplyTransform', '-t', 'n', '-i', test, '-r', ref,
               '-o', out, '-d', trm, '--input-coords', 'A',
               '--output-coords', 'R', '--out-graph', ograph]
        subprocess.check_call(cmd)
        ovol = aims.read(out)
        self.assertEqual(list(ovol.getSize()), [14, 19, 27, 1])
        self.assertEqual(ovol[7, 5, 9, 0], 100)
        self.assertEqual(ovol[6, 5, 9, 0], 0)
        self.assertEqual(ovol[10, 17, 17, 0], 100)
        self.assertEqual(ovol[8, 12, 13, 0], 200)
        # test output graph
        with open(ograph) as f:
            graph = yaml.safe_load(f)
        th = aims.read(test + '.minf')
        oh = aims.read(out + '.minf')
        tref = th['referential']
        oref = oh['referential']
        refs = {tref, oref, 'A'}
        srefs = {
            'A', aims.StandardReferentials.mniTemplateReferentialID(),
            aims.StandardReferentials.commonScannerBasedReferential() + '_'
            + tref,
            f'disk_space_{tref}',
        }
        orefs = {
            'R',
            aims.StandardReferentials.commonScannerBasedReferential() + '_'
            + oref,
            f'disk_space_{oref}',
        }
        arefs = {'R', }
        self.assertEqual(set(graph.keys()), refs)
        self.assertEqual(set(graph[tref].keys()), srefs)
        self.assertEqual(set(graph[oref].keys()), orefs)
        self.assertEqual(set(graph['A'].keys()), arefs)

    def test_08_graph_via_mni(self):
        test, ref = self.get_vols(True, True, True)
        graph, refs = self.prepare_graph((test, ref))
        graph_f = osp.join(self.work_dir, 'graph.yaml')
        aims.write(graph, graph_f, options={'embed_affines': True})

        test, ref, out = self.prepare_vols(
            False, False, False,
            refs=(test.header()['referential'], ref.header()['referential']))

        cmd = ['AimsApplyTransform', '-t', 'n', '-i', test, '-r', ref,
               '-o', out, '-g', graph_f]
        subprocess.check_call(cmd)
        ovol = aims.read(out)
        self.assertEqual(list(ovol.getSize()), [14, 19, 27, 1])
        self.assertEqual(ovol[6, 2, 5, 0], 100)
        self.assertEqual(ovol[5, 2, 5, 0], 0)
        self.assertEqual(ovol[10, 15, 13, 0], 100)
        self.assertEqual(ovol[8, 9, 10, 0], 200)


if __name__ == '__main__':
    # AimsApplyTransformTestCase.base_work_dir = '/tmp/aims_test_applytrans'
    unittest.main()
