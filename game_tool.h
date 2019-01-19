#pragma once
#include <stdbool.h>
#include <Windows.h>

////////////////////////////////////////////////////
//                     Common                     //
////////////////////////////////////////////////////
#define mthread unsigned int __stdcall
#define CONSOLE_WIDTH get_console_width()
#define CONSOLE_HEIGHT get_console_height()
#define CONSOLE_FONT L"D2Coding"


////////////////////////////////////////////////////
//                      LOG                       //
////////////////////////////////////////////////////
char log_lvl;
typedef enum _log_level {
	LOG_NONE = 0,
	LOG_ERROR,
	LOG_WARNING,
	LOG_INFO,
	LOG_TEST
} log_level;
#define LOGGING(LVL, ...) { \
	if ( LVL >= log_lvl) { \
		fprintf(stderr, "[%s:%d] ", __func__, __LINE__); \
		fprintf(stderr, __VA_ARGS__); \
	} \
}
#define SET_LOG_LVL(X) log_lvl = X;

////////////////////////////////////////////////////
//                    console                     //
////////////////////////////////////////////////////
HANDLE cursor_mutex;
HANDLE color_mutex;

typedef enum _view {
	hide,
	visible
} view;
typedef enum _color {
	CR_BLACK = 0,
	CR_DARK_BLUE,
	CR_DARK_GREEN,
	CR_DARK_SKYBLUE,
	CR_DARK_RED,
	CR_DARK_PINK,
	CR_DARK_YELLOW,
	CR_SILVER,
	CR_GRAY,
	CR_BLUE,
	CR_GREEN,
	CR_SKYBLUE,
	CR_RED,
	CR_PINK,
	CR_YELLOW,
	CR_WHITE
} color;
typedef struct _COLORS {
	color background;
	color forground;
} COLORS;
COLORS __NOW_CONSOLE_COLOR__;
COLORS __BEFORE_CONSOLE_COLOR__;

void cursor_view(view show);
int get_console_width(void);
int get_console_height(void);
COORD get_now_cursor_position(void);
void gotoxy(int x, int y);
void SetColor(color background, color forground);
void SetColorS(COLORS colors);

////////////////////////////////////////////////////
//                  progress bar                  //
////////////////////////////////////////////////////
typedef enum _pb_type {
	block,
	infinity
} pb_type;

typedef struct _progressbar {
	//common
	unsigned short now_step;		//current progress step

	//block type
	unsigned short now_value;		//current progress bar value
	unsigned short min_value;		//minimum progress bar value default) 0
	unsigned short max_value;		//maximum progress bar value default) 100

	//infinity type
	bool		   use_thead;		//create thread. only once used progressbar_step
	unsigned char  progressbar_size;//infinity progress bar size default) 5

	//0. block progress bar (default)
	//1. infinity progress bar
	pb_type	       type;
	bool		   is_running;		//check progress running
	bool		   is_complete;		//check progress complete
	unsigned short animation_delay;	//progress animation delay	default) 100
	COLORS		   color;			//progress bar color		default) CR_BLACK, CR_GREEN
	char		  *progress;		//progress bar string		default) "¦¡"

	//must initialize
	COORD		   position;		//progress bar position
	COORD		   size;			//progress bar size

	//private variable
	short		   one_step;		//a constant one step
	short		   tmp_step;		//a variable remainder step
	HANDLE		   thread_handle;	//infinity type thread handle
	unsigned int   thread_id;		//infinity type thread id
} progressbar;

progressbar new_progressbar(void);
void progressbar_step(progressbar *pb);

