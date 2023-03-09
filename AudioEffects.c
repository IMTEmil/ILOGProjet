#include "AudioEffects.h"

// unsafe use of fopen warnings
#pragma warning(disable:4996)

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

// warnings for buffer overrun
#pragma warning(disable:6386)

int Delay(WAVHEADER wavh, FILE* fin, FILE* fout, DELAY_PARAMETERS dparam)
{
	unsigned int maxsize = wavh.subc.SampleRate * dparam.sDelayTime * wavh.subc.NumChannels;
	int read = 0;
	int write = maxsize - wavh.subc.NumChannels;
	short* buffer = NULL;

	short* outputSample = (short*)malloc(sizeof(short) * wavh.subc.NumChannels);
	if (outputSample == NULL) return -1;
	memset(outputSample, 0, sizeof(short) * wavh.subc.NumChannels);

	buffer = (short*)malloc(maxsize * sizeof(short));
	if (buffer == NULL)
	{
		free(outputSample);
		return -1;
	}
	memset(buffer, 0, maxsize * sizeof(short));

	while (fread(buffer + write, sizeof(short), wavh.subc.NumChannels, fin))
	{
		// delay line
		for (int i = 0; i < wavh.subc.NumChannels; i++)
		{
			outputSample[i] = (short)buffer[read + i];

			outputSample[i] += (short)buffer[write + i];

			if (outputSample[i] >= 32767) outputSample[i] = 32766;
			if (outputSample[i] <= -32768) outputSample[i] = -32767;

			buffer[write + i] += (short)(buffer[read + i] * dparam.feedback);
		}

		read += wavh.subc.NumChannels;
		if (read == maxsize) read = 0;

		write += wavh.subc.NumChannels;
		if (write == maxsize) write = 0;

		fwrite(outputSample, sizeof(short), wavh.subc.NumChannels, fout);
	}

	free(buffer);

	free(outputSample);

	return 0;
}

void MuteLeftChannel(short* sample)
{
	*sample = 0;
}

void MuteRightChannel(short* sample)
{
	*(sample + 1) = 0;
}

void MuteSample(short* sample)
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

static char* AddOutputPrefix(char* szFileName)
{
	char* szOutput = (char*)malloc((strlen(szFileName) + 3) * sizeof(char));

	if (szOutput != NULL)
	{
		strcpy_s(szOutput, (strlen(szFileName) + 2) * sizeof(char), "o_");

		strcat(szOutput, szFileName);
	}

	return szOutput;
}

int CopyWAVFileAddEffect(char* szFileName, void(*feffect)(), DELAY_PARAMETERS dparam)
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
				if (feffect == (void *)Delay)
				{
					feffect(wavh, fin, fout, dparam);
				}
				else CopyWAVData(fin, fout, feffect);
			}

			fclose(fout);
		}

		fclose(fin);
	}
	
	return 0;
}
/*
int main(int argc, char** argv)
{
	char* szFileName = "audio.wav";

	CopyWAVFileAddEffect(szFileName, (void*)&Delay, (DELAY_PARAMETERS){0.3, 1});

	return 0;
}
*/