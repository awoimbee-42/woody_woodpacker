#include <syscall.h>
#include <unistd.h>

static const char linebreak = '\n';

static void print_int(unsigned long i, char sep)
{
	char buf[20];
	buf[19] = sep;
	register size_t j = 0;
	while (i || !j) {
		buf[18 - j] = i % 10 + '0';
		++j;
		i /= 10;
	}
	++j;
	syscall(SYS_write, 1, &buf[20 - j], j);
}

static void print_str(char *s)
{
	register size_t l = 0;
	for (; s[l]; ++l);
	s[l++] = '\n';
	syscall(SYS_write, 1, s, l);
}

int main(int argc, char *argv[], char *envp[])
{
	// print_int((unsigned long)&argc, '\n');
	// print_int((unsigned long)argv, '\n');
	// print_int((unsigned long)envp, '\n');

	print_int(argc, '\n');
	syscall(SYS_write, 1, "\nargv:\n", 7);
	for (; *argv; ++argv) {
		// print_int((unsigned long)*argv, '\t');
		print_str(*argv);
	}
	syscall(SYS_write, 1, "\nenvp:\n", 7);
	for (; *envp; ++envp) {
		// print_int((unsigned long)*envp, '\t');
		print_str(*envp);
	}
}
