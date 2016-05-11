#include "str_utils.h"
#include "matcher.h"
#include "malloc_safe.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

// Lot of this stuff is actually not needed anymore,
// it was written for the ESP AT firmware, which is no longer used.

/**
 * Escape a char.
 * @returns what to put after backslash, or '\0' for no escape.
 */
static char escape_char(char c)
{
	switch (c) {
		case '\r': return 'r';
		case '\n': return 'n';
		case '\t': return 't';
		case '\\': return '\\';
		default: return 0;
	}
}


/**
 * Escape string in place
 */
void str_escape_ip(char * buf, size_t buf_len)
{
	size_t i = 0;

	// string length (updated when escapes are performed)
	size_t slen = strlen(buf);

	for (; i < buf_len - 1 && buf[i] != 0; i++) {
		char replace = escape_char(buf[i]);

		// Escape, shift trailing chars
		if (replace != 0) {
			if (i >= buf_len - 2) {
				break; // discard the char, escape wouldn't fit.
			}

			// (could be faster if moved starting at the end)

			char m = buf[i + 1]; // remember next char

			buf[i] = '\\';
			buf[i + 1] = replace;

			slen++; // account for the added backslash

			// shift trailing chars
			for (size_t j = i + 2; j <= slen; j++) {
				char n = buf[j];
				buf[j] = m;
				m = n;
			}

			i++; // skip the insterted slash
		}
	}

	buf[i] = 0; // add terminator (in case end of string was reached)
}


void str_escape(char *dest, const char *src, size_t dest_len)
{
	size_t di = 0, si = 0;

	for (; src[si] != 0 && di < dest_len - 1; si++) {
		char orig = src[si];
		char replace = escape_char(orig);

		if (replace == 0) {
			dest[di++] = orig;
		} else {
			if (di >= dest_len - 2) {
				break; // out of space
			}

			dest[di++] = '\\';
			dest[di++] = replace;
		}
	}

	dest[di] = 0; // append terminator
}



int32_t strpos(const char *haystack, const char *needle)
{
	const char *p = strstr(haystack, needle);
	if (p) return (p - haystack);
	return -1;   // Not found = -1.
}


int32_t strpos_upto(const char *haystack, const char *needle, size_t limit)
{
	if (limit <= 0) return strpos(haystack, needle);

	matcher_t m = {needle, 0};
	char c;

	for (size_t i = 0; i < limit; i++, haystack++) {
		c = *haystack;
		if (c == 0) break;

		if (matcher_test(&m, (uint8_t)c)) {
			return i - strlen(needle) + 1; // match occured on the last needle char
		}
	}

	return -1;
}


int32_t strpos_upto_match(const char *haystack, const char *needle, const char *endmatch)
{
	if (endmatch == NULL) return strpos(haystack, needle);

	matcher_t matcher_needle = {needle, 0};
	matcher_t matcher_end = {endmatch, 0};
	char c;

	for (int i = 0;; i++, haystack++) {
		c = *haystack;
		if (c == 0) break;

		// match
		if (matcher_test(&matcher_needle, (uint8_t)c)) {
			return i - strlen(needle) + 1; // match occured on the last needle char
		}

		// end
		if (matcher_test(&matcher_end, (uint8_t)c)) {
			return -1;
		}
	}

	return -1;
}

size_t str_copy(char * dest, const char *src)
{
	char c;
	size_t i = 0;
	while ((c = *src++) != 0) {
		*dest++ = c;
		i++;
	}
	return i;
}


/**
 * Decode URL-encoded string in place.
 */
void urldecode_ip(char *str)
{
	unsigned int x;

	for (size_t i = 0; str[i] != 0; i++) {
		char c = str[i];
		if (c == '+') {
			str[i] = ' ';
		} else if (c == '%') {
			// decode the byte
			sscanf(&str[i + 1], "%02x", &x);
			str[i] = (char)x;

			// shift following chars
			for (size_t a = i + 3, b = i + 1;; a++, b++) {
				str[b] = str[a]; // move
				if (str[a] == 0) break;
			}
		}
	}
}


/**
 * url-decode string, put output in a buffer.
 */
void urldecode(char *dest, const char *src)
{
	unsigned int x;
	size_t si = 0, di = 0;

	for (; src[si] != 0; si++) {
		char c = src[si];
		if (c == '+') {
			dest[di++] = ' ';
		} else if (c == '%') {
			// decode the byte
			sscanf(&src[si + 1], "%02x", &x);
			dest[di++] = (char)x;

			si += 2;
		} else {
			dest[di++] = c;
		}
	}

	// add terminator
	dest[di] = 0;
}



/**
 * url-decode string, put output in a buffer.
 * Limit operation to N chars in input string
 */
void urldecode_n(char *dest, const char *src, size_t count)
{
	unsigned int x;
	size_t si = 0, di = 0;

	for (; src[si] != 0 && si < count; si++) {
		char c = src[si];
		if (c == '+') {
			dest[di++] = ' ';
		} else if (c == '%') {
			// decode the byte
			sscanf(&src[si + 1], "%02x", &x);
			dest[di++] = (char)x;

			si += 2;
		} else {
			dest[di++] = c;
		}
	}

	// add terminator
	dest[di] = 0;
}


bool get_query_value(char *buffer, const char *querystring, const char *key, size_t buf_len)
{
	bool retval;

	size_t qs_len = strlen(querystring);

	char *ptrn = malloc_s(strlen(key) + 3); // &key=\0
	sprintf(ptrn, "&%s=", key);
	matcher_t m = {ptrn, 1}; // pretend ampersand was already matched

	for (size_t i = 0; i < qs_len; i++) {
		char c = querystring[i];
		if (matcher_test(&m, (uint8_t)c)) {
			// found the match
			i++; // advance past the equals sign

			size_t seg_end = i;
			while (seg_end < qs_len && querystring[seg_end] != '&') {
				seg_end++;
			}

			if (seg_end - i > buf_len) seg_end = i + buf_len;

			if (seg_end == i) {
				buffer[0] = 0; // strncpy behaves strange with length 0
			} else {
				urldecode_n(buffer, querystring + i, seg_end - i);
			}

			retval = true;
			goto done;
		}
	}

	// not found
	retval = false;
done:
	free(ptrn);
	return retval;
}
