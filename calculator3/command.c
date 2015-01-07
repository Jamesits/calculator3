#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "errorcode.h"
#include "angle.h"

extern int angle_mode;

void quit(char *s)
{
	puts("bye.");
	exit(EXIT_SUCCESS);
}

void help(char *s)
{
	puts("Usage:");
	puts("\ttype any math expressions and press enter.");
	puts("\tSupported operators: + - * / %(mod) ^(exp)");
	puts("\tSupported functions: sin cos tan log ln frac(! works sometimes too)");
	puts("\tType $ for result of last expression.");
	puts("");
	puts("Available commands:");
	puts("\tabout \t\tprint information about the author.");
	puts("\tbaseconv \tstart base conversion.");
	puts("\tclear \t\tclear screen.");
	puts("\thelp \t\tprint this help message.");
	puts("\tquit \t\tquit program.");
	puts("\tversion \tprint program version.");
	puts("Caution: all functions and commands are case-sensitive.");
	puts("");
}

void print_version(char *s)
{
	puts("Simple calculator version 0.09");
	puts("");
}

void print_about(char *s)
{
	puts("by James Swineson, 2014-12");
	puts("");
	puts("3rd party software:");
	puts("Tokenizer modified from en.literateprograms.org/Shunting_yard_algorithm_(C)");
}

void clear_screen(char *s)
{
	// TODO: not working
	return;
	// assume *nix
    //system("clear >null");
	// assume Windows
    //system("cls >null");

	//system("clear");
}

void change_angle_mode(char *s)
{
	//printf("[Debug] get string %s\n", s);
	if (strstr(s, "deg") == s) angle_mode = MODE_DEG;
	else if (strstr(s, "rad") == s) angle_mode = MODE_RAD;
	else if (strstr(s, "gra") == s) angle_mode = MODE_GRA;
	else raise_error(4, __func__);
}

void base_conversion(char *s)
{
	char input[30];
	int from, to;
	printf("Origin number: ");
	scanf("%30s", input);
	printf("From base: ");
	scanf("%d", &from);
	printf("To base: ");
	scanf("%d", &to);

	if (from == to && from == 10) puts("I know you are using \"Base 10\"!");

	// convert to decimal
	int decimal = 0;
	for (int i = 0; i < strlen(input); i++)
	{
		if (input[i] >= '0' && (input[i] < '0' + from))
			decimal += (input[i] - '0') * pow(from, strlen(input) - i - 1);
		else {
			raise_error(5, __func__);
		}
	}

	// generate output string
	char output[33];
	char *pc = &output[32];
	*(pc--) = 0;
	*pc = '0';
	do {
		*pc-- = (char)(decimal % to + '0');
		decimal /= to;
	} while (decimal > 0);

	printf("Result: %s\n", pc + 1);
}

struct command_s {
	char command[20];
	void (*eval)(char *s);
} commands[]={
	{"version", print_version},
	{"quit", quit},
	{"exit", quit},
	{"help", help},
	{"baseconv", base_conversion},
	{"about", print_about},
	{"clear", clear_screen},
	{"mode", change_angle_mode}
};

int iscommand(char *s)
{
    if ( DEBUG ) printf("[Debug] processing command '%s'\n", s);
	while (*s == '*' || *s == '(') s++;	// get rid of the annoying wrongly-added *'s
	char temp[20];
	sscanf(s, "%20s", temp);
    for (int i = 0; i < strlen(temp); i++)
        if (temp[i] == ')') {temp[i] = 0; break;}
	for (int i = 0; i<sizeof commands/sizeof commands[0]; ++i) {
		if(strcmp(temp, commands[i].command) == 0) {
			(*(commands[i].eval))(s + strlen(commands[i].command) + 1);
			return 1;
		}
	}
	return 0;
}
