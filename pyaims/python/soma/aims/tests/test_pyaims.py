
import unittest
import doctest
import os
import os.path as osp
import tempfile
import sys
import gc

from soma.test_utils import test_notebook as tnb
from soma import aims
from soma.aims import demotools


glob_refs = 0


def get_test_dir():
    tests_dir = os.getenv("BRAINVISA_TEST_RUN_DATA_DIR")
    if not tests_dir:
        tests_dir = tempfile.gettempdir()
    tests_dir = os.path.join(tests_dir, "tmp_tests_brainvisa")
    if not os.path.exists(tests_dir):
        os.makedirs(tests_dir)
    return tests_dir


def setup_doctest(test):
    tests_dir = get_test_dir()
    os.chdir(tests_dir)
    demotools.install_demo_data("test_data.zip", install_dir=os.getcwd())


def teardown_doctest(test):
    os.chdir(test.globs['curdir'])


class PyaimsNBTest(unittest.TestCase):
    def test_rcptr(self):
        x = aims.AffineTransformation3d()
        self.assertTrue(x.__refcount__() == 1)
        y = x
        self.assertTrue(x.__refcount__() == 1)
        del y
        test_dir = get_test_dir()
        trf = osp.join(test_dir, 'transform.trm')
        aims.write(x, trf)
        reader = aims.aimssip.Reader_AffineTransformation3d(trf)
        del x
        print('test read')
        x = reader.read()
        print('read done')
        self.assertTrue(x.__refcount__() == 1)
        y = aims.rc_ptr(x)
        self.assertTrue(x.__refcount__() == 2)
        self.assertTrue(y.__refcount__() == 2)
        del x
        gc.collect()
        # the rc_ptr object keeps a ref to the python RCObject
        self.assertTrue(y.__refcount__() == 2)
        del y
        x = reader.read()
        y = aims.rc_ptr(x)
        del y
        gc.collect()
        self.assertTrue(x.__refcount__() == 1)

    def test_rcobject_subclass(self):
        import sip

        class XT(aims.AffineTransformation3d):
            def __init__(self, *args, **kwargs):
                super().__init__(*args, **kwargs)
                global glob_refs
                glob_refs += 1

            def __del__(self):
                print('del XT, pyowned:', sip.ispyowned(self),
                      ', refs:', self.__refcount__())
                sip.transferto(self, None)
                global glob_refs
                glob_refs -= 1

        print('test XT')
        x = XT()
        print('x.__refcount__():', x.__refcount__(), ', exp: 1')
        print('glob_refs:', glob_refs, ', exp: 1')
        self.assertTrue(x.__refcount__() == 1)
        self.assertTrue(glob_refs == 1)
        del x
        print('glob_refs:', glob_refs, ', exp: 0')
        self.assertTrue(glob_refs == 0)
        x = XT()
        y = aims.rc_ptr_AffineTransformation3d(x)
        print('x.__refcount__():', x.__refcount__(), ', exp: 2')
        print('y.__refcount__():', y.__refcount__(), ', exp: 2')
        print('glob_refs:', glob_refs, ', exp: 1')
        self.assertTrue(x.__refcount__() == 2)
        self.assertTrue(y.__refcount__() == 2)
        self.assertTrue(glob_refs == 1)
        del x
        print('---1')
        print('y.__refcount__():', y.__refcount__(), ', exp: 2')
        gc.collect()
        print('---2')
        # the rc_ptr object keeps a ref to the python RCObject
        print('y.__refcount__():', y.__refcount__(), ', exp: 2')
        print('glob_refs:', glob_refs, ', exp: 1')
        self.assertTrue(y.__refcount__() == 3)
        # self.assertTrue(glob_refs == 1)
        del y
        print('glob_refs:', glob_refs, ', exp: 0')
        self.assertTrue(glob_refs == 0)
        x = XT()
        y = aims.rc_ptr_AffineTransformation3d(x)
        del y
        gc.collect()
        print('glob_refs:', glob_refs, ', exp: 1')
        self.assertTrue(glob_refs == 1)
        self.assertTrue(x.__refcount__() == 1)
        del x
        gc.collect()
        print('glob_refs:', glob_refs, ', exp: 0')
        self.assertTrue(glob_refs == 0)

    def test_testrcobject(self):
        import sip

        grc = aims.carto.TestRCObject.globalCount
        print('test TestRCOPject')
        x = aims.carto.TestRCObject(True)
        print('x.__refcount__():', x.__refcount__(),
              ', exp: 1;, ispyowned:', sip.ispyowned(x))
        self.assertTrue(grc() == 1)
        del x
        self.assertTrue(grc() == 0)
        x = aims.carto.TestRCObject(True)
        y = aims.rc_ptr_RCObject(x)
        print('x.__refcount__():', x.__refcount__(),
              ', exp: 1;, ispyowned:', sip.ispyowned(x))

        x = aims.carto.TestRCObject(True)
        self.assertTrue(x.__refcount__() == 1)
        y = aims.rc_ptr_RCObject(x)
        self.assertTrue(x.__refcount__() == 2)
        self.assertTrue(y.__refcount__() == 2)
        del x
        gc.collect()
        # the rc_ptr object keeps a ref to the python RCObject
        self.assertTrue(y.__refcount__() == 2)
        self.assertTrue(grc() == 1)
        del y
        self.assertTrue(grc() == 0)
        x = aims.carto.TestRCObject(True)
        y = aims.rc_ptr_RCObject(x)
        del y
        gc.collect()
        self.assertTrue(x.__refcount__() == 1)
        self.assertTrue(grc() == 1)
        del x
        self.assertTrue(grc() == 0)

        x = aims.carto.TestRCObject(True)
        self.assertTrue(x.__refcount__() == 1)
        y = aims.rc_ptr_RCObject(x)
        z = aims.rc_ptr_RCObject(x)
        self.assertTrue(y.__refcount__() == 3)
        self.assertTrue(grc() == 1)
        del y
        del x
        gc.collect()
        self.assertTrue(z.__refcount__() == 2)
        self.assertTrue(grc() == 1)
        del z
        gc.collect()
        self.assertTrue(grc() == 0)

    @unittest.skipIf(tnb.main_jupyter is None, 'Notebook tests not supported')
    def test_pyaims_notebook(self):
        # try the notebook version if it can be processed on this system
        print('testing pyaims tutorial notebook')
        tnb.test_notebook(
            os.path.join(os.path.dirname(sys.argv[0]),
                         'pyaims_tutorial_nb.ipynb'),
            timeout=300)

    @unittest.skipIf(tnb.main_jupyter is None, 'Notebook tests not supported')
    def test_vol_orient_notebook(self):
        # try the notebook version if it can be processed on this system
        print('testing volume orientation notebook')
        tnb.test_notebook(
            os.path.join(os.path.dirname(sys.argv[0]),
                         'volume_orientation.ipynb'),
            timeout=30)


def test_suite():
    suite = unittest.TestSuite()
    tutorial_file = "pyaims_tutorial.rst"
    doctest_suite = unittest.TestSuite(
        doctest.DocFileSuite(tutorial_file,
                             globs={'curdir': os.path.abspath(os.curdir),
                                    'print_function': print},
                             setUp=setup_doctest,
                             tearDown=teardown_doctest,
                             optionflags=doctest.ELLIPSIS))
    suite.addTest(doctest_suite)
    loader = unittest.defaultTestLoader
    tests = loader.loadTestsFromTestCase(PyaimsNBTest)
    suite.addTests(tests)
    return suite


if __name__ == '__main__':
    unittest.main(defaultTest='test_suite')
