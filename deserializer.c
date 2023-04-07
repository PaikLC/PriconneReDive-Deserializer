
#include "deserializer.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Just change the bytes order
static unsigned long normalizeLength(unsigned long strangeLength)
{
    unsigned long result = 0;

    result = (0x000000FF & strangeLength) << 24 | (0x0000FF00 & strangeLength) << 8 | (0xFF000000 & strangeLength) >> 24 | (0x00FF0000 & strangeLength) >> 8;
    return result;
}

// Just change the bytes order
static short normalizeCommand(short strangeCommand)
{
    short result = 0;

    result = (0x00FF & strangeCommand) << 8 | (0xFF00 & strangeCommand) >> 8;
    return result;
}

// Get the memory to save the text and read form the file adding the '\0'
//  at the end of the string
static unsigned char *readArgument(unsigned long length, FILE *data)
{
    unsigned char *argument = NULL;
    if(length == 0) 
        return NULL;
    else length = normalizeLength(length);

    argument = (unsigned char *)malloc( sizeof(char) * length + 1 );
    if (argument != NULL)
    {
        fread(argument, length, 1, data);
    }
    else
    {
        fprintf(stderr, "ERROR: Can't read the argument from the file\n");
    }
    argument[length] = '\0';
    return argument;
}

//  Get the base64 from the file
//  Arguments:
//      str: String to deserealize
static void getBase64(unsigned char *str)
{
    if (str == NULL)
    {
        return;
    }
    
    for(int i = 0; i < strlen((char *)str); i += 3)
    {
        str[i] = 255 - str[i];
    }
}

void freeArguments(ARGS *argu)
{
    if (argu == NULL)
    {
        return;
    }
    if (argu->nextArgument == NULL)
    {
        free(argu);
        return;
    }
    

    ARGS *pNext = argu, *pTMP = NULL;
    while (pNext->nextArgument != NULL)
    {
        pTMP = pNext;
        pNext = pNext->nextArgument;
    }
    freeExtraArguments(pNext->extra);
    free(pNext);
    pTMP->nextArgument = NULL;
    return freeArguments(argu);
}

void freeExtraArguments(extraArgu *text)
{
    if (text == NULL)
    {
        return;
    }
    
    if (text->nextArgument == NULL)
    {
        free(text);
        return;
    }

    extraArgu *extra = text, *tmp = NULL;
    while (extra->nextArgument != NULL)
    {
        tmp = extra;
        extra = extra->nextArgument;
    }
    free(extra);
    tmp->nextArgument = NULL;
    return freeExtraArguments(text);
}

void evalCommand(ARGS *data)
{
    enum categoryNumber cat = normalizeCommand(data->command.number);
    enum categoryCommand comand = 0;
    switch (cat)
    {
    case TITLE:
    case OUTLINE:
    case VISIBLE:
    case FACE:
    case FOCUS:
    case BACKGROUND:
    case PRINT:
    case TAG:
    case GOTO:
    case BGM:
    case TOUCH:
    case CHOICE:
    case VO:
    case WAIT:
        comand = System;
        break;

    case IN_L:
    case IN_R:
    case OUT_L:
    case OUT_R:
    case FADEIN:
    case FADEOUT:
    case IN_FLOAT:
    case OUT_FLOAT:
    case JUMP:
    case SHAKE:
    case POP:
    case NOD:
        comand = Motion;
        break;

    case SE:
    case BLACK_OUT:
    case BLACK_IN:
    case WHITE_OUT:
    case WHITE_IN:
    case TRANSITION:
    case SITUATION:
    case COLOR_FADEIN:
    case FLASH:
    case SHAKE_TEXT:
    case TEXT_SIZE:
    case SHAKE_SCREEN:
    case DOUBLE:
    case SCALE:
    case TITLE_TELOP:
    case WINDOW_VISIBLE:
    case LOG:
    case NOVOICE:
        comand = System;
        break;

    case CHANGE:
    case FADEOUT_ALL:
        comand = Motion;
        break;

    case MOVIE:
    case MOVIE_STAY:
    case BATTLE:
    case STILL:
    case BUSTUP:
    case ENV:
    case TUTORIAL_REWARD:
    case NAME_EDIT:
        comand = System;
        break;

    case EFFECT:
    case EFFECT_DELETE:
        comand = Effect;
        break;

    case EYE_OPEN:
    case MOUTH_OPEN:
    case AUTO_END:
        comand = System;
        break;

    case EMOTION:
    case EMOTION_END:
        comand = Effect;
        break;

    case ENV_STOP:
    case BGM_PAUSE:
    case BGM_RESUME:
    case BGM_VOLUME_CHANGE:
    case ENV_RESUME:
    case ENV_VOLUME:
    case SE_PAUSE:
    case CHARA_FULL:
        comand = System;
        break;

    case SWAY:
        comand = Motion;
        break;

    case BACKGROUND_COLOR:
        comand = System;
        break;

    case PAN:
        comand = Motion;
        break;

    case STILL_UNIT:
        comand = System;
        break;

    case SLIDE_CHARA:
        comand = Motion;
        break;

    case SHAKE_SCREEN_ONCE:
    case TRANSITION_RESUME:
    case SHAKE_LOOP:
    case SHAKE_DELETE:
    case UNFACE:
    case WAIT_TOKEN:
        comand = System;
        break;

    case EFFECT_ENV:
        comand = Effect;
        break;

    case BRIGHT_CHANGE:
    case CHARA_SHADOW:
    case UI_VISIBLE:
    case FADEIN_ALL:
    case CHANGE_WINDOW:
    case BG_PAN:
    case STILL_MOVE:
    case STILL_NORMALIZE:
    case VOICE_EFFECT:
    case TRIAL_END:
    case SE_EFFECT:
        comand = System;
        break;

    case CHARACTER_UP_DOWN:
        comand = Motion;
        break;

    case BG_CAMERA_ZOOM:
    case BACKGROUND_SPLIT:
    case CAMERA_ZOOM:
    case SPLIT_SLIDE:
    case BGM_TRANSITION:
    case SHAKE_ANIME:
    case INSERT_STORY:
    case PLACE:
    case IGNORE_BGM:
    case MULTI_LIPSYNC:
    case JINGLE:
    case TOUCH_TO_START:
    case EVENT_ADV_MOVE_HORIZONTAL:
    case BG_PAN_X:
    case BACKGROUND_BLUR:
    case SEASONAL_REWARD:
    case MINI_GAME:
        comand = System;
        break;
    
    default:
        comand = Non;
        break;
    }
    data->command.Bytes.category = comand;
    data->command.Bytes.number = cat;
}

unsigned int deserializeFile(const char *file, ARGS *text)
{
    FILE *data = NULL;
    ARGS *pNext = text;
    extraArgu *epNext = NULL;

    unsigned int readed = 0;
    unsigned long length = 0;

    data = fopen(file, "rb");
    if (data == NULL)
    {
        return readed;
    }
    else if (text == NULL)
    {
        return readed;
    }
    
    do
    {
        fread(&pNext->command, sizeof(short), 1, data);
        fread(&length, sizeof(unsigned long), 1, data);

        if (length != 0)
        {
            pNext->argument = readArgument(length, data);
        }
        else
        {
            readed++;

            pNext->nextArgument = (ARGS *)malloc(sizeof(ARGS) * 1);
            pNext->argument = NULL;
            pNext->extra = NULL;
            evalCommand(pNext);

            pNext = pNext->nextArgument; // Some corroboration?
            continue;
        }
        
        fread(&length, sizeof(unsigned long), 1, data);
        length = normalizeLength(length);

        if ((length != 0) && (ferror(data) == 0) && (feof(data) == 0))
        {
            epNext = (extraArgu *)malloc(sizeof(extraArgu) * 1);

            epNext->argument = (unsigned char *)malloc(sizeof(unsigned char) * (length + 1));
            fread(epNext->argument, length, 1, data);
            getBase64(epNext->argument);

            epNext->argument[length] = '\0';

            pNext->extra = epNext;
            epNext->nextArgument = NULL;
        }
        
        fread(&length, sizeof(unsigned long), 1, data);
        length = normalizeLength(length);

        while ((length != 0) && (ferror(data) == 0) && (feof(data) == 0))
        {
            epNext->nextArgument = (extraArgu *)malloc(sizeof(extraArgu) * 1);
            epNext = epNext->nextArgument;

            epNext->argument = (unsigned char *)malloc(sizeof(unsigned char) * (length + 1));
            fread(epNext->argument, length, 1, data);
            getBase64(epNext->argument);

            epNext->argument[length] = '\0';

            fread(&length, sizeof(unsigned long), 1, data);
            length = normalizeLength(length);

            if (length == 0)
            {
                epNext->nextArgument = NULL;
                continue;
            }
        }
        
        readed++;

        pNext->nextArgument = (ARGS *)malloc(sizeof(ARGS) * 1);
        evalCommand(pNext);

        pNext = pNext->nextArgument;

    } while ((feof(data) == 0) && (ferror(data) == 0));
    
    pNext->nextArgument = NULL;

    if (ferror(data) != 0)
    {
        perror(file);
    }
    
    fclose(data);
    return readed;
}