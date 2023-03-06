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

int WriteWAVHeaderToFile(
	WAVHEADER *wavh, 
	char *fname,
	FILE *f
)
{
	if (f == NULL) return -1;
	
	fwrite(wavh, sizeof(WAVHEADER), 1, f);

	return 0;
}

void MuteLeftChannel(short* sample)
{
	(short)*(sample) = 0;
}

void MuteRightChannel(short* sample)
{
	(short)*(sample + 1) = 0;
}

void MuteSample(short* sample)
{
	MuteRightChannel(sample);
	MuteLeftChannel(sample);
}

void AddEffectOnSample(short* sample, void(*process)())
{
	process(sample);
}

short* GetNextBuffer(
	FILE* f,
	WAVHEADER wavh,
	bool *eof
)
{
	long i = 0;
	void* Buffer = calloc(wavh.subc.BitsPerSample / 8, wavh.subc.NumChannels);

	if (Buffer != NULL) fread(Buffer, wavh.subc.BitsPerSample / 8, wavh.subc.NumChannels, f);

	*eof = ((uint32_t)(i = ftell(f)) <= wavh.riffh.ChunkSize);

	return Buffer;
}

int CopyWAVFileAddEffect(char* fname)
{
	FILE* f = NULL;
	FILE* f_out = NULL;

	WAVHEADER wavh = { 0 };

	short* buffer = NULL;

	char *szOutput = (char *)malloc((strlen(fname) + 2) * sizeof(char));

	if (szOutput == NULL) return -1;

	f = fopen(fname, "rb");

	strcpy_s(szOutput, (strlen(fname) + 2) * sizeof(char), "o_");

	strcat(szOutput, fname);

	f_out = fopen(szOutput, "wb");

	if (f == NULL || f_out == NULL) return -1;

	wavh = GetWAVHEADER(f);

	fwrite(&wavh,sizeof(WAVHEADER), 1, f_out);

	buffer = (short*)calloc(sizeof(short), 2);
	if (buffer != NULL)
	{
		int buffer_size = 2;
		while (fread(buffer, buffer_size, 2, f))
		{
			AddEffectOnSample(buffer, &MuteLeftChannel);
			fwrite(buffer, buffer_size, 2, f_out);
		}
		free(buffer);
	}

	free(szOutput);

	fclose(f_out);

	fclose(f);
	
	return 0;
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

		CopyWAVFileAddEffect(szFileName);

		fclose(file);
	}

	return 0;
}

