#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void version()
{
	printf("Version : 0.99\n");
	exit(0);
}

void help()
{
	printf(
			"Usage: SEND_SIDB -f [FILE]\n"
			"       SEND_SIDB -f [FILE] -c [COUNT] -s [SIZE]\n"
			" -h         help\n"
			" -v         Version\n"
			" -f [FILE]  input file\n"
			" -c [COUNT] count\n"
			" -s [SIZE]  size\n"
	);
	exit(0);
}

int main(int argc, char *argv[])
{
	int			opt;
	int			opt_ok = 0;
	int			opt_f = 0;
	int			opt_c = 0;
	int			opt_s = 0;
	int			len = 0;
	int			cnt = 0;
	int			size = 0;
	char		path[BUFSIZ];

	while((opt = getopt(argc, argv, "hvf:c:s:")) != -1)
	{
		switch(opt)
		{
			case 'h':
				help();
				break;
			case 'v':
				version();
				break;
			case 'f':
				len = strlen(optarg);
				memcpy(path, optarg, len);
				opt_f = 1;
				opt_ok = 1;
				break;
			case 'c':
				cnt = atoi(optarg);
				opt_c = 1;
				break;
			case 's':
				size = atoi(optarg);
				opt_s = 1;
				break;
		}
	}

	if(opt_ok != 1)
	{
		help();
		exit(0);
	}

	if(opt_f == 1) printf("INPUT FILE=%.*s\n", len, path);
	if(opt_c == 1) printf("INPUT COUNT=%d\n", cnt);
	if(opt_s == 1) printf("INPUT SIZE=%d\n", size);

	printf("SEND FILE=%.*s COUNT=%d SIZE=%d\n", len, path, cnt, size);

	return 0;
}
