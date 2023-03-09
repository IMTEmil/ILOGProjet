src=Java
JAVA_HOME:=$(JAVA_HOME)

execute: compile
	java -Djava.library.path=${src} -cp ${src} TestNative audio.wav 3 0.5 1
	java -Djava.library.path=${src} -cp ${src} TestNative o_audio.wav 1

compile: header AudioEffectsJNI.c AudioEffects.c
	gcc -I"${JAVA_HOME}/include" -I"${JAVA_HOME}\include\win32" -shared -o ${src}/AudioEffects.dll AudioEffectsJNI.c AudioEffects.c

header: ${src}/TestNative.java
	javac -h ${src} ${src}/TestNative.java

clean:
	rm -rf ${src}/AudioEffects.dll ${src}/TestNative.class o_audio.wav ${src}/TestNative.h