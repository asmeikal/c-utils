//
//  lineParser.c
//  openCL
//
//  Created by Michele Laurenti on 21/12/15.
//
//

#include "lineParser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#include <Debug.h>
#include <StringUtils.h>

#define STRING_SIZE	256
#define LINES_DEFAULT	16

#define DEBUG		"lineParser"

/**
 * @function parseLines
 * @param file
 * The name of the file to be parsed.
 */
char **parseLines(const char * const file)
{
	const char * const fname = "parseLines";
	FILE *fd;
	char *ret, **lines, **lines_tmp;
	char line_tmp[STRING_SIZE] = {0};
	size_t n_lines = 0, max_lines = LINES_DEFAULT;

	Debug_assert(DEBUG, NULL != file, "%s: NULL == file.\n", fname);

	/* open file */
	fd = fopen(file, "r");
	if (NULL == fd) {
		Debug_out(DEBUG, "%s: '%s': %s.\n", fname, file, strerror(errno));
		goto error;
	}

	/* allocs */
	lines = calloc(max_lines, sizeof(char *));
	if (NULL == lines) {
		Debug_out(DEBUG, "%s: calloc failed.\n", fname);
		goto clean1;
	}

	/* parse file */
	do {
		if (n_lines == max_lines) {
			max_lines = max_lines * 2;
			lines_tmp = lines;
			lines = calloc(max_lines, sizeof(char *));
			if (NULL == lines) {
				Debug_out(DEBUG, "%s: calloc failed.\n", fname);
				lines = lines_tmp;
				goto clean2;
			}
			memcpy(lines, lines_tmp, n_lines * sizeof(char *));
			free(lines_tmp);
		}
		ret = fgets(line_tmp, STRING_SIZE, fd);
		if (NULL != ret) {
			if (!StringUtils_endsWith(line_tmp, "\n")) {
				Debug_out(DEBUG, "%s: line %ld is too long.\n", fname, n_lines+1);
				goto clean2;
			}
			lines[n_lines] = StringUtils_clone(line_tmp);
			if (NULL == lines[n_lines]) {
				Debug_out(DEBUG, "%s: StringUtils_clone failed.\n", fname);
				goto clean2;
			}
		} else {
			lines[n_lines] = NULL;
		}
		++n_lines;
	} while (NULL != ret);

	/* squeeze vector of lines */
	lines_tmp = lines;
	lines = calloc(n_lines, sizeof(char *));
	if (NULL == lines) {
		Debug_out(DEBUG, "%s: calloc failed.\n", fname);
		lines = lines_tmp;
		goto clean2;
	}
	memcpy(lines, lines_tmp, n_lines * sizeof(char *));
	free(lines_tmp);

	fclose(fd);
	return lines;

	/* errors */
clean2:	freeLines(lines);
clean1:	fclose(fd);
error:	return NULL;
}

/**
 * @function freeLines
 */
void freeLines(char **lines)
{
	if (NULL == lines) {
		return;
	}
	char **start = lines;
	while (NULL != *lines) {
		free(*lines);
		++lines;
	}
	free(start);
}

/**
 * @function printLines
 */
void printLines(char **lines)
{
	if (NULL == lines) {
		return;
	}
	int i = 0, j;
	j = ((int) log10(countLines(lines))) + 2;
	printf("\n");
	while (NULL != *lines) {
		++i;
		printf("%-*d|\t%s", j, i, *lines);
		++lines;
	}
	printf("\n");
}

/**
 * @function countLines
 */
int countLines(char **lines)
{
	if (NULL == lines) {
		return 0;
	}
	int c = 0;
	while(NULL != *lines) {
		++c;
		++lines;
	}
	return c;
}

