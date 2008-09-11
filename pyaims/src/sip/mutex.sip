
namespace carto
{

class Mutex
{
%TypeHeaderCode
#include <cartobase/thread/mutex.h>
%End

public:
	Mutex(int);
	virtual ~Mutex();

	void 	lock();
	void 	unlock();
};

class MutexedObject : carto::Mutex
{
%TypeHeaderCode
#include <cartobase/smart/mutexrcptr.h>
%End

public:
	MutexedObject();
 	MutexedObject(const carto::MutexedObject &);
	virtual 	~MutexedObject();

	carto::Mutex	*mutex() const;
};
};
