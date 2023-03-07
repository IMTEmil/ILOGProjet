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

void MuteLeftChannel(short* sample);

void MuteRightChannel(short* sample);

void MuteSample(short* sample);

int CopyWAVFileAddEffect(char* szFileName, void(*feffect)());

#endif /*_AUDIO_EFFECTS_H*/