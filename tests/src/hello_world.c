#include <syscall.h>
#include <unistd.h>

int main(void)
{
	syscall(SYS_write, 1, "Hello world\n", 6);
	return (0);
}
