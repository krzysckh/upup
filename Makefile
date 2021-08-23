CC=c99
CFLAGS=-Wall
PREFIX=/usr

upup:
	$(CC) upup.c -o upup $(CFLAGS)
clean:
	rm upup example.html
install: upup
	cp upup $(PREFIX)/bin/upup
	cp upup.1 $(PREFIX)/share/man/man1/upup.1
uninstall:
	rm $(PREFIX)/bin/upup
	rm $(PREFIX)/share/man/man1/upup.1
example: upup
	./upup -o example.html -f example.uu -l "http://krzysckh.org/style.css" -t "eggs :)"
