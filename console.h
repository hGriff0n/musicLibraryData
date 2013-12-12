

#pragma once

#include "consoleHelper.h"
//#pragma warning(disable:4305)		// Can't disable

/*:::::::*\
  typedefs
\*:::::::*/

// ease-of-use (saves typing and editing)
#define CMD_ARGS_DEF regIter&, counterType&, listType&, songType&, cmdType&
#define CMD_ARGS_DEC regIter& reg, counterType& counters, listType& lists, songType& songs, cmdType& cmds

/*:::::::*\
 functions
\*:::::::*/

// calls the inputed command with all necessary args
// improves capability for piping commands (chaining)
bool dispatch(cmdType&, regIter&, counterType&, listType&, songType&);

// returns the type keyword from global state if command was piped, from regIter if not
std::string loadState(regIter&, bool);

// global state class
class state {
	private:
		std::map<std::string, std::string> saves;
	public:
		state();

		void save(std::string&, std::string&);
		template <size_t N, size_t T>
		void save(const char(&key)[N], const char(&val)[T]) {
			return this->save(std::string(key), std::string(val));
		}
		template <size_t N>
		void save(const char(&key)[N],std::string& val) {
			return this->save(std::string(key), val);
		}
		std::string& load(std::string&);
		template <size_t N>
		std::string& load(const char(&key)[N]) {
			return load(std::string(key));
		}
};


// polymorphic base class for commands

// each child class implements a specific functionality
// (partially described by its name) through it's inherited run method
// it also provides a help method for use by the helper command in output helpful information

struct cmd {
	virtual bool run(CMD_ARGS_DEF) = 0; // runs the command
	virtual std::string help() = 0;		// provides command specific help data

	virtual void add(std::string, const infoFn&);	// adds a function to informer (shell implementation on others)
};

// outputs type data to screen
class shower : public cmd {
	private:
		std::ostream& logSpot;
		int cull,top;

	public:
		shower(std::ostream&);

		bool run(CMD_ARGS_DEF);
		std::string help();
};

// sorts type data
class sorter : public cmd {
	private:
		char type;
	public:
		sorter();

		bool run(CMD_ARGS_DEF);
		std::string help();
};

// outputs help data to screen
class helper : public cmd {
	private:
	public:
		helper();

		bool run(CMD_ARGS_DEF);
		std::string help();
};

// exits program
class quitter : public cmd {
	private:
	public:
		quitter();

		bool run(CMD_ARGS_DEF);
		std::string help();
};

// logs type data to an excel file (unimplemented)
class logger : public cmd {
	private:
	public:
		logger();

		bool run(CMD_ARGS_DEF);
		std::string help();
};

// provides infometric analyisis songData
class informer : public cmd {
	private:
		std::ostream& logSpot;
		funcType infos;

	public:
		informer(std::ostream&);

		bool run(CMD_ARGS_DEF);
		std::string help();

		void add(std::string, const infoFn&);
};

// executes various system commands (only implemented for 'cls')
class executor : public cmd {
	private:
	public:
		executor() { };

		bool run(CMD_ARGS_DEF);
		std::string help();
};

/*
// create new cmds
class commander : public cmd {
	private:
	public:
		commander();

		bool run(CMD_ARGS_DEF);
		std::string help();
};
*/

// global state declaration
extern state glState;
