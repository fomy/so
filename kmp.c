#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

static int *kmp_table = NULL;

void build_kmp_table(char *word)
{
	kmp_table = malloc(strlen(word) * sizeof(int));
	kmp_table[0] = -1;
	kmp_table[1] = 0;

	int pos = 2;
	int cnd = 0;// current matched substring

	printf("[ -1 0 ");
	while (pos < strlen(word)) {
		if (word[pos-1] == word[cnd]) {
			cnd++;
			kmp_table[pos] = cnd;	
			pos++;
			printf("%d ", cnd);
		} else if (cnd != 0) {
			cnd = kmp_table[cnd];
		} else {
			kmp_table[pos] = 0;
			pos++;
			printf("0 ");
		}
	}
	printf("]\n");
}

/*
 * Searching word in string;
 * Return the index of the first matching.
 * Return -1 if fail
 */
int kmp_search(char *string, char *word)
{
	int pos = 0; // the beginning of the current match in string 
	int i = 0; // the position of the current char in word

	while (pos + i < strlen(string)) {
		if (string[pos + i] == word[i]) {
			i++;
			if (i == strlen(word))
				return pos;
		} else {
			if (i == 0) {
				pos++;
			} else {
				pos += i - kmp_table[i];
				i = kmp_table[i];
			}
		}
	}
	return -1;
}

int main(int argc, char **argv)
{
	assert(argc == 3);
	build_kmp_table(argv[2]);
	int pos = kmp_search(argv[1], argv[2]);
	printf("pos = %d\n", pos);
	return 0;
}
