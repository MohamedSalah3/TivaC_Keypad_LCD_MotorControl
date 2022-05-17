 #ifndef _STRINGS_H_
 #define _STRINGS_H_
 
// itoa:  convert n to characters in s.
void itoa(int n, char s[]);

// reverse:  reverse string s in place.
void reverse(char s[]);

// strlen: counts the number of s. 
int strlen(const char* s);

// strncpy:  copy the source string into the destination not exceeding n characters. 
char* strncpy(char *dst, const char *src, int n);

// isspace:  return true if the character is whitespace. 
int isspace(char c);

// isdigit:  return true if the character is a digit. 
int isdigit(char c);

// atoi:  convert string s to integer 
int atoi(const char *s);
#endif
