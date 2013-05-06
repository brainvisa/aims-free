#include <aims/getopt/getopt2.h>
#include <aims/sparsematrix/sparseMatrix.h>

using namespace aims;
using namespace carto;
using namespace std;

int main( int argc, const char* argv[] )
{
  try
  {
    std::vector< std::string > sparseMatrixFileNames;
    std::string outputSumSparseMatrixFileName = "";
    std::string matrixFormat = "binar";
    bool verbose = false;

    AimsApplication app( argc, argv, "Sum Sparse Matrices" );
    app.addOptionSeries(sparseMatrixFileNames, "-i", "input sparse matrices filenames");
    app.addOption(outputSumSparseMatrixFileName, "-o", "output sparse matrix: sum of the input matrices");
    app.addOption(matrixFormat, "-fmt", "input sparse format: binary or ascii, default = binar", true);
    app.addOption( verbose, "-verbose", "show as much information as possible", true );
    app.initialize();

    //Reading inputs
    if(verbose) std::cout << "Reading 1st input matrix..." << std::flush;
    if(sparseMatrixFileNames.empty())
    {
      throw std::runtime_error( "Not input matrices...");
    }
    aims::SparseMatrix sumSparseMatrix;
    sumSparseMatrix.read( sparseMatrixFileNames[0], matrixFormat );

    if( verbose ) cout << " done.\n";

    int32_t size1 = sumSparseMatrix.getSize1();
    int32_t size2 = sumSparseMatrix.getSize2();
    uint n = sparseMatrixFileNames.size();
    for (uint mat = 1; mat < n; ++mat)
    {
      aims::SparseMatrix currentSparseMatrix;
      if( verbose )
        cout << "reading matrix " << mat+1 << " / " << n << "..." << flush;
      currentSparseMatrix.read( sparseMatrixFileNames[mat], matrixFormat );
      if( verbose ) cout << " done.\n";
      if( currentSparseMatrix.getSize1()==size1
          and currentSparseMatrix.getSize2() == size2 )
      {
        sumSparseMatrix += currentSparseMatrix;
        if( verbose ) cout << "Added.\n";
      }
      else
      {
        throw std::runtime_error( "matrices of different sizes" );
      }
    }
    sumSparseMatrix.write( outputSumSparseMatrixFileName, matrixFormat );

    return EXIT_SUCCESS;
  }
  catch( carto::user_interruption & )
  {
  // Exceptions thrown by command line parser (already handled, simply exit)
  }
  catch( exception & e )
  {
    cerr << e.what() << endl;
  }

  return EXIT_FAILURE;
}
