/*
   Flick Flop

    1 piece becomes a flippin' master
    other pieces are game pieces. once attached they take the masters color

   by Nick Bentley and Jonathan Bobrow
*/

#define FLOPPER_INTERVAL 2000         // time between team flops
#define FLOPPER_INTERVAL_RANGE 1000   // interval can be +/- this amount
//#define NUM_TEAMS 3                 // let's make this variable to playtest

Timer flopperTimer;

enum gameRole {
  FLICKER,
  FLOPPER,
  NUM_ROLES
};

enum flickStatus {
  READY,
  SET
};

byte role = FLICKER;
byte teamBroadcast = 0;
byte team = 1;
byte numTeams = 3;
bool isCommittedToTeam = false;
bool wasAlone = false;
bool bRange = false;

void setup() {

}

void loop() {

  /*
     HANDLE INPUTS
  */

  // switch roles with button double click
  if (buttonDoubleClicked()) {

    role++;
    isCommittedToTeam = false;

    if (role >= NUM_ROLES) {
      role = 0;
    }
  }

  if (buttonPressed()) {
    if (role == FLICKER && isAlone()) {
      if (isCommittedToTeam) {
        // reset our team
        isCommittedToTeam = false;
      }
      else {
        // change team
        team++;
        if (team > numTeams) {
          team = 1;
        }
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

  // share which piece type we are and the team we need to broadcast
  // lower 2 bits communicate the game role - -
  // upper 4 bits communicate the piece mode - - - -
  byte sendData = (teamBroadcast << 2) + ((int)isCommittedToTeam << 1) + role;

  setValueSentOnAllFaces(sendData);

}

/*
   COMMUNICATION HELPERS
*/

//Call these formulas when we want to separate the sent data
byte getTeamFromReceivedData(byte data) {
  byte t = (data >> 2) & 15;  // keep only the 4 bits of info
  //  Serial.print("team from received data: ");
  //  Serial.println(t);
  return t;
}

byte getStatusFromReceivedData(byte data) {
  byte s = (data >> 1) & 1;
  //  Serial.print("role from received data: ");
  //  Serial.println(role);
  return s;
}

byte getRoleFromReceivedData(byte data) {
  byte r = data & 1;  // keep only the lower 1 bit of info
  //  Serial.print("role from received data: ");
  //  Serial.println(role);
  return r;
}

/*
   UPDATE LOOPS (Logic)
*/
void updateFlopper() {
  // I'm a flopper, all I have to do is change which team I am broadcasting every so often...
  if (flopperTimer.isExpired()) {
    // let's flop!
    team = (team % numTeams) + 1;  //getRandomTeam(team);
    uint16_t flopDuration;
    if (bRange) {
      map(rand(5), 0, 5, FLOPPER_INTERVAL - FLOPPER_INTERVAL_RANGE, FLOPPER_INTERVAL + FLOPPER_INTERVAL_RANGE);
    }
    else {
      flopDuration = FLOPPER_INTERVAL;
    }
    flopperTimer.set(flopDuration);
  }

  // broadcast our team color
  teamBroadcast = team;
  isCommittedToTeam = true;
}

byte getRandomTeam( byte curTeam) {
  byte t = 255;
  while ( t != curTeam ) {
    t = 1 + rand(numTeams - 1);
  }
  return t;
}

void updateFlicker() {

  FOREACH_FACE(f) {
    if (!isValueReceivedOnFaceExpired(f)) {
      byte data = getLastValueReceivedOnFace(f);
      byte neighborTeam = getTeamFromReceivedData(data);
      byte neighborRole = getRoleFromReceivedData(data);
      byte neighborStatus = getStatusFromReceivedData(data);

      // if I was alone and now have a neighbor, take the team broadcast from the flopper
      if (wasAlone) {

        if ( neighborRole == FLOPPER ||
             (neighborRole == FLICKER && neighborStatus == SET ) ) {

          team = neighborTeam;
          isCommittedToTeam = true; // We're now this team for real
          wasAlone = false;
          break;
        }
      }
      else {
        // if I hear the flopper update its team, broadcast that team
        if ( neighborRole == FLOPPER ) {
          teamBroadcast = neighborTeam;
          break;
        } else if ( neighborRole == FLICKER && neighborStatus == SET ) {

          if (teamBroadcast != neighborTeam) {
            teamBroadcast = neighborTeam;
            break;
          }
        }
      }
    }
  }

  if (isAlone() && !isCommittedToTeam ) {
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
    if (!isAlone()) {
      byte index = (millis() / 300) % 6;
      setColorOnFace( getColorForTeam(teamBroadcast), index  );
      setColorOnFace( getColorForTeam(teamBroadcast), (index + 1) % 6);
      setColorOnFace( getColorForTeam(teamBroadcast), (index + 2) % 6 );
    }
  }
  else {
    // one light to root for our team
    setColorOnFace( getColorForTeam(team), 0 );
  }

}

Color getColorForTeam( byte t ) {

  Color c = WHITE;  // default...

  switch (t) {
    case 0: c = RED; break; // not currently used as a team
    case 1: c = makeColorHSB(160, 255, 255); break;
    case 2: c = makeColorHSB(200, 255, 255); break;
    case 3: c = makeColorHSB(280, 255, 255); break;
    case 4: c = makeColorHSB(30, 255, 255); break;
    default: WHITE;
  }

  return c;
}

