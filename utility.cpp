#include "Utility.h"

#include <sstream>


//http://stackoverflow.com/questions/236129/splitting-a-string-in-c
vector<string> TokenizeOverSpaces(string string_)
{
	std::istringstream input_string_stream(string_);

	vector<string> tokens;
	copy(std::istream_iterator<string>(input_string_stream),
			std::istream_iterator<string>(),
			std::back_inserter<vector<string> >(tokens));

	return tokens;
}

//http://stackoverflow.com/questions/101439/the-most-efficient-way-to-implement-an-integer-based-power-function-powint-int
int IntegerPow(int base, int exp)
{
    int result = 1;
    while (exp)
    {
        if (exp & 1)
            result *= base;
        exp >>= 1;
        base *= base;
    }

    return result;
}