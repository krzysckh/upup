CC=c99
CFLAGS=-Wall
PREFIX=/usr

upup:
	$(CC) upup.c -o upup $(CFLAGS)
clean:
	rm upup
install: upup
	cp upup $(PREFIX)/bin/upup
	cp upup.1 $(PREFIX)/share/man/man1/upup.1
uninstall:
	rm $(PREFIX)/bin/upup
	rm $(PREFIX)/share/man/man1/upup.1
