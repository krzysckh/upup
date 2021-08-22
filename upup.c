#include <unistd.h>
#include <stdio.h>
#include <getopt.h> /* c99 so need to include that */

typedef int bool;
#define true 1
#define false 0

int main(int argc, char *argv[]) {
	bool bare = false;

	char *outn = NULL, *inn = NULL, *title = NULL, *cssln = NULL;

	int opt;

	while ((opt = getopt(argc, argv, "bho:f:t:l:")) != -1) {
		switch(opt) {
			case 'b':
				bare = true;
				break;
			case 'h':
				printf("usage: upup [args]\n");
				return 0;
			case 'o':
				outn = optarg;
				break;
			case 'f':
				inn = optarg;
				break;
			case 't':
				title = optarg;
				break;
			case 'l':
				cssln = optarg;
				break;
			default:
				fprintf(stderr, "usage: upup [args]\n");
				return 1;
		}
	}

	FILE *inf = (inn == NULL) ? stdin : fopen(inn, "r");
	FILE *outf = (outn == NULL) ? stdout : fopen(outn, "w");

	if (inf == NULL) {
		fprintf(stderr, "file %s does not exist\n", inn);
		if (outf != stdout) fclose(outf);
		return 1;
	}

	char c = 0;

	bool bold = false, italic = false, crossed = false, header = false, code = false;

	if (!bare) {
		fprintf(outf,
				"<!DOCTYPE html>\n"
				"<html>\n"
				"<head>\n"
				"<meta charset=\"utf-8\">\n"
		       );
		if (title != NULL) {
			fprintf(outf, "<title> %s </title>\n", title);
		}
		fprintf(outf,
				"



	while (c != EOF) {
		c = fgetc(inf);

		switch(c) {
			case '\\':
				fputc(fgetc(inf), outf);
				break;
			case '\n':
				/*switch (true) {
					case bold:
						fprintf(outf, "</b>"); break;
					case italic:
						fprintf(outf, "</i>"); break;
					case crossed:
						fprintf(outf, "</s>"); break;
					case header:
						fprintf(outf, "</h1>"); break;
					case code:
						fprintf(outf, "</code> </pre>"); break;
				}*/
				if (bold) { fprintf(outf, "</b>"); bold = false; }
				if (italic) { fprintf(outf, "</i>"); italic = false; }
				if (crossed) { fprintf(outf, "</s>"); crossed = false; }
				if (header) { fprintf(outf, "</h1>"); header = false; }
				if (code) { fprintf(outf, "</code></pre>"); code = false; }

				fprintf(outf, "<br />");
				fprintf(outf, "\n");
				break;
			case '*':
				if (bold) {
					bold = !bold;
					fprintf(outf, "</b>");
				} else {
					fprintf(outf, "<b>");
					bold = true;
				} break;
			case '-':
				if (crossed) {
					crossed = !crossed;
					fprintf(outf, "</s>");
				} else {
					fprintf(outf, "<s>");
					crossed = true;
				} break;
			case '_':
				if (italic) {
					italic = !italic;
					fprintf(outf, "</i>");
				} else {
					fprintf(outf, "<i>");
					italic = true;
				} break;
			case '+':
				if (header) {
					header = !header;
					fprintf(outf, "</h1>");
				} else {
					fprintf(outf, "<h1>");
					header = true;
				} break;
			case '~':
				if (code) {
					code = !code;
					fprintf(outf, "</code></pre>");
				} else {
					fprintf(outf, "<pre><code>");
					code = true;
				} break;
			case '^':
				fprintf(outf, "\n<hr />\n");
				break;
			case EOF:
				break;
			default:
				fputc(c, outf);
				break;
		}

	}



	return 0;
}
