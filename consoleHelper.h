

#pragma once

#pragma warning(disable:4996)

#include <vector>
#include <functional>
#include <string>
#include <regex>
#include <map>

// simplifies regex declarations
using namespace std::tr1;

// forward declaration of cmd struct
struct cmd;

// holds song information
struct song {
	std::string title;
	std::map<std::string, std::string> data;

	song(std::string& art, std::string& alb, std::string& til, std::string& typ) : title(til) {
		data["artist"] = art;
		data["album"] = alb;
		data["ext"] = typ;
	}
};

/*:::::::*\
  typedefs
\*:::::::*/

// for ease-of-use/comprehension
typedef std::map<std::string, std::map<std::string, int>> counterType;
typedef std::map<std::string, std::vector<std::string>> listType;
typedef std::map<std::string, cmd*> cmdType;
typedef std::vector<song> songType;
typedef sregex_iterator regIter;

typedef std::function<void(std::ostream&, regIter&, counterType&, listType&, songType&)> infoFn;
typedef std::map<std::string, infoFn> funcType;

// saves typing and editing
#define INFO_ARG_LIST std::ostream& out, regIter& reg, counterType& count, listType& list, songType& songs


/*:::::::*\
 functions
\*:::::::*/

// returns a regex
regIter parse(std::string&, std::regex&);

// is the passed string a type keyword (artist,album,ext)
bool isTypeKeyword(std::string&);

// returns help data for the type keywords
std::string typeHelp(std::string&);

// converts the passed string to an int (basically atoi)
int toint(std::string&);

// converts the passed int to a string (unimplemented, possibly unnecessary)
std::string tostring(int);

// truncates the float to x decimal places
float truncTo(float, int);

// is the command piped
// (sort artist | show)	- show is piped
int isPiped(regIter&);

// trims x characters off of the edges of the string
std::string trim(std::string&&, int);