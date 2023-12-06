from __future__ import print_function

import unittest
import doctest
import zipfile
import os
from shutil import rmtree
import tempfile
import sys
import six.moves.urllib.request

from soma.test_utils import test_notebook as tnb


def setup_doctest(test):
    tests_dir = os.getenv("BRAINVISA_TEST_RUN_DATA_DIR")
    if not tests_dir:
        tests_dir = tempfile.gettempdir()
    tests_dir = os.path.join(tests_dir, "tmp_tests_brainvisa")
    if not os.path.exists(tests_dir):
        os.makedirs(tests_dir)
    os.chdir(tests_dir)
    if not os.path.exists("test_data.zip"):
        print("Download https://brainvisa.info/download/data/test_data.zip "
              "to ", tests_dir)
        six.moves.urllib.request.urlretrieve(
            "https://brainvisa.info/download/data/test_data.zip",
            "test_data.zip")
    if os.path.exists("data_for_anatomist"):
        rmtree("data_for_anatomist")
    if os.path.exists("data_unprocessed"):
        rmtree("data_unprocessed")
    zf = zipfile.ZipFile("test_data.zip")
    zf.extractall()


def teardown_doctest(test):
    os.chdir(test.globs['curdir'])


class PyaimsNBTest(unittest.TestCase):
    @unittest.skipIf(tnb.main_jupyter is None, 'Notebook tests not supported')
    def test_pyaims_notebook(self):
        # try the notebook version if it can be processed on this system
        print('testing notebook')
        tnb.test_notebook(
            os.path.join(os.path.dirname(sys.argv[0]),
                          'pyaims_tutorial_nb.ipynb'),
            timeout=300)


def test_suite():
    suite = unittest.TestSuite()
    tutorial_file = "pyaims_tutorial.rst"
    doctest_suite = unittest.TestSuite(
        doctest.DocFileSuite(tutorial_file,
                             globs={'curdir': os.path.abspath(os.curdir),
                                    'print_function': print_function},
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
