#include <unistd.h>
#include <stdio.h>
#include <getopt.h> /* c99 so need to include that */
#include <string.h> /* strcmp */

typedef int bool;
#define true 1
#define false 0

#define link_buff 1000

struct template {
	char *bold_open;
	char *bold_close;
	char *crossed_open;
	char *crossed_close;
	char *italic_open;
	char *italic_close;
	char *header_open;
	char *header_close;
	char *code_open;
	char *code_close;
	char *hr;
	char *br;
};

int main(int argc, char *argv[]) {
	bool bare = false;

	char *outn = NULL, *inn = NULL, *title = NULL, *cssln = NULL;
	
	int to = 0;

	/* 0 - html; 1 - md; */

	int opt;

	while ((opt = getopt(argc, argv, "bho:f:t:l:T:")) != -1) {
		switch(opt) {
			case 'b':
				bare = true;
				break;
			case 'h':
				printf("usage: upup [-f infile] [-o outfile] [-t title] [-l cssfile] [-T md/ansi/html] [-bh]\n");
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
			case 'T':
				if (strcmp(optarg, "html") == 0) {
					to = 0;
				} else if (strcmp(optarg, "md") == 0 || strcmp(optarg, "markdown") == 0) {
					to = 1;
				} else if (strcmp(optarg, "ansi") == 0) {
					to = 2;
				} else {
					fprintf(stderr, "%s is not a legal argument for -%c\n", optarg, opt);
					return 1;
				}
				break;
			default:
				fprintf(stderr, "usage: upup [-f infile] [-o outfile] [-t title] [-l cssfile] [-T md/ansi/html] [-bh]\n");
				return 1;
		}
	}

	if (argv[optind] != NULL) {
		inn = argv[optind];
	}
	
	struct template marks;

	switch (to) {
		case 0: ;
			marks.bold_open = "<b>";
			marks.bold_close = "</b>";
			marks.crossed_open = "<s>";
			marks.crossed_close = "</s>";
			marks.italic_open = "<i>";
			marks.italic_close = "</i>";
			marks.header_open = "<h1>";
			marks.header_close = "</h1>";
			marks.code_open = "<pre><code>";
			marks.code_close = "</code></pre>";
			marks.hr = "<hr />";
			marks.br = "<br />";
			break;
		case 1: ;
			marks.bold_open = "**";
			marks.bold_close = "** ";
			marks.crossed_open = "~";
			marks.crossed_close = "~ ";
			marks.italic_open = "*";
			marks.italic_close = "* ";
			marks.header_open = "# ";
			marks.header_close = "";
			marks.code_open = "```";
			marks.code_close = "```";
			marks.hr = "---";
			marks.br = "\n";
			break;
		case 2: ;
			marks.bold_open = "\033[1m";
			marks.bold_close = "\033[22m";
			marks.crossed_open = "\033[9m";
			marks.crossed_close = "\033[29m";
			marks.italic_open = "\033[3m";
			marks.italic_close = "\033[23m";
			marks.header_open = "\033[1m\033[3m\t ";
			marks.header_close = "\033[22m\033[23m\n";
			marks.code_open = "\033[3m";
			marks.code_close = "\033[23m";
			marks.hr = "----------\n";
			marks.br = "\n";
			break;
	}



	FILE *inf = (inn == NULL) ? stdin : fopen(inn, "r");
	FILE *outf = (outn == NULL) ? stdout : fopen(outn, "w");

	if (inf == NULL) {
		fprintf(stderr, "file %s does not exist\n", inn);
		if (outf != stdout) fclose(outf);
		return 1;
	}

	char c = 0;

	bool bold = false, italic = false, crossed = false, header = false;

	switch (to) {
		case 0:
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
				if (cssln != NULL) {
					fprintf(outf, "<link rel=\"stylesheet\" href=\"%s\">\n", cssln);
				}

				fprintf(outf,
						"</head>\n"
						"<body>\n"
				       );
			}
			break;
		case 1:
			break;
		case 2:
			fprintf(outf, "\n");
			break;
	}


	while (c != EOF) {
		c = fgetc(inf);

		switch(c) {
			case '\\':
				fputc(fgetc(inf), outf);
				break;
			case '\n':
				if (bold) { fprintf(outf, "%s", marks.bold_close); bold = false; }
				if (italic) { fprintf(outf, "%s", marks.italic_close); italic = false; }
				if (crossed) { fprintf(outf, "%s", marks.crossed_close); crossed = false; }
				if (header) { fprintf(outf, "%s", marks.header_close); header = false; }
				fprintf(outf, "%s", marks.br);

				if (to == 0) {
					fprintf(outf, "\n");
				}
				break;
			case '*':
				if (bold) {
					bold = !bold;
					fprintf(outf, "%s", marks.bold_close);
				} else {
					fprintf(outf, "%s", marks.bold_open);
					bold = true;
				} break;
			case '-':
				if (crossed) {
					crossed = !crossed;
					fprintf(outf, "%s", marks.crossed_close);
				} else {
					fprintf(outf, "%s", marks.crossed_open);
					crossed = true;
				} break;
			case '_':
				if (italic) {
					italic = !italic;
					fprintf(outf, "%s", marks.italic_close);
				} else {
					fprintf(outf, "%s", marks.italic_open);
					italic = true;
				} break;
			case '+':
				if (header) {
					header = !header;
					fprintf(outf, "%s", marks.header_close);
				} else {
					fprintf(outf, "%s", marks.header_open);
					header = true;
				} break;
			case '~':
				fprintf(outf, "%s", marks.code_open);
				while ((c = fgetc(inf)) != '~') {
					fputc(c, outf);
				}
				fprintf(outf, "%s", marks.code_close);
				break;
			case '^':
				fprintf(outf, "%s", marks.hr);
				break;
			case '{':
				switch (to) {
					case 0:
						fprintf(outf, "<img alt=\"img\" src=\"");
						while ((c = fgetc(inf)) != '}') {
							fputc(c, outf);
						}
						fprintf(outf, "\" />");
						break;
					case 1:
						fprintf(outf, "![img](");
						while ((c = fgetc(inf)) != '}') {
							fputc(c, outf);
						}
						fprintf(outf, ")\n");
						break;
					case 2:
						fprintf(outf, "%sIMAGE (", marks.bold_open);
						while ((c = fgetc(inf)) != '}') {
							fputc(c, outf);
						}
						fprintf(outf, ")%s", marks.bold_close);
						break;
				}
				break;
			case '&': ;
				char link[link_buff];
				int countl = 0, i = 0;

				switch (to) {
					case 0: ;
							fprintf(outf, "<a href=\"");
							while ((c = fgetc(inf)) != '[') {
								fputc(c, outf);
							} fprintf(outf, "\"> ");
							while ((c = fgetc(inf)) != ']') {
								fputc(c, outf);
							} fprintf(outf, "</a>");
							break;
					case 1: ;
							while ((c = fgetc(inf)) != '[') {
								link[countl] = c;
								countl++;
							} fprintf(outf, "["); 
							while ((c = fgetc(inf)) != ']') {
								fputc(c, outf);
							} 
							fprintf(outf, "](");
							for (i = 0; i < countl; i++) fputc(link[i], outf);
							fprintf(outf, ") ");
							break;
					case 2: ;
							while ((c = fgetc(inf)) != '[') {
								link[countl] = c;
								countl++;
							}
							while ((c = fgetc(inf)) != ']') {
								fputc(c, outf);
							} 
							fprintf(outf, " %s(", marks.bold_open);
							for (i = 0; i < countl; i++) fputc(link[i], outf);
							fprintf(outf, ")%s ", marks.bold_close);
							break;
				}
				break;
			case EOF:
				break;
			default:
				fputc(c, outf);
				break;
		}

	}

	if (!bare) {
		switch (to) {
			case 0:
				fprintf(outf,
						"</body>\n"
						"</html>\n"
				       );
				break;
			case 1:
				break;
			case 2:
				fprintf(outf, "\n");
				break;
		}
	}

	return 0;
}
