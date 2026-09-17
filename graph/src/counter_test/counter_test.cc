#include <cartobase/stream/fistreambuf.h>
#include <cartobase/stream/counter.h>
#include <fstream>
#include <iostream>

using namespace std;
using namespace carto;

int main()
{
	istream source(cin.rdbuf());
	fistreambuf<CounterExtractor> counter(cin.rdbuf());
	source.rdbuf(&counter);

	bool newline = true;

	while (1)
	{
		source.get();
		source.unget();
		int c = source.get();
		if (c == EOF)
			break;
		if (newline)
		{
			cout << counter.extractor().line() << ' ';
			newline = false;
		}
		cout << static_cast<char>(c);
		if (c == '\n')
		{
			newline = true;
		}
	}
}
