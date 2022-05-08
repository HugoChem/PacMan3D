@ Impenetrable wall
X Impenetrable invisible wall
~ Ghostly wall [Invisible] (PacMan can't pass through.)
? Ghostly wall [Visible]
# Magic wall [Invisible] (The ghosts can't pass through.)

E Ghost exit point. Should be near a spawner.

. Classic pac dot
o Power pellet

P Offset player spawn (only one)
p Fixed player spawn

H Grand spawner (Spawns the classic 8x5 ghost spawner, does NOT double as a ghost spawn point!)
h Little spawner (Spawns a 5x3 ghost spawner, AND doubles as a spawn point.)

G Ghost spawn point [Invisible]

Small spawner schematic:
XXEXX
XghgX
XXXXX

==========================
========= GHOSTS =========

Instructions:
In the other file, simply input the name of the ghosts, one line at the time.
On the next line after a ghost, input the time before it leaves the ghost spawner.

-- Classics --

Blinky 	- The chasser, chases PacMan directly.
Pinky 	- The embusher, tries to approach PacMan from the front.
Inky	- The trickster, has for target the oposite spot of blinky in relative to PacMan. [WARNING: You cannot put Inky first. He pairs up with the *previous* ghost you've inputed. Meaning you may also pair it with other ghosts!]
Clyde	- The.. stupid- Ahem, behaves like Blinky but scatters whenever he gets too close. (8 tiles away.)