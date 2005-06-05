/* $Id$ */

#include <sys/types.h>

#include <err.h>
#include <grp.h>
#include <limits.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define _PATH_BSHELL "/bin/sh"

void usage(void);

int loginenv;

int
main(int argc, char *argv[])
{
	const char *errstr;
	struct passwd *pwd;
	struct group *grp;
	gid_t gid;
	int ch;

	while ((ch = getopt(argc, argv, "l")) != -1)
		switch (ch) {
		case 'l':
			loginenv = 1;
			break;
		default:
			usage();
			/* NOTREACHED */
		}
	argc -= optind;
	argv += optind;
	if (argc != 0 && argc != 1)
		usage();
	if ((pwd = getpwuid(getuid())) == NULL)
		err(1, "getpwuid");
	if (argc == 1) {
		if ((grp = getgrnam(argv[0])) == NULL) {
			gid = strtonum(argv[0], 0, GID_MAX, &errstr);
			if (errstr != NULL)
				err(1, "%s", argv[0]);
		}
		gid = grp->gr_gid;
	} else {
		gid = pwd->pw_gid;
	}
	if (setgid(gid) == -1)
		warn("setgid %d", gid);
	execl(pwd->pw_shell, pwd->pw_shell, NULL);
	err(1, "%s", pwd->pw_shell);
	/* NOTREACHED */
}

void
usage(void)
{
	extern char *__progname;

	fprintf(stderr, "usage: %s [-l] [group]\n", __progname);
	exit(1);
}
