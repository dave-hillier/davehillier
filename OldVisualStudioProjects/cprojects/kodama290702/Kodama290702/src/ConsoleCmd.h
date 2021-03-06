#define MAX_CMDS 512 // including variables!
#define MAX_ARGS 32

class ConsoleCmd {
public:
	char *name;
	void (*func)(int argv, char *argc[]);
	int key;
};

class ConsoleCmds {
public:
	ConsoleCmds();
	virtual ~ConsoleCmds () {};

	void Add (char *name, void (*func)(int argc, char *argv[]));
	void Execute (char *cmd);
	void Delete (char *cmd) {};

	// int AutoComplete(char *cmd); //returns number possiblities
	// void ListPossible(char *cmd);

private:
	ConsoleCmd cmds[MAX_CMDS];

	int used, size;
};

extern ConsoleCmds *command;