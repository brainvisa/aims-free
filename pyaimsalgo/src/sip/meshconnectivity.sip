%ModuleHeaderCode
	#include <aims/connectivity/meshcc.h>
%End


TimeTexture_FLOAT *AimsMeshLabelConnectedComponent2(
  AimsSurfaceTriangle& mesh,
  const TimeTexture_FLOAT &inittex, float threshold, int mode=1)
  /PyName=AimsMeshLabelConnectedComponent, Factory, ReleaseGIL/;
%MethodCode
  sipRes = new TimeTexture_FLOAT;
  (*sipRes)[0] = AimsMeshLabelConnectedComponent(a0->begin()->second,
                                                 a1->begin()->second, a2, a3);
%End

TimeTexture_S16 *AimsMeshLabelConnectedComponent2(
  AimsSurfaceTriangle& mesh,
  const TimeTexture_S16 &inittex, short threshold, int mode=1)
  /PyName=AimsMeshLabelConnectedComponent, Factory, ReleaseGIL/;
%MethodCode
  sipRes = new TimeTexture_S16;
  (*sipRes)[0] = AimsMeshLabelConnectedComponent(a0->begin()->second,
                                                 a1->begin()->second, a2, a3);
%End


TimeTexture_FLOAT *AimsMeshLabelConnectedComponent2Texture(
  AimsSurfaceTriangle& mesh,
  const TimeTexture_FLOAT &inittex, float threshold )
  /PyName=AimsMeshLabelConnectedComponent2Texture, Factory, ReleaseGIL/;
%MethodCode
  sipRes = new TimeTexture_FLOAT;
  (*sipRes)[0] = AimsMeshLabelConnectedComponent2Texture(
    a0->begin()->second, a1->begin()->second, a2 );
%End

TimeTexture_S16 *AimsMeshLabelConnectedComponent2Texture(
  AimsSurfaceTriangle& mesh,
  const TimeTexture_S16 &inittex, short threshold )
  /PyName=AimsMeshLabelConnectedComponent2Texture, Factory, ReleaseGIL/;
%MethodCode
  sipRes = new TimeTexture_S16;
  (*sipRes)[0] = AimsMeshLabelConnectedComponent2Texture(
    a0->begin()->second, a1->begin()->second, a2 );
%End

TimeTexture_S16 AimsMeshFilterConnectedComponent2(
  const AimsSurfaceTriangle & mesh, const TimeTexture_S16 & inittex,
  int16_t label, const int16_t & background = 0, unsigned long ncomp = 1,
  unsigned long min_npts = 0, float min_surf = 0. )
  /PyName=AimsMeshFilterConnectedComponent, Factory, ReleaseGIL/;
%Docstring
out_tex = AimsMeshFilterConnectedComponent(mesh, inittex, label, background=0,
                                           ncomp=1, min_npts=0, min_surf=0.)

Split label "label" into connected components, then filter smaller ones
out.
Original values are left unchanged, except for filtered out regions which
will be given the label "background" (0 by default).

Filtering can keep the "ncomp" largest components, and suppress regions
with fewer than "min_npts" vertices, and regions under the surface area
"min_surf". If any of these criterions is 0, then filtering for this
criterion doesn't happen.
%End
%MethodCode
  sipRes = new TimeTexture_S16;
  (*sipRes)[0] = AimsMeshFilterConnectedComponent(
    a0->begin()->second, a1->begin()->second, a2, a3, a4, a5, a6 );
%End

TimeTexture_S32 AimsMeshFilterConnectedComponent2(
  const AimsSurfaceTriangle & mesh, const TimeTexture_S32 & inittex,
  int32_t label, const int32_t & background = 0, unsigned long ncomp = 1,
  unsigned long min_npts = 0, float min_surf = 0. )
  /PyName=AimsMeshFilterConnectedComponent, Factory, ReleaseGIL/;
%Docstring
out_tex = AimsMeshFilterConnectedComponent(mesh, inittex, label, background=0,
                                           ncomp=1, min_npts=0, min_surf=0.)

Split label "label" into connected components, then filter smaller ones
out.
Original values are left unchanged, except for filtered out regions which
will be given the label "background" (0 by default).

Filtering can keep the "ncomp" largest components, and suppress regions
with fewer than "min_npts" vertices, and regions under the surface area
"min_surf". If any of these criterions is 0, then filtering for this
criterion doesn't happen.
%End
%MethodCode
  sipRes = new TimeTexture_S32;
  (*sipRes)[0] = AimsMeshFilterConnectedComponent(
    a0->begin()->second, a1->begin()->second, a2, a3, a4, a5, a6 );
%End

TimeTexture_FLOAT AimsMeshFilterConnectedComponent2(
  const AimsSurfaceTriangle & mesh, const TimeTexture_FLOAT & inittex,
  float label, const float & background = 0, unsigned long ncomp = 1,
  unsigned long min_npts = 0, float min_surf = 0. )
  /PyName=AimsMeshFilterConnectedComponent, Factory, ReleaseGIL/;
%Docstring
out_tex = AimsMeshFilterConnectedComponent(mesh, inittex, label, background=0,
                                           ncomp=1, min_npts=0, min_surf=0.)

Split label "label" into connected components, then filter smaller ones
out.
Original values are left unchanged, except for filtered out regions which
will be given the label "background" (0 by default).

Filtering can keep the "ncomp" largest components, and suppress regions
with fewer than "min_npts" vertices, and regions under the surface area
"min_surf". If any of these criterions is 0, then filtering for this
criterion doesn't happen.
%End
%MethodCode
  sipRes = new TimeTexture_FLOAT;
  (*sipRes)[0] = AimsMeshFilterConnectedComponent(
    a0->begin()->second, a1->begin()->second, a2, a3, a4, a5, a6 );
%End


