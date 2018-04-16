# Video Poker

## About

A 5-card poker game for [Gamebuino Classic](http://gamebuino.com) and [MAKERbuino](http://makerbuino.com/).

![Screenshot](src/videopoker.gif?raw=true)

## Installation

Copy `POKER.HEX` and `POKER.INF` from the `bin/` folder to your SD card.

## Instructions

### Controls

- **Left, Right** - Card selection.
- **Up, Down** - Increase / Decrease bet.
- **A Button** - Hold / Unhold card.
- **B Button** - Deal / Draw.
- **C Button** - Pause menu.

### Gameplay

During the bet round, use **Up** or **Down** to increase or decrease your bet. The minimum bet is $5 and the maximum is $100. Press **B** to deal the cards. Select cards using **Left** or **Right**. Press **A** to toggle Hold on the selected card. Press **B** to exchange unheld cards for new ones from the deck. 

Winning hands and rewards are as follows:

**Royal Flush:** 250 x Bet

**Straight Flush:** 50 x Bet

**Four of a Kind:** 25 x Bet

**Full House:** 9 x Bet

**Flush:** 6 x Bet

**Straight:** 4 x Bet

**Three of a Kind:** 3 x Bet

**Two Pair:** 2 x Bet

**Pair of Jacks or Better:** 1 x Bet

### License

Video Poker for Gamebuino (C) 2018 Mike Del Pozzo

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or any later version.

See [LICENSE](LICENSE) for details.
