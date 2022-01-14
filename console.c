#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX(a,b) ((a) > (b) ? (a) : (b))

int main(int argc, char *argv[]) {
	int ch;
	
	struct stat std;
	char *copy, *temp;
	int max_x, max_y, max_line_strlen = 0;
	int x,y;
	int scroll_x, scroll_y;
	int max_scroll_y, max_scroll_x;
	int i, fd;
	char **lines;
	
	initscr();
	raw();
	keypad(stdscr,TRUE);
	noecho();
	
	idlok(stdscr,TRUE);
	scrollok(stdscr,TRUE);
	
	stat(argv[1],&std);
	copy = malloc(std.st_size);
	fd = open(argv[1],O_RDONLY);
	read(fd,copy,std.st_size);
	close(fd);
	temp = copy;
	
	for (i = 0; *temp; temp++) {
		if (*temp == '\n') {i++;}
	}
	lines = malloc((i+1)*sizeof(char *));
	temp = copy;
	for (i = 0; temp && *temp; i++) {
		char *curr = strsep(&temp,"\n");
		lines[i] = curr;
	}
	lines[i] = NULL;
	max_scroll_y = i;
	max_scroll_x = 0;
	

	getmaxyx(stdscr, max_y, max_x);
	for (i = 0; lines[i]; i++) {
		printw("lines[%d]: '%s'\n",i,lines[i]);
		max_line_strlen = MAX(max_line_strlen,strlen(lines[i]));
		max_scroll_x = (max_line_strlen > max_x) ? (max_line_strlen - max_x) : 0;
	}
	if (max_scroll_y > max_y) {
		max_scroll_y -= max_y;
	} else {
		max_scroll_y = 0;
	}
	scroll_x = 0;
	scroll_y = 0;
	printw("max_x: %d  max_y: %d\n", max_x, max_y);
	printw("max_scroll_x: %d  max_scroll_y: %d\n", max_scroll_x, max_scroll_y);
	printw("scroll_x: %d  scroll_y: %d\n", scroll_x, scroll_y);
	
	getyx(stdscr, y ,x);
	while(1) {
		int old_scroll_x = scroll_x;
		int old_scroll_y = scroll_y;
		
		ch = getch();
		if (ch >= 0x20 && ch < 0x7F) {
			break;
		} else {
			switch (ch) {
				case KEY_UP:
					if (y == 0) {
						if (scroll_y > 0) { scroll_y--; }
					} else {
						y--;
					}
					break;
				case KEY_DOWN:
					if (y == max_y - 1) {
						if (scroll_y < max_scroll_y) { scroll_y++; }
					} else {
						y++;
					}
					break;
				case KEY_LEFT:
					if (x == 0) {
						if (scroll_x > 0) { scroll_x--; }
					} else {
						x--;
					}
					break;			
				case KEY_RIGHT:
					if (x == max_x - 1) {
						if (scroll_x < max_scroll_x) { scroll_x++; }
					} else {
						x++;
					}
					break;
				default:
					break;
			}
		}
		if (scroll_x == old_scroll_x) {
			if (scroll_y != old_scroll_y) {
				if (scroll_y > old_scroll_y) {
					while (scroll_y > old_scroll_y) {
						scroll(1);
						old_scroll_y++;
					}
				} else {
					while (scroll_y < old_scroll_y) {
						scroll(-1);
						old_scroll_y--;
					}
				}
			}
		} else {
			move(0,0);
			int i,j;
			for (j = scroll_y; j < scroll_y + max_y && lines[j]; j++) {
			for (i = scroll_x; i < scroll_x + max_x && i < strlen(lines[j]) && lines[j][i]; i++) {
					addch(lines[j][i]);
				}
				addch('\n');
			}
		}
		move(y,x);
		refresh();		
	}
	getch();
	endwin();
	
	return 0;
}
