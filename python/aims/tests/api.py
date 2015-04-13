# -*- coding: utf-8 -*-

import unittest
import tempfile
import os
import shutil
import subprocess
import sys
import types

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
          print >> sys.stderr, 'WARNING: image file comparison of empty images (%s, %s)' % (image1, image2)

        # Process differences between pixels
        if not numpy.array_equal(i1, i2) :
          testcase.fail(msg)
          #print >> sys.stderr, 'ERROR: image (%s, %s) content is not the same' % (image1, image2)

        # testunit 1.63 for python 2.6 (installed on MacOS) does not have assertDictEqual method
        if hasattr(testcase, 'assertDictEqual'):
          # Process differences between headers
          testcase.assertDictEqual(h1, h2, msg)
          #print >> sys.stderr, 'ERROR: image (%s, %s) header is not the same' % (image1, image2)
          
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

        if type(run_files) in (types.ListType, types.TupleType):
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

        retcode = subprocess.call(self.__command,
                                  stdout=fout,
                                  stderr=ferr,
                                  cwd=run_directory)
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


class CommandsTestManager(unittest.TestCase):

    """ CommandsTestManager class is a base class to test commands.
        Derived class needs to define the following attributes :
          - ref_directory string path to the base reference directory.
            It is where results of the reference tests are stored.
          - run_directory string path to the base run directory
            It is where results of the current run tests are stored.
          - test_cases an iterable containing CommandTest instances.
            CommandTest are the command line to execute during the test.
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

        return os.path.join(self.ref_directory, testcommand.get_test_name())

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

        return os.path.join(self.run_directory, testcommand.get_test_name())

    def create_ref_directory(self, testcommand):
        """ Creates a reference test directory for a CommandTest.
          If the reference test directory does not exist the
          CommandTest is run to be used as a reference.

        Parameters
        ----------
        testcommand : CommandTest (mandatory)
            command to generate reference files.

        Returns
        -------
        test_directory : str
            path to the reference test directory for the unit test.
        """

        test_ref_directory = os.path.join(
            self.ref_directory, testcommand.get_test_name())
        if not os.path.exists(test_ref_directory):
            os.makedirs(test_ref_directory)

        if not all([os.path.exists(os.path.join(
                test_ref_directory, run_file)) \
                for run_file in testcommand.get_run_files()]):

            # Run the test as reference in the test reference directory
            testcommand.execute(test_ref_directory)

        return test_ref_directory

    def create_run_directory(self, testcommand):
        """ Creates a test run directory for a CommandTest.

        Parameters
        ----------
        testcommand : CommandTest (mandatory)
            command for which to create a run directory.

        Returns
        -------
        test_directory : str
            a path to the test run directory for the unit test.
        """

        test_directory = os.path.join(
            self.run_directory, testcommand.get_test_name())
        if not os.path.exists(test_directory):
            os.makedirs(test_directory)
        return test_directory

    def remove_run_directory(self, testcommand):
        """ Remove a test run directory for a given test.

        Parameters
        ----------
        testcommand : CommandTest (mandatory)
            command for which to delete a run directory.

        Returns
        -------
        test_directory : str
            a path to the test run directory for the unit test.
        """

        test_directory = os.path.join(
            self.run_directory, testcommand.get_test_name())
        if os.path.exists(test_directory):
            shutil.rmtree(test_directory)

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
            return [os.path.join(directory, f) for f in testcommand.get_run_files()]

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

    def setUp(self):
        """ Overriden unittest.TestCase that setup
            reference and run directories for CommandTest
            contained in the test_cases attribute.
        """

        tempdir = tempfile.gettempdir()

        if hasattr(self, 'test_cases'):
            # Create directories for test cases
            for c in self.test_cases:
                # Create reference directory
                self.create_ref_directory(c)

                # Remove run directory and re-create it
                self.remove_run_directory(c)
                self.create_run_directory(c)

        else:
            raise Exception(
                'An attribute named test_cases that contains a list of CommandTest must be added.')

    def runTest(self):
        """
        Run registered TestCommand.
        """

        n_failed = 0
        for c in self.test_cases:
            # Run the test in the test run directory
            try:
                c.execute(self.get_run_directory(c))
                #print >> sys.stdout, 'command run normally:', c

                # Compare each produced run file with its matching reference
                ref_files = self.get_ref_files(c)
                run_files = self.get_run_files(c)

                for ref_file, run_file in zip(ref_files, run_files):
                    sys.stderr.write('compare files: %s <=> %s\n' % (ref_file, run_file))
                    FileComparison.assertEqual(self, run_file, ref_file)
                    
            except Exception, e:
                print 'Test failure:', e, 'while running:', c
                n_failed += 1
        if n_failed != 0:
            raise RuntimeError('%d / %d tests failed.'
                % (n_failed, len(self.test_cases)))

    def tearDown(self):
        """
        Remove last run directories for registered TestCommand.
        """
        for c in self.test_cases:
            # Remove run directory
            self.remove_run_directory(c)
