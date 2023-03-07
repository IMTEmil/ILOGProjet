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

static void AddEffectOnSample(short* sample, void(*process)());

static WAVHEADER GetWAVHEADER(
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

static size_t WriteWAVHeaderToFile(
	WAVHEADER *wavh,
	FILE *f
)
{
	return fwrite(wavh, sizeof(WAVHEADER), 1, f);
}

static int CopyWAVData(FILE *fin, FILE *fout, void(*feffect))
{
	short* buffer = (short*)calloc(sizeof(short), 2);

	if (buffer == NULL) return -1;

	while (fread(buffer, sizeof(short), 2, fin))
	{
		AddEffectOnSample(buffer, feffect);
		if (!(fwrite(buffer, sizeof(short), 2, fout) > 0))
		{
			free(buffer);
			return -1;
		}
	}

	free(buffer);

	return 0;
}

static void MuteLeftChannel(short* sample)
{
	(short)*(sample) = 0;
}

static void MuteRightChannel(short* sample)
{
	(short)*(sample + 1) = 0;
}

static void MuteSample(short* sample)
{
	MuteRightChannel(sample);
	MuteLeftChannel(sample);
}

static void AddEffectOnSample(short* sample, void(*process)())
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

char* AddOutputPrefix(char* szFileName)
{
	char* szOutput = (char*)malloc((strlen(szFileName) + 3) * sizeof(char));

	if (szOutput != NULL)
	{
		strcpy_s(szOutput, (strlen(szFileName) + 2) * sizeof(char), "o_");

		strcat(szOutput, szFileName);
	}

	return szOutput;
}

int CopyWAVFileAddEffect(char* szFileName, void(*feffect)())
{
	FILE* fin = NULL;
	FILE* fout = NULL;

	WAVHEADER wavh = { 0 };

	char* szOutputFileName = NULL;

	fin = fopen(szFileName, "rb");
	
	if (fin != NULL)
	{
		szOutputFileName = AddOutputPrefix(szFileName);

		fout = fopen(szOutputFileName, "wb");

		free(szOutputFileName);

		if (fout != NULL)
		{
			wavh = GetWAVHEADER(fin);

			if (WriteWAVHeaderToFile(&wavh, fout) > 0)
			{
				CopyWAVData(fin, fout, feffect);
			}

			fclose(fout);
		}

		fclose(fin);
	}
	
	return 0;
}

int main(int argc, char** argv)
{
	char* szFileName = "audio.wav";

	CopyWAVFileAddEffect(szFileName, &MuteL);

	return 0;
}

