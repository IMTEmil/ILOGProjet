#ifndef _AUDIO_EFFECTS_H
#define _AUDIO_EFFECTS_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#define MuteL MuteLeftChannel
#define MuteR MuteRightChannel
#define MuteAll MuteSample

int CopyWAVFileAddEffect(char* szFileName, void(*feffect)());

#endif /*_AUDIO_EFFECTS_H*/