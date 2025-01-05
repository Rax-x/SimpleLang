#ifndef _STRING_VIEW_H_
#define _STRING_VIEW_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
    
#define STRING_VIEW_FORMAT "%.*s"
#define STRING_VIEW_ARG(sv) (sv).count, (sv).data

#define STRING_VIEW_EMPTY (new_string_view("", 0))

#define STRING_VIEW_NPOS ((size_t)-1)

typedef struct {
    const char* data;
    size_t count;
} string_view_t;

/**
 * @brief Creates a new string view from a character array and length.
 *
 * This function constructs a string view object that references the specified character
 * array and its length. The character array is not copied; the string view simply
 * provides a view into the existing data.
 *
 * @param data A pointer to the character array.
 * @param count The number of characters in the character array.
 * @return A new string view object.
 */
string_view_t new_string_view(const char* data, size_t count);


/**
 * @brief Creates a new string view from a null-terminated character string.
 *
 * This function constructs a string view object that references the specified null-terminated
 * character string. The string is assumed to be null-terminated, and the length is calculated
 * automatically.
 *
 * @param data A pointer to the null-terminated character string.
 * @return A new string view object.
 */
string_view_t new_string_view_from_cstr(const char* data);
    
/**
 * @brief Returns a pointer to the underlying character data of the string view.
 *
 * This function provides a pointer to the beginning of the character array that
 * backs the string view. The returned pointer is constant and should not be modified.
 * If the string view is empty returns `NULL`.
 *
 * @param sv The string view to access.
 * @return A constant pointer to the character data of the string view.
 */
const char* string_view_data(string_view_t sv);

/**
 * @brief Returns the length of the string view.
 *
 * This function returns the number of characters in the provided string view.
 *
 * @param sv The string view to measure.
 * @return The length of the string view.
 */
size_t string_view_size(string_view_t sv);

/**
 * @brief Returns the character at the specified index in the string view.
 *
 * This function provides access to the character located at the given index within the
 * provided string view. If the index is out of bounds (i.e., less than 0 or greater than or
 * equal to the length of the string view), the function returns the null character.
 * If the string view is empty, the behavior is undefined.
 *
 * @param sv The string view to access.
 * @param index The index of the character to retrieve.
 * @return The character at the specified index.
 */
char string_view_at(string_view_t sv, size_t index);

/**
 * @breif Returns the first character of the string view.
 *
 * This function returns the character at the beginning of the provided string view.
 * If the string view is empty, the behavior is undefined.
 *
 * @param sv The string view to access.
 * @return The first character of the string view.
 */
char string_view_front(string_view_t sv);

/**
 * @breif Returns the last character of the string view.
 *
 * This function returns the character at the end of the provided string view.
 * If the string view is empty, the behavior is undefined.
 *
 * @param sv The string view to access.
 * @return The last character of the string view.
 */
char string_view_back(string_view_t sv);

/**
 * @brief Checks if the string view is empty.
 *
 * This function returns `true` if the provided string view has a length of zero,
 * indicating that it contains no characters. Otherwise, it returns `false`.
 *
 * @param sv The string view to check.
 * @return `true` if the string view is empty, `false` otherwise.
 */
bool string_view_is_empty(string_view_t sv);

/**
 * @brief Trims whitespace characters from the beginning of a string view.
 *
 * This function modifies the provided string view in-place by removing leading whitespace characters.
 * Whitespace characters are defined as space, tab, newline, carriage return, form feed, and vertical tab.
 *
 * @param sv A pointer to the string view to be trimmed.
 */
void string_view_trim_left(string_view_t *sv);

/**
 * @brief Trims whitespace characters from the end of a string view.
 *
 * This function modifies the provided string view in-place by removing trailing whitespace characters.
 * Whitespace characters are defined as space, tab, newline, carriage return, form feed, and vertical tab.
 *
 * @param sv A pointer to the string view to be trimmed.
 */
void string_view_trim_right(string_view_t *sv);

/**
 * @brief Trims whitespace characters from both ends of a string view.
 *
 * This function modifies the provided string view in-place by removing leading and trailing whitespace characters.
 * Whitespace characters are defined as space, tab, newline, carriage return, form feed, and vertical tab.
 *
 * @param sv A pointer to the string view to be trimmed.
 */
void string_view_trim(string_view_t *sv);

/**
 * @brief Removes a prefix of specified length from a string view.
 *
 * This function modifies the provided string view in-place by removing the first `n` characters.
 * If `n` is greater than or equal to the length of the string view, the resulting string view
 * will be empty.
 *
 * @param sv A pointer to the string view to be modified.
 * @param n The number of characters to remove from the beginning of the string view.
 */
void string_view_remove_prefix(string_view_t *sv, size_t n);

/**
 * @brief Removes a suffix of specified length from a string view.
 *
 * This function modifies the provided string view in-place by removing the last `n` characters.
 * If `n` is greater than or equal to the length of the string view, the resulting string view
 * will be empty.
 *
 * @param sv A pointer to the string view to be modified.
 * @param n The number of characters to remove from the end of the string view.
 */
void string_view_remove_suffix(string_view_t *sv, size_t n);

/**
 * @brief Swaps the contents of two string views.
 *
 * This function efficiently exchanges the data and length of two string view objects.
 * No memory allocation or copying is performed.
 *
 * @param sv1 A pointer to the first string view.
 * @param sv2 A pointer to the second string view.
 */
void string_view_swap(string_view_t* sv1, string_view_t* sv2);

/**
 * @brief Copies a portion of a string view into a character array.
 *
 * This function copies at most `count` characters from the specified string view, starting at index `start`,
 * into the destination character array. If the end of the string view is reached before `count` characters
 * have been copied, the remaining characters in `dest` are left unmodified.
 *
 * @param sv The source string view.
 * @param dest The destination character array.
 * @param count The maximum number of characters to copy.
 * @param start The starting index in the string view.
 */
void string_view_copy(string_view_t sv, char *dest, size_t count, size_t start);

/**
 * @brief Creates a new string view that is a substring of the given string view.
 *
 * This function returns a new string view that represents a substring of the original string view.
 * The substring starts at index `start` and has a maximum length of `count` characters.
 * If `start` is greater than or equal to the length of the original string view, or if `count` is 0,
 * an empty string view is returned.
 *
 * @param sv The original string view.
 * @param start The starting index of the substring.
 * @param count The maximum length of the substring.
 * @return A new string view representing the substring.
 */
string_view_t string_view_substr(string_view_t sv, size_t start, size_t count);

/**
 * @brief Compares two string views lexicographically.
 *
 * This function compares two string views lexicographically and returns an integer value indicating their relative order.
 *
 * * Returns a negative value if `sv1` is less than `sv2`.
 * * Returns 0 if `sv1` is equal to `sv2`.
 * * Returns a positive value if `sv1` is greater than `sv2`.
 *
 * The comparison is based on the numerical values of the corresponding characters in the two string views.
 *
 * @param sv1 The first string view.
 * @param sv2 The second string view.
 * @return An integer indicating the comparison result.
 */
int string_view_compare(string_view_t sv1, string_view_t sv2);

/**
 * @brief Checks if two string views are equal.
 *
 * This function returns `true` if the two string views have the same length and contain the same characters,
 * otherwise it returns `false`.
 *
 * @param sv1 The first string view.
 * @param sv2 The second string view.
 * @return `true` if the string views are equal, `false` otherwise.
 */
bool string_view_equal(string_view_t sv1, string_view_t sv2);

/**
 * @brief Checks if a string view starts with a given prefix.
 *
 * This function returns `true` if the specified string view `sv` starts with the given prefix,
 * otherwise it returns `false`.
 *
 * @param sv The string view to check.
 * @param prefix The prefix to look for.
 * @return `true` if `sv` starts with `prefix`, `false` otherwise.
 */
bool string_view_starts_with(string_view_t sv, string_view_t prefix);

/**
 * @brief Checks if a string view ends with a given suffix.
 *
 * This function returns `true` if the specified string view `sv` ends with the given suffix,
 * otherwise it returns `false`.
 *
 * @param sv The string view to check.
 * @param suffix The suffix to look for.
 * @return `true` if `sv` ends with `suffix`, `false` otherwise.
 */
bool string_view_ends_with(string_view_t sv, string_view_t suffix); 

/**
 * @brief Finds the first occurrence of a character in a string view.
 *
 * This function searches for the first occurrence of the character `c` within the string view `sv`,
 * starting at index `start`. If the character is found, its index is returned; otherwise,
 * `STRING_VIEW_NPOS` is returned to indicate not found.
 *
 * @param sv The string view to search.
 * @param c The character to find.
 * @param start The starting index of the search.
 * @return The index of the first occurrence of `c`, or `STRING_VIEW_NPOS` if not found.
 */
size_t string_view_find_char(string_view_t sv, char c, size_t start);

/**
 * @brief Finds the first occurrence of a substring within a string view.
 *
 * This function searches for the first occurrence of the substring `needle` within the string view `haystack`,
 * starting at index `start`. If the substring is found, the index of its first character is returned;
 * otherwise, `STRING_VIEW_NPOS` is returned to indicate not found.
 *
 * @param haystack The string view to search.
 * @param needle The substring to find.
 * @param start The starting index of the search.
 * @return The index of the first occurrence of `needle`, or `STRING_VIEW_NPOS` if not found.
 */
size_t string_view_find_substring(string_view_t haystack, string_view_t needle, size_t start);

#if defined(__STDC__) && defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L

/**
 * @brief Checks if a string view contains a specified element.
 *
 * This macro provides a generic way to check if a string view contains either a character or a substring.
 * The appropriate function (`string_view_find_char` or `string_view_find_substring`) is selected based on the type of the `needle` argument.
 *
 * @param haystack The string view to search.
 * @param needle The element to find (can be a character or a substring).
 * @return `true` if the element is found within the haystack, `false` otherwise.
 */
#define string_view_contains(haystack, needle)                          \
    (_Generic((needle),                                                 \
              int : string_view_find_char,                              \
              string_view_t : string_view_find_substring)(sv, needle, 0) != STRING_VIEW_NPOS)
#else

/**
 * @brief Checks if a string view contains a specific character.
 *
 * This macro is a convenience wrapper around `string_view_find_char` to determine if a character exists within a string view.
 * It returns `true` if the character is found, otherwise `false`.
 *
 * @param haystack The string view to search.
 * @param needle The character to find.
 * @return `true` if the character is found, `false` otherwise.
 */
#define string_view_contains_char(haystack, needle) \
    (string_view_find_char(haystack, needle, 0) != STRING_VIEW_NPOS)

/**
 * @brief Checks if a string view contains a specific substring.
 *
 * This macro is a convenience wrapper around `string_view_find_substring` to determine if a substring exists within a string view.
 * It returns `true` if the substring is found, otherwise `false`.
 *
 * @param haystack The string view to search.
 * @param needle The substring to find.
 * @return `true` if the substring is found, `false` otherwise.
 */
#define string_view_contains_substring(haystack, needle) \
    (string_view_find_substring(haystack, needle, 0) != STRING_VIEW_NPOS)

#endif

#endif

#ifdef STRING_VIEW_IMPLEMENTATION

#include <ctype.h>
#include <string.h>

inline string_view_t new_string_view(const char* data, size_t size){
    return (string_view_t) {
        .data = data,
        .count = size
    };
}

inline string_view_t new_string_view_from_cstr(const char* data){
    return new_string_view(data, strlen(data));
}

inline char string_view_at(string_view_t sv, size_t index){
    return index < sv.count
        ? sv.data[index]
        : '\0';
}

inline char string_view_front(string_view_t sv){    
    return string_view_at(sv, 0);
}

inline char string_view_back(string_view_t sv){
    return string_view_at(sv, string_view_size(sv) - 1);
}

inline const char* string_view_data(string_view_t sv){
    return sv.data;
}

inline size_t string_view_size(string_view_t sv){
    return sv.count;
}

inline bool string_view_is_empty(string_view_t sv){
    return sv.count == 0;
}

void string_view_trim_left(string_view_t *sv) {
    while(isspace(sv->data[0]) && sv->count > 0){
        sv->data++;
        sv->count--;
    }
}

void string_view_trim_right(string_view_t *sv) {
    while(isspace(string_view_back(*sv)) && sv->count > 0){
        sv->count--;
    }
}

inline void string_view_trim(string_view_t *sv){
    string_view_trim_left(sv);
    string_view_trim_right(sv);
}

void string_view_remove_prefix(string_view_t *sv, size_t n){
    if(n > sv->count){
        *sv = STRING_VIEW_EMPTY;
    }else{
        sv->data += n;
        sv->count -= n;
    }
}

inline void string_view_remove_suffix(string_view_t *sv, size_t n){
    sv->count -= (n > sv->count)
        ? sv->count
        : n;
}

void string_view_swap(string_view_t* sv1, string_view_t* sv2){
    string_view_t tmp = *sv1;
    *sv1 = *sv2;
    *sv2 = tmp;
}

void string_view_copy(string_view_t sv, char *dest, size_t count, size_t start){

    if(dest == NULL || start >= sv.count) return;
    if(sv.count - start < count) {
        count = sv.count - start;
    }

    memcpy(dest, &sv.data[start], count);
}

string_view_t string_view_substr(string_view_t sv, size_t start, size_t count){  
    if(count > (sv.count - start)) {
        count = sv.count - start;
    }

    return (start < sv.count) 
        ? new_string_view(&sv.data[start], count)
        : STRING_VIEW_EMPTY;
}

int string_view_compare(const string_view_t sv1, const string_view_t sv2){
    
    if(sv1.count > sv2.count) return 1;
    if(sv1.count < sv2.count) return -1;
    
    return strncmp(sv1.data, sv2.data, sv1.count);
}

inline bool string_view_equal(string_view_t sv1, string_view_t sv2){
    return string_view_compare(sv1, sv2) == 0;
}

inline bool string_view_starts_with(string_view_t sv, string_view_t prefix){
    if(prefix.count > sv.count) return false;
    return strncmp(sv.data, prefix.data, prefix.count) == 0;
}


bool string_view_ends_with(string_view_t sv, string_view_t suffix){
    
    if(suffix.count > sv.count) return false;

    const size_t pos = sv.count - suffix.count;
    return strncmp(&sv.data[pos], suffix.data, suffix.count) == 0;
}

size_t string_view_find_char(string_view_t sv, char c, size_t start){

    for(size_t i = start; i < sv.count; i++){
        if(sv.data[i] == c) return i;
    }

    return STRING_VIEW_NPOS;
}

size_t string_view_find_substring(string_view_t haystack, string_view_t needle, size_t start){

    if(needle.count > haystack.count) return STRING_VIEW_NPOS;

    const size_t count = haystack.count - needle.count;
    for(size_t i = start; i <= count; i++){
        if(strncmp(&haystack.data[i], needle.data, needle.count) == 0) return i;
    }

    return STRING_VIEW_NPOS;
}


#endif
