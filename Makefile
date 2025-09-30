build: main.c
	gcc -Wall -Wextra -std=c99 -g -o sfl main.c MEMfunctions.c SFLfunctions.c
run_sfl: build
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./sfl
clean:
	rm sfl