# -*- coding: utf-8 -*-
from __future__ import print_function

import os
import shutil
import subprocess
import sys
import errno
import warnings

import soma.test_utils


class ImageFileComparison:

    """ Class to compare test command output image files.
    """

    @staticmethod
    def assertEqual(testcase, image1, image2, msg=''):
        """ Static method to do image file comparison. It reads the content
            of each image and compare it.

        Parameters
        ----------
        testcase : unittest.TestCase
            the case for which comparison is done.

        image1 : str (mandatory)
            the first image file to compare.

        image2 : str (mandatory)
            the second image file to compare.

        msg : str
            message to be displayed for the test.
        """
        import soma.aims
        import numpy

        if os.path.exists(image1):
            d1 = soma.aims.read(image1)
            h1 = dict(d1.header())
            i1 = d1.arraydata()
        else:
            h1 = dict()
            i1 = numpy.array([])


        if os.path.exists(image2):
            d2 = soma.aims.read(image2)
            h2 = dict(d2.header())
            i2 = d2.arraydata()
        else:
            h2 = dict()
            i2 = numpy.array([])

        if ((i1 == numpy.array([])) and (i2 == numpy.array([]))):
          print('WARNING: image file comparison of empty images (%s, %s)'
                % (image1, image2), file=sys.stderr)

        # Process differences between pixels
        if not numpy.array_equal(i1, i2) :
          testcase.fail(msg)
          #print('ERROR: image (%s, %s) content is not the same' % (image1, image2), file=sys.stderr)

        # testunit 1.63 for python 2.6 (installed on MacOS) does not have assertDictEqual method
        if hasattr(testcase, 'assertDictEqual'):
          # Process differences between headers
          testcase.assertDictEqual(h1, h2, msg)
          #print('ERROR: image (%s, %s) header is not the same' % (image1, image2), file=sys.stderr)

        i1 = None
        i2 = None


class TextFileComparison:

    """ Class to compare test command output text files.
    """

    @staticmethod
    def assertEqual(testcase, file1, file2, msg=''):
        """ Static method to do text file comparison. It reads the content
            of each file and compare it.

        Parameters
        ----------
        testcase : unittest.TestCase
            the case for which comparison is done.

        file1 : str (mandatory)
            the first file to compare.

        file2 : str (mandatory)
            the second file to compare.

        msg : str
            message to be displayed for the test.
        """
        f1 = open(file1, 'r')
        f2 = open(file2, 'r')
        f1content = f1.read()
        f2content = f2.read()
        f1 = None
        f2 = None
        testcase.assertEqual(f1content, f2content, msg)


class FileComparison:

    """ Class to compare test command output files.
    """

    TXT_EXTENSIONS = ('.out', '.err', '.txt', '.trm', '.minf')

    @staticmethod
    def isTextFile(file):
        """ Static method to determine wether a file must be compared as
          a text file.

        Parameters
        ----------

        file : str (mandatory)
            the file to compare.

        Returns
        ----------
        isTextFile : bool
            True if the file has a text file extension ('.out', '.err', '.txt', '.trm', '.minf'),
            False otherwise.
        """
        return (os.path.splitext(file)[1] in FileComparison.TXT_EXTENSIONS)

    @staticmethod
    def assertEqual(testcase, file1, file2, msg=''):
        """ Static method to do file comparison. It determines wether
          the file is a text file or an image file and do the comparison.

        Parameters
        ----------
        testcase : unittest.TestCase
            the case for which comparison is done.

        file1 : str (mandatory)
            the first file to compare.

        file2 : str (mandatory)
            the second file to compare.

        msg : str
            message to be displayed for the test.
        """
        f1istext = FileComparison.isTextFile(file1)
        f2istext = FileComparison.isTextFile(file2)

        if f1istext != f2istext:
            raise Exception(
                'Comparison is not possible because files %s and %s are not of the same type' % (file1, file2))
        elif f1istext:
            TextFileComparison.assertEqual(testcase, file1, file2, msg)
        else:
            ImageFileComparison.assertEqual(testcase, file1, file2, msg)


class CommandTest:

    """ CommandsTest class references a test command and its output files.
        It is used for testing a command and compare its output files to
        reference files.
    """

    def __init__(self,
                 testname,
                 command=None,
                 run_files=[]):
        """ Constructor of a CommandTest.

        Parameters
        ----------

        testname : str (mandatory)
            name of the test.

        command : list (mandatory)
            command to be run during test. The command must only contains
            relative file pathes for files to be compared during test.

        run_files : list
            list of relative file pathes generated by the command and to
            be compared during test.
        """

        self.__testname = testname
        self.__command = command

        self.__outfile = testname + '.out'
        self.__errfile = testname + '.err'

        # Processes files basename
        self.__run_files = [os.path.basename(f) for f in run_files]
        self.__last_run_directory = None

        if type(run_files) in (list, tuple):
            # Add the output file and error file
            self.__run_files += [self.__outfile, self.__errfile]

    def execute(self, run_directory=None):
        """ Excute the command in a specific directory.

        Parameters
        ----------
        run_directory : str
            directory in which output files will be generated by the command.
        """

        self.__last_run_directory = run_directory
        outfile = self.__outfile
        errfile = self.__errfile
        if run_directory is not None:
            outfile = os.path.join(run_directory, outfile)
            errfile = os.path.join(run_directory, errfile)

        fout = open(outfile, 'w+')
        ferr = open(errfile, 'w+')

        if not os.path.exists(run_directory):
            raise RuntimeError(
                'Command exited because run directory \'%s\' does not exists.'
                % (run_directory,))
        if sys.version_info >= (2, 7):
            retcode = subprocess.call(self.__command,
                                      stdout=fout,
                                      stderr=ferr,
                                      cwd=run_directory)
        else:
            # python 2.6 / Mac cat get a INTR signal
            # https://bugs.python.org/issue1068268
            p = subprocess.Popen(self.__command,
                                  stdout=fout,
                                  stderr=ferr,
                                  cwd=run_directory)
            while p.returncode is None:
                try:
                    p.communicate()
                    p.wait()
                except OSError as e:
                    if e.errno != errno.EINTR:
                        raise
            retcode = p.returncode
        if retcode != 0:
            raise RuntimeError(
                'Command exit code was not 0. code: %d, Command: %s'
                % (retcode, ' '.join(self.__command)))

    def get_last_run_files(self):
        """ Returns files generated during the last run.

        Returns
        -------
        last_run_files : list
            list of output files generated by the command during
            last run.
        """

        if self.__last_run_directory is None:
            return []
        else:
            return [os.path.join(self.__last_run_directory, f) for f in self.__run_files]

    def get_test_name(self):
        """ Returns the test name of the test command.

        Returns
        -------
        test_name : str
            test name of the test command.
        """

        return self.__testname

    def get_run_files(self):
        """ Returns the files referenced as output
        for the test command.

        Returns
        -------
        test_name : str
            list of the files referenced as output
            for the test command.
        """

        return self.__run_files

    def __str__(self):
        return ' '.join(self.__command) + \
               ' run in \'%(last_run_directory)s\'. [output => \'%(outfile)s\', error =>\'%(errfile)s\']' \
                % { 'last_run_directory': self.__last_run_directory,
                    'outfile': self.__outfile,
                    'errfile': self.__errfile }


class CommandsTestManager(soma.test_utils.SomaTestCase):

    """ CommandsTestManager class is a base class to test commands.
        Derived class needs to define an iterable containing CommandTest
        instances (test_cases). CommandTest are the command line to execute
        during the test.
    """

    def get_ref_directory(self, testcommand):
        """ Get reference directory for a CommandTest.

        Parameters
        ----------

        testname : str (mandatory)
            name of the test.

        Returns
        -------
        ref_directory : str
            path to the refernce directory of the unit test.
        """

        return os.path.join(self.private_ref_data_dir(),
                            testcommand.get_test_name())

    def get_run_directory(self, testcommand):
        """ Get run directory for a CommandTest.

        Parameters
        ----------

        testname : str (mandatory)
            name of the test.

        Returns
        -------
        run_directory : str
            path to the run directory of the unit test.
        """

        return os.path.join(self.private_run_data_dir(),
                            testcommand.get_test_name())

    @staticmethod
    def get_command_files(testcommand, directory):
        """ Static method that returns pathes to files
        generated during a CommandTest run.

        Parameters
        ----------
        testcommand : CommandTest (mandatory)
            command for which to get the files.

        directory : str (mandatory)
            directory of the run.

        Returns
        -------
        test_directory : list
            a list of pathes to files generated by the command.
        """

        if directory is None:
            return []
        else:
            return [os.path.join(directory, f)
                    for f in testcommand.get_run_files()]

    def get_ref_files(self, testcommand):
        """ Returns pathes to reference files
        generated for a CommandTest.

        Parameters
        ----------
        testcommand : CommandTest (mandatory)
            command for which to get the files.

        Returns
        -------
        ref_files : list
            a list of pathes to reference files for the command.
        """

        return CommandsTestManager.get_command_files(
            testcommand, self.get_ref_directory(testcommand))

    def get_run_files(self, testcommand):
        """ Returns pathes to run files
        generated for a CommandTest.

        Parameters
        ----------
        testcommand : CommandTest (mandatory)
            command for which to get the files.

        Returns
        -------
        ref_files : list
            a list of pathes to run files for the command.
        """

        return CommandsTestManager.get_command_files(
            testcommand, self.get_run_directory(testcommand))

    def get_run_ref_files(self, testcommand, check_ref_files=True):
        """
        Get the run and reference output files altogether. Optionaly checks if
        some reference files are not found or there are extra reference files.

        Parameters
        ----------
        testcommand : CommandTest (mandatory)
            command for which to get the files.

        Returns
        -------
        files: list of tuples
            list of tuples (run_file, ref_file).
        """
        files = zip(self.get_run_files(testcommand),
                    self.get_ref_files(testcommand))
        if check_ref_files:
            ref_files = set([t[1] for t in files])
            real_ref_files = set(os.listdir(self.get_ref_directory(testcommand)))
            if ref_files < real_ref_files:
                diff = real_ref_files - ref_files
                msg_fmt = ("Found extra reference files ({s}); "
                           "consider re-running \'ref\' mode")
                msg = msg_fmt.format(s=diff)
                warnings.warn(msg)
            if ref_files > real_ref_files:
                diff = ref_files - real_ref_files
                msg_fmt = ("Missing reference files ({s}); "
                           "test should not habe been run mode")
                msg = msg_fmt.format(s=diff)
                warnings.warn(msg)
        return files

    def setUp(self):
        """ Overriden unittest.TestCase that setup
            reference and run directories for CommandTest
            contained in the test_cases attribute.
        """
        super(CommandsTestManager, self).setUp()
        if hasattr(self, 'test_cases'):
            # Create directories for test cases
            for c in self.test_cases:
                if self.test_mode == soma.test_utils.ref_mode:
                    # Remove reference directory and re-create it
                    cmd_ref_dir = self.get_ref_directory(c)
                    if os.path.exists(cmd_ref_dir):
                        shutil.rmtree(cmd_ref_dir)
                    os.makedirs(cmd_ref_dir)
                else:
                    # Remove run directory and re-create it
                    cmd_run_dir = self.get_run_directory(c)
                    if os.path.exists(cmd_run_dir):
                        shutil.rmtree(cmd_run_dir)
                    os.makedirs(cmd_run_dir)
        else:
            raise Exception(
                'An attribute named test_cases that contains a list of CommandTest must be added.')

    def runTest(self):
        """
        Run registered TestCommand.
        """

        n_failed = 0
        for c in self.test_cases:
            try:
                if self.test_mode == soma.test_utils.ref_mode:
                    # Run the test in the test ref directory
                    c.execute(self.get_ref_directory(c))
                    #print('command run normally:', c, file=sys.stderr)
                else:
                    # Get reference and run files (this is a bit of extra work
                    # if we don't run the test but simplify code)
                    run_ref_files = self.get_run_ref_files(c)

                    # Check that reference files exist (otherwise don't run the
                    # test)
                    ref_files = [t[1] for t in run_ref_files]
                    if not all(map(os.path.exists, ref_files)):
                        msg_fmt = ("Can't run %s because some reference files "
                                   "are missing; re-run in ref mode.")
                        msg = msg_fmt % c.get_test_name()
                        raise EnvironmentError(msg)

                    # Run the test in the test run directory
                    c.execute(self.get_run_directory(c))
                    #print('command run normally:', c, file=sys.stderr)

                    # Compare each run file with its reference
                    for run_file, ref_file in run_ref_files:
                        sys.stderr.write('compare files: %s <=> %s\n' % (ref_file, run_file))
                        FileComparison.assertEqual(self, run_file, ref_file)

            except Exception as e:
                print('Test failure:', e, 'while running:', c, file=sys.stderr)
                n_failed += 1
        if n_failed != 0:
            raise RuntimeError('%d / %d tests failed.'
                               % (n_failed, len(self.test_cases)))

    def tearDown(self):
        """
        Remove last run directories for registered TestCommand.
        """
        if self.test_mode == soma.test_utils.run_mode:
            for c in self.test_cases:
                # Remove run directory
                self.remove_run_directory(c)
