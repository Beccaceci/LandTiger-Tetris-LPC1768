🧱 LandTiger Tetris - LPC1768 Embedded System
A high-performance, real-time Tetris implementation developed for the LandTiger NXP LPC1768 (ARM Cortex-M3) development board. This project demonstrates advanced embedded programming techniques, including hardware timer interrupts, DAC audio synthesis, and real-time ADC signal processing.
+3

📖 Project Description
This project aims to enhance a standard Tetris clone with advanced hardware-integrated features for the LandTiger board. It addresses the challenge of managing complex game logic alongside high-frequency audio generation and analog sensor polling on a resource-constrained microcontroller.
+3

The project is intended for students and developers interested in ARM Cortex-M3 architectures, real-time operating principles, and hardware peripheral management. The goal is to provide a responsive, "feature-complete" gaming experience that utilizes every major peripheral on the LandTiger board.

✨ Features
Core Gameplay: Implements standard Tetris mechanics, including piece rotation, horizontal movement, and line clearing.
ADC Speed Control: Game speed is dynamically adjusted via the board's potentiometer, ranging from 1 to 5 squares per second.
Dual-Timer Audio Engine: Features a non-blocking background music system playing Ode to Joy and a melodramatic Funeral March for Game Over.
Power-Up System: Every 5 lines cleared spawns a random power-up:
Clear Half: Wipes the bottom half of the playing field, awarding points only for rows containing blocks.
Slow Down: Forces the game to 1 square/second for a 15-second duration.
Random Malus: Every 10 cleared lines adds a randomized row at the bottom to increase difficulty.
Pause/Resume Logic: Allows the user to freeze gameplay at any time using on-board buttons.
🛠️ Technologies and Tools Used
C Language: Used for high-performance firmware development.
ARM Keil uVision 5: The primary IDE for compilation, debugging, and flash management.
+1
LPC1768 (Cortex-M3): The target hardware platform.
LandTiger Emulator: Used for initial logic testing before hardware deployment.
DAC (Digital-to-Analog Converter): For real-time sound generation via pin P0.26.
📥 Installation Instructions
Clone the Repository:
Bash
git clone https://github.com/YOUR_USERNAME/LandTiger-Tetris-LPC1768.git
Open Project: Open the .uvproj file in Keil uVision 5.
Prepare Hardware: Connect your LandTiger board via the debugger.
Important Jumper Setup: Ensure the JP2 jumper is closed to enable the speaker output.
🎮 Usage Guide
Start Game: Press KEY1 to start the game from the idle screen.
Controls:
Joystick Left/Right: Move the falling piece.
Joystick Up: Rotate the piece.
Joystick Down: Enable soft-drop (doubles current speed).
Potentiometer: Rotate to change the base falling speed.
KEY2: Perform a hard-drop.
Pause: Press KEY1 during gameplay to pause.
📂 Project Structure
/game: Contains the core Tetris engine (tetris.c), piece definitions, and graphics rendering.
/music: Houses the audio engine (lib_music.c) and note frequency definitions.
/GLCD: Low-level drivers for the TFT LCD.
/timer: Hardware timer abstraction layer for managing T0, T1, T2, and T3.
/adc: Potentiometer sampling and ADC interrupt management.
/RIT: Repetitive Interrupt Timer logic for joystick polling.
⚙️ Configuration
The game tempo and audio articulation can be customized in music.h:
RHYTHM_TICK: Change this value to adjust the global tempo of the background music.
Articulation Ratio: Adjust the 80/20 play-to-gap ratio in IRQ_timer.c to change how "staccato" the notes sound.
🧪 Testing
As specified in the project requirements, the music logic cannot be fully tested on the emulator and requires physical hardware for verification.
+1
Logic Testing: Use the LandTiger Emulator to verify piece collisions and power-up triggers.
Audio Testing: Deploy to the physical LPC1768 board to verify the DAC output on pin P0.26.