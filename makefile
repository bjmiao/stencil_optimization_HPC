CC=gcc

all: main_1d main_2d

opt: main_1d_opt main_2d_opt

save: save_1d save_2d 

main_1d: main_1d.c
	$(CC) main_1d.c -o main_1d.x -O2

main_1d_opt: main_1d.c
	$(CC) main_1d.c -o main_1d_opt.x -D__OPT -O2 

save_1d: main_1d.c
	$(CC) main_1d.c -o main_1d_std.x -D__SAVE

main_2d: main_2d.c
	$(CC) main_2d.c -o main_2d.x -O2

main_2d_opt: main_2d.c
	$(CC) main_2d.c -o main_2d_opt.x -D__OPT -O2 

save_2d: main_2d.c
	$(CC) main_2d.c -o main_2d_std.x -D__SAVE -g

.PHONY: clean

clean:
	rm *.x
