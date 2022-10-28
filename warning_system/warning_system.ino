#include <SDHCI.h>
#include <Audio.h>

SDClass theSD;
AudioClass *theAudio = AudioClass::getInstance();

bool bPlaying = false;
bool bStart = false;
File myFile;

void changeState() {    // interrupt handler
  bStart = bStart ? false : true;
}

void player_end() {//強制停止できる
  if (bPlaying) {
    theAudio->stopPlayer(AudioClass::Player0 ,AS_STOPPLAYER_NORMAL);
  bPlaying = false;
  myFile.close();
  Serial.println("End play");
  }
}

//ズレた場合----
void player_start1() {
  Serial.println("Start play");
  myFile = theSD.open("Slipping.mp3"); //←←ファイルパス←←
  if (!myFile) {
    Serial.println("File open error\n");
    while(1);
  }
  int err = theAudio->writeFrames(AudioClass::Player0, myFile);
  if ((err != AUDIOLIB_ECODE_OK) && (err != AUDIOLIB_ECODE_FILEEND)) {
    Serial.println("File Read Error! =" + String(err));
    player_end();
  }
  theAudio->setVolume(-160);
  theAudio->startPlayer(AudioClass::Player0);
  bPlaying = true;
}

//逆走の場合----
void player_start2() {
  Serial.println("Start play");
  myFile = theSD.open("Reversing.mp3"); //←←ファイルパス←←
  if (!myFile) {
    Serial.println("File open error\n");
    while(1);
  }
  int err = theAudio->writeFrames(AudioClass::Player0, myFile);
  if ((err != AUDIOLIB_ECODE_OK) && (err != AUDIOLIB_ECODE_FILEEND)) {
    Serial.println("File Read Error! =" + String(err));
    player_end();
  }
  theAudio->setVolume(-160);
  theAudio->startPlayer(AudioClass::Player0);
  bPlaying = true;
}

int key = 0;
void setup() {
  int err;
  Serial.begin(115200);

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
void loop() {
  
  int err;
  loop_count += 1;
  key = loop_count % 3;
  Serial.println(key);
  if(key == 1 || key == 2){
    bPlaying = true;
  }
  if (key == 1) {
    player_start1();
  }else if (key == 2) {
    player_start2();
  }
  
  err = theAudio->writeFrames(AudioClass::Player0, myFile);
  if (err == AUDIOLIB_ECODE_FILEEND) {
    Serial.println("Audio file ended");
    player_end();
  }
  

  usleep(40000);
  delay(30000);
  bPlaying = false;
  
}