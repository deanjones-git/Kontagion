# Kontagion
My implementation of Project 3 for CS 32 - Kontagion, where Socrates Nguyen bravely fights off Dr. Evyl's formidable bacteria army.

![Image of Gameplay](https://i.imgur.com/WIXafmi.png)
After cloning into this repository, you can start playing the game by opening Kontagion.sln in Visual Studio and hitting Run.

The controls for the game are simple - → key moves Socrates clockwise, ← counter-clockwise. The space key fires Socrate's Spray, which replenishes automatically. The Enter key triggers Socrate's radial Flame blast, which does not replenish automatically.    
<img src="https://i.imgur.com/5Hudw7O.png" width="40" height="40">

The goal for each level is to exterminate all the bacteria. Dirt piles randomly generated within the environment provide useful barriers between Socrates and Bacteria, as Bacteria cannot destroy them. Socrates' Spray and Flame can however, so consider yourself forewarned.  
<img src="https://i.imgur.com/ppGItdu.png" width="40" height="40">

Bacteria spawn from Holes, which increase in number as the levels advance. When a Hole disappears, it has emptied all its bacteria; until then, it will keep releasing them.  
<img src="https://i.imgur.com/UNzdZtc.png" width="40" height="40">

There are 3 kinds of Bacteria: Salmonella, Aggressive Salmonella, and Ecoli. Salmonella and Aggressive Salmonella are visually indistinguishable, but Aggressive Salmonella pursue Socrates if he is close. Otherwise, both types of Salmonella will move semi-randomly and eat food (Pizza). If enough Pizza is eaten, Bacteria will duplicate. Ecoli are more single-minded and pursue Socrates no matter what, meaning they can become stuck if Dirt is in between but otherwise are quite formidable.
<p float="left" padding>
  <img src="https://i.imgur.com/g4vAY8r.png" width="40" height="40">
  <img src="https://i.imgur.com/L9hV4J4.png" width="40" height="40">
</p>

To aid Socrates on his mission, Goodie objects will randomly spawn from time to time. There are Extra Life Goodies, Flamethrower Goodies, and Restore Health Goodies, all fairly self-explanatory.
<p float="left">
  <img src="https://i.imgur.com/DPj64IN.png" width="40" height="40">
  <img src="https://i.imgur.com/OqnOV5k.png" width="40" height="40">
  <img src="https://i.imgur.com/0L40Nxh.png" width="40" height="40">
</p>

However, Fungus may also spawn from time to time. These appear on the perimeter of the dish and may only be cleared away by Flame blasts. 
<img src="https://i.imgur.com/dfCIPBN.png" width="40" height="40">

If you are interested in the implementation for various Game Actors, I encourage you to read Kontagion/report.docx. This details every method for each Actor and how I ensured functionality. If you'd like to tinker around with the numbers or are just curious how much an Ecoli decreases your health by or how often a Flamethrower Goodie spawns, I highly recommend you read it as well.

Happy hunting!
