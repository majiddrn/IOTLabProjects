## In this exercise, we have two boards. 

**Server board:** 
  - as `Access Point`
  - running a `Web Server` 
  - connected with a `7-Color LED` 
  - connected with a `RGB LED`
<br>

**Client board:** 
  - Connected to Server board via `Wi-Fi`
  - Connected with a `Touch Sensor`

## Logic we implemented for this excersie:

  1. The client board receives a color code through `serial communication` and sends it to the server board through a `POST` request, Then the server board displays that color code (Hexadecimal) on the RGB LED.

  2. By touching the touch sensor, the client board sends a `POST` request to the server board to toggle the `7-color LED`, and the server board receives and executes this request.

  3. Get the current state of the two LEDs with `GET` methods. `Postman` is used to see the status.

  4. Error handling 
    - `Invalid request(Wrong Body)`, the server will send `404` along with the appropriate message.
    - `No connection` to the network .
    -  When `server is down`. 
