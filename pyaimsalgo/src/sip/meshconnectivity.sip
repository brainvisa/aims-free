%ModuleHeaderCode
	#include <aims/connectivity/meshcc.h>
%End


TimeTexture_FLOAT *AimsMeshLabelConnectedComponent2(AimsSurfaceTriangle& mesh,
	const TimeTexture_FLOAT &inittex, float threshold, int mode=1) /PyName=AimsMeshLabelConnectedComponent, ReleaseGIL/;
%MethodCode
  sipRes = new TimeTexture_FLOAT;
  (*sipRes)[0] = AimsMeshLabelConnectedComponent(a0->begin()->second,
					a1->begin()->second, a2, a3);
%End

TimeTexture_S16 *AimsMeshLabelConnectedComponent2(AimsSurfaceTriangle& mesh,
	const TimeTexture_S16 &inittex, short threshold, int mode=1) /PyName=AimsMeshLabelConnectedComponent, ReleaseGIL/;
%MethodCode
  sipRes = new TimeTexture_S16;
  (*sipRes)[0] = AimsMeshLabelConnectedComponent(a0->begin()->second,
					a1->begin()->second, a2, a3);
%End