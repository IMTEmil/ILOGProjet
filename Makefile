execute: compile
	java -Djava.library.path=src -cp src TestNative audio.wav


compile: header AudioEffectsJNI.c AudioEffects.c
	gcc -I"C:\Program Files\Java\jdk-16.0.1\include" -I"C:\Program Files\Java\jdk-16.0.1\include\win32" -shared -o src/AudioEffects.dll AudioEffectsJNI.c AudioEffects.c

header: src/TestNative.java
	javac -h src src/TestNative.java

clean:
	rm -rf src/AudioEffects.dll src/TestNative.class o_audio.wav src/TestNative.h