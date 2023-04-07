/*
    Library to get the serialized story files

    This is done in pure C.

    I use esterTion code
        https://github.com/esterTion/unity-texture-toolkit
    and satrjet
        https://github.com/starjet/derestage_storydecode
    as a base to do the whole library

    Also you will need to decode base64 text...
    */

#ifndef _REDIVE_DESER_H_
#define _REDIVE_DESER_H_

#include <stdio.h>

// This is praimary used in every argument
// and if you read a little bit more you will have an idea
enum categoryNumber
{
   NONE = -1,
   TITLE = 0,
   OUTLINE = 1,
   VISIBLE = 2,
   FACE = 3,
   FOCUS = 4,
   BACKGROUND = 5,
   PRINT = 6,
   TAG = 7,
   GOTO = 8,
   BGM = 9,
   TOUCH = 10,
   CHOICE = 11,
   VO = 12,
   WAIT = 13,
   IN_L = 14,
   IN_R = 15,
   OUT_L = 16,
   OUT_R = 17,
   FADEIN = 18,
   FADEOUT = 19,
   IN_FLOAT = 20,
   OUT_FLOAT = 21,
   JUMP = 22,
   SHAKE = 23,
   POP = 24,
   NOD = 25,
   SE = 26,
   BLACK_OUT = 27,
   BLACK_IN = 28,
   WHITE_OUT = 29,
   WHITE_IN = 30,
   TRANSITION = 31,
   SITUATION = 32,
   COLOR_FADEIN = 33,
   FLASH = 34,
   SHAKE_TEXT = 35,
   TEXT_SIZE = 36,
   SHAKE_SCREEN = 37,
   DOUBLE = 38,
   SCALE = 39,
   TITLE_TELOP = 40,
   WINDOW_VISIBLE = 41,
   LOG = 42,
   NOVOICE = 43,
   CHANGE = 44,
   FADEOUT_ALL = 45,
   MOVIE = 46,
   MOVIE_STAY = 47,
   BATTLE = 48,
   STILL = 49,
   BUSTUP = 50,
   ENV = 51,
   TUTORIAL_REWARD = 52,
   NAME_EDIT = 53,
   EFFECT = 54,
   EFFECT_DELETE = 55,
   EYE_OPEN = 56,
   MOUTH_OPEN = 57,
   AUTO_END = 58,
   EMOTION = 59,
   EMOTION_END = 60,
   ENV_STOP = 61,
   BGM_PAUSE = 62,
   BGM_RESUME = 63,
   BGM_VOLUME_CHANGE = 64,
   ENV_RESUME = 65,
   ENV_VOLUME = 66,
   SE_PAUSE = 67,
   CHARA_FULL = 68,
   SWAY = 69,
   BACKGROUND_COLOR = 70,
   PAN = 71,
   STILL_UNIT = 72,
   SLIDE_CHARA = 73,
   SHAKE_SCREEN_ONCE = 74,
   TRANSITION_RESUME = 75,
   SHAKE_LOOP = 76,
   SHAKE_DELETE = 77,
   UNFACE = 78,
   WAIT_TOKEN = 79,
   EFFECT_ENV = 80,
   BRIGHT_CHANGE = 81,
   CHARA_SHADOW = 82,
   UI_VISIBLE = 83,
   FADEIN_ALL = 84,
   CHANGE_WINDOW = 85,
   BG_PAN = 86,
   STILL_MOVE = 87,
   STILL_NORMALIZE = 88,
   VOICE_EFFECT = 89,
   TRIAL_END = 90,
   SE_EFFECT = 91,
   CHARACTER_UP_DOWN = 92,
   BG_CAMERA_ZOOM = 93,
   BACKGROUND_SPLIT = 94,
   CAMERA_ZOOM = 95,
   SPLIT_SLIDE = 96,
   BGM_TRANSITION = 97,
   SHAKE_ANIME = 98,
   INSERT_STORY = 99,
   PLACE = 100,
   IGNORE_BGM = 101,
   MULTI_LIPSYNC = 102,
   JINGLE = 103,
   TOUCH_TO_START = 104,
   EVENT_ADV_MOVE_HORIZONTAL = 105,
   BG_PAN_X = 106,
   BACKGROUND_BLUR = 107,
   SEASONAL_REWARD = 108,
   MINI_GAME = 109,
   MAX = 110
};

// Some sort of number that told what system y send the argument
enum categoryCommand
{
    Non = 0,
    System = 1,
    Motion = 2,
    Effect = 3
};

// Some bits to store this thing
union category
{
    struct catBits
    {
        signed number   : 8;
        unsigned category   : 8;
    }Bytes;
    short number;
    
};

typedef struct extraDeserialize
{
    unsigned char *argument;
    struct extraDeserialize       *nextArgument;
}extraArgu;

// The data ready to be decode form base64
typedef struct deserializedData
{
    union category      command;
    unsigned char       *argument;
    struct extraDeserialize           *extra;
    struct deserializedData                *nextArgument;
}ARGS;

//  Works with the file and returns the text ready to be decoded
//  Arguments:
//      file: Directory of the file
//      text: the pointer of the structure ARGS
//              NOTE: This has to be a valid pointer
//  Return:
//      The number of decoded elements.
//      0 in case of error.
unsigned int deserializeFile(const char *file, ARGS *text);

//  Evaluates the command part and puts it value
//  Argument:
//      data: Pointer to de data that is going to be
//          evaluated.
void evalCommand(ARGS *data);

//  Free from memory the structure ARGS and if there is
//          more in the list they free all of them.
//      Note: Also call freeExtraArguments
//  Argument:
//      argu: pointer to the structure to free
void freeArguments(ARGS *argu);

//  Free the extra arguments that have ARGS if is 
//      there is any...
//     Note: More or less the same behaviour than freeArguments
//  Argument:
//      text: Ponter to the structure to free
void freeExtraArguments(extraArgu *text);

#endif