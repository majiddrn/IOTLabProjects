# StudentCardManagementSystem
This project provides an implementation of an RFID card reader and writer using the MFRC522 module. The system has four states: *Standby*, *Authentication*, *Reset* and *Registration*. The state changes based on commands received from `the serial port`.

The block of RFID cards we'll work with is block `number 2`, therefore we won't change the rest of the blocks. <br> 
Also, there is no need to change the keys and access level of the blocks.

## Requirements

The project requires the following Arduino libraries:

    SPI.h
    MFRC522.h

## Hardware

    Arduino IDE (or platformIO extension on VScode)
    ESP32 board
    MFRC522 RFID module
    RFID Cards
    Jumper wires


<br>

## State description
###  1. **Standby** <br>
    
   > Standby state is the base state of the system. To enter this state from other states, we'll use the proper command. <br> In this state, the RFID module is disabled.

  Here, we process the `commands` received from the serial port.

 - `mfr -r:` leads us to Standby state.
 - `mfr -R:` leads us to Reset state. 
 - `mfr -auth:` leads us to Authentication state. 
 - `mfr -reg [id]:` leads us to Registration state with a given studentID.
 - `mfr -setlist [id,id,...]` gives us premition to define valid IDs while running the code. 

  ### 2. **Reset** <br>

  > In this state, by placing a card on the RFID module, block number 2 becomes `completely zero`.

  We'll only be in this mode for `10 seconds` and after this time we'll return to `Standby state`.

  ### 3. **Registration** <br>

  > Here, by placing a card on the RFID module, the studentID sent by `mfr -reg [id]` command will be written in block number 2. 

  The studentID must be written as a number, not an ascii strings. It also must only occupy space in the `first 5 bytes` of block 2.

  We'll only be in this mode for `10 seconds` and after this time we'll return to `Standby state`.

  ### 4. **Authentication** <br>

  > By placing a card on the module, the studentID written in the first 5 bytes of block number 2 `is read`. <br>
  Then the ID will be checked by a predefined list (hard coded) and it will show the proper message.

  In this state we don't have any time limitation. <br> To exit this state use the `mfr -r` command.

  ### 5. **setPredefinedIDs** <br>

  > The `setPredefinedIDs` function is designed to handle the `mfr -setlist` command and update the predefined ID list in the system. 

  When the `mfr -setlist` command is received, this function is invoked with a list of IDs as a string. Each ID in the list is separated by a   `comma`.    The function splits the ID list by comma and checks each ID to ensure it's a `5-digit number`.

  If all IDs in the list are valid, the function updates the predefined ID list.

