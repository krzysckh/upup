CFLAGS=-Wall -Wextra -std=c89
PREFIX=/usr

%.gz: %
	gzip -k $<

all: upup upup.1.gz

clean:
	rm -rf upup upup.1.gz
install: all
	install -Dm755 -s upup -t $(PREFIX)/bin/
	install -Dm644 upup.1.gz -t $(PREFIX)/share/man/man1/
uninstall:
	rm -rf $(PREFIX)/bin/upup
	rm -rf $(PREFIX)/share/man/man1/upup.1.gz
