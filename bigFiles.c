#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <libgen.h>

int main(int argc, char *argv[])
{

	int c;
	char *counter = NULL;

	int pipe1[2] = {-1, -1};
	int pipe2[2] = {-1, -1};
	int pipe3[2] = {-1, -1};

	pid_t child1 = -1;
	pid_t child2 = -1;
	pid_t child3 = -1;

	while ((c = getopt(argc, argv, "n:")) != -1)
	{
		switch (c)
		{
		case 'n':
			counter = optarg;
			break;
		default:
			// oops
			break;
		}

	}

	


	pipe(pipe1);
	pipe(pipe2);
	pipe(pipe3);

	child1 = fork();

	switch (child1)
	{
	case -1:
		perror("Failed");
		break;

	case 0:

		dup2(pipe1[STDOUT_FILENO], STDOUT_FILENO);
		close(pipe1[STDIN_FILENO]);
		close(pipe1[STDOUT_FILENO]);
		close(pipe2[STDIN_FILENO]);
		close(pipe2[STDOUT_FILENO]);		
		close(pipe3[STDIN_FILENO]);
		close(pipe3[STDOUT_FILENO]);
		execlp("ls", "ls", (char *)NULL);
		perror("pithy message 1");
		_exit(3);

	default:
		break;
	}

	child2 = fork();

	switch (child2)
	{
	case -1:
		perror("Failed");
		break;

	case 0:

		/* writing parents STDIN into child*/

		dup2(pipe1[STDIN_FILENO], STDIN_FILENO);
		dup2(pipe2[STDOUT_FILENO], STDOUT_FILENO);

		close(pipe1[STDIN_FILENO]);
		close(pipe1[STDOUT_FILENO]);
		close(pipe2[STDIN_FILENO]);
		close(pipe2[STDOUT_FILENO]);
		close(pipe3[STDIN_FILENO]);
		close(pipe3[STDOUT_FILENO]);
		execlp("xargs", "xargs", "du", "-s", (char *)NULL);
		perror("pithy message 2");
		_exit(3);

		default:
			break;
		}

		child3 = fork();

		switch (child3)
		{
		case -1:
			perror("Failed");
			break;

		case 0:

			/* writing parents STDIN into child*/

			dup2(pipe2[STDIN_FILENO], STDIN_FILENO);
			dup2(pipe3[STDOUT_FILENO], STDOUT_FILENO);
			close(pipe1[STDIN_FILENO]);
			close(pipe1[STDOUT_FILENO]);
			close(pipe2[STDIN_FILENO]);
			close(pipe2[STDOUT_FILENO]);
			close(pipe3[STDIN_FILENO]);
			close(pipe3[STDOUT_FILENO]);
			execlp("sort", "sort", "-nr", (char *)NULL);
			perror("pithy message 3");
			_exit(3);

		default:
			break;
		}

		dup2(pipe3[STDIN_FILENO], STDIN_FILENO);

		close(pipe1[STDIN_FILENO]);
		close(pipe1[STDOUT_FILENO]);
		close(pipe2[STDIN_FILENO]);
		close(pipe2[STDOUT_FILENO]);
		close(pipe3[STDIN_FILENO]);
		close(pipe3[STDOUT_FILENO]);

		if (counter == NULL)
		{
			execlp("head", "head", (char *)NULL);
			perror("pithy message 4");
			_exit(3);
		}
		else
		{
			execlp("head", "head", "-n", counter, (char *)NULL);
		}
		perror("failed");
		perror("pithy message 5");
		_exit(3);
		return 0;
	}

