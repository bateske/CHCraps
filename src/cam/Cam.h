// The dice cam: a full-screen view down the table to the back wall, where
// the throw plays out in 3D.
//
//   begin()    whip from the table into the cam, dice in the shooter's hand
//   (shaking)  the dice rattle in the foreground, the power bar swings
//   release()  the rules have rolled: the dice fly, bounce off the back
//              wall's pyramids, tumble and settle on the rolled numbers
//   (result)   the camera leans in on them
//   leave()    whip back to the table, which slides in under the streaks
#pragma once
#include <stdint.h>

namespace cam {

enum Phase : uint8_t { OFF, WHIP_IN, SHAKE, TUMBLE, RESULT, WHIP_OUT, ENTER };

void begin(uint8_t point);              // point: for the plate (0 = coming out)
void release(uint8_t a, uint8_t b);     // throw now (or as soon as the whip-in ends)
void cancel();                          // dice back down, no throw
void leave();                           // RESULT -> back to the table
void skip();                            // jump the tumble to its end
void result(uint8_t a, uint8_t b);      // the dice have stopped: show their sum
void update();                          // one 60 Hz tick
Phase phase();
bool active();                          // the cam owns the screen
uint16_t restT();                       // ticks since the dice came to rest
void hot(bool on);                      // a hot hand: the dice burn (set before begin())
bool burning();
// Draw the cam (true), or false if the table should draw itself; then
// tableIn() says how far (px) to slide the freshly drawn table down.
bool render(uint32_t frame);
int  tableIn();
void finishTable();                     // after the table drew: apply the slide

}  // namespace cam
