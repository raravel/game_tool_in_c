#include <stdio.h>
#include "game_tool.h"

int main(void)
{
	printf("旨收收收收收收收收收收旬\n");
	printf("早          早\n");
	printf("曲收收收收收收收收收收旭\n");
	printf("旨收收收收收收收收收收旬\n");
	printf("早          早\n");
	printf("曲收收收收收收收收收收旭\n");
	printf("旨收收收收收收收收收收旬\n");
	printf("早          早\n");
	printf("曲收收收收收收收收收收旭\n");

	progressbar bar1 = new_progressbar();
	bar1.size = (COORD) { 10, 1 };
	bar1.position = (COORD) { 1, 1 };
	bar1.progress = "≠";
	bar1.animation_delay = 100;

	progressbar bar2 = new_progressbar();
	
	bar2.size = (COORD) { 10, 1 };
	bar2.position = (COORD) { 1, 4 };
	bar2.progress = "﹥";
	bar2.animation_delay = 100;
	bar2.type = infinity;
	bar2.progressbar_size = 15;
	bar2.use_thead = false;

	progressbar_step(&bar2);


	progressbar bar3 = new_progressbar();
	bar3.size = (COORD) { 10, 1 };
	bar3.position = (COORD) { 1, 7 };
	bar3.animation_delay = 50;
	bar3.progress = "收";
	bar3.color = (COLORS) { CR_BLACK, CR_WHITE };
	bar3.type = infinity;
	bar3.progressbar_size = 3;
	bar3.use_thead = false;

	while (!bar1.is_complete) {
		progressbar_step(&bar1);
		progressbar_step(&bar2);
		progressbar_step(&bar3);
	}
	

	while (1);
	return 0;
}
