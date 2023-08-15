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

//GD
static void imagefrompicture(PA_PluginParameters params);
static void imagetopicture(PA_PluginParameters params);

//GD Color
static void imagecolorclosest(PA_PluginParameters params);
static void imagecolorclosestalpha(PA_PluginParameters params);
static void imagecolorclosesthwb(PA_PluginParameters params);
static void imagecolorexact(PA_PluginParameters params);
static void imagecolorexactalpha(PA_PluginParameters params);
static void imagecolorresolve(PA_PluginParameters params);
static void imagecolorresolvealpha(PA_PluginParameters params);
static void imagegetpixel(PA_PluginParameters params);
static void imagesetpixel(PA_PluginParameters params);

//GD Image

static void imagegammacorrect(PA_PluginParameters params);
static void imageflip(PA_PluginParameters params);
static void imagecrop(PA_PluginParameters params);
static void imagecropauto(PA_PluginParameters params);
static void imagerotate(PA_PluginParameters params);
static void imagefilter(PA_PluginParameters params);

typedef enum {
    
    IMG_PNG =  0,
    IMG_BMP =  1,
    IMG_GIF =  2,
    IMG_JPEG = 3,
    IMG_TIFF = 4,
    IMG_WBMP = 5,
    IMG_WEBP = 6
    
} IMG_PNG_T;

typedef enum {
    
    IMG_FLIP_HORIZONTAL =  0,
    IMG_FLIP_VERTICAL =  1,
    IMG_FLIP_BOTH =  2
    
} IMG_FLIP_T;

typedef enum {
    
    IMG_FILTER_NONE = 0,
    IMG_FILTER_NEGATE,
    IMG_FILTER_GRAYSCALE,
    IMG_FILTER_EDGEDETECT,
    IMG_FILTER_EMBOSS,
    IMG_FILTER_GAUSSIAN_BLUR,
    IMG_FILTER_SELECTIVE_BLUR,
    IMG_FILTER_MEAN_REMOVAL,
    IMG_FILTER_SMOOTH,
    IMG_FILTER_CONTRAST,
    IMG_FILTER_BRIGHTNESS,
    IMG_FILTER_SCATTER,
    IMG_FILTER_PIXELATE,
    IMG_FILTER_COLORIZE,
    IMG_FILTER_CONVOLUTION
    
}IMG_FILTER_T;

#endif /* PLUGIN_GD_H */
