/* --------------------------------------------------------------------------------
 #
 #	4DPlugin-GD.h
 #	source generated by 4D Plugin Wizard
 #	Project : GD
 #	author : miyako
 #	2023/08/14
 #  
 # --------------------------------------------------------------------------------*/

#ifndef PLUGIN_GD_H
#define PLUGIN_GD_H

#include "4DPluginAPI.h"
#include "C_TEXT.h"
#include "C_LONGINT.h"
#include "4DPlugin-JSON.h"
#include "gd.h"

#pragma mark -

static void imagefrompicture(PA_PluginParameters params);
static void imagetopicture(PA_PluginParameters params);
static void imageresolution(PA_PluginParameters params);
static void imagecolorresolve(PA_PluginParameters params);
static void imagecolorat(PA_PluginParameters params);
static void imageantialias(PA_PluginParameters params);
static void imagegammacorrect(PA_PluginParameters params);
static void imageflip(PA_PluginParameters params);
static void imagecrop(PA_PluginParameters params);
static void imagesetpixel(PA_PluginParameters params);

typedef enum {
    
    IMG_PNG =  0,
    IMG_BMP =  1,
    IMG_GIF =  2,
    IMG_JPEG = 3,
    IMG_TIFF = 4,
    IMG_WBMP = 5,   //input only
    IMG_WEBP = 6    //input only
    
} IMG_PNG_T;


#endif /* PLUGIN_GD_H */
