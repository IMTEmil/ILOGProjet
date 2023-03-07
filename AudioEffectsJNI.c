#include "AudioEffectsJNI.h"

JNIEXPORT void JNICALL Java_TestNative_CopyWAVFileAddEffect
(JNIEnv* env, jobject jobj, jstring string, jint n)
{
	char* szJString = (char*)(*env)->GetStringUTFChars(env, string, NULL);

	void *addFunc = NULL;

	switch (n)
	{
	case 0:
		addFunc = &MuteAll;
		break;
	case 1:
		addFunc = &MuteL;
		break;
	case 2:
		addFunc = &MuteR;
		break;
	default:
		break;
	}

	CopyWAVFileAddEffect(szJString, addFunc);

	(*env)->ReleaseStringUTFChars(env, string, szJString);
};