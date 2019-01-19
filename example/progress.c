#include <stdio.h>
#include "game_tool.h"

int main(void)
{
	progressbar bar = new_progressbar();
	SET_LOG_LVL(LOG_TEST);
	
	bar.size = (COORD) { 10, 1 };
	bar.position = (COORD) { 1, 1 };
	bar.progress = "■";
	bar.animation_delay = 100;
	bar.type = infinity;
	bar.progressbar_size = 15;
	bar.use_thead = false;
	bar.test = 0;

	printf("┏━━━━━━━━━━┓\n");
	printf("┃          ┃\n");
	printf("┗━━━━━━━━━━┛\n");
	printf("┏━━━━━━━━━━┓\n");
	printf("┃          ┃\n");
	printf("┗━━━━━━━━━━┛\n");

	progressbar_step(&bar);


	progressbar bar2 = new_progressbar();
	bar2.size = (COORD) { 10, 1 };
	bar2.position = (COORD) { 1, 4 };
	bar2.animation_delay = 50;
	bar2.progress = "━";
	bar2.color = (COLORS) { CR_BLACK, CR_WHITE };
	bar2.type = infinity;
	bar2.progressbar_size = 3;
	bar2.use_thead = false;
	bar2.test = 1;

	while (1) {
		progressbar_step(&bar);
		progressbar_step(&bar2);
	}
	

	while (1);
	return 0;
}