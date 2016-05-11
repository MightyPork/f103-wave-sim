#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define streq(a, b) (strcmp((a), (b)) == 0)
#define streqi(a, b) (strcasecmp((a), (b)) == 0)

/**
 * Escape string, storing result in a buffer.
 */
void str_escape(char *dest, const char *src, size_t dest_len);


/**
 * Escape special chars in a string, IN PLACE.
 *
 * If string is too long after escaping, last chars are dropped.
 *
 * @param buf the buffer, containing 0-terminated string.
 * @param buflen buffer length
 */
void str_escape_ip(char *buf, size_t buf_len);


/**
 * Get position of needle in a haystack.
 * -1 if not found.
 */
int32_t strpos(const char *haystack, const char *needle);


/**
 * Find substring position, ending at index 'limit'.
 * Limit <= 0 means no limit.
 * Returns index of the first character of needle in haystack.
 */
int32_t strpos_upto(const char *haystack, const char *needle, size_t limit);


/**
 * Find substring position, ending when endmatch is encountered. (Substring within endmatch *can* be reported).
 * Returns index of the first character of needle in haystack.
 */
int32_t strpos_upto_match(const char *haystack, const char *needle, const char *endmatch);


/**
 * Like sprintf, except without formatting
 */
size_t str_copy(char * dest, const char *src);


/**
 * Decode url-encoded string, store result in dest.
 */
void urldecode(char *dest, const char *src);


/**
 * Decode url-encoded string in place.
 */
void urldecode_ip(char *str);


/**
 * Retrieve & url-decode a query string value by name.
 *
 * @param buffer - target buffer
 * @param querystring - string (foo=bar&baz=moi)
 * @param key - key to retrieve
 * @param buf_len - length of the target buffer
 * @return true if found.
 */
bool get_query_value(char *buffer, const char *querystring, const char *key, size_t buf_len);
