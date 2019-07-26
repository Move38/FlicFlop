enum blinkTypes {FLOPPER, FLICKER};
byte blinkType = FLICKER;

byte scoringTeam = 0;
byte signalTeam = 0;
byte teamHues[4] = {0, 45, 125, 230};

Timer flopTimer;
#define FLOP_INTERVAL 2000

Timer animTimer;
#define ANIMATION_INTERVAL 200
byte spinFace = 0;

enum celebrationStates {INERT, CELEBRATE, RESOLVING};
byte celebrationState = INERT;

void loop() {

  //RUN LOOPS
  if (blinkType == FLICKER) {
    flickerLoop();
    flickerDisplay();
  } else if (blinkType == FLOPPER) {
    flopperLoop();
    flopperDisplay();
  }

  //SET UP COMMUNICATION
  byte sendData = (signalTeam << 5) + (scoringTeam << 3) + (celebrationState);

  //DUMP BUTTON PRESSES
  buttonDoubleClicked();
  buttonLongPressed();

}

void flickerLoop() {

  if (signalTeam == 0) {

    //look for neighbors with a signal
    FOREACH_FACE(f) {
      
    }

  } else {

    //listen for going alone
    if (isAlone()) {

    }

  }

  if (scoringTeam == 0) {

    //listen for long press to become flopper

  } else {

    //listen for signal change

  }

  if (buttonLongPressed()) {
    signalTeam = 1;
    scoringTeam = 0;
    blinkType = FLOPPER;
  }
}

void flopperLoop() {

  if (flopTimer.isExpired()) {
    signalTeam += 1;
    if (signalTeam == 4) {
      signalTeam = 1;
    }
  }

  if (buttonLongPressed()) {
    signalTeam = 0;
    scoringTeam = 0;
    blinkType = FLICKER;
  }
}

void flickerDisplay() {

}

void flopperDisplay() {

}
