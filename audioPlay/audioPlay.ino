/*
 *  Spresense_audio_mp3_player.ino - Sound player example application
 *  Copyright 2019 Sony Semiconductor Solutions Corporation
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <SDHCI.h>
#include <Audio.h>
#include <String.h>

SDClass theSD;
File *file2;
AudioClass *theAudio = AudioClass::getInstance();
String filename1 = "Slipping.mp3";
String filename2 = "Reversing.mp3";
bool bPlaying = false;
bool bStart = false;


void changeState() {    // interrupt handler
  bStart = bStart ? false : true;
  Serial.println("changeState was done");
}

void player_end(File *myFile) {
  if (bPlaying) 
    theAudio->stopPlayer(AudioClass::Player0 ,AS_STOPPLAYER_NORMAL);
  bPlaying = false;
  myFile->close();
  Serial.println("End play");
}

void player_start(String filename, File &myFile) {
  Serial.println("Start play");
  myFile = theSD.open(filename);
  if (!myFile) {
    Serial.println("File open error\n");
    while(1);
  }
  int err = theAudio->writeFrames(AudioClass::Player0, myFile);
  if ((err != AUDIOLIB_ECODE_OK) && (err != AUDIOLIB_ECODE_FILEEND)) {
    Serial.println("File Read Error! =" + String(err));
    
  }
  theAudio->setVolume(-160);
  theAudio->startPlayer(AudioClass::Player0);
  bPlaying = true;
  
  while (true){
  err = theAudio->writeFrames(AudioClass::Player0, myFile);
  usleep(40000);
  if (err == AUDIOLIB_ECODE_FILEEND){
    break;
  }
}
  theAudio->stopPlayer(AudioClass::Player0);
  myFile.close();
  bPlaying = false;
  theAudio->setReadyMode();
  
  theAudio->end();
  theAudio->begin();
  theAudio->setRenderingClockMode(AS_CLKMODE_NORMAL);
  theAudio->setPlayerMode(AS_SETPLAYER_OUTPUTDEVICE_SPHP ,AS_SP_DRV_MODE_LINEOUT);

  err = theAudio->initPlayer(AudioClass::Player0 ,AS_CODECTYPE_MP3 ,"/mnt/sd0/BIN"
                            ,AS_SAMPLINGRATE_AUTO ,AS_CHANNEL_STEREO);
}

int intPin = 4;
void setup() {
  int err;
  Serial.begin(115200);
  //pinMode(intPin, INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(intPin) ,changeState ,FALLING);

  theAudio->begin();
  theAudio->setRenderingClockMode(AS_CLKMODE_NORMAL);
  theAudio->setPlayerMode(AS_SETPLAYER_OUTPUTDEVICE_SPHP ,AS_SP_DRV_MODE_LINEOUT);

  err = theAudio->initPlayer(AudioClass::Player0 ,AS_CODECTYPE_MP3 ,"/mnt/sd0/BIN"
                            ,AS_SAMPLINGRATE_AUTO ,AS_CHANNEL_STEREO);
  if (err != AUDIOLIB_ECODE_OK) {
    Serial.println("Player0 initialize error");
    while(1);
  }
  Serial.println("Player0 initialized");
}
int loop_count = 0;
int key = 0;
void loop() {
  loop_count ++;
  key = loop_count % 3;
  Serial.println(key);
  int err;
  if (key == 1 || key == 2){
    Serial.println("if was done");
    changeState();
  }
  Serial.print("bstart = ");Serial.print("  ");  Serial.println(bStart);
  Serial.print("bPlaying = ");Serial.print("  ");  Serial.println(bPlaying);
  
  if (!bStart && !bPlaying) {//どちらもfalseなら実行

    return;
  } else if (bStart && !bPlaying && key == 1) {
    File myFile;
    player_start(filename1, myFile); 
    changeState();
    file2 = &myFile;
    return;
    
  } else if (bStart && !bPlaying && key == 2){
    File myFile;
    player_start(filename2, myFile); 
    changeState();
    file2 = &myFile;
    return;
  }
  
  
  delay(30000);
  return;
}