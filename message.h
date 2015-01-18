/*
**  Premise headers:
**      types.h
*/

#define MOUSE_MESSAGE 0
#define KEYBOARD_MESSAGE 1
#define FOCUS_MESSAGE 2
//WARNING:
// 		garantee that the first and second elements are fixed!
typedef struct mouse_message
{
	int msg_type;
	uint dom_id;
	uint x;
	uint y;
	uint mouse_event_type;
} MouseMsg;

//A possible form of keyboard message, maybe changed.
//Other struct of message should have a msg_type as the first member
typedef struct kdb_messge
{
	int msg_type;
	uint dom_id;
	char key_value;
} KBDMsg;

typedef struct focus_message
{
	int msg_type;
	uint dom_id;
	int focus_or_not;
} FocusMsg;
