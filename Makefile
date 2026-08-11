CFLAGS = -Wall -Wextra -Werror -std=c17 -g -fsanitize=address,undefined -lm

%: %.c
	gcc $(CFLAGS) -o $@ $<
