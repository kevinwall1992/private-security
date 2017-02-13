#include "Utility.h"

#include <sstream>
#include <fstream>
#include <iterator>
#include <sys/stat.h>


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

bool FileExists(string filepath)
{
	struct stat buffer;
	return (stat (filepath.c_str(), &buffer) == 0);
}

//http://stackoverflow.com/questions/9435385/split-a-string-using-c11
vector<string> SplitString(string string_, char delimiter)
{
  std::stringstream ss(string_);
  string item;
  vector<string> elements;

  while (std::getline(ss, item, delimiter)) 
    elements.push_back(std::move(item));

  return elements;
}

string TextFileToString(string filepath)
{
	string stringed;

	std::ifstream file;
	file.open(filepath);
	string temp;
	while(std::getline(file, temp)) 
		stringed+= temp+ "\n";

	return stringed;
}
