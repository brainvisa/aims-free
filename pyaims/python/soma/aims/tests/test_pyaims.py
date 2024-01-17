
import unittest
import doctest
import os
import tempfile
import sys

from soma.test_utils import test_notebook as tnb
from soma.aims import demotools


def setup_doctest(test):
    tests_dir = os.getenv("BRAINVISA_TEST_RUN_DATA_DIR")
    if not tests_dir:
        tests_dir = tempfile.gettempdir()
    tests_dir = os.path.join(tests_dir, "tmp_tests_brainvisa")
    if not os.path.exists(tests_dir):
        os.makedirs(tests_dir)
    os.chdir(tests_dir)
    demotools.install_demo_data("test_data.zip", install_dir=os.getcwd())


def teardown_doctest(test):
    os.chdir(test.globs['curdir'])


class PyaimsNBTest(unittest.TestCase):
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
