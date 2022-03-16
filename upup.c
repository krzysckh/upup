#include <stdio.h>
#include <wctype.h>
#include <string.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

typedef struct {
	char *paragraph_open;
	char *paragraph_close;
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
} Template;

typedef enum {
	html,
	markdown,
	ansi,
	mom
} Out;

int main(int argc, char *argv[]) {
	srand(time(NULL));

	bool bare = false;
	 
	char *title = NULL,
	     *css = NULL,
	     *text,
	     *tmp_os_com,
	     *tmp,
	     *tmp_href;

	FILE *out = stdout,
	     *in;

	int opt,
	    sz,
	    i,
	    j,
	    k,
	    c,
	    mom_img_fname_cnt = 0;

	bool bold = false,
	     italic = false,
	     crossed = false,
	     header = false,
	     paragraph = false,
	     firstnop = true;

	Out out_t = html;

	while ((opt = getopt(argc, argv, "bho:t:l:T:")) != -1) {
		switch(opt) {
			case 'b':
				bare = true;
				break;
			case 'h':
				printf("usage: %s [-o file] [-t title] [-l file] [-T type] [-bh]\n", argv[0]);
				exit(0);
			case 'o':
				out = fopen(optarg, "w");
				break;
			case 't':
				title = optarg;
				break;
			case 'l':
				css = optarg;
				break;
			case 'T':
				if (strcmp(optarg, "html") == 0) {
					out_t = html;
				} else if (strcmp(optarg, "ansi") == 0) {
					out_t = ansi;
				} else if (strcmp(optarg, "md") == 0) {
					out_t = markdown;
				} else if (strcmp(optarg, "mom") == 0) {
					out_t = mom;
				} else {
					fprintf(stderr, "%s: illegal out type: %s\n", argv[0], optarg);
					exit(1);
				}
				break;
		}
	}

	if (optind < argc) {
		in = fopen(argv[optind], "rb+");
		if (in == NULL) {
			fprintf(stderr, "%s: cannot open %s for reading\n", argv[0], argv[optind]);
			exit(1);
		}
	} else {
		in = tmpfile();
		while ((c = fgetc(stdin)) != EOF) {
			fputc(c, in);
		}
	}

	fseek(in, 0L, SEEK_END);
	sz = ftell(in);
	fseek(in, 0L, SEEK_SET);
	text = malloc(sizeof(char) * sz);

	for (i = 0; i < sz; i++) {
		text[i] = fgetc(in);
	}

	Template mark;

	switch (out_t) {
		case html:
			mark.paragraph_open = "<p>";
			mark.paragraph_close = "</p>\n";
			mark.bold_open = "<b>";
			mark.bold_close = "</b>";
			mark.crossed_open = "<s>";
			mark.crossed_close = "</s>";
			mark.italic_open = "<i>";
			mark.italic_close = "</i>";
			mark.header_open = "<h1>";
			mark.header_close = "</h1>\n";
			mark.code_open = "<pre><code>";
			mark.code_close = "</code></pre>\n";
			mark.hr = "<hr />";
			mark.br = "<br />";
			break;
		case markdown:
			mark.paragraph_open = "";
			mark.paragraph_close = "\n\n";
			mark.bold_open = "**";
			mark.bold_close = "** ";
			mark.crossed_open = "~";
			mark.crossed_close = "~ ";
			mark.italic_open = "*";
			mark.italic_close = "* ";
			mark.header_open = "# ";
			mark.header_close = "\n";
			mark.code_open = "```";
			mark.code_close = "```";
			mark.hr = "---";
			mark.br = "\n";
			break;
		case ansi:
			mark.paragraph_open = "";
			mark.paragraph_close = "\n\n";
			mark.bold_open = "\033[1m";
			mark.bold_close = "\033[22m";
			mark.crossed_open = "\033[9m";
			mark.crossed_close = "\033[29m";
			mark.italic_open = "\033[3m";
			mark.italic_close = "\033[23m";
			mark.header_open = "\033[1m\033[3m\t ";
			mark.header_close = "\033[22m\033[23m\n";
			mark.code_open = "\033[3m";
			mark.code_close = "\033[23m";
			mark.hr = "----------\n";
			mark.br = "\n";
			break;
		case mom:
			mark.paragraph_open = "\n.PP\n";
			mark.paragraph_close = "";
			mark.bold_open = "\\*[BOLDER]";
			mark.bold_close = "\\*[BOLDERX]";
			mark.crossed_open = "";
			mark.crossed_close = "";
			mark.italic_open = "\\*[SLANT]";
			mark.italic_close = "\\*[SLANTX]";
			mark.header_open = ".HEADING 1 \"";
			mark.header_close = "\"\n";
			mark.code_open = "\n.CODE\n";
			mark.code_close = "\n.CODE OFF\n";
			mark.hr = "\n.DRH\n.BR\n";
			mark.br = "\n.BR\n";
			break;
	}

	if (!bare) {
		switch (out_t) {
			case html:
				fprintf(out, "<!DOCTYPE html>\n<html>\n<head>\n<meta charset=\"utf-8\">");
				if (css != NULL) {
					fprintf(out, "<link rel=\"stylesheet\" href=\"%s\">\n", css);
				}
				if (title != NULL) {
					fprintf(out, "<title>%s</title>\n", title);
				}
				fprintf(out, "</head><body>");
				break;
			case markdown:
				break;
			case ansi:
				break;
			case mom:
				fprintf(out, 
					"\\\" compile with preconv -e utf8 <file> | groff -mom -m www -Tps\n"
					".PRINTSTYLE TYPESET\n"
					".FT NotoSerif-Regular\n"
				);
				if (title != NULL) {
					fprintf(out, ".TITLE \"%s\"\n", title);
				}
				fprintf(out, ".START\n");
				break;
		}
	}

	i = 0;
	while (i < sz) {
		if (!paragraph && !iswspace(text[i]) && !firstnop && !header) {
			fprintf(out, "%s", mark.paragraph_open);
			paragraph = true;
		}
		switch(text[i]) {
			case '\\':
				i ++;
				fputc(text[i], out);
				break;
			case '\n':
				if (i + 1 >= sz)
					break;
				if (text[i+1] == '\n' && paragraph && !header) {
					fprintf(out, "%s", mark.paragraph_close);
					paragraph = false;
				}
				if (bold)	{ fprintf(out, "%s", mark.bold_close	); bold = false; }
				if (italic)	{ fprintf(out, "%s", mark.italic_close	); italic = false; }
				if (crossed)	{ fprintf(out, "%s", mark.crossed_close	); crossed = false; }
				if (header)	{ fprintf(out, "%s", mark.header_close 	); header = false; }
				
				if (paragraph)
					fprintf(out, "%s", mark.br);
				break;
			case '*':
				if (bold)
					fprintf(out, "%s", mark.bold_close);
				else
					fprintf(out, "%s", mark.bold_open);

				bold = !bold;
				break;
			case '-':
				if (crossed)
					fprintf(out, "%s", mark.crossed_close);
				else
					fprintf(out, "%s", mark.crossed_open);

				crossed = !crossed;
				break;
			case '_':
				if (italic)
					fprintf(out, "%s", mark.italic_close);
				else
					fprintf(out, "%s", mark.italic_open);

				italic = !italic;
				break;
			case '+':
				if (paragraph) {
					paragraph = false;
					fprintf(out, "%s", mark.paragraph_close);
				}
				if (header)
					fprintf(out, "%s", mark.header_close);
				else
					fprintf(out, "%s", mark.header_open);

				header = !header;
				break;
			case '~':
				i ++;
				fprintf(out, "%s", mark.code_open);
				while (text[i] != '~' && i != sz) {
					fputc(text[i], out);
					i ++;
				}
				fprintf(out, "%s", mark.code_close);
				break;
			case '^':
				if (paragraph) {
					paragraph = false;
					fprintf(out, "%s", mark.paragraph_close);
				}
				fprintf(out, "%s", mark.hr);
				break;
			case '{':
				i ++;
				j = i;

				do {
					j ++;
				} while (text[j] != '}' && j < sz);

				tmp = malloc(sizeof(char) * (j - i + 1));

				for (k = 0; k < (j - i + 1); k++) {
					tmp[k] = '\0';
				}

				j = 0;
				for (; (text[i] != '}' && i < sz); i++, j++) {
					tmp[j] = text[i];
				}

				switch (out_t) {
					case html:
						fprintf(out, "<img src=\"%s\" />\n", tmp);
						break;
					case markdown:
						fprintf(out, "![IMAGE](%s)\n", tmp);
						break;
					case ansi:
						fprintf(out, "[ image: %s ]\n", tmp);
						break;
					case mom:
						/* fikoła zajebe */
						tmp_os_com = malloc(sizeof(char) * (41 + strlen(tmp) + 12 + 1));
						/*      MAGIC NUMBERS
						 *
						 * 43 - length of commands,
						 * 10 - strlen("2147483647")
						 * 1 - NULL terminator
						 */
						for (j = 0; j < (int)(39 + strlen(tmp) + 12 + 1); j++) {
							tmp_os_com[j] = '\0';
						}

						if (strncmp(tmp, "http", 4) == 0) {
							sprintf(tmp_os_com, 
								"curl -s %s | convert - PS:/tmp/_upup_tmp%d.ps",
								tmp,
								mom_img_fname_cnt
							       );
						} else {
							sprintf(tmp_os_com, 
								"convert %s PS:/tmp/_upup_tmp%d.ps",
								tmp,
								mom_img_fname_cnt
							       );
						}

						fprintf(stderr, "running %s\n", tmp_os_com);
						system(tmp_os_com);
						
						fprintf(out,
							".PSPIC "
							"/tmp/_upup_tmp%d.ps\n",
							mom_img_fname_cnt
							);

						mom_img_fname_cnt ++;

						free(tmp_os_com);
						break;
				}
				
				free(tmp);
				break;
			case '&':
				i ++;
				j = i;

				do {
					j ++;
				} while (text[j] != '[' && j < sz);
				/* get href length */

				tmp_href = malloc(sizeof(char) * (j - i + 1));
				for (k = 0; k < (j - i + 1); k++) {
					tmp_href[k] = '\0';
				}

				j = 0;
				for (; (text[i] != '[' && i < sz); i++, j++) {
					tmp_href[j] = text[i];
				}
				/* get href into tmp_href */
				i ++;
				j = i;

				do {
					j ++;
				} while (text[j] != ']' && j < sz);
				/* get desc length */

				tmp = malloc(sizeof(char) * (j - i + 1));
				for (k = 0; k < (j - i + 1); k++) {
					tmp[k] = '\0';
				}

				j = 0;
				for (; (text[i] != ']' && i < sz); i++, j++) {
					tmp[j] = text[i];
				}
				/* get desc into tmp */

				switch (out_t) {
					case html:
						fprintf(out, "<a href=\"%s\">%s</a>", tmp_href, tmp);
						break;
					case markdown:
						fprintf(out, "[%s](%s)", tmp, tmp_href);
						break;
					case ansi:
						fprintf(out, 
							"%s%s%s <-> link to %s\n",
							mark.bold_open,
							tmp,
							mark.bold_close,
							tmp_href
						);
						break;
					case mom:
						fprintf(out, "\n.URL %s \"%s\"\n", tmp_href, tmp);
						break;
				}


				free(tmp);
				free(tmp_href);
				break;
			default:
				if (firstnop && !header) {
					fprintf(out, "%s", mark.paragraph_open);
					firstnop = false;
					paragraph = true;
				}

				fputc(text[i], out);
				break;
		}
		i ++;

	}

	if (paragraph)
		fprintf(out, "%s", mark.paragraph_close);

	if (!bare) {
		switch (out_t) {
			case html:
				fprintf(out, "</body></html>");
				break;
			case markdown:
			case ansi:
			case mom:
				break;
		}

	}


	free(text);
	fclose(in);
	fclose(out);
	return 0;
}
