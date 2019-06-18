#ifndef HELPERS_H
#define HELPERS_H

extern AXError _AXUIElementGetWindow(AXUIElementRef ref, uint32_t *wid);

struct rgba_color
{
    bool is_valid;
    uint32_t p;
    float r;
    float g;
    float b;
    float a;
};

static struct rgba_color
rgba_color_from_hex(uint32_t color)
{
    struct rgba_color result;
    result.is_valid = true;
    result.p = color;
    result.r = ((color >> 16) & 0xff) / 255.0;
    result.g = ((color >> 8) & 0xff) / 255.0;
    result.b = ((color >> 0) & 0xff) / 255.0;
    result.a = ((color >> 24) & 0xff) / 255.0;
    return result;
}

static struct rgba_color
rgba_color_dim(struct rgba_color color)
{
    struct rgba_color result;
    result.is_valid = true;
    result.p = color.p;
    result.r = 0.25f*color.r;
    result.g = 0.25f*color.g;
    result.b = 0.25f*color.b;
    result.a = 0.25f*color.a;
    return result;
}

static inline bool is_root(void)
{
    return getuid() == 0 || geteuid() == 0;
}

static inline bool string_equals(const char *a, const char *b)
{
    return a && b && strcmp(a, b) == 0;
}

static inline char *cfstring_copy(CFStringRef string)
{
    CFIndex num_bytes = CFStringGetMaximumSizeForEncoding(CFStringGetLength(string), kCFStringEncodingUTF8);
    char *result = malloc(num_bytes + 1);
    if (!result) return NULL;

    if (!CFStringGetCString(string, result, num_bytes + 1, kCFStringEncodingUTF8)) {
        free(result);
        result = NULL;
    }

    return result;
}

static inline char *string_copy(char *s)
{
    int length = strlen(s);
    char *result = malloc(length + 1);
    if (!result) return NULL;

    memcpy(result, s, length);
    result[length] = '\0';
    return result;
}

static void fork_exec(char *command, char *arg1, char *arg2)
{
    int pid = fork();
    if (pid != 0) return;

    char *exec[] = { "/usr/bin/env", "sh", command, arg1, arg2, NULL};
    printf("execcing command '%s' with arg1: '%s' and arg2: '%s'\n", command, arg1, arg2);
    exit(execvp(exec[0], exec));
}

static bool fork_exec_wait(char *command)
{
    int pid = fork();
    if (pid == -1) {
        return false;
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
    } else {
        char *exec[] = { "/usr/bin/env", "sh", "-c", command, NULL};
        exit(execvp(exec[0], exec));
    }

    return true;
}

static bool ax_privilege(void)
{
    const void *keys[] = { kAXTrustedCheckOptionPrompt };
    const void *values[] = { kCFBooleanTrue };
    CFDictionaryRef options = CFDictionaryCreate(NULL, keys, values, array_count(keys), &kCFCopyStringDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    bool result = AXIsProcessTrustedWithOptions(options);
    CFRelease(options);
    return result;
}

static inline uint32_t ax_window_id(AXUIElementRef ref)
{
    uint32_t wid = 0;
    _AXUIElementGetWindow(ref, &wid);
    return wid;
}

static inline pid_t ax_window_pid(AXUIElementRef ref)
{
    return *(pid_t *)((void *) ref + 0x10);
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
static inline bool psn_equals(ProcessSerialNumber *a, ProcessSerialNumber *b)
{
    Boolean result;
    SameProcess(a, b, &result);
    return result == 1;
}
#pragma clang diagnostic pop

static bool rect_is_in_direction(CGRect r1, CGRect r2, int direction)
{
    CGPoint r1_max = { CGRectGetMaxX(r1), CGRectGetMaxY(r1) };
    CGPoint r2_max = { CGRectGetMaxX(r2), CGRectGetMaxY(r2) };

    switch (direction) {
    case DIR_NORTH: if (r2.origin.y > r1_max.y) return false; break;
    case DIR_WEST:  if (r2.origin.x > r1_max.x) return false; break;
    case DIR_SOUTH: if (r2_max.y < r1.origin.y) return false; break;
    case DIR_EAST:  if (r2_max.x < r1.origin.x) return false; break;
    }

    switch (direction) {
    case DIR_NORTH:
    case DIR_SOUTH:
        return (r2.origin.x >= r1.origin.x && r2.origin.x <= r1_max.x) ||
               (r2_max.x >= r1.origin.x && r2_max.x <= r1_max.x) ||
               (r1.origin.x > r2.origin.x && r1.origin.x < r2_max.x);
    case DIR_WEST:
    case DIR_EAST:
        return (r2.origin.y >= r1.origin.y && r2.origin.y <= r1_max.y) ||
               (r2_max.y >= r1.origin.y && r2_max.y <= r1_max.y) ||
               (r1.origin.y > r2.origin.y && r1_max.y < r2_max.y);
    }

    return false;
}

static uint32_t rect_distance(CGRect r1, CGRect r2, int direction)
{
    CGPoint r1_max = { CGRectGetMaxX(r1), CGRectGetMaxY(r1) };
    CGPoint r2_max = { CGRectGetMaxX(r2), CGRectGetMaxY(r2) };

    switch (direction) {
    case DIR_NORTH: return r2_max.y > r1.origin.y ? r2_max.y - r1.origin.y : r1.origin.y - r2_max.y;
    case DIR_WEST:  return r2_max.x > r1.origin.x ? r2_max.x - r1.origin.x : r1.origin.x - r2_max.x;
    case DIR_SOUTH: return r2.origin.y < r1_max.y ? r1_max.y - r2.origin.y : r2.origin.y - r1_max.y;
    case DIR_EAST:  return r2.origin.x < r1_max.x ? r1_max.x - r2.origin.x : r2.origin.x - r1_max.x;
    }

    return UINT32_MAX;
}

static int regex_match(bool valid, regex_t *regex, const char *match)
{
    if (!valid || !match) return REGEX_MATCH_UD;
    int result = regexec(regex, match, 0, NULL, 0);
    return result == 0 ? REGEX_MATCH_YES : REGEX_MATCH_NO;
}

#endif
