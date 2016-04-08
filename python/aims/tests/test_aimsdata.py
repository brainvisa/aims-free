# -*- coding: utf-8 -*-
import tempfile
import os
from aims.tests.api import CommandTest, CommandsTestManager
    
"""
Class to test structuring elements
"""
class AimsStructuringElementTests(CommandsTestManager):
  @staticmethod
  def get_test_dir():
    tests_dir = os.getenv("BRAINVISA_TESTS_DIR")
    if not tests_dir:
        tests_dir = tempfile.gettempdir()
    if not os.path.exists(tests_dir):
        os.makedirs(tests_dir)
    return tests_dir

  ref_directory = os.path.join(AimsStructuringElementTests.get_test_dir(),
                               'tmp_tests_aimsdata', 'ref')
  run_directory = os.path.join(AimsStructuringElementTests.get_test_dir(),
                               'tmp_tests_aimsdata', 'run')
  
  test_cases = (

             # Test connectivity 4xy amplitude 1
             CommandTest( 'AimsStructuringElement_4xy_a1',
                    command = [ 'AimsStructuringElement_test',
                                  '-o', 'elem_4xy_a1.ima',
                                  '-c', '4xy',
                                  '-a', '1' ],
                    run_files = [ 'elem_4xy_a1.ima' ] ),

             # Test connectivity 4xz amplitude 1
             CommandTest( 'AimsStructuringElement_4xz_a1',
                    command = [ 'AimsStructuringElement_test',
                                  '-o', 'elem_4xz_a1.ima',
                                  '-c', '4xz',
                                  '-a', '1' ],
                    run_files = [ 'elem_4xz_a1.ima' ] ),

             # Test connectivity 4yz amplitude 1
             CommandTest( 'AimsStructuringElement_4yz_a1',
                    command = [ 'AimsStructuringElement_test',
                                  '-o', 'elem_4yz_a1.ima',
                                  '-c', '4yz',
                                  '-a', '1' ],
                    run_files = [ 'elem_4yz_a1.ima' ] ),

             # Test connectivity 4xydiag amplitude 1
             CommandTest( 'AimsStructuringElement_4xydiag_a1',
                    command = [ 'AimsStructuringElement_test',
                                  '-o', 'elem_4xydiag_a1.ima',
                                  '-c', '4xydiag',
                                  '-a', '1' ],
                    run_files = [ 'elem_4xydiag_a1.ima' ] ),

             # Test connectivity 4xzdiag amplitude 1
             CommandTest( 'AimsStructuringElement_4xzdiag_a1',
                    command = [ 'AimsStructuringElement_test',
                                  '-o', 'elem_4xzdiag_a1.ima',
                                  '-c', '4xzdiag',
                                  '-a', '1' ],
                    run_files = [ 'elem_4xzdiag_a1.ima' ] ),

             # Test connectivity 4yzdiag amplitude 1
             CommandTest( 'AimsStructuringElement_4yzdiag_a1',
                    command = [ 'AimsStructuringElement_test',
                                  '-o', 'elem_4yzdiag_a1.ima',
                                  '-c', '4yzdiag',
                                  '-a', '1' ],
                    run_files = [ 'elem_4yzdiag_a1.ima' ] ),
                    
             # Test connectivity 6 amplitude 1
             CommandTest( 'AimsStructuringElement_6_a1',
                    command = [ 'AimsStructuringElement_test',
                                  '-o', 'elem_6_a1.ima',
                                  '-c', '6',
                                  '-a', '1' ],
                    run_files = [ 'elem_6_a1.ima' ] ),

             # Test connectivity 8xy amplitude 1
             CommandTest( 'AimsStructuringElement_8xy_a1',
                    command = [ 'AimsStructuringElement_test',
                                  '-o', 'elem_8xy_a1.ima',
                                  '-c', '8xy',
                                  '-a', '1' ],
                    run_files = [ 'elem_8xy_a1.ima' ] ),

             # Test connectivity 8xz amplitude 1
             CommandTest( 'AimsStructuringElement_8xz_a1',
                    command = [ 'AimsStructuringElement_test',
                                  '-o', 'elem_8xz_a1.ima',
                                  '-c', '8xz',
                                  '-a', '1' ],
                    run_files = [ 'elem_8xz_a1.ima' ] ),

             # Test connectivity 8yz amplitude 1
             CommandTest( 'AimsStructuringElement_8yz_a1',
                    command = [ 'AimsStructuringElement_test',
                                  '-o', 'elem_8yz_a1.ima',
                                  '-c', '8yz',
                                  '-a', '1' ],
                    run_files = [ 'elem_8yz_a1.ima' ] ),

             # Test connectivity 8xyz amplitude 1
             CommandTest( 'AimsStructuringElement_8xyz_a1',
                    command = [ 'AimsStructuringElement_test',
                                  '-o', 'elem_8xyz_a1.ima',
                                  '-c', '8xyz',
                                  '-a', '1' ],
                    run_files = [ 'elem_8xyz_a1.ima' ] ),
                    
             # Test connectivity 18 amplitude 1
             CommandTest( 'AimsStructuringElement_18_a1',
                    command = [ 'AimsStructuringElement_test',
                                  '-o', 'elem_18_a1.ima',
                                  '-c', '18',
                                  '-a', '1' ],
                    run_files = [ 'elem_18_a1.ima' ] ),

             # Test connectivity 26 amplitude 1
             CommandTest( 'AimsStructuringElement_26_a1',
                    command = [ 'AimsStructuringElement_test',
                                  '-o', 'elem_26_a1.ima',
                                  '-c', '26',
                                  '-a', '1' ],
                    run_files = [ 'elem_26_a1.ima' ] ),

             # Test connectivity 9xyz- amplitude 1
             CommandTest( 'AimsStructuringElement_9xyz-_a1',
                    command = [ 'AimsStructuringElement_test',
                                  '-o', 'elem_9xyz-_a1.ima',
                                  '-c', '9xyz-',
                                  '-a', '1' ],
                    run_files = [ 'elem_9xyz-_a1.ima' ] ),

             # Test connectivity 9xyz+ amplitude 1
             CommandTest( 'AimsStructuringElement_9xyz+_a1',
                    command = [ 'AimsStructuringElement_test',
                                  '-o', 'elem_9xyz+_a1.ima',
                                  '-c', '9xyz+',
                                  '-a', '1' ],
                    run_files = [ 'elem_9xyz+_a1.ima' ] ),

             # Test connectivity 9xzy- amplitude 1
             CommandTest( 'AimsStructuringElement_9xzy-_a1',
                    command = [ 'AimsStructuringElement_test',
                                  '-o', 'elem_9xzy-_a1.ima',
                                  '-c', '9xzy-',
                                  '-a', '1' ],
                    run_files = [ 'elem_9xzy-_a1.ima' ] ),

             # Test connectivity 9xzy+ amplitude 1
             CommandTest( 'AimsStructuringElement_9xzy+_a1',
                    command = [ 'AimsStructuringElement_test',
                                  '-o', 'elem_9xzy+_a1.ima',
                                  '-c', '9xzy+',
                                  '-a', '1' ],
                    run_files = [ 'elem_9xzy+_a1.ima' ] ),

             # Test connectivity 9yzx- amplitude 1
             CommandTest( 'AimsStructuringElement_9yzx-_a1',
                    command = [ 'AimsStructuringElement_test',
                                  '-o', 'elem_9yzx-_a1.ima',
                                  '-c', '9yzx-',
                                  '-a', '1' ],
                    run_files = [ 'elem_9yzx-_a1.ima' ] ),

             # Test connectivity 9yzx+ amplitude 1
             CommandTest( 'AimsStructuringElement_9yzx+_a1',
                    command = [ 'AimsStructuringElement_test',
                                  '-o', 'elem_9yzx+_a1.ima',
                                  '-c', '9yzx+',
                                  '-a', '1' ],
                    run_files = [ 'elem_9yzx+_a1.ima' ] ),

             # Test connectivity 5x-y amplitude 1
             CommandTest( 'AimsStructuringElement_5x-y_a1',
                    command = [ 'AimsStructuringElement_test',
                                  '-o', 'elem_5x-y_a1.ima',
                                  '-c', '5x-y',
                                  '-a', '1' ],
                    run_files = [ 'elem_5x-y_a1.ima' ] ),

             # Test connectivity 5x+y amplitude 1
             CommandTest( 'AimsStructuringElement_5x+y_a1',
                    command = [ 'AimsStructuringElement_test',
                                  '-o', 'elem_5x+y_a1.ima',
                                  '-c', '5x+y',
                                  '-a', '1' ],
                    run_files = [ 'elem_5x+y_a1.ima' ] ),

             # Test connectivity 5xy- amplitude 1
             CommandTest( 'AimsStructuringElement_5xy-_a1',
                    command = [ 'AimsStructuringElement_test',
                                  '-o', 'elem_5xy-_a1.ima',
                                  '-c', '5xy-',
                                  '-a', '1' ],
                    run_files = [ 'elem_5xy-_a1.ima' ] ),

             # Test connectivity 5xy+ amplitude 1
             CommandTest( 'AimsStructuringElement_5xy+_a1',
                    command = [ 'AimsStructuringElement_test',
                                  '-o', 'elem_5xy+_a1.ima',
                                  '-c', '5xy+',
                                  '-a', '1' ],
                    run_files = [ 'elem_5xy+_a1.ima' ] ),

             # Test connectivity 5x-z amplitude 1
             CommandTest( 'AimsStructuringElement_5x-z_a1',
                    command = [ 'AimsStructuringElement_test',
                                  '-o', 'elem_5x-z_a1.ima',
                                  '-c', '5x-z',
                                  '-a', '1' ],
                    run_files = [ 'elem_5x-z_a1.ima' ] ),

             # Test connectivity 5x+z amplitude 1
             CommandTest( 'AimsStructuringElement_5x+z_a1',
                    command = [ 'AimsStructuringElement_test',
                                  '-o', 'elem_5x+z_a1.ima',
                                  '-c', '5x+z',
                                  '-a', '1' ],
                    run_files = [ 'elem_5x+z_a1.ima' ] ),

             # Test connectivity 5xz- amplitude 1
             CommandTest( 'AimsStructuringElement_5xz-_a1',
                    command = [ 'AimsStructuringElement_test',
                                  '-o', 'elem_5xz-_a1.ima',
                                  '-c', '5xz-',
                                  '-a', '1' ],
                    run_files = [ 'elem_5xz-_a1.ima' ] ),

             # Test connectivity 5xz+ amplitude 1
             CommandTest( 'AimsStructuringElement_5xz+_a1',
                    command = [ 'AimsStructuringElement_test',
                                  '-o', 'elem_5xz+_a1.ima',
                                  '-c', '5xz+',
                                  '-a', '1' ],
                    run_files = [ 'elem_5xz+_a1.ima' ] ),

             # Test connectivity 5y-z amplitude 1
             CommandTest( 'AimsStructuringElement_5y-z_a1',
                    command = [ 'AimsStructuringElement_test',
                                  '-o', 'elem_5y-z_a1.ima',
                                  '-c', '5y-z',
                                  '-a', '1' ],
                    run_files = [ 'elem_5y-z_a1.ima' ] ),

             # Test connectivity 5y+z amplitude 1
             CommandTest( 'AimsStructuringElement_5y+z_a1',
                    command = [ 'AimsStructuringElement_test',
                                  '-o', 'elem_5y+z_a1.ima',
                                  '-c', '5y+z',
                                  '-a', '1' ],
                    run_files = [ 'elem_5y+z_a1.ima' ] ),

             # Test connectivity 5yz- amplitude 1
             CommandTest( 'AimsStructuringElement_5yz-_a1',
                    command = [ 'AimsStructuringElement_test',
                                  '-o', 'elem_5yz-_a1.ima',
                                  '-c', '5yz-',
                                  '-a', '1' ],
                    run_files = [ 'elem_5yz-_a1.ima' ] ),

             # Test connectivity 5yz+ amplitude 1
             CommandTest( 'AimsStructuringElement_5yz+_a1',
                    command = [ 'AimsStructuringElement_test',
                                  '-o', 'elem_5yz+_a1.ima',
                                  '-c', '5yz+',
                                  '-a', '1' ],
                    run_files = [ 'elem_5yz+_a1.ima' ] ),
             # Test connectivity sphere amplitude 1
             CommandTest( 'AimsStructuringElement_sphere_a1',
                    command = [ 'AimsStructuringElement_test',
                                  '-o', 'elem_sphere_a1.ima',
                                  '-c', 'sphere',
                                  '-a', '1' ],
                    run_files = [ 'elem_sphere_a1.ima' ] ),

             # Test connectivity sphere amplitude 5
             CommandTest( 'AimsStructuringElement_sphere_a5',
                    command = [ 'AimsStructuringElement_test',
                                  '-o', 'elem_sphere_a5.ima',
                                  '-c', 'sphere',
                                  '-a', '5', '-a', '5', '-a', '5' ],
                    run_files = [ 'elem_sphere_a5.ima' ] ),

             # Test connectivity cube amplitude 5
             CommandTest( 'AimsStructuringElement_cube_a5',
                    command = [ 'AimsStructuringElement_test',
                                  '-o', 'elem_cube_a5.ima',
                                  '-c', 'cube',
                                  '-a', '5', '-a', '5', '-a', '5' ],
                    run_files = [ 'elem_cube_a5.ima' ] ),

             # Test connectivity diskxy amplitude 5
             CommandTest( 'AimsStructuringElement_diskxy_a5',
                    command = [ 'AimsStructuringElement_test',
                                  '-o', 'elem_diskxy_a5.ima',
                                  '-c', 'diskxy',
                                  '-a', '5', '-a', '5', '-a', '5' ],
                    run_files = [ 'elem_diskxy_a5.ima' ] ),

             # Test connectivity diskxz amplitude 5
             CommandTest( 'AimsStructuringElement_diskxz_a5',
                    command = [ 'AimsStructuringElement_test',
                                  '-o', 'elem_diskxz_a5.ima',
                                  '-c', 'diskxz',
                                  '-a', '5', '-a', '5', '-a', '5' ],
                    run_files = [ 'elem_diskxz_a5.ima' ] ),

             # Test connectivity diskyz amplitude 5
             CommandTest( 'AimsStructuringElement_diskyz_a5',
                    command = [ 'AimsStructuringElement_test',
                                  '-o', 'elem_diskyz_a5.ima',
                                  '-c', 'diskyz',
                                  '-a', '5', '-a', '5', '-a', '5' ],
                    run_files = [ 'elem_diskyz_a5.ima' ] ),

             # Test connectivity squarexy amplitude 5
             CommandTest( 'AimsStructuringElement_squarexy_a5',
                    command = [ 'AimsStructuringElement_test',
                                  '-o', 'elem_squarexy_a5.ima',
                                  '-c', 'squarexy',
                                  '-a', '5', '-a', '5', '-a', '5' ],
                    run_files = [ 'elem_squarexy_a5.ima' ] ),

             # Test connectivity squarexz amplitude 5
             CommandTest( 'AimsStructuringElement_squarexz_a5',
                    command = [ 'AimsStructuringElement_test',
                                  '-o', 'elem_squarexz_a5.ima',
                                  '-c', 'squarexz',
                                  '-a', '5', '-a', '5', '-a', '5' ],
                    run_files = [ 'elem_squarexz_a5.ima' ] ),

             # Test connectivity squareyz amplitude 5
             CommandTest( 'AimsStructuringElement_squareyz_a5',
                    command = [ 'AimsStructuringElement_test',
                                  '-o', 'elem_squareyz_a5.ima',
                                  '-c', 'squareyz',
                                  '-a', '5', '-a', '5', '-a', '5' ],
                    run_files = [ 'elem_squareyz_a5.ima' ] ),

  )

