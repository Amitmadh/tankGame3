חגי לב הכהן - 322996059
עמית מדהלה - 211779582

How the backward rule work:
When a tank performs a backward action, the tank afterwards has 2 chances to cancel it. It can cancel it ONLY with a forward movement.
Any other actions, will be ignored.
So for example, if a tank performs backward movement on step 1. Then, it will have a chance to cancel it with a forward movement, on either step 2 or step 3.
If it didn't cancel it on either step 2 or step 3, it will perform the backward movement on step 3.

Note:
Each output file ends in an endline (\n) character.


input format (as stated in the instructions):
Line 1: map name / description - internal for the file, can be ignored by the game manager
Line 2: MaxSteps for the game on this map, as: MaxSteps = <NUM>
Line 3: NumShells per tank, as: NumShells = <NUM>
Line 4: Number of Rows in house, as: Rows = <NUM>
Line 5: Number of Cols in house, as: Cols = <NUM>
Lines 6 and on: the map itself, as described below


output format (as stated in the instructions):
Tanks are “sequenced” by their creation order: top to bottom, left to right on the board.
The output file contains one line per game round, listing all tank actions in order, comma-separated.
If a tank is killed, its action for that round is marked with (killed), and future rounds show killed.
If an action is invalid, it's marked (ignored).
Action names follow the enum.
The final line announces the winner in one of these formats:
Player <X> won with <Y> tanks still alive
Tie, both players have zero tanks
Tie, reached max steps = <max_steps>, player 1 has <X> tanks, player 2 has <Y> tanks
Tie, both players have zero shells for <40> steps