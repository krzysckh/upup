CFLAGS=-Wall -Wextra -std=c89
PREFIX=/usr

upup: upup.c
clean:
	rm upup
install: upup
	cp upup $(PREFIX)/bin/upup
	cp upup.1 $(PREFIX)/share/man/man1/upup.1
uninstall:
	rm $(PREFIX)/bin/upup
	rm $(PREFIX)/share/man/man1/upup.1
