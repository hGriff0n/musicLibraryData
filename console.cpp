

#include "console.h"

#include "libxl.h"
#include <iostream>
using namespace libxl;


bool dispatch(cmdType& cmds, regIter& reg, counterType& counters, listType& lists, songType& songs) {
	// if the regIter is valid
	if ( reg != regIter() ) {
		// if the inputed command is one of the commands in cmds
		if ( cmds.find(reg->str()) != cmds.end() )
			cmds[reg->str()]->run(++reg, counters, lists, songs, cmds);
		else {
			// else fallback to exec and info
			// they execute code if the command is "accepted"
			cmds["exec"]->run(reg, counters, lists, songs, cmds);
			cmds["info"]->run(reg, counters, lists, songs, cmds);
		}
	}
	return true;
}
std::string loadState(regIter& reg, bool piped) {
	if ( piped )
		return glState.load("list");
	else {
		auto key = reg++->str();
		glState.save("list", key);
		return key;
	}
}


state::state() { }
void state::save(std::string& key,std::string& val) { saves[key] = val;  }
std::string& state::load(std::string& key) { return saves[key]; }


void cmd::add(std::string str, const infoFn& fn) { };

shower::shower(std::ostream& str) : logSpot(str),cull(0),top(0) { }
bool shower::run(CMD_ARGS_DEC) {

	auto _cull = this->cull;
	auto _top = this->top;

	// save / load state
	auto key = loadState(reg, isPiped(reg));


	// act on flags from cmd
	while(reg != regIter()) {
		
		auto flag = reg->str();
		bool set = (flag == "-cF" || flag == "-tF");

		auto i = toint((++reg)++->str());

		if (flag == "-cF" || flag == "-c") {
			if ( set ) this->cull = i;
			_cull = i;
		} else if (flag == "-tF" || flag == "-t") {
			if ( set ) this->top = i;
			_top = i;
		} else {}
		// add in piping support

	}

	// initialize loop variables
	auto i = -1;
	auto end = (_top == 0) ? lists[key].size() : _top;

	// display request info
	while(++i != end) {
		if ( counters[key][lists[key][i]] == _cull ) break;
		logSpot << i + 1 << ": " << lists[key][i] << " with " << counters[key][lists[key][i]] << " songs" << std::endl;
	}

	// round out display with "others"
	if(_cull != 0 && _top == 0)
		logSpot << i + 1 << ": Other (" << _cull << ((_cull != 1) ? "- " : " ")	<< "each) with " << end - i << " songs" << std::endl;

	return true;
}
std::string shower::help() {
	return "\n-- show [type] : -c|F [cull] -t|F [top]\n"
		"--- outputs the results of type to the screen\n"
		"--- '-c' only displays unique results whose count is greater than 'cull' (def: 0)\n"
		"--- '-t' only displays the first 'top' results (def: 0, shows all)\n"
		"--- 'F' sets the default value for the flag to the arg\n";
}

sorter::sorter() : type(0) { }
bool sorter::run(CMD_ARGS_DEC) {
	// save state and determine which vector to sort
	auto str = (reg++)->str();
	glState.save("list",str);
	bool piped = false;

	// act on cmd flags
	char _type = this->type;
	while ( reg != regIter() && !piped ) {
		bool set = (reg->str() == "-sF");

		if ( reg->str() == "-s" || reg->str() == "-sF" ) {
			auto i = toint((++reg)->str());

			if ( set ) this->type = i;
			_type = i;

		} else if ( reg->str() == "|" )
			piped = true;


		++reg;
	}

	// sort the vector
	switch( _type ) {
		case 0:
			// Most first sort
			std::sort(lists[str].begin(),lists[str].end(),[&](const std::string& first,const std::string& second) {
				return counters[str][first] > counters[str][second]; });
			break;
		case 1:
			// Most last sort
			std::sort(lists[str].begin(),lists[str].end(),[&](const std::string& first,const std::string& second) {
				return counters[str][first] < counters[str][second]; });
		default:
			break;
	}

	// pipe execution to another command if requested
	if ( piped )
		dispatch(cmds,reg,counters,lists,songs);

	return true;
}
std::string sorter::help() {
	std::cout << glState.load("list");
	return "\n-- sort [type] : -s 0\n"
		"--- sorts the list of type based on how much is in your library\n"
		"--- '-s' how the sort is performed, 0 = most first 1 = least first (def : 0)\n"
		"--- '-a' alphabetized ?";
}

helper::helper() { }
bool helper::run(CMD_ARGS_DEC) {
	if(reg != regIter())
		if (isTypeKeyword(reg->str()))
			std::cout << typeHelp(reg->str());
		else
			std::cout << cmds[reg->str()]->help();
	else
		std::cout << "\nenter help [cmd] to learn more about cmd\n"
		"cmds: sort,show,log,info,help,total,cls,exit\n\n"
		"enter help [type] to learn more about type\n"
		"types: artist,album,ext\n";
	std::cout << std::endl;
	return true;
}
std::string helper::help() {
	return "\n-- help : [cmd] [type]\n"
		"--- displays information about cmd or type\n";
}

quitter::quitter() { }
bool quitter::run(CMD_ARGS_DEC) {

	exit(0);
}
std::string quitter::help() {
	return "\n-- exits the program\n";
}

logger::logger() { }
bool logger::run(CMD_ARGS_DEC) {
	//Book* book = xlCreateBook();
	return true;
}
std::string logger::help() {
	return "\n-- log [type] -f file :\n"
		"--- stores what would be displayed by show type into file\n"
		"--- status: unimplemented\n";
}

informer::informer(std::ostream& str) : logSpot(str) { }
bool informer::run(CMD_ARGS_DEC) {
	auto str = reg->str();
	if ( infos.find(str) != infos.end() )
		infos[str](logSpot, ++reg, counters, lists, songs);
	return true;
}
std::string informer::help() {
	return "\n--total: total number of songs in library\n"
		"\n--find: infometrics of one artist, album, or extension"
		"\n--- 'p': output as percent"
		"\n--- 'b': output as percent and whole\n"
		"\n--group: infometrics of multiple artists, albums, or extensions"
		"\n--- 't': number of songs from the top x"
		"\n--- 'c': number of songs from items with x or more songs"
		"\n--- 'p': output as percent\n";
}
void informer::add(std::string str, const infoFn& fn) {
	infos[str] = fn;
}

bool executor::run(CMD_ARGS_DEC) {
	auto str = reg->str();
	if ( str == "cls" )
		system(str.c_str());
	return true;
}
std::string executor::help() {
	return "\n-- its complicated\n"
		"--- executes system functions such as:\n"
		"---- cls, {to be filled}"
		"--- status: it's here\n";
}

/*
commander::commander() { }
bool commander::run(CMD_ARGS_DEC) {
	return true;
}
std::string commander::help() {
	return "\ncommander\n";
}
*/

// global state definition
state glState;
