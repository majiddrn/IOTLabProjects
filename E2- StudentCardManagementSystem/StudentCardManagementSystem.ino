#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         0           // Configurable, see typical pin layout above
#define SS_PIN          5          // Configurable, see typical pin layout above
#define MAX_DURATION 10000
#define MAX_ID_COUNT 100

String sid;
unsigned long defined_IDs[] = {23456, 12345, 345678, 901234, 1262667};

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance


enum State {
    Standby,
    Authentication,
    Registration,
    Reset
};

State currentState = Standby; // Start in Standby state
unsigned long resetStart = 0, registrationStart = 0;
MFRC522::MIFARE_Key key;
//*****************************************************************************************//
void setup() {
  Serial.begin(9600);                                           // Initialize serial communications with the PC
  SPI.begin();                                                  // Init SPI bus
  mfrc522.PCD_Init();                                              // Init MFRC522 card
  Serial.println("CurrentState: Standby. Enter your commands! \n");
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
    case Reset:
      ResetState();
      break;
    case Registration:
      RegistrationState();
      break;
  }
}

void StandbyState(){

  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');

    if (command.equals("mfr -r")) {
      currentState = Standby;
      Serial.println("Already in Standby State. \n");
    } 
    
    else if (command.equals("mfr -auth")) {
      currentState = Authentication;
      Serial.println("CurrentState: Authentication. Place the card near the RFID module");
    } 
    
    else if (command.equals("mfr -R")) {
      currentState = Reset;
      resetStart = millis(); // Record the start time of Reset state
      Serial.println("CurrentState: Reset.");
      Serial.println("*Attention* You have only 10 seconds to put your Card on the RFID module");
    } 
    
    else if (command.startsWith("mfr -reg")) {
      String studentID = command.substring(9);
      Serial.println("CurrentState: Registration.");
      Serial.println("*Attention* You have only 10 seconds to put your Card on the RFID module");
      // Serial.println(studentID);
      // Extract student ID from command
      if (studentID.length() == 5) {
        // Perform registration here by writing studentID to card
        sid = studentID.toInt();
        Serial.print("ID: ");
        Serial.println(sid);
        currentState = Registration;
        registrationStart = millis(); // Record the start time of Registration state
        Serial.println("Entered Registration Mode.");
      }
      else Serial.println("Wrong studentID format. Please enter a 5-digit studentID. \n");
    }

    else if (command.startsWith("mfr -setlist")) {
      String idList = command.substring(13); // Extract ID list from command
      Serial.print("New Valid IDs: ");
      Serial.println(idList);
      setPredefinedIDs(idList); // Call the function to set the predefined IDs
    }
  
  }
}

void AuthenticationState() {
  
   if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    MFRC522::StatusCode status;
    byte buffer[18];
    byte size = sizeof(buffer);

    // Prepare the key (used both as key A and as key B)
    // using FFFFFFFFFFFFh which is the default at chip delivery from the factory
    MFRC522::MIFARE_Key key;
    for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

    if (mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 2, &key, &(mfrc522.uid)) == MFRC522::STATUS_OK) {
 
      // Read the student ID from the card
      if (mfrc522.MIFARE_Read(2, buffer, &size) == MFRC522::STATUS_OK) {
        Serial.print("Block2 content: ");
        for (int i = 0; i < size; i++) {
          Serial.print(buffer[i], HEX);
          Serial.print(" ");
        }
        Serial.println();

        // Converting the first 5bytes of the buffer array into a single unsigned long value, studentID.
        unsigned long studentID = 0;
        for (int i = 0; i < 5; i++) {
          // This operation combines the shift operation's result with the current value of `studentID` 
          // using a bitwise OR operation.
          studentID |= (buffer[i] << (i * 8));
        }
        
        Serial.print("studentID: ");
        Serial.println(studentID);

        // Compare the student ID with the predefined list
        if (checkStudentID(studentID)) {
          Serial.println("Student ID matches. Access Granted. \n");
        } else {
          Serial.println("Student ID does not match. Access Denied. \n");
        }
        Serial.println("Use 'mfr -r' command to switch into Standby state. \n");

      } else {
        Serial.println("Error reading data from the card.");
      }

      // Halt PICC
      mfrc522.PICC_HaltA();
      // Stop encryption on PCD
      mfrc522.PCD_StopCrypto1();
    } 
    else {
      Serial.println("Authentication failed. Unable to read the card.");
    }
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

bool checkStudentID(unsigned long studentID) {

  for (int i = 0; i < sizeof(defined_IDs) / sizeof(defined_IDs[0]); i++) {
    if (studentID == defined_IDs[i]) {
      return true;
    }
  }
  
  return false;
}

void ResetState() {
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
        // Prepare the key (used both as key A and as key B)
        MFRC522::MIFARE_Key key;
        for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

    // Authenticate using the key
    if (mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 2, &key, &(mfrc522.uid)) == MFRC522::STATUS_OK) {
      // Write the zeros to the card
      byte buffer[16];
      for (byte i = 0; i < 16; i++) {
        buffer[i] = 0;
      }

      if (mfrc522.MIFARE_Write(2, buffer, 16) == MFRC522::STATUS_OK) {
        Serial.println("Reset successful. Block 2 of the card is now zeroed. \n");
      } else {
        Serial.println("Reset failed. Unable to write to the card.");
      }

      mfrc522.PICC_HaltA();
      mfrc522.PCD_StopCrypto1();
    } 
    else {
      Serial.println("Authentication failed. Unable to write to the card.");
    }
  }

  if (millis() - resetStart >= MAX_DURATION) {
        currentState = Standby;
        Serial.println("Reset state timed out. Switched to Standby state. \n");
    }
}

void RegistrationState() {

  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
        // Prepare the key (used both as key A and as key B)
        MFRC522::MIFARE_Key key;
        for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  // Authenticate using the key
  if (mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 2, &key, &(mfrc522.uid)) == MFRC522::STATUS_OK) {
    // Write the student ID to the card
    byte buffer[16];
    for (byte i = 0; i < 16; i++) {
      buffer[i] = 0;
    }

    // Use memcpy to copy the student ID to the first 5 bytes of block #2
    memcpy(buffer, &sid, sizeof(sid));

    // Converting sid as an string to an int
    unsigned long long  INT_sid = sid.toInt();

    /* 
    Extracting individual bytes from a multi-byte integer (INT_sid) 
    and storing them in an array (buffer).
    This process is known as byte manipulation.
    */
    for (byte i = 0; i < 5; i++) {
      /* 
      [INT_sid & (0xFF << (i * 8))] -> This operation is masking all but one specific byte of INT_sid.
      The 0xFF is a byte of all 1's, and shifting it to the left by i * 8 positions it to cover the byte you're interested in.
      [>> (i*8)] -> This operation is shifting the targeted byte to the right to position it as the least significant byte.
      */
      buffer[i] = (INT_sid & (0xFF << (i * 8))) >> (i*8);
    }

    if (mfrc522.MIFARE_Write(2, buffer, 16) == MFRC522::STATUS_OK) {
      Serial.println("Registration successful. Student ID written to the card. \n");
    } else {
      Serial.println("Writing failed");
    }

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
  } 
  else {
    Serial.println("Authentication failed. Unable to write to the card.");
  }
  }

  if (millis() - registrationStart >= MAX_DURATION) {
        currentState = Standby;
        Serial.println("Registration state timed out. Switched to Standby state. \n");
    }

}

void setPredefinedIDs(String idList) {
  // Split the IDs by comma and store them in an array
  int idCount = 0;

  int pos = 0;
  while ((pos = idList.indexOf(',')) != -1) {
    String currentID = idList.substring(0, pos);
    // Check if current ID is a 5-digit number
    if (currentID.length() == 5 && currentID.toInt() <= 99999 && currentID.toInt() >= 10000) {
      defined_IDs[idCount] = currentID.toInt();
      idCount++;
    } else {
      Serial.println("Invalid ID in list. Please enter only 5-digit IDs.");
      return; // Exit the function if any ID is invalid
    }
    idList.remove(0, pos + 1);
  }
  // Check the last ID
  if (idList.length() == 5 && idList.toInt() <= 99999 && idList.toInt() >= 10000) {
    defined_IDs[idCount] = idList.toInt(); // Add the last ID
    idCount++;
  } else {
    Serial.println("Invalid ID in list. Please enter only 5-digit IDs.");
    return; // Exit the function if any ID is invalid
  }

  Serial.println("Predefined ID list updated successfully! \n");
}

//*****************************************************************************************//
