#ifndef _AUDIO_EFFECTS_H
#define _AUDIO_EFFECTS_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

typedef struct RIFFHEADER
{
	char ChunkId[4];
	
	uint32_t ChunkSize;

	char Format[4];
} RIFFHEADER;

typedef struct SUBCHUNK
{
	char Subchunk1Id[4];

	uint32_t Subchunk1Size;

	uint16_t AudioFormat;

	short NumChannels;

	uint32_t SampleRate;

	uint32_t ByteRate;

	short BlockAlign;

	short BitsPerSample;

	char Subchunk2Id[4];

	uint32_t Subchunk2Size;
} SUBCHUNK;

typedef struct WAVHEADER
{
	RIFFHEADER riffh;

	SUBCHUNK subc;
} WAVHEADER;

typedef struct DELAY_PARAMETERS
{
	double feedback;

	int sDelayTime;

} DELAY_PARAMETERS;

void MuteLeftChannel(short* sample);

void MuteRightChannel(short* sample);

void MuteSample(short* sample);

int Delay(WAVHEADER wavh, FILE* fin, FILE* fout, DELAY_PARAMETERS dparam);

int CopyWAVFileAddEffect(char* szFileName, void(*feffect)(), DELAY_PARAMETERS dparam);

#endif /*_AUDIO_EFFECTS_H*/