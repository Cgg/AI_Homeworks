#ifndef _DUCKS_CONSTANTS_H_
#define _DUCKS_CONSTANTS_H_

namespace ducks {

///this enumeration is used to represent the species of a duck
enum ESpecies
{
    SPECIES_UNKNOWN=-1,		///< the species is unknown
    SPECIES_WHITE,			///< the duck belongs to the white (common) species
    SPECIES_BLACK,			///< the duck belongs to the black (endangered) species
    SPECIES_BLUE,			///< the duck belongs to the blue species
    SPECIES_RED,			///< the duck belongs to the red species
    SPECIES_GREEN,			///< the duck belongs to the green species
    SPECIES_YELLOW,			///< the duck belongs to the yellow species
};

///each of the three actions a bird can make
enum EAction
{
    ACTION_ACCELERATE=0,
    ACTION_KEEPSPEED=1,
    ACTION_STOP=2
};

///used to represent the current movement state of a duck
enum EMovement
{
    BIRD_STOPPED=0,
    MOVE_WEST=(1<<0),
    MOVE_EAST=(1<<1),
    MOVE_UP=(1<<2),
    MOVE_DOWN=(1<<3),
    BIRD_DEAD=(1<<4)
};

/*namespace ducks*/ }

#endif
