#include "AudioEffects.h"

// unsafe use of fopen warnings
#pragma warning(disable:4996)

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

static int EndianSwap(
	int nnn
)
{
	int b1, result = 0;
	b1 = (nnn & 0x00FF);
	nnn = nnn >> 8;
	result = b1;
	b1 = (nnn & 0x00FF);
	result = (result << 8) | b1;
	nnn = result;

	return nnn;
}

WAVHEADER GetWAVHEADER(
	FILE *f
)
{
	WAVHEADER wavh = { 0 };

	RIFFHEADER riffh = { 0 };

	SUBCHUNK subc = { 0 };

	if (fread(&wavh.riffh, sizeof(riffh), 1, f) != 1) return (WAVHEADER) { 0 };

	if (fread(&wavh.subc, sizeof(subc), 1, f) != 1) return (WAVHEADER) { 0 };

  	return wavh;
}

int WriteWAVHeader(
	WAVHEADER *wavh, 
	char *fname,
	FILE *f
)
{
	if (f == NULL) return -1;
	
	fwrite(wavh, sizeof(WAVHEADER), 1, f);

	return 0;
}

void MuteSample(short* sample)
{
	(short)*(sample) = 0;
	(short)*(sample + 1) = 0;
}

void MuteLeft(short* sample)
{
	(short)*(sample) = 0;
}

void MuteRight(short* sample)
{
	(short)*(sample + 1) = 0;
}

void AddEffectOnSample(short* sample, void(*process)())
{
	process(sample);
}

/*
int SetHeadToDataStart(
	FILE* f
)
{
	return fseek(f, 44, SEEK_SET);
}
*/

void *GetNextBuffer(
	FILE *f, 
	WAVHEADER wavh
)
{
	void* Buffer = calloc(wavh.subc.BitsPerSample / 8, wavh.subc.NumChannels);

	if (Buffer != NULL) fread(Buffer, wavh.subc.BitsPerSample / 8, wavh.subc.NumChannels, f);

	return Buffer;
}

void WriteWAVFile(char* fname)
{
	short *cbuffer = (short*)malloc((size_t)(44100 * 2));
	uint32_t w_index = 0;
	uint32_t r_index = 0;

	free(cbuffer);
}

int main(int argc, char** argv)
{
	FILE* file = NULL;

	WAVHEADER wavh = { 0 };

	short* Buffer = NULL;

	char* szFileName = "audio.wav";

	file = fopen(szFileName, "r");

	if (file != NULL)
	{
		wavh = GetWAVHEADER(file);

		// eliminates JUNK wav headers
		if (memcmp(wavh.subc.Subchunk1Id, "fmt ", sizeof(char) * 4) != 0) return 0;

		fseek(file, 200, SEEK_SET);

		Buffer = GetNextBuffer(file, wavh);

		AddEffectOnSample(Buffer, &MuteSample);

		short sss[2] = { 0 };

		memcpy(sss, Buffer, 2 * sizeof(short));

		free(Buffer);

		fclose(file);
	}

	return 0;
}