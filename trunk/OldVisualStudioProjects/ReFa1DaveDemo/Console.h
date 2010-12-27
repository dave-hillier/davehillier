// Console.h: interface for the Console class.
//
//////////////////////////////////////////////////////////////////////

#define CON_BUFFER_SIZE 200
#define CON_LINE_WIDTH 256

class Console  
{
public:
	Console();
	virtual ~Console();

	void Render ();
	void Printf (char *format, ...);
	void Clear ();
	void NewLine();

	void Show () { visible = true; };
	void Hide () { visible = false; };
	void Toggle () { visible = !visible; };

	bool visible;

private:
	void AddLine (char *str);

	// circular buffer (FIFO)
	char buffer[CON_BUFFER_SIZE][CON_LINE_WIDTH];// clear on init?
	int head, tail;

	int back_scroll; // view history if its bigger than screen
	int screen_lines;

};
extern Console *con;