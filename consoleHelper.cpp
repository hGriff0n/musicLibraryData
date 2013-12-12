

#include "consoleHelper.h"


regIter parse(std::string& input, std::regex& parseTree) {
	return regIter(input.cbegin(), input.cend(), parseTree);
}
int toint(std::string& str) {
	return atoi(str.c_str());
}
bool isTypeKeyword(std::string& word) {
	return word == "artist" || word == "album" || word == "ext";
}
std::string typeHelp(std::string& type) {
	char switcher = (type == "artist") ? 1 : (type == "album") ? 2 : 3;
	switch ( switcher ) {
		case 1:
			return "Song Artist (the people that wrote/recorded the song)\n";
			break;
		case 2:
			return "Album (the place that the song is found on)\n";
			break;
		case 3:
			return "Extension (mp3 or m4a)\n";
			break;
		default:
			return "";
	}
}
std::string tostring(int x) {
	return "";
}
int isPiped(regIter& reg) {
	if ( reg != regIter() )
		return (*(reg->str().begin()) == '-');
	return 2;
}
float truncTo(float num, int places) {
	num = (int)(num * pow(10, places));
	return num / pow(10, places);
}
std::string trim(std::string&& str, int places) {
	return str.substr(places, str.size() - 2 * places);
}