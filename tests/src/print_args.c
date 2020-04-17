#include <unistd.h>

static char linebreak = '\n';

void print_int(unsigned long i)
{
	int len = 0;
	for (unsigned long t = i; t != 0; t/=10, ++len);
	char buf[len];
	for (int j = 0; j < len; ++j) {
		buf[len - j - 1] = i % 10 + '0';
		i /= 10;
	}
	write(1, buf, len);
	write(1, &linebreak, 1);
}

void print_str(char *s)
{
	int l;
	for (l = 0; s[l]; ++l);
	write(1, s, l);
	write(1, &linebreak, 1);
}

int main(int argc, char *argv[], char *envp[])
{
	print_int((unsigned long)&argc);
	print_int((unsigned long)argv);
	print_int((unsigned long)envp);

	print_int(argc);
	write(1, "\nargv:\n", 7);
	for (; *argv; ++argv) {
		print_int((unsigned long)*argv);
		print_str(*argv);
	}
	write(1, "\nenvp:\n", 7);
	for (; *envp; ++envp) {
		print_int((unsigned long)*envp);
		print_str(*envp);
	}
}
