#include "game_tool.h"
#include <stdio.h>
#include <process.h>

////////////////////////////////////////////////////
//                  INITIALIZER                   //
////////////////////////////////////////////////////
#ifdef _MSC_VER
#define INITIALIZER(f) \
    static void f();\
    static int __f1(){f();return 0;}\
    __pragma(data_seg(".CRT$XIU"))\
    static int(*__f2) () = __f1;\
    __pragma(data_seg())\
    static void f()
#else
#define INITIALIZER(f) \
    __attribute__((constructor)) static void f()
#endif

static void game_tool_deinit(void)
{
	//console deinit
	CloseHandle(cursor_mutex);
	CloseHandle(color_mutex);
}

INITIALIZER(game_tool_init) {
	//console
	cursor_mutex = CreateMutex(NULL, FALSE, NULL);
	if (cursor_mutex == NULL)
	{
		printf("Create cursor_mutex error: %d\n", GetLastError());
		exit(0);
	}
	color_mutex = CreateMutex(NULL, FALSE, NULL);
	if (color_mutex == NULL)
	{
		printf("Create color_mutex error: %d\n", GetLastError());
		exit(0);
	}

	system("COLOR 0F");
	__BEFORE_CONSOLE_COLOR__ = (COLORS) { CR_BLACK, CR_WHITE };
	__NOW_CONSOLE_COLOR__ = (COLORS) { CR_BLACK, CR_WHITE };

	CONSOLE_FONT_INFOEX CFI = { sizeof(CONSOLE_FONT_INFOEX), 0, {10,22}, 0, 400, CONSOLE_FONT };
	SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &CFI);

	cursor_view(hide);

	atexit(game_tool_deinit);
}

////////////////////////////////////////////////////
//                    console                     //
////////////////////////////////////////////////////
void cursor_view(char show)
{
	HANDLE hConsole;
	CONSOLE_CURSOR_INFO ConsoleCursor;

	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	ConsoleCursor.bVisible = show;
	ConsoleCursor.dwSize = 1;

	SetConsoleCursorInfo(hConsole, &ConsoleCursor);
}

int get_console_width(void)
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	int columns;

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	return columns;
}

int get_console_height(void)
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	int rows;

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
	return rows;
}

COORD get_now_cursor_position(void)
{
	CONSOLE_SCREEN_BUFFER_INFO curInfo;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	return curInfo.dwCursorPosition;
}

void gotoxy(int x, int y)
{
	DWORD mutex_wait = WaitForSingleObject(cursor_mutex,INFINITE);
	if (mutex_wait == WAIT_OBJECT_0) 
	{
		COORD pos = { x,y };
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
	}
}

void SetColorS(COLORS colors)
{
	SetColor(colors.background, colors.forground);
}

void SetColor(color background, color forground)
{
	DWORD mutex_wait = WaitForSingleObject(color_mutex, INFINITE);
	if (mutex_wait == WAIT_OBJECT_0) 
	{
		__BEFORE_CONSOLE_COLOR__ = __NOW_CONSOLE_COLOR__;
		__NOW_CONSOLE_COLOR__.background = background;
		__NOW_CONSOLE_COLOR__.forground  = forground;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (background*16) + forground);
	}
}

////////////////////////////////////////////////////
//                  progress bar                  //
////////////////////////////////////////////////////
static void print_progress_bar(progressbar *pb)
{
	COORD spos = get_now_cursor_position();
	SetColorS(pb->color);
	for (int i = 0; i < pb->size.Y; i++) {
		gotoxy(spos.X, spos.Y + i);
		printf("%s", pb->progress);
	}
	gotoxy(spos.X+1, spos.Y);
	SetColorS(__BEFORE_CONSOLE_COLOR__);
}

static void step_block_pb(progressbar *pb)
{
	if (pb->is_running) {
		int step = pb->size.X / pb->max_value;
		COORD spos = get_now_cursor_position();

		gotoxy(pb->position.X + pb->now_step, pb->position.Y);
		for (int i = 0; i < step; i++, pb->now_step++) {
			print_progress_bar(pb);
		}
		if ((double)(step + pb->now_value)*0.1 > (double)pb->size.X / (double)pb->tmp_step) {
			print_progress_bar(pb);
			pb->now_step += 1;
			pb->tmp_step -= 1;
		}

		pb->now_value += 1;
		if (pb->now_value == pb->max_value) {
			while (pb->now_step < pb->size.X) {
				print_progress_bar(pb);
				pb->now_step += 1;
				Sleep(pb->animation_delay);
			}
			pb->is_running = false;
			pb->is_complete = true;
		}
		gotoxy(spos.X, spos.Y);
		Sleep(pb->animation_delay);
	}
	else {
		if (pb->size.X < pb->max_value)
			pb->tmp_step = pb->size.X;
		else
			pb->tmp_step = pb->size.X % pb->max_value;
		pb->is_running = true;
		step_block_pb(pb);
	}
}

static void step_infinity_pb(progressbar *pb)
{
	int move_size = pb->size.X + pb->progressbar_size, i;
	COORD spos = get_now_cursor_position();

	//delete before progressbar
	if (pb->now_step >= pb->progressbar_size && pb->now_step < pb->size.X + pb->progressbar_size) {
		SetColorS(__BEFORE_CONSOLE_COLOR__);
		gotoxy(pb->position.X + pb->now_step-pb->progressbar_size, pb->position.Y);
		printf(" ");
		SetColorS(__BEFORE_CONSOLE_COLOR__);
	}

	if (pb->now_step == pb->size.X + pb->progressbar_size) {
		pb->now_step = 0;
	}
	
	if (pb->now_step < pb->size.X) {
		gotoxy(pb->position.X + pb->now_step, pb->position.Y);
		print_progress_bar(pb);
	}
	Sleep(pb->animation_delay);
	pb->now_step += 1;
	gotoxy(spos.X, spos.Y);
}

static mthread infinity_thread(void *arg)
{
	progressbar *pb = (progressbar*)arg;
	while (pb->is_running && !pb->is_complete) {
		step_infinity_pb(pb);
	}
	return 0;
}

void progressbar_step(progressbar *pb)
{
	if (!pb || pb->is_complete) {
		return;
	}
	if (pb->type == block) {
		step_block_pb(pb);
	} else {
		if (pb->use_thead) {
			if (!pb->is_running) {
				pb->thread_handle = (HANDLE)_beginthreadex(
					NULL, 0, infinity_thread, (void *)pb, 0, &pb->thread_id);
				pb->is_running = true;
			}
		} else {
			step_infinity_pb(pb);
		}
	}
}

progressbar new_progressbar(void)
{
	progressbar pb = {
		/* now_step */		0,
		/* now_value */		0,
		/* min_value */		0,
		/* max_value */		100,

		/* use_thread */	false,
		/*progressbar_size*/5,
		
		/* type */			block,
		/* is_running */	false,
		/* is_compete */	false,
		/*animation_delay*/	100,
		/* color */			{ CR_BLACK, CR_GREEN},
		/* progress */		"¦¡",
		
		/* position */		NULL,
		/* size */			NULL,
		
		/* one_step */		1,
		/* tmp_step */		0,
		/* thread_handle */ NULL,
		/* thread_id */		0
	};
	return pb;
}
