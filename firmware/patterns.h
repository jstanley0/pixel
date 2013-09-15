
// called about 120 times per second; sets new RGB values
typedef void (*PATTERN_FUNC)(unsigned char *colors);

#define PATTERN_COUNT 2
extern PATTERN_FUNC PATTERNS[];
