from soma import aims

from anatomist.wip.parametrics.shapes import Cone, Cube, Cylinder, Sphere
from soma.wip.aimsalgo.moment         import Moment

object = Cone( aims.Point3df( -4, 5, 10 ), aims.Point3df( 4, 3, -2 ), 5 )
moment = Moment()
moment.process( object.surface() )
