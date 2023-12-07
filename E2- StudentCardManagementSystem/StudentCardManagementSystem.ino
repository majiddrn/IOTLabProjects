#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         0           // Configurable, see typical pin layout above
#define SS_PIN          5          // Configurable, see typical pin layout above
#define REGISTRATION_DURATION 10000

String sid;
String studentIDs[] = {"12345", "67890", "11223", "44556", "78901"};

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

// Adding enum for modes
enum State {
    Standby,
    Authentication,
    Registration,
    Reset
};

State currentState = Standby; // Start in Standby state
unsigned long registrationStart;

//*****************************************************************************************//
void setup() {
  Serial.begin(9600);                                           // Initialize serial communications with the PC
  SPI.begin();                                                  // Init SPI bus
  mfrc522.PCD_Init();                                              // Init MFRC522 card
  Serial.println(F("Read personal data on a MIFARE PICC:"));    //shows in serial that it is ready to read
}

//*****************************************************************************************//
void StandbyState();
void AuthenticationState();
void ResetState();
void RegistrationState();

/*****************************************************************************************/

void loop() {
  // Switch the execution based on the current state
  switch(currentState) {
    case Standby:
      StandbyState();
      break;
    case Authentication:
      AuthenticationState();
      break;
    // case Reset:
    //   Reset();
    //   break;
    // case Registration:
    //   Registration();
    //   break;
  }
}

void StandbyState(){

  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');

    if (command.equals("mfr -r")) {
      currentState = Standby;
      Serial.println("Already in Standby State.");
    } 
    
    else if (command.equals("mfr -auth")) {
      currentState = Authentication;
      Serial.println("CurrentState: Authentication.");
    } 
    
    else if (command.equals("mfr -R")) {
      currentState = Reset;
      // modeStartTime = millis();
      Serial.println("CurrentState: Reset.");
    } 
    
    else if (command.startsWith("mfr -reg")) {
      String studentID = command.substring(10);
      Serial.println(studentID);
      // Extract student ID from command
      if (studentID.length() == 10) {
        // Perform registration here by writing studentID to card
        sid = studentID;
        Serial.println(sid);
        currentState = Registration;
        // modeStartTime = millis();
        Serial.println("CurrentState: Registration.");
      } 
      else {
        Serial.println("Invalid format.");
      }

    }
    
  }

}

void AuthenticationState(){
  
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    MFRC522::StatusCode status;
    byte buffer[18];
    byte size = sizeof(buffer);

    // Prepare the key (used both as key A and as key B)
    // using FFFFFFFFFFFFh which is the default at chip delivery from the factory
    MFRC522::MIFARE_Key key;
    for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

    // Authenticate using key A
    status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 2, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("Authentication failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }

    // Read block 2
    status = (MFRC522::StatusCode)mfrc522.MIFARE_Read(2, buffer, &size);
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("Reading failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }

    // Extract student number from first 5 bytes of buffer
    String readID = "";
    for (byte i = 0; i < 5; i++) {
      readID += String(buffer[i]);
    }

    // Check if readID is in the predefined list
    bool idFound = false;
    for (int i = 0; i < sizeof(studentIDs) / sizeof(String); i++) {
      if (readID.equals(studentIDs[i])) {
        idFound = true;
        break;
      }
    }

    if (idFound) {
      Serial.println(F("Access Granted."));
    } else {
      Serial.println(F("Access Denied."));
    }

    // Halt PICC
    mfrc522.PICC_HaltA();
    // Stop encryption on PCD
    mfrc522.PCD_StopCrypto1();
  } 

  // Check if it's time to exit Authentication state
    if (Serial.available() > 0) {
        String command = Serial.readStringUntil('\n');
        if (command.equals("mfr -r")) {
            currentState = Standby;
            Serial.println("Switched to Standby state.");
        }
    }

}

void RestState(){

}

void RegistrationState(){
  
}




//*****************************************************************************************//
