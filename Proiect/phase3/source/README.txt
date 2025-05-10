Created the calculate_score method for phase3

Also included the files used to test both the set, for the calculate score function works by using a set data structure, since the hunts have only 1 user per treasure, the same user can have multiple treasures at different treasure IDs, and, using the set we can calculate the score of it.
Also, the set has been tested AND the calculate score method on the binary files BIN01 and BIN02 from hunt BIN1, also added the same user, named user1 to BIN1 with value 100, and after calling the calculate score function, user1 has 101 score, for he has value 100 in treasure BIN01 and value 1, in treasure BIN02.
The change however has been made locally, only on my laptop drive and the changed binary files have not been updated on github.
Also, it was a good opportunity to test again the treasure manager after some weeks from the phase1.
treasureHunting.o has no use, there was an attempt to include the structures GPSCoordinates from treasureHunt and the defined constants for the size of the fields from the treasure structure, however in the end, they have been declared in the calculate_score.c file and used to work make the function work.
