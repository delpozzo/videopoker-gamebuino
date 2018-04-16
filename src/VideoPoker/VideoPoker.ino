// ###########################################################################
//          Title: Video Poker
//         Author: Mike Del Pozzo
//    Description: A 5-card poker game for Gamebuino Classic and MAKERbuino.
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

#include <SPI.h>
#include <Gamebuino.h>
#include <EEPROM.h>
#include "Sprite.h"
#include "Sound.h"

extern const byte font3x3[]; //a really tiny font
extern const byte font3x5[]; //a small but efficient font (default)
extern const byte font5x7[]; //a large, comfy font

// ######################################################
//    Defines, Enums, Structs, and Globals
// ######################################################

#define CARDYPOS 14
#define CARD1XPOS 2
#define CARD2XPOS 18
#define CARD3XPOS 34
#define CARD4XPOS 50
#define CARD5XPOS 66
#define YHELDPOS 37
#define YWINPOS 20
#define XWINPOS 7
#define INSTXPOS 3
#define INSTYPOS 42
#define BANKXPOS 3
#define BANKYPOS 0
#define XOFFSET 3
#define YOFFSET 3
#define FLIPTIME 5
#define WINFLASHTIME 40

enum CARDSUITS {CLUBS=0, DIAMONDS=1, HEARTS=2, SPADES=3};
enum CARDSTATES {INDECK=0, DRAWN=1, HELD=2, DISCARDED=3};
enum FACECARDS {JACK=11, QUEEN=12, KING=13, ACE=14};
enum ROUNDS {BET=0, DEAL=1, DRAW=2};
enum HANDVALUES {JACKSORBETTER=0, TWOPAIR=1, THREEOFAKIND=2, STRAIGHT=3, FLUSH=4, FULLHOUSE=5, FOUROFAKIND=6, STRAIGHTFLUSH=7, ROYALFLUSH=8, NOHAND=9};

// Card structure
typedef struct CARD_S
{
  byte suit; // CLUBS, DIAMONDS, HEARTS, or SPADES
  byte value; // 2-10, J, Q, K, or A
  byte state; // INDECK, DRAWN, HELD, or DISCARDED
  int flipTimer; // Timer for flipping card
} Card;

Card CardDeck[52]; // global array of cards
Card *Hand[5]; // current cards in play
int HandValue[5]; // int array of hand values

Gamebuino gb; //  gamebuino object
int cardSelect;
int bet;
int lastBet;
long bank;
int handResult;
int winTimer;
int winAmount;
byte currentRound;
boolean lockInput;

// ######################################################
//    Game Setup and Main Logic
// ######################################################

// Entry point
void setup()
{
  gb.begin();
  gb.titleScreen(gameLogo);
  gb.pickRandomSeed();
  startGame();
}

void startGame()
{
  bank = loadWallet();
  if(bank < 5) bank = 100;
  bet = 5;
  lastBet = 0;
  lockInput = false;
  handResult = NOHAND;
  winTimer = 0;
  winAmount = 0;

  initCardDeck();
  clearHand();
  currentRound = BET;
}

// Main game loop
void loop()
{
  if(gb.update())
  {
    updateInput();
    updateRound();
    updateDisplay();
  }
}

void updateInput()
{
  if(lockInput) 
  {
    return;
  }
  
  // Button Up - Increase Bet
  if(currentRound == BET && gb.buttons.pressed(BTN_UP))
  {
    switch(bet)
    {
      case 5: bet = 10; playSound(sndBet2, 0);
          break;
      case 10: bet = 25; playSound(sndBet3, 0);
          break;
      case 25: bet = 50; playSound(sndBet4, 0);
          break;
      case 50: bet = 100; playSound(sndBet5, 0);
          break;
      case 100: bet = 5; playSound(sndBet1, 0);
          break;                           
    }

    if(bet > bank)
    {
      bet = 5;
      playSound(sndBet1, 0);
    }
  }
  
  // Button Down - Decrese Bet
  if(currentRound == BET && gb.buttons.pressed(BTN_DOWN))
  {
    switch(bet)
    {
      case 5: bet = 100; playSound(sndBet5, 0);
          break;
      case 10: bet = 5; playSound(sndBet1, 0);
          break;
      case 25: bet = 10; playSound(sndBet2, 0);
          break;
      case 50: bet = 25; playSound(sndBet3, 0);
          break;
      case 100: bet = 50; playSound(sndBet4, 0);
          break;                           
    }

    if(bet > bank)
    {
      bet = 5;
      playSound(sndBet1, 0);
    }
  }
      
  // Button Left - Card select left
  if(currentRound == DEAL && gb.buttons.pressed(BTN_LEFT))
  {
    cardSelect--;
    if(cardSelect < 1)
    {
      cardSelect = 1;
    }   
  }

  // Button Right - Card select right
  if(currentRound == DEAL && gb.buttons.pressed(BTN_RIGHT))
  {
    cardSelect++;
    if(cardSelect > 5)
    {
      cardSelect = 5;
    }
  }
  
  // Button A - Hold Card
  if(currentRound == DEAL && gb.buttons.pressed(BTN_A))
  {
    if(cardSelect > 0 && Hand[cardSelect-1] != NULL)
    {
      playSound(sndHold, 0);
      if(Hand[cardSelect-1]->state == HELD)
      {
        Hand[cardSelect-1]->state = DRAWN;
      }
      else
      {
        Hand[cardSelect-1]->state = HELD;
      }
    }
  }
    
  // Button B - Deal / Draw
  if(gb.buttons.pressed(BTN_B))
  {
    if(currentRound == BET)
    {
      clearHand();
      bank -= bet;
      lastBet = bet;
      winAmount = 0;
      dealCards();
      currentRound = DEAL;
    }
    else if(currentRound == DEAL)
    {
      currentRound = DRAW;
      drawCards();
    }
  }
    
  // Button C - Pause Game
  if(gb.buttons.pressed(BTN_C))
  {
    playSound(sndPause, 0);
    pauseGame();
  }
}

void updateRound()
{
  if(bank < 5 && currentRound == BET)
  {
    playSound(sndLose, 0);
    gameOver();
  }

  if(bet > bank)
  {
    bet = 5;
  }  
  
  if(currentRound == DRAW)
  {
    lockInput = true;

    if(Hand[0]->flipTimer <= 0 && Hand[1]->flipTimer <= 0 && Hand[2]->flipTimer <= 0 && Hand[3]->flipTimer <= 0 && Hand[4]->flipTimer <= 0)
    {
      lockInput = false;
      currentRound = BET;
    }
  }

  if(currentRound == DEAL)
  {
    lockInput = true;

    if(Hand[0]->flipTimer <= 0 && Hand[1]->flipTimer <= 0 && Hand[2]->flipTimer <= 0 && Hand[3]->flipTimer <= 0 && Hand[4]->flipTimer <= 0)
    {
      lockInput = false;
    }
  }  
}

void displayCards()
{
  switch(cardSelect)
  {
    case 0: break;
    case 1: gb.display.drawBitmap(CARD1XPOS, CARDYPOS, cardBorder, NOROT, NOFLIP);
            break;
    case 2: gb.display.drawBitmap(CARD2XPOS, CARDYPOS, cardBorder, NOROT, NOFLIP);
            break;
    case 3: gb.display.drawBitmap(CARD3XPOS, CARDYPOS, cardBorder, NOROT, NOFLIP);
            break;
    case 4: gb.display.drawBitmap(CARD4XPOS, CARDYPOS, cardBorder, NOROT, NOFLIP);
            break;
    case 5: gb.display.drawBitmap(CARD5XPOS, CARDYPOS, cardBorder, NOROT, NOFLIP);
            break;
  }
  
  for(int i = 0; i < 5; i++)
  {
    int x;
    
    switch(i)
    {
      case 0: x = CARD1XPOS;
              break;
      case 1: x = CARD2XPOS;
              break;
      case 2: x = CARD3XPOS;
              break;
      case 3: x = CARD4XPOS;
              break;
      case 4: x = CARD5XPOS;
              break;
    }

    if(Hand[i] != NULL)
    {
      if(Hand[i]->flipTimer > 0)
      {
        Hand[i]->flipTimer--;
        gb.display.drawBitmap(x, CARDYPOS, cardBack, NOROT, NOFLIP);
        if(Hand[i]->flipTimer == 1) playSound(sndFlip, 0);
        continue;
      }
      
      gb.display.cursorY = CARDYPOS + YOFFSET;
      gb.display.cursorX = x + XOFFSET;
      
      if(Hand[i]->value > 10)
      {
        switch(Hand[i]->value)
        {
          case JACK: gb.display.print(F("J"));
              break;
          case QUEEN: gb.display.print(F("Q"));
              break;
          case KING: gb.display.print(F("K"));
              break;
          case ACE: gb.display.print(F("A"));
              break;
        }
      }
      else
      {
        gb.display.print(Hand[i]->value);
      }

      switch(Hand[i]->suit)
      {
        case CLUBS: gb.display.drawBitmap(x, CARDYPOS, cardClub, NOROT, NOFLIP);
                  break;
        case DIAMONDS: gb.display.drawBitmap(x, CARDYPOS, cardDiamond, NOROT, NOFLIP);
                  break;
        case HEARTS: gb.display.drawBitmap(x, CARDYPOS, cardHeart, NOROT, NOFLIP);
                  break;
        case SPADES: gb.display.drawBitmap(x, CARDYPOS, cardSpade, NOROT, NOFLIP);
                  break;
      }

      if(Hand[i]->state == HELD)
      {
        gb.display.setFont(font3x3);
        gb.display.cursorY = YHELDPOS;
        gb.display.cursorX = x + XOFFSET - 2;
        gb.display.print(F("hold"));
        gb.display.setFont(font3x5);
      }
    }
    else
    {
      gb.display.drawBitmap(x, CARDYPOS, cardBack, NOROT, NOFLIP);
    }
  }
}

void displayWin()
{
  if(winTimer < WINFLASHTIME>>1)
  {
    displayCards();
  }
  else
  {
    gb.display.cursorY = YWINPOS;
    gb.display.cursorX = XWINPOS;
    
    switch(handResult)
    {
      case JACKSORBETTER:
               gb.display.println(F("Jacks or better!"));
               break;
      case TWOPAIR:
               gb.display.println(F("Two pair!"));
               break;
      case THREEOFAKIND:
               gb.display.println(F("Three of a kind!"));
               break;
      case STRAIGHT:
               gb.display.println(F("Straight!"));
               break;
      case FLUSH:
               gb.display.println(F("Flush!"));
               break;
      case FULLHOUSE:
               gb.display.println(F("Full House!"));
               break;
      case FOUROFAKIND:
               gb.display.println(F("Four of a kind!"));
               break;
      case STRAIGHTFLUSH:
               gb.display.println(F("Straight flush!"));
               break;
      case ROYALFLUSH:
               gb.display.println(F("Royal flush!"));
               break;
    }

    gb.display.cursorX = XWINPOS;
    gb.display.cursorY = YWINPOS+8;
    gb.display.print(F("Bet $"));
    gb.display.print(lastBet);
    gb.display.print(F(" Won $"));
    gb.display.print(winAmount);
  }
  
  winTimer++;
  if(winTimer > WINFLASHTIME)
  {
    playSound(sndWin, 0);
    winTimer = 0;
  }
}

void updateDisplay()
{
  gb.display.cursorY = BANKYPOS;
  gb.display.cursorX = BANKXPOS;
  gb.display.print(F("Bank: $"));
  gb.display.print(bank);

  gb.display.cursorY = BANKYPOS + 7;
  gb.display.cursorX = BANKXPOS;
  gb.display.print(F(" Bet: $"));
  gb.display.print(bet);
  
  if(handResult == NOHAND) displayCards();
  else displayWin();

  if(currentRound == BET)
  {
    gb.display.cursorY = INSTYPOS;
    gb.display.cursorX = INSTXPOS;
    gb.display.print(F("Up/Down:Bet  B:Deal"));
  }
  else if(currentRound == DEAL)
  {
    gb.display.cursorY = INSTYPOS;
    gb.display.cursorX = INSTXPOS;
    gb.display.print(F("A:Hold  B:Draw"));    
  }
}

// ######################################################
//    Pause, Game Over, and Save/Load Functions
// ######################################################

// Show pause menu
void pauseGame()
{ 
  while(1)
  {
    if(gb.update())
    {
        gb.display.cursorY = 18;
        gb.display.cursorX = 20;
        gb.display.println(F("P A U S E D"));

        gb.display.cursorY = 30;
        gb.display.println(F("A: Title Screen"));
        gb.display.println(F("B: Show Hand Info"));
        gb.display.println(F("C: Resume Game"));
        
        // DEBUG START
        // Show card values in pause
        /*
        gb.display.cursorY = 0;
        for(int i = 0; i < 5; i++)
        {
          gb.display.print(HandValue[i]);
          gb.display.print(F(" "));
        }
        */
        //DEBUG END
        
        // Button A - Return to Title Screen
        if(gb.buttons.pressed(BTN_A))
        {
          setup();
          return;
        }

        // Button C - Resume Game
        if(gb.buttons.pressed(BTN_C))
        {
          playSound(sndResume, 0);
          return;
        }

        // Button B - Show Hand Info
        if(gb.buttons.pressed(BTN_B))
        {
          showHandInfo();
        }
      }
   }
}

void showHandInfo()
{
  byte page = 0;
  
  while(1)
  {
    if(gb.update())
    {
        gb.display.cursorY = 0;
        gb.display.cursorX = 0;
        switch(page)
        {
          case 0:
          gb.display.println(F("Royal Flush:250*Bet"));
          gb.display.println(F("Straight Flush:50*Bet"));
          gb.display.println(F("Four of a Kind:25*Bet"));
          gb.display.println(F("Full House:9*Bet"));
          gb.display.println(F("Flush:6*Bet"));
          gb.display.println(F(""));
          gb.display.println(F("A: Next Page"));
          gb.display.println(F("C: Back"));
          break;

          case 1:
          gb.display.println(F("Straight:4*Bet"));
          gb.display.println(F("Three of a Kind:3*Bet"));
          gb.display.println(F("Two Pair:2*Bet"));
          gb.display.println(F("Jacks or Better:1*Bet"));
          gb.display.println(F(""));
          gb.display.println(F(""));
          gb.display.println(F("A: Prev Page"));
          gb.display.println(F("C: Back"));
          break;
        }

        // Button A - Next Page
        if(gb.buttons.pressed(BTN_A))
        {
          page = 1 - page;
        }
        // Button C - Back
        if(gb.buttons.pressed(BTN_C))
        {
          return;
        }
      }
   }
}

// Show Game Over
void gameOver()
{ 
  while(1)
  {
    if(gb.update())
    {
        gb.display.cursorY = 18;
        gb.display.cursorX = 10;
        gb.display.println(F("G A M E  O V E R"));

        gb.display.cursorY = 30;
        gb.display.println(F("A: Title Screen"));
        gb.display.println(F("B: New Game"));

        // Button A - Return to Title Screen
        if(gb.buttons.pressed(BTN_A))
        {
          setup();
          return;
        }
      
        // Button B - New Game
        if(gb.buttons.pressed(BTN_B))
        {
          startGame();
          return;
        }
      }
   }
}

void saveWallet()
{
  //Decomposition from a long to 4 bytes by using bitshift.
  //One = Most significant -> Four = Least significant byte
  byte four = (bank & 0xFF);
  byte three = ((bank >> 8) & 0xFF);
  byte two = ((bank >> 16) & 0xFF);
  byte one = ((bank >> 24) & 0xFF);
  
  //Write the 4 bytes into the eeprom memory.
  EEPROM.write(0, four);
  EEPROM.write(1, three);
  EEPROM.write(2, two);
  EEPROM.write(3, one);
}

long loadWallet()
{
  //Read the 4 bytes from the eeprom memory.
  long four = EEPROM.read(0);
  long three = EEPROM.read(1);
  long two = EEPROM.read(2);
  long one = EEPROM.read(3);
  
  //Return the recomposed long by using bitshift.
  return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

// ######################################################
//    Sound Functions
// ######################################################

void playSound(const int *snd, byte channel)
{
  gb.sound.command(0, snd[0], 0, channel);      // 0: Set note volume
  gb.sound.command(1, snd[1], 0, channel);      // 1: Select instrument
  gb.sound.command(2, snd[2], snd[3], channel); // 2: Volume slide
  gb.sound.command(3, snd[4], snd[5], channel); // 3: Pitch slide
  gb.sound.command(4, snd[6], snd[7], channel); // 4: Tremolo
  
  gb.sound.playNote(snd[8], snd[9], channel);   // Play note (pitch, duration, channel)
}

// ######################################################
//    Card Functions
// ######################################################

// Initializes the CardDeck[]
void initCardDeck()
{
  byte suit = 0;
  
  for(int i = 0; i < 52; i++)
  {
    CardDeck[i].suit = suit;
    CardDeck[i].value = 2+(i%13);
    CardDeck[i].state = INDECK;
    CardDeck[i].flipTimer = 0;

    if(2+(i%13) == 14) suit++;
  }
}

// Clears the Hand[] and puts cards back in deck
void clearHand()
{
  cardSelect = 0;
  winTimer = 0;
  handResult = NOHAND;
  
  for(int i = 0; i < 5; i++)
  {
    Hand[i] = NULL;
  }

  for(int j = 0; j < 52; j++)
  {
    CardDeck[j].state = INDECK;
    CardDeck[j].flipTimer = 0;
  }
}

Card* pickRandomCard()
{
  PICKCARD:
  int card = random(52);

  if(CardDeck[card].state == INDECK)
  {
    CardDeck[card].state = DRAWN;
    return &CardDeck[card];
  }
  else
  {
    goto PICKCARD;
  }
  
  return NULL;
}

void dealCards()
{
  cardSelect = 0;
  
  for(int i = 0; i < 5; i++)
  {
    Hand[i] = pickRandomCard();
    Hand[i]->flipTimer = (1+i)*FLIPTIME;
  }
}

void drawCards()
{
  int drawCtr = 1;
  cardSelect = 0;
  
  for(int i = 0; i < 5; i++)
  {
    if(Hand[i]->state == DRAWN)
    {
      Hand[i]->state = DISCARDED;
      Hand[i] = pickRandomCard();
      Hand[i]->flipTimer = drawCtr*FLIPTIME;
      drawCtr++;
    }
  }
  
  handResult = checkHand();
  saveWallet();
}

void sortCards()
{
  qsort(HandValue, 5, sizeof(int), valueCompare);
}

int valueCompare(const void * a, const void * b)
{
    return ( *(int*)a - *(int*)b );
}

int checkHand()
{
  for(int i = 0; i<5; i++)
  {
    HandValue[i] = Hand[i]->value;
  }
  
  sortCards();

  if(isRoyalFlush()) 
  {
    winAmount = 250 * lastBet;
    bank += winAmount;
    return ROYALFLUSH;
  }
  if(isStraightFlush()) 
  {
    winAmount = 50 * lastBet;
    bank += winAmount;
    return STRAIGHTFLUSH;
  }
  if(isFourOfAKind()) 
  {
    winAmount = 25 * lastBet;
    bank += winAmount;
    return FOUROFAKIND;
  }
  if(isFullHouse()) 
  {
    winAmount = 9 * lastBet;
    bank += winAmount;
    return FULLHOUSE;
  }
  if(isFlush()) 
  {
    winAmount = 6 * lastBet;
    bank += winAmount;
    return FLUSH;
  }
  if(isStraight()) 
  {
    winAmount = 4 * lastBet;
    bank += winAmount;
    return STRAIGHT;
  }
  if(isThreeOfAKind())
  {
    winAmount = 3 * lastBet;
    bank += winAmount;
    return THREEOFAKIND;
  }
  if(isTwoPair())
  {
    winAmount = 2 * lastBet;
    bank += winAmount;
    return TWOPAIR;
  }
  if(isJacksOrBetter())
  {
    winAmount = lastBet;
    bank += winAmount;
    return JACKSORBETTER;
  }
  return NOHAND;
}

bool isJacksOrBetter()
{
  for(int i = 0; i < 4; i++)
  {
    if(HandValue[i] == HandValue[i+1] && HandValue[i] >= JACK) return true;
  }
  return false;
}

bool isTwoPair()
{
  if((HandValue[0] == HandValue[1] && HandValue[2] == HandValue[3]) ||
    (HandValue[1] == HandValue[2] && HandValue[3] == HandValue[4]) ||
    (HandValue[0] == HandValue[1] && HandValue[3] == HandValue[4])) return true;
  return false;
}

bool isThreeOfAKind()
{
  for(int i = 0; i < 3; ++i)
  {
    if((HandValue[i] == HandValue[i+1]) && (HandValue[i+1] == HandValue[i+2])) return true;
  }
  return false;
}

bool isStraight()
{
  if(HandValue[4] == HandValue[3]+1 && HandValue[3]+1 == HandValue[2]+2 && HandValue[2]+2 == HandValue[1]+3 && HandValue[1]+3 == HandValue[0]+4) return true;
  if(HandValue[4] == ACE && HandValue[0] == 2 && HandValue[1] == 3 && HandValue[2] == 4 && HandValue[3] == 5) return true;
  return false;
}

bool isFlush()
{
  if(Hand[0]->suit == Hand[1]->suit && Hand[1]->suit == Hand[2]->suit && Hand[2]->suit == Hand[3]->suit
  && Hand[3]->suit == Hand[4]->suit) return true;
  return false;
}

bool isFullHouse()
{
  if((HandValue[0] == HandValue[1] && HandValue[1] == HandValue[2] && HandValue[3] == HandValue[4]) ||
      (HandValue[0] == HandValue[1] && HandValue[2] == HandValue[3] && HandValue[3] == HandValue[4])) return true;
  return false;
}

bool isFourOfAKind()
{
  if((HandValue[0] == HandValue[1] && HandValue[1] == HandValue[2] && HandValue[2] == HandValue[3]) || 
    (HandValue[1] == HandValue[2] && HandValue[2] == HandValue[3] && HandValue[3] == HandValue[4])) return true;
  return false;
}

bool isStraightFlush()
{
  if(isStraight() && isFlush()) return true;
  return false;
}

bool isRoyalFlush()
{
  if(isFlush() && HandValue[0] == 10 && HandValue[1] == JACK && HandValue[2] == QUEEN
    && HandValue[3] == KING && HandValue[4] == ACE) return true;
  return false;
}

void testHand(int hand)
{
  switch(hand)
  {
    case STRAIGHT:
      Hand[0] = &CardDeck[25]; Hand[0]->state = DRAWN;
      Hand[1] = &CardDeck[1]; Hand[1]->state = DRAWN;
      Hand[2] = &CardDeck[2]; Hand[2]->state = DRAWN;
      Hand[3] = &CardDeck[3]; Hand[3]->state = DRAWN;
      Hand[4] = &CardDeck[0]; Hand[4]->state = DRAWN;
      break;
    case FLUSH:
      Hand[0] = &CardDeck[13]; Hand[0]->state = DRAWN;
      Hand[1] = &CardDeck[15]; Hand[1]->state = DRAWN;
      Hand[2] = &CardDeck[17]; Hand[2]->state = DRAWN;
      Hand[3] = &CardDeck[18]; Hand[3]->state = DRAWN;
      Hand[4] = &CardDeck[19]; Hand[4]->state = DRAWN;
    break;
    case FULLHOUSE:
      Hand[0] = &CardDeck[14]; Hand[0]->state = DRAWN;
      Hand[1] = &CardDeck[13]; Hand[1]->state = DRAWN;
      Hand[2] = &CardDeck[27]; Hand[2]->state = DRAWN;
      Hand[3] = &CardDeck[1]; Hand[3]->state = DRAWN;
      Hand[4] = &CardDeck[0]; Hand[4]->state = DRAWN;
    break;
    case FOUROFAKIND:
      Hand[0] = &CardDeck[0]; Hand[0]->state = DRAWN;
      Hand[1] = &CardDeck[13]; Hand[1]->state = DRAWN;
      Hand[2] = &CardDeck[26]; Hand[2]->state = DRAWN;
      Hand[3] = &CardDeck[14]; Hand[3]->state = DRAWN;
      Hand[4] = &CardDeck[39]; Hand[4]->state = DRAWN;
    break;
    case STRAIGHTFLUSH:
      Hand[0] = &CardDeck[0]; Hand[0]->state = DRAWN;
      Hand[1] = &CardDeck[1]; Hand[1]->state = DRAWN;
      Hand[2] = &CardDeck[2]; Hand[2]->state = DRAWN;
      Hand[3] = &CardDeck[3]; Hand[3]->state = DRAWN;
      Hand[4] = &CardDeck[4]; Hand[4]->state = DRAWN;
      break;
    case ROYALFLUSH:
      Hand[0] = &CardDeck[9]; Hand[0]->state = DRAWN;
      Hand[1] = &CardDeck[10]; Hand[1]->state = DRAWN;
      Hand[2] = &CardDeck[11]; Hand[2]->state = DRAWN;
      Hand[3] = &CardDeck[12]; Hand[3]->state = DRAWN;
      Hand[4] = &CardDeck[8]; Hand[4]->state = DRAWN;
      break;
  }
}

