#include "../woodpacker.h"

unsigned char key[KEY_MAXLEN] = {0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f};

void	fatal_error(char *s)
{
	if (!s)
		s = "undefined error";
	char *more_info = "no additional information";
	if (errno != 0)
		more_info = strerror(errno);
	dprintf(2, "ERROR (woody_woodpacker): %s (%s)\n", s, more_info);
	exit(EXIT_FAILURE);
}

struct s_file
{
	size_t siz;
	void *dat;
};

static size_t get_file_size(int fd)
{
	struct stat st;
	if (syscall(SYS_fstat, fd, &st) == -1)
		fatal_error("could not stat requested file");
	return (st.st_size);

}

/** safely open an elf file */
static struct s_file open_elf(char *fname)
{
	struct s_file f;
	int fd;

	fd = open(fname, O_RDONLY);
	if (fd == -1)
		fatal_error("Could not open file");
	f.siz = get_file_size(fd);
	if (f.siz < sizeof(Elf64_Ehdr))
		fatal_error("File is too small");
	f.dat = mmap(NULL, f.siz, PROT_READ, MAP_PRIVATE, fd, 0);
	if (f.dat == MAP_FAILED)
		fatal_error("Could not map file");
	if ((close(fd)) == -1)
		fatal_error(NULL);
	Elf64_Ehdr *h = (Elf64_Ehdr *)f.dat;
	if (h->e_ident[1] != 'E' || h->e_ident[2] != 'L' || h->e_ident[3] != 'F')
		fatal_error("File is not elf");
	if (h->e_type != ET_EXEC && h->e_type != ET_DYN)
		fatal_error("Only handles EXEC & DYN elf files");
	if (h->e_ident[EI_CLASS] != 2)
		fatal_error("Architecture not suported. x86_64 only\n");

	return (f);
}

int		main(int argc, char **argv)
{
	if (argc != 2)
		fatal_error("Usage : ./woody_woodpacker <file>\n");

	struct s_file f = open_elf(argv[1]);
	handle_elf64(f.dat, f.siz);

	printf("key_value: ");
	for (int i = 0; i < KEY_MAXLEN; i++)
		printf("%02X", key[i]);
	printf("\n");
	return (EXIT_SUCCESS);
}
