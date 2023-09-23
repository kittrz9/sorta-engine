#ifndef WINDOW_H
#define WINDOW_H

extern int windowWidth;
extern int windowHeight;

// opaque pointer just in case I want to allow this to use something other than GLFW
// could probably make this change to a pointer to a custom struct based on some macro
typedef void* SORTA_WINDOW;
extern SORTA_WINDOW window;

void initWindow(void);
void destroyWindow(void);

#endif
