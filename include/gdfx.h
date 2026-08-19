#ifndef GDFX_H
#define GDFX_H 1

#ifdef __cplusplus
extern "C" {
#endif

gdImagePtr gdImageSquareToCircle(gdImagePtr im, int radius);

char * gdImageStringFTCircle(
    gdImagePtr im,
    int cx,
    int cy,
    double radius,
    double textRadius,
    double fillPortion,
    char *font,
    double points,
    char *top,
    char *bottom,
    int fgcolor);

void gdImageSharpen (gdImagePtr im, int pct);

#ifdef __cplusplus
}
#endif

#endif /* GDFX_H */
