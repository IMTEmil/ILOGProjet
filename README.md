# ILOG : projet

## description

Ce projet a été réalisé dans le cadre du cours ILOG  donné par Christophe Tombelle à IMT Nord Europe en 2022/2023.
L'objectif de ces travaux est de mettre en œuvre une partie des connaissances acquises au cours de l'UV. 

Afin d'approfondir l'utilisation de JNI (Java Native Interface) entre le Java et le C : on a mis en place un module natif en C de traitement du signal audio basique (délai, mute), qu'on peut importer dans un projet Java.

## livrables  :

 - module C de traitement audio (*AudioEffects.c* / *AudioEffects.h*)
 - classe Java (*TestNative.Java*) implémentant une méthode native provenant du module précédent, mise en place de l'interface JNI (*AudioEffectsJNI.c*)
 - un Makefile pour automatiser la compilation, l'interface JNI et le nettoyage (sous Windows)

#### Utilisation
Pour créer le fichier d'entête C nécessaire à l'implémentation de la méthode native :
```
$ make header
```
Pour créer la bibliothèque dynamique (dll) utilisable par notre classe Java : 
```
$ make compile
```
Pour lancer un exemple de compilation de notre classe java sur le fichier "audio.wav":
```
$ make execute
```
Pour lancer notre programme depuis le dossier Java (cf. le point ci-dessous):
```
$ java -Djava.library.path=. TestNative "fichier.wav" 1 2 3
```
Les paramètres du programme sont les suivants :

 - Nom du fichier audio : pour l'instant ne prend en charge que des fichiers wav standards (ne fonctionne qu'avec des header "fmt " et non "JUNK" ) dont la résolution en bits est de 16 (2 octets par échantillons).
 - Effet à appliquer au son : 
 -0 : pour mute le son
 -1 : pour garder que le canal gauche
 -2 : pour garder que le canal droit
 -3 : pour appliquer un délai
 - Si délai choisi : feedback 
 - Si délai choisi : temps du délai

#### Points abordés
De façon non exhaustive ce projet aborde les thèmes suivants :

 Production de logiciel / Bibliothèque, 
 compilation C et Java, 
 Versionning,
 Java Native Interface, 
 Généricité,
 Buffer circulaire pour le traitement de flux audio.
