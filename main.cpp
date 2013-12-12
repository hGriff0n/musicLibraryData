

#include <filesystem>
#include <algorithm>
#include <iostream>

#include "console.h"

using namespace std;
using namespace std::tr2::sys;


// extracts various song data from the iTunes file directory
string artistFromPath(const path&, regex&);
string albumFromPath(const path&, regex&);
string titleFromPath(const path&, regex&);

// is the file a music file?
bool isMusicExtension(string&);

// setup console commands (see for implementation)
void setUpCommands(map<string,cmd*>&);

// setup music library path (assumed to be iTunes)
void setUpDirectory(path&);

// setup infometric commands and functions
void setupInfo(cmd*);

// grabs the inputed command and formats it to a regIter based on the passed regex
// string& required to insure the regIter is still valid after grabLine returns
regIter grabLine(istream&,string&,regex&);


int main(int argc,const char* argv[],char* envp[]) {
	path base_dir(getenv("USERPROFILE"));
	setUpDirectory(base_dir);

	// regex strings
	regex consoleSplit("(\".*\")|([^ +]+)");	// splits a string by spaces or double quotes (can include spaces)
												// ex. Hello "I'm a" wandering pirate -> [ Hello, "I'm a", wandering, pirate ]
	regex numTrimmer(" *[0-9]+ *");				// removes all numbers from a string
	regex scoreTrimmer("(_+ *)");				// removes all underscores and the end of words in a string

	counterType counters;	// number of songs by type-descrip
	listType lists;			// number of descrip by type
	songType songList;		// list of all songs

	cout << "Gathering music information...\n";

	// gather data from file structure
	for(auto it = recursive_directory_iterator(base_dir); it != recursive_directory_iterator(); ++it) {
		auto temp = it->path();
		if(!is_directory(temp) && isMusicExtension(temp.extension()))
			songList.emplace_back(artistFromPath(temp,scoreTrimmer), // displays odd when _ is on begin/end
								  albumFromPath(temp,scoreTrimmer),
								  titleFromPath(temp,numTrimmer),
								  temp.extension());
	}

	cout << "Storing music information...\n";

	// push data into counters
	for(auto& it : songList)
		for(auto& dat : it.data)
			++(counters[dat.first][dat.second]);

	// push data into lists
	for(auto& it : counters["artist"]) lists["artist"].push_back(it.first);
	for(auto& it : counters["album"]) lists["album"].push_back(it.first);
	for(auto& it : counters["ext"]) lists["ext"].push_back(it.first);

	// set up console program
	map<string,cmd*> cmdList;
	setUpCommands(cmdList);

	// setup parsing objects
	string command;
	regIter reg;

	// tell user that they can now use the program
	cout << "Music information gathered...\n\n"
		"Begin use (enter 'help' for help)\n" << endl;

	// repeats forever until 'exit' is entered
	while ( 1 ) {
		reg = grabLine(cin, command, consoleSplit);

		dispatch(cmdList,reg,counters,lists,songList);

		cout << endl;
	}
	
}

string artistFromPath(const path& p,regex& r) {
	return regex_replace(p.parent_path().parent_path().filename(),r," ");
}
string albumFromPath(const path& p,regex& r) {
	return regex_replace(p.parent_path().filename(),r," ");
}
string titleFromPath(const path& p,regex& r) {
	return regex_replace(p.basename(),r,"");
}
bool isMusicExtension(string& ext) {
	return ext == ".mp3" || ext == ".m4a";
}

void setUpCommands(map<string,cmd*>& cmds) {
	// tie command classes to console keywords (polymorphic)
	cmds["show"] = new shower(cout);
	cmds["sort"] = new sorter();
	cmds["help"] = new helper();
	cmds["exit"] = new quitter();
	cmds["info"] = new informer(cout);
	cmds["log"] = new logger();
	cmds["exec"] = new executor();

	// setup infometric functions in the info command
	setupInfo(cmds["info"]);
}
void setUpDirectory(path& p) {
	p /= "Music";
	p /= "iTunes";
	p /= "iTunes Media";
	p /= "Music";
}
void setupInfo(cmd* info) {
	info->add("total",[] (INFO_ARG_LIST) { out << "You have a total of " << songs.size() << " songs" << endl; });
	info->add("find", [] (INFO_ARG_LIST) { 
		string key = loadState(reg, isPiped(reg)), flg = reg->str(), search = trim((++reg)->str(),1);
		auto idx = find(list[key].begin(), list[key].end(), search);
		auto num = count[key][*idx];
		auto percent = 100 * truncTo(num / (float)(songs.size()), 3);

		out << search;
		
		if ( flg == "-p" )
			out << " is " << percent << "% of your library";
		else if ( flg == "-b" )
			out << " is " << percent << "% of your library with " << count[key][*idx] << " songs";
		else
			out << " has " << num << " songs in your library";

		out << endl;
	});
	info->add("group",[] (INFO_ARG_LIST) {
		string key = loadState(reg, isPiped(reg));
		bool percent = false, cull = false, finished = false;
		int num = 0, total = 0, loc = 0;

		while ( reg != regIter() ) {
			if ( reg->str() == "-p" )
				percent = true;
			else if ( reg->str() == "-t" ) {
				num = toint((++reg)->str()) - 1;
			}
			else if ( reg->str() == "-c" ) {
				cull = true;
				num = toint((++reg)->str()) + 1;
			}
			++reg;
		}

		while (!finished) {
			auto nSongs = count[key][list[key][loc]];

			if ( (cull && nSongs >= num) || loc <= num )
				total += nSongs;
			else
				finished = true;

			++loc;
		}

		if ( cull )
			out << "All " << key << " with over " << num - 1 << " songs";
		else
			out << "The top " << num + 1 << " " << key;

		if ( percent )
			out << " are " << 100 * truncTo(total / (float)(songs.size()), 3) << "% of your library";
		else
			out << " have " << total << " songs in your library";

		out << endl;
	});
}

regIter grabLine(istream& input,string& cmd,regex& parseTree) {
	getline(input, cmd);
	return parse(cmd, parseTree);
}
