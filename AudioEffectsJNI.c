#include "Java/TestNative.h"
#include "AudioEffects.h"

JNIEXPORT void JNICALL Java_TestNative_CopyWAVFileAddEffect
  (JNIEnv * env, jclass class, jstring fileName, jint funcID, jdouble feedback, jint sDelayTime)
{
	char* szJString = (char*)(*env)->GetStringUTFChars(env, fileName, NULL);

	void *funcAdress = NULL;

	DELAY_PARAMETERS dparam = { feedback, sDelayTime };

	switch (funcID)
	{
	default:
	case 0:
		funcAdress = &MuteSample;
		break;
	case 1:
		funcAdress = &MuteLeftChannel;
		break;
	case 2:
		funcAdress = &MuteRightChannel;
		break;
	case 3:
		funcAdress = &Delay;
	}

	CopyWAVFileAddEffect(szJString, funcAdress, dparam);

	(*env)->ReleaseStringUTFChars(env, fileName, szJString);
};