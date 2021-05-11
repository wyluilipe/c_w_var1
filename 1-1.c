#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

int cmprstr(const char *str1, const char *str2)
{
	if (strlen(str1) != strlen(str2))
		return 0;
	for (int i = 0; i < strlen(str1); i++)
		if (str1[i] != str2[i])
			return 0;
	return 1;
}

struct list {
	int i;
	int pid;
	struct list *next;
};

struct list *addToList(struct list *head, int pid, int i)
{
	struct list *tmp = malloc(sizeof(*tmp));
	tmp->pid = pid;
	tmp->i = i;
	tmp->next = head;
	return tmp;
}

int findNextCmd(char **arr, int i)
{
	int k;
	for (k = i; arr[k] != NULL; k++);
	return k + 1;
}

int findName(struct list *head, int pid)
{
	int key = 0;
	struct list *tmp = head;
	while (tmp != NULL) {
		if (tmp->pid == pid)
			key = tmp->i;
		tmp = tmp->next;
	}
	return key;
}

void start(int argc, char **arr)
{
	struct list *head = NULL;
	int res, status;
	int tres, tstatus;
	int pid;
	int name;
	int i = 1;
	while (i < argc) {
		if ((pid = fork()) == 0) {
			execvp(arr[i], &arr[i]);
			perror(arr[i]);
			return;
		} else
			head = addToList(head, pid, i);
		i = findNextCmd(arr, i);
	}
	res = wait4(-1, &status, WNOHANG, NULL);
	do {
		tres = wait4(-1, &tstatus, WNOHANG, NULL);
		if (tres > 0) {
			res = tres;
			status = tstatus;
		}
	} while (tres > -1);
	name = findName(head, res);
	printf("last cmd = %s signo = %d\n", arr[name], status);
}

int main(int argc, char **argv)
{
	char **arr = malloc((argc + 1) * sizeof(*arr));
	for (int i = 0; i < argc; i++)
		if (cmprstr(argv[i], "---"))
			arr[i] = NULL;
		else
			arr[i] = argv[i];
	arr[argc] = NULL;
	start(argc, arr);
	return 0;
}