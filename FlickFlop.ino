/*
   Flick Flop

    1 piece becomes a flippin' master
    other pieces are game pieces. once attached they take the masters color

   by Nick Bentley and Jonathan Bobrow
*/

#define FLOPPER_INTERVAL 2000         // time between team flops
#define FLOPPER_INTERVAL_RANGE 0      // interval can be +/- this amount
//#define NUM_TEAMS 3                 // let's make this variable to playtest

Timer flopperTimer;

enum gameRole {
  FLICKER,
  FLOPPER,
  NUM_ROLES
};

byte role = FLICKER;
byte team = 0;
byte numTeams = 4;
bool isCommittedToTeam = false;
bool wasAlone = false;

void setup() {

}

void loop() {

  /*
     HANDLE INPUTS
  */

  // switch roles with button double click
  if (buttonDoubleClicked()) {

    role++;

    if (role >= NUM_ROLES) {
      role = 0;
    }
  }

  if (isAlone() && buttonPressed() ) {
    if (isCommittedToTeam) {
      // reset our team
      isCommittedToTeam = false;
    }
    else {
      // change team
      team++;
      if (team > numTeams) {
        team == 1;
      }
    }
  }

  // Update
  switch (role) {
    case FLICKER:   updateFlicker();  break;
    case FLOPPER:   updateFlopper();  break;
  }

  // Display
  switch (role) {
    case FLICKER:   displayFlicker();  break;
    case FLOPPER:   displayFlopper();  break;
  }

}

/*
   UPDATE LOOPS (Logic)
*/
void updateFlopper() {
  // I'm a flopper, all I have to do is change which team I am broadcasting every so often...
  if (flopperTimer.isExpired()) {
    // let's flop!
    team = getRandomTeam(team);
    flopperTimer.set(FLOPPER_INTERVAL + rand(FLOPPER_INTERVAL_RANGE) - FLOPPER_INTERVAL_RANGE/2); 
  }

  // broadcast our team color
  setValueSentOnAllFaces(team);
}

byte getRandomTeam( byte curTeam) {
  byte t = 255;
  while ( t != curTeam ) {
    t = 1 + rand(numTeams);
  }
  return t;
}

void updateFlicker() {

  FOREACH_FACE(f) {
    if (!isValueReceivedOnFaceExpired(f)) {
      byte neighbor = getLastValueReceivedOnFace(f);

      // if I was alone and now have a neighbor, take the team broadcast from the flopper
      if (wasAlone) {
        team = neighbor;
        wasAlone = false;
        break;
      }
      else {
        // if I hear the flopper update its team, broadcast that team
        if (neighbor != 0) {
          setValueSentOnAllFaces(neighbor);
          break;
        }
      }
    }
  }

  if (isAlone()) {
    wasAlone = true;
  }

}

/*
   DISPLAY LOOPS (Pretty)
*/

void displayFlopper() {
  setColor( OFF );  // reset display
  setColor( getColorForTeam(team) );
}

void displayFlicker() {
  setColor( OFF );  // reset display

  if (isCommittedToTeam) {
    // solid for our team color
    setColor( getColorForTeam(team) );
  }
  else {
    // one light to root for our team
    setColorOnFace( getColorForTeam(team), 0 );
  }
}

Color getColorForTeam( byte t ) {

  Color c = WHITE;  // default...

  switch (t) {
    case 1: c = makeColorHSB(160, 255, 255); break;
    case 2: c = makeColorHSB(200, 255, 255); break;
    case 3: c = makeColorHSB(240, 255, 255); break;
    case 4: c = makeColorHSB(280, 255, 255); break;
    default: WHITE;
  }

  return c;
}

