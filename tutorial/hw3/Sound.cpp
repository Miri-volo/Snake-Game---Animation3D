#include <windows.h>
#include <mmsystem.h>
#include "Sound.h"
#include<iostream>

#include<fstream>
#pragma comment( lib, "Winmm.lib" )



//importent to use wav files only!!!
void Sound::MenuMusic()
{
    PlaySound(TEXT("../tutorial/hw3/menuMusic.wav"), NULL, SND_FILENAME | SND_ASYNC); //music from https://www.chosic.com/download-audio/29784/
}

void Sound::GameMusic()
{
    PlaySound(TEXT("../tutorial/hw3/gameMusic.wav"), NULL, SND_FILENAME | SND_ASYNC); //music from https://www.chosic.com/download-audio/29784/
}

void Sound::Stop()
{
    PlaySound(NULL, NULL, 0);
}