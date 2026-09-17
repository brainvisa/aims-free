
#ifndef AIMS_PRIMALSKETCH_FINITEELEMENTSMOOTHER_D_H
#define AIMS_PRIMALSKETCH_FINITEELEMENTSMOOTHER_D_H

#include <cstdlib>
#include <aims/primalsketch/finiteElementSmoother.h>
#include <aims/mesh/curv.h>
#include <cartobase/type/converter.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

namespace aims {

template<int D, typename T> Texture<T> FiniteElementSmoother<D,T>::doSmoothing(const Texture<T> & ima, int maxiter, bool verbose)
{
	Texture<T>	textOut;
	TimeTexture<T> textIn, textTmp1;
	TimeTexture<float> textTmp2;
	Texture<float>	smooth, lapl;
	float			s;
	unsigned		i,n=ima.nItem();
	int 			iter;
	//float threshold=0.1;
	
	carto::Converter<TimeTexture<T> , TimeTexture<float> > conv;
	textIn[0]=ima;

	if (maxiter >= 0)
	{
		conv.convert(textIn, textTmp2);
		smooth=textTmp2[0];
		if (verbose) std::cout << "Starting smoothing in " << maxiter << " iterations" << std::endl;
		for (iter=0; iter< maxiter; ++iter)
		{
			lapl =  AimsMeshLaplacian(smooth, weightLapl);
			//lapl =  AimsMeshLaplacian(smooth, weightLapl, threshold);
			//lapl =  AimsMeshLaplacian(smooth, weightLapl, FLT_MAX);
			for ( i=0; i<n; ++i)
			{
				s = smooth.item(i) + _dt * lapl.item(i);
				smooth.item(i) = s;
			}
		}
		if (verbose) std::cout  << std::endl;

		textTmp2[0]=smooth;

		carto::Converter< TimeTexture<float> , TimeTexture<T> > conv2;
		conv2.convert( textTmp2, textTmp1);

		textOut=textTmp1[0];
		return textOut;
	}
	else
	{
		std::cerr << "diffusionConvolution Smoother: must have tIn < tOut" << std::endl;
		exit(EXIT_FAILURE);
	}
}


}

#endif
