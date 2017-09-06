
project (Nifti)

#
# Create the NIFTI library
#
ADD_LIBRARY(Nifti
NiftiHeader.h
NiftiIO.h

NiftiHeader.cxx
NiftiIO.cxx
)

#
# Find Headers
#
INCLUDE_DIRECTORIES(
${CMAKE_CURRENT_SOURCE_DIR}/../Nifti
${CMAKE_CURRENT_SOURCE_DIR}/../Common
)
