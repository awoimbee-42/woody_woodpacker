#include <unistd.h>

int main(void)
{
	write(1, "Hello world\n", 6);
	return (0);
}
