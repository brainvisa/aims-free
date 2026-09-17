
// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/topology/topoBase.h>
#include <aims/topology/cc6adjacent.h>
#include <aims/topology/cc26adjacent.h>
#include <aims/topology/connectivity6.h>


TopologyBase::TopologyBase() : _cstar( 0 ), _cbar( 0 )
{
  CC26Adjacent adj26;
  _comp26 = new Components26Neighborhood( connex26, adj26 );

  Connectivity6 connex6;
  CC6Adjacent adj6;
  _comp18 = new Components18Neighborhood( connex6, adj6 );
}


TopologyBase::~TopologyBase()
{
  delete _comp26;
  delete _comp18;
}


void TopologyBase::flipX()
{
  int *xptr = myX + 1;
  for ( int i=26; i--; xptr++ )  *xptr = !(*xptr);
}


void TopologyBase::computeLocalCCNumbers()
{
  _cstar = _comp26->getScalar( myX );
  flipX();
  _cbar = _comp18->getScalar( myX );
}
