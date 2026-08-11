CFLAGS = -Wall -Wextra -Werror -std=c17 -q -fanitize=adress,undefined

%: %.c
	gcc $(CFLAGS) -o $@ $<
