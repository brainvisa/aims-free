
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/bucket/bucketgen.h>
#include <aims/vector/vector.h>
#include <cmath>

using namespace aims;
using namespace carto;
using namespace std;

namespace
{
	struct SuperQuadricGen : public BucketMapGenerator::Generator
	{
		virtual BucketMap<void>*
		generator( const carto::GenericObject & ) const;
		virtual Object parameters() const;
	};
	
	map<string, rc_ptr<BucketMapGenerator::Generator> > & generators()
	{
		static map<string, rc_ptr<BucketMapGenerator::Generator> >	functs;
		
		if( functs.empty() )
		{
			functs[ "quadric"        ]
				= rc_ptr<BucketMapGenerator::Generator>( new SuperQuadricGen );
		}
		
		return functs;
	}
	
	BucketMap<void>* SuperQuadricGen::generator( const carto::GenericObject & params ) const
	{
		return BucketMapGenerator::quadric( params );
	}
	
	Object SuperQuadricGen::parameters() const
	{
		Object		dictionary = Object::value( Dictionary() );
		Dictionary	& parameters = dictionary->value<Dictionary>();
		parameters[ "boudingboxorigin" ] = Object::value( string( "3D position of the bounding box origin" ) );
		parameters[ "boudingboxsizes" ] = Object::value( string( "3D sizes of the bounding box edges" ) );
		parameters[ "resolution" ] = Object::value( string( "3D resolution to use for sampling" ) );
		parameters[ "coefficients" ] = Object::value( string( "Coefficients array to use for super quadric object" ) );
		parameters[ "order" ] = Object::value( string( "Order to use for super quadric object" ) );
	
		return dictionary;
	}
}

BucketMap<void>*
BucketMapGenerator::generate( const Object params )
{
	return generate( *params );
}


BucketMap<void>*
BucketMapGenerator::generate( const GenericObject & params )
{
	string	type = params.getProperty( "type" )->value<string>();
	
	const map<string,rc_ptr<Generator> >	& functs = generators();
	
	map<string,rc_ptr<Generator> >::const_iterator
		ifn = functs.find( type );
	if( ifn != functs.end() )
		return ifn->second->generator( params );
	else
	{
		cerr << "BucketMapGenerator::generate: unknown bucket map type " << type << endl;
		throw runtime_error( string( "can't generate bucket map of type " ) + type  );
	}
	return 0;
}

Object BucketMapGenerator::description()
{
	const map<string,rc_ptr<Generator> >	& functs = generators();
	map<string,rc_ptr<Generator> >::const_iterator
		ifn, efn = functs.end();
	Object				desc = Object::value( ObjectVector() );
	ObjectVector				& l = desc->value<ObjectVector>();
	Dictionary::const_iterator	id, ed;
	
	for( ifn=functs.begin(); ifn!=efn; ++ifn )
	{
		Object	o = Object::value( Dictionary() );
		Dictionary	& d = o->value<Dictionary>();
		l.push_back( o );
		d[ "type" ] = Object::value( ifn->first );
	
		Object		dx = ifn->second->parameters();
		Dictionary	& p = dx->value<Dictionary>();
		for( id=p.begin(), ed=p.end(); id!=ed; ++id )
			d[ id->first ] = id->second;
	}
	return desc;
}


BucketMap<void>* BucketMapGenerator::quadric( const GenericObject & params )
{
	ObjectVector originVector, sizesVector, resolutionVector, coefficientsVector;
	ObjectVector::iterator	currentIterator, endIterator;
	Point3df origin, sizes, resolution;
	std::vector<double> * coefficients;
	unsigned order;

	/* Get generic object values */
	order = (unsigned)params.getProperty( "order" )->getScalar();
	coefficientsVector = params.getProperty( "resolution" )->value<ObjectVector>();
	originVector = params.getProperty( "boundingboxorigin" )->value<ObjectVector>();
	sizesVector = params.getProperty( "boundingboxsizes" )->value<ObjectVector>();
	resolutionVector = params.getProperty( "resolution" )->value<ObjectVector>();

	/* Get typed objects */
	origin = Point3df(originVector[0]->getScalar(),
						originVector[1]->getScalar(),
						originVector[2]->getScalar());
	sizes = Point3df(sizesVector[0]->getScalar(),
						sizesVector[1]->getScalar(),
						sizesVector[2]->getScalar());
	resolution = Point3df(resolutionVector[0]->getScalar(),
							resolutionVector[1]->getScalar(),
							resolutionVector[2]->getScalar());

	/* Copy coefficients to vector of double */
	coefficients = new std::vector<double>((unsigned)pow((order + 1.0), 3));

	endIterator = coefficientsVector.end();
	for( currentIterator = coefficientsVector.begin(); currentIterator != endIterator; ++currentIterator )
    {
		coefficients->push_back((*currentIterator)->getScalar());
	}

	return quadric( order, coefficients, origin, sizes, resolution );
}

BucketMap<void>*
BucketMapGenerator::quadric( unsigned /*order*/,
                             const std::vector<double> * /*coefficients*/,
                             const Point3df & /*origin*/, const Point3df & /*sizes*/,
                             const Point3df & /*resolution*/ )
{

	BucketMap<void>* result = 0;


	return( result );
}
