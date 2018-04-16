// ###########################################################################
//          Title: Video Poker Sounds
//         Author: Mike Del Pozzo
//    Description: Sounds for Video Poker.
//        Version: 1.0.0
//           Date: 15 Apr 2018
//        License: GPLv3 (see LICENSE)
//
//    Video Poker for Gamebuino Copyright (C) 2018 Mike Del Pozzo
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    any later version.
// ###########################################################################


const int sndWin[10] =
{
  9,   // 0: Volume
  0,   // 1: Instrument ID
  0,   // 2: Volume Slide Step Duration
  0,  // 3: Volume Slide Step Depth
  3,   // 4: Pitch Slide Step Duration
  5,   // 5: Pitch Slide Step Depth
  0,   // 6: Tremolo Step Duration
  0,   // 7: Tremolo Step Depth
  16,  // 8: Pitch
  8    // 9: Duration
};

const int sndLose[10] =
{
  9,   // 0: Volume
  0,   // 1: Instrument ID
  0,   // 2: Volume Slide Step Duration
  0,  // 3: Volume Slide Step Depth
  2,   // 4: Pitch Slide Step Duration
  -2,   // 5: Pitch Slide Step Depth
  0,   // 6: Tremolo Step Duration
  0,   // 7: Tremolo Step Depth
  6,  // 8: Pitch
  4    // 9: Duration
};

const int sndFlip[10] =
{
  9,   // 0: Volume
  0,   // 1: Instrument ID
  0,   // 2: Volume Slide Step Duration
  0,  // 3: Volume Slide Step Depth
  1,   // 4: Pitch Slide Step Duration
  2,   // 5: Pitch Slide Step Depth
  0,   // 6: Tremolo Step Duration
  0,   // 7: Tremolo Step Depth
  0,  // 8: Pitch
  2    // 9: Duration
};

const int sndHold[10] =
{
  9,   // 0: Volume
  0,   // 1: Instrument ID
  0,   // 2: Volume Slide Step Duration
  0,  // 3: Volume Slide Step Depth
  1,   // 4: Pitch Slide Step Duration
  2,   // 5: Pitch Slide Step Depth
  0,   // 6: Tremolo Step Duration
  0,   // 7: Tremolo Step Depth
  0,  // 8: Pitch
  4    // 9: Duration
};

const int sndBet1[10] =
{
  9,   // 0: Volume
  0,   // 1: Instrument ID
  0,   // 2: Volume Slide Step Duration
  0,  // 3: Volume Slide Step Depth
  1,   // 4: Pitch Slide Step Duration
  2,   // 5: Pitch Slide Step Depth
  0,   // 6: Tremolo Step Duration
  0,   // 7: Tremolo Step Depth
  6,  // 8: Pitch
  4    // 9: Duration
};

const int sndBet2[10] =
{
  9,   // 0: Volume
  0,   // 1: Instrument ID
  0,   // 2: Volume Slide Step Duration
  0,  // 3: Volume Slide Step Depth
  1,   // 4: Pitch Slide Step Duration
  2,   // 5: Pitch Slide Step Depth
  0,   // 6: Tremolo Step Duration
  0,   // 7: Tremolo Step Depth
  8,  // 8: Pitch
  4    // 9: Duration
};

const int sndBet3[10] =
{
  9,   // 0: Volume
  0,   // 1: Instrument ID
  0,   // 2: Volume Slide Step Duration
  0,  // 3: Volume Slide Step Depth
  1,   // 4: Pitch Slide Step Duration
  2,   // 5: Pitch Slide Step Depth
  0,   // 6: Tremolo Step Duration
  0,   // 7: Tremolo Step Depth
  10,  // 8: Pitch
  4    // 9: Duration
};

const int sndBet4[10] =
{
  9,   // 0: Volume
  0,   // 1: Instrument ID
  0,   // 2: Volume Slide Step Duration
  0,  // 3: Volume Slide Step Depth
  1,   // 4: Pitch Slide Step Duration
  2,   // 5: Pitch Slide Step Depth
  0,   // 6: Tremolo Step Duration
  0,   // 7: Tremolo Step Depth
  12,  // 8: Pitch
  4    // 9: Duration
};

const int sndBet5[10] =
{
  9,   // 0: Volume
  0,   // 1: Instrument ID
  0,   // 2: Volume Slide Step Duration
  0,  // 3: Volume Slide Step Depth
  1,   // 4: Pitch Slide Step Duration
  2,   // 5: Pitch Slide Step Depth
  0,   // 6: Tremolo Step Duration
  0,   // 7: Tremolo Step Depth
  14,  // 8: Pitch
  4    // 9: Duration
};

const int sndResume[10] =
{
  9,   // 0: Volume
  0,   // 1: Instrument ID
  0,   // 2: Volume Slide Step Duration
  0,   // 3: Volume Slide Step Depth
  4,   // 4: Pitch Slide Step Duration
  -5,  // 5: Pitch Slide Step Depth
  0,   // 6: Tremolo Step Duration
  0,   // 7: Tremolo Step Depth
  60,  // 8: Pitch
  6    // 9: Duration
};

const int sndPause[10] =
{
  9,   // 0: Volume
  0,   // 1: Instrument ID
  0,   // 2: Volume Slide Step Duration
  0,   // 3: Volume Slide Step Depth
  4,   // 4: Pitch Slide Step Duration
  5,   // 5: Pitch Slide Step Depth
  0,   // 6: Tremolo Step Duration
  0,   // 7: Tremolo Step Depth
  55,  // 8: Pitch
  6    // 9: Duration
};

