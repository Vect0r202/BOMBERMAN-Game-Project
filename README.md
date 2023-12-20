# BOMBERMAN-Game-Project

💣 Dive into the maze with our Arduino-based BOMBERMAN 💣 game! 🕹️ Navigate the grid, avoid 👾 enemies, and use your bombs strategically 🔲. Can you beat the highscore? 🏆 Tune in, strategize, and blast your way to victory! 🎮🔥🕵️‍♂️🏁🛠️📈🎮
  
  Introduction
    Welcome to the BOMBERMAN game project, meticulously crafted by Vect0r. This interactive and engaging game is a modern take on the classic maze and strategy game, designed to offer hours of fun and challenge. Inspired by the retro classic, this Arduino-based game         involves navigating a player through a matrix of walls, planting bombs to eliminate obstacles and adversaries while avoiding getting caught in the blast.


  📈Features📈
    -8x8 LED Matrix Display: The heart of our game's visual interface, it dynamically displays the game's status, including the player's position, walls, enemies, and bomb blasts.🔲
    -Joystick Control: Maneuver your player through the maze with a responsive joystick, making gameplay intuitive and immersive.🕹️
    -Liquid Crystal Display (LCD): Keeps you informed with real-time game stats such as lives, score, and elapsed time. Additionally, the LCD scrolls important messages and game info.📺
    -EEPROM Highscore Memory: Your top scores are saved and remembered, thanks to the EEPROM integration.
    -Multiple Game Elements: Includes walls to navigate, enemies to evade or destroy, and bombs to strategically place and detonate.👾🧱💣
    -Sound Integration: Every move, explosion, and game event is accompanied by sounds, adding an extra layer of excitement to the game (note: actual sound code implementation may vary, it is not finished yet).🔊
    
  🛠️Settings Menu🛠️
    Adjust the LCD and matrix brightness, toggle sounds, and view game details like high scores and game credits.

  🕵️‍♂️Game Over Logic🕵️‍♂️
    The game concludes with a bright light-up of the matrix, signaling the end and resetting the game for another round.
    
  Hardware Components
    -Arduino UNO
    -8x8 LED Matrix
    -Joystick Module
    -LCD 16x2
    -Buzzer (for sound effects)
    -Various resistors, wires, and breadboard for circuit assembly
  Game Logic
    The game initiates with the player at the bottom left corner of the matrix. Your objective is to move through the maze, created by walls, without getting caught by moving enemies. The player can plant bombs to destroy walls and enemies, clearing a path and scoring       points. However, if an enemy collides with the player or if the player is caught in a bomb blast, lives are lost. The game ends when the player runs out of lives or when the timer runs out.

 💻 Software Design💻
    The code is structured into several sections, each handling different aspects of the game:

      -Matrix Management: Controls the LED matrix display, including brightness and drawing game elements.
      -Player Mechanics: Manages player movement, blinking visibility for invulnerability phases, and player respawning.
      -Enemy Logic: Handles enemy movement, direction changes, and life status.
      -Bomb Mechanics: Deals with bomb placement, timing, explosion effects, and scoring.
      -LCD Interface: Manages the scrolling text, real-time game information display, and menu interactions.
      -EEPROM Integration: Ensures high scores are stored and retrieved from the Arduino's non-volatile memory.
      -Menu System: Provides the interface for starting the game, viewing high scores, adjusting settings, and learning about the game.
      -Settings Adjustments: Allows players to customize their gaming experience, including brightness and sound settings.
  Installation
    -Assemble the hardware components according to the provided schematics.
    -Upload the Arduino code to your board.
    -Ensure all connections are secure and the components are powered.
    -Start the game and navigate the menus using the joystick.
  Contributions and Contact
   😊 This project is open for contributions, improvements, and community support. Feel free to fork, submit pull requests, or reach out with suggestions. 😊

  📹 YouTube Video: https://www.youtube.com/watch?v=R4axHzi49r4
