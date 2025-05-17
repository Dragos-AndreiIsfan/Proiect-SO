gcc -Wall -c thubFunctions2.c
gcc -Wall -c calculate_score.c
gcc -Wall -c ../../phase1/src/treasureHunting.c
gcc -Wall -o p treasure_hub2.c thubFunctions2.o treasureHunting.o calculate_score.o


