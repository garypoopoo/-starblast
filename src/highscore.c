#include "highscore.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
int highscore_load(void) {
    return EM_ASM_INT({
        var v = localStorage.getItem('starblast_hi');
        return v ? parseInt(v) : 0;
    });
}
void highscore_save(int score) {
    EM_ASM({ localStorage.setItem('starblast_hi', $0); }, score);
}
#else
// Desktop fallback: just return 0 / do nothing
int highscore_load(void) { return 0; }
void highscore_save(int score) { (void)score; }
#endif
