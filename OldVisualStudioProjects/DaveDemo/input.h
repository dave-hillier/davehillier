// TODO correct codes

// using some un-used chars
#define VK_MWHEELUP		233
#define VK_MWHEELDOWN	234
#define VK_UNKNOWN		235
#define NUM_KEYS			0xFF

typedef struct {
	int		code;
	char	ident[20];
} keycode_t;
extern keycode_t keynames[];
class Input  
{
public:
	Input();
	virtual ~Input();
	void Down (int ch);
	void Up (int ch);
	inline int State(int ch) { return keys[ch]; };
	void Update();

	float mouse_x, mouse_y;

private:
	void MouseUpdate();
	bool keys[NUM_KEYS];
	char keyshiftlookup[NUM_KEYS];
	char keylookup[NUM_KEYS];
};

extern Input input;