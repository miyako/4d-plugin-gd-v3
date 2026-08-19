![version](https://img.shields.io/badge/version-19%2B-5682DF)
![platform](https://img.shields.io/static/v1?label=platform&message=mac-intel%20|%20mac-arm%20|%20win-64&color=blue)
[![license](https://img.shields.io/github/license/miyako/4d-plugin-gd-v3)](LICENSE)
![downloads](https://img.shields.io/github/downloads/miyako/4d-plugin-gd-v3/total)

# 4d-plugin-gd-v3

Processes images using the [GD Graphics Library](https://github.com/libgd/libgd): decode a `Picture` into an in-memory GD image, run pixel-level and whole-image operations against it (color lookup/allocation, get/set pixel, gamma correction, flip, crop, rotate, and a set of Photoshop-style filters), then re-encode back out to a `Picture` in any of PNG, BMP, GIF, JPEG, TIFF, WBMP, or WebP. Every command that returns image data returns it as a `Picture` object wrapped in a `.gd2` container internally — you never see the `.gd2` bytes directly, but it's worth knowing that's the format round-tripped between plugin calls, since some picture-inspection tools may show it if you inspect the picture's raw representations.

## Summary

| Command | Returns | Purpose |
|---|---|---|
| [`imagefrompicture`](#imagefrompicture) | Object | Decode a `Picture` (PNG/BMP/GIF/JPEG/TIFF/WBMP/WebP) into a GD-backed image |
| [`imagetopicture`](#imagetopicture) | Object | Encode a GD-backed image back to a `Picture`, in any supported format |
| [`imagecolorclosest`](#imagecolorclosest) | Longint | Find the closest matching color already in the image's palette |
| [`imagecolorclosestalpha`](#imagecolorclosestalpha) | Longint | Same as above, including alpha in the distance calculation |
| [`imagecolorclosesthwb`](#imagecolorclosesthwb) | Longint | Closest color match using hue/whiteness/blackness distance |
| [`imagecolorexact`](#imagecolorexact) | Longint | Find an exact RGB match already in the image's palette |
| [`imagecolorexactalpha`](#imagecolorexactalpha) | Longint | Find an exact RGBA match already in the image's palette |
| [`imagecolorresolve`](#imagecolorresolve) | Longint | Exact match if one exists, otherwise allocate or approximate |
| [`imagecolorresolvealpha`](#imagecolorresolvealpha) | Longint | Same as above, including alpha |
| [`imagegetpixel`](#imagegetpixel) | Longint | Read a single pixel's color index/value |
| [`imagesetpixel`](#imagesetpixel) | Object | Write a single pixel and return the updated image |
| [`imagegammacorrect`](#imagegammacorrect) | Object | Apply gamma correction across the whole image |
| [`imageflip`](#imageflip) | Object | Flip the image horizontally, vertically, or both |
| [`imagecrop`](#imagecrop) | Object | Crop to a rectangle |
| [`imagecropauto`](#imagecropauto) | Object | Auto-crop by transparent/black/white/side-color/threshold detection |
| [`imagerotate`](#imagerotate) | Object | Rotate by an arbitrary angle, with interpolation |
| [`imagefilter`](#imagefilter) | Object | Apply one of GD's built-in image filters (blur, emboss, pixelate, convolution, etc.) |

**Platforms:** macOS (Intel & Apple Silicon), Windows 64-bit — 4D v19 and later.

---

## Requirements & platform notes

- This is a pure image-processing plugin with no platform-specific code path at all — every command calls straight into the cross-platform GD library. There is no macOS/Windows behavioral divergence to document for any command below.
- Every command except `imagefrompicture` takes a GD-produced image (the object returned in a previous call's `image` property) as its picture input, **not** an arbitrary `Picture` you loaded from a file or `BLOB TO PICTURE`. Passing a `Picture` that wasn't itself produced by this plugin's `image` output will fail to decode and the command will behave as if it received no image at all (see each command's Description for the exact fallback).
- **All "returns an object" commands share the same shape:** `{success : Boolean; image : Picture}`. `image` is present only when `success` is `true` — always check `success` first, and never assume `image` exists otherwise.
- **Options objects are optional almost everywhere, but each key inside one is read unconditionally once the object itself is present** — i.e. the plugin does not require every key, but a command that reads several related keys together (e.g. `imagefilter`'s per-filter parameters) reads whichever ones are present and simply leaves the rest at a documented default. Passing an empty object (`New object`) is safe and equivalent to omitting the parameter entirely, for every command that accepts one.
- Commands that operate on true-color images (`imagegammacorrect`, `imageflip`, `imagecrop`, `imagecropauto`, `imagerotate`, `imagefilter`) silently no-op — returning `success : false` with no `image` — if the decoded image is not a true-color image. In practice, images produced by `imagefrompicture`/other plugin commands in this same plugin are always true-color, so this only matters if you feed in a palette-based image some other way.
- None of these commands raise a 4D error on bad input. Every failure path is silent: `success` stays `false` (or the Longint-returning commands return `-1`) and you get no other signal. Always check the result before using it.

---

## imagefrompicture

### Syntax

```4d
imagefrompicture ( picture ; format ) → Object
```

| Parameter | Type | Description |
|---|---|---|
| `picture` | Picture | Source picture to decode. Read unconditionally — mandatory. |
| `format` | Longint | Source image format. One of the `IMG_*` constants below. Mandatory. |
| Result | Object | `{success : Boolean; image : Picture}` — `image` present only if `success` is `true`. |

**`format` accepts:**

| Constant | Value |
|---|---|
| `IMG_PNG` | 0 (also the fallback for any unrecognized value) |
| `IMG_BMP` | 1 |
| `IMG_GIF` | 2 |
| `IMG_JPEG` | 3 |
| `IMG_TIFF` | 4 |
| `IMG_WBMP` | 5 |
| `IMG_WEBP` | 6 |

### Description

This is the entry point for the whole plugin: it decodes `picture` according to `format` into GD's internal representation, then immediately re-encodes that into the plugin's own `.gd2` transport format and hands it back as `image`. Every other command in this plugin expects `image` (or a picture derived from it by a previous command) as its own `picture`/`image` input — they will not correctly decode an arbitrary `Picture` loaded some other way.

If `picture` fails to decode as the format named by `format` (wrong format value for the actual bytes, corrupt data, or an empty picture), `success` stays `false` and no `image` key is set. There is no separate error code — you cannot distinguish "wrong format" from "corrupt data" from "empty picture" from the result alone.

### Example

From the plugin's own test method (`TEST.4dm`):
```4d
$file:=Folder:C1567(fk resources folder:K87:11).file("sample.png")
$data:=$file.getContent()
BLOB TO PICTURE:C682($data; $picture)

$GD:=imagefrompicture($picture; IMG_PNG)

If ($GD.success)
	// $GD.image is now usable by every other command in this plugin
End if
```

Decoding a JPEG instead:
```4d
$GD:=imagefrompicture($picture; IMG_JPEG)
If (Not($GD.success))
	ALERT("Could not decode the picture as JPEG.")
End if
```

---

## imagetopicture

### Syntax

```4d
imagetopicture ( image ; format ; options ) → Object
```

| Parameter | Type | Description |
|---|---|---|
| `image` | Picture | A GD-backed image, e.g. from [`imagefrompicture`](#imagefrompicture)'s `image` result. Mandatory. |
| `format` | Longint | Target encoding format. One of the `IMG_*` constants (see [`imagefrompicture`](#imagefrompicture)). Mandatory. |
| `options` | Object | Encoding options, all keys optional (see table below). Optional parameter — omit entirely for encoder defaults. |
| Result | Object | `{success : Boolean; image : Picture}`. |

**`options` keys:**

| Key | Type | Applies to | Description |
|---|---|---|---|
| `resolutionX` | Longint | all | DPI, horizontal. Only applied if **both** `resolutionX` and `resolutionY` are present and greater than 0. |
| `resolutionY` | Longint | all | DPI, vertical. Same condition as `resolutionX`. |
| `compression` | Longint | BMP | Compression mode passed straight to GD's BMP encoder. Defaults to `-1` (GD's own default) if omitted. |
| `quality` | Longint | JPEG | 0–100 JPEG quality. Defaults to `100` if omitted. |
| `saveAlpha` | Boolean | PNG (true-color) | Whether to preserve the alpha channel. No default is applied unless the key is present. |
| `interlace` | Boolean | PNG/GIF/JPEG | Whether to interlace/progressive-encode. No default applied unless present. |
| `transparent` | Boolean | GIF | Whether color index `0` is treated as transparent. No default applied unless present. |
| `antiAliased` | Boolean | all | Enables GD's anti-aliased drawing mode on the image before encoding. |
| `dontBlend` | Boolean | all | Only read if `antiAliased` is also present; controls whether anti-aliasing blends with the anti-aliasing color. If `antiAliased` is present without `dontBlend`, plain anti-aliasing is used. |

### Description

Encodes `image` into `format`, honoring `options` where relevant. `.gd2` is decoded from `image` internally first — same caveat as every other command: `image` must be a GD-produced picture, not an arbitrary loaded one.

BMP, GIF, JPEG, TIFF, and PNG all encode through a byte buffer and only set `success : true` if GD's encoder actually returned data (`bytes` non-null) — so a genuine encoder failure on those formats correctly reports `success : false`. **WBMP and WEBP are the exception:** both set `success : true` unconditionally, without checking whether the underlying encode call actually returned bytes. If you request `IMG_WBMP` or `IMG_WEBP` and the encode silently fails, you can get `success : true` back with no usable `image` key set — check for the presence of `image` after this call for those two formats specifically, don't rely on `success` alone.

### Example

From the plugin's own test method (`TEST_resolution.4dm`):
```4d
$options:=New object:C1471("resolutionX"; 300; "resolutionY"; 300)
$PNG:=imagetopicture($GD.image; IMG_PNG; $options)

If ($PNG.success)
	WRITE PICTURE FILE:C680(System folder:C487(Desktop:K41:16)+"1.png"; $PNG.image)
End if
```

Encoding to JPEG with a specific quality, no options object needed for the rest:
```4d
$options:=New object:C1471("quality"; 85)
$JPEG:=imagetopicture($GD.image; IMG_JPEG; $options)
```

Round-tripping through every format the plugin's own test suite exercises (from `TEST.4dm`):
```4d
$PNG:=imagetopicture($GD.image; IMG_PNG)
$BMP:=imagetopicture($GD.image; IMG_BMP)
$GIF:=imagetopicture($GD.image; IMG_GIF)
$WEBP:=imagetopicture($GD.image; IMG_WEBP)
$TIFF:=imagetopicture($GD.image; IMG_TIFF)
$JPEG:=imagetopicture($GD.image; IMG_JPEG)
$WBMP:=imagetopicture($GD.image; IMG_WBMP)
```

---

## imagecolorclosest

### Syntax

```4d
imagecolorclosest ( image ; red ; green ; blue ) → Longint
```

| Parameter | Type | Description |
|---|---|---|
| `image` | Picture | GD-backed image. Mandatory. |
| `red` | Longint | 0–255. |
| `green` | Longint | 0–255. |
| `blue` | Longint | 0–255. |
| Result | Longint | Index/value of the closest matching color already present in the image, or `-1` if `image` failed to decode. |

### Description

Wraps GD's `gdImageColorClosest`: finds the color already used somewhere in the image whose RGB distance to `(red, green, blue)` is smallest — it does **not** allocate a new color, and it does not guarantee an exact match. Returns `-1` only when `image` itself couldn't be decoded (see the Requirements note on picture provenance) — an empty/blank image still returns a valid closest-match result, it just won't be a meaningful one.

### Example

From the plugin's own test method (`TEST_color.4dm`):
```4d
$color:=imagecolorclosest($GD.image; 0; 0; 0)
```

Checking for the decode-failure sentinel explicitly:
```4d
$color:=imagecolorclosest($GD.image; 255; 0; 0)
If ($color=-1)
	ALERT("Image could not be decoded.")
End if
```

---

## imagecolorclosestalpha

### Syntax

```4d
imagecolorclosestalpha ( image ; red ; green ; blue ; alpha ) → Longint
```

| Parameter | Type | Description |
|---|---|---|
| `image` | Picture | GD-backed image. Mandatory. |
| `red` | Longint | 0–255. |
| `green` | Longint | 0–255. |
| `blue` | Longint | 0–255. |
| `alpha` | Longint | GD's alpha range, 0 (opaque) – 127 (fully transparent). |
| Result | Longint | Closest matching color including alpha distance, or `-1` on decode failure. |

### Description

Same matching behavior as [`imagecolorclosest`](#imagecolorclosest), but the distance calculation also weighs `alpha`. Note GD's alpha scale is inverted from typical 0–255 alpha and only spans 0–127 — `0` is fully opaque, `127` is fully transparent.

### Example

From the plugin's own test method (`TEST_color.4dm`):
```4d
$color:=imagecolorclosestalpha($GD.image; 0; 0; 0; 0)
```

---

## imagecolorclosesthwb

### Syntax

```4d
imagecolorclosesthwb ( image ; red ; green ; blue ) → Longint
```

| Parameter | Type | Description |
|---|---|---|
| `image` | Picture | GD-backed image. Mandatory. |
| `red` | Longint | 0–255. |
| `green` | Longint | 0–255. |
| `blue` | Longint | 0–255. |
| Result | Longint | Closest match by hue/whiteness/blackness distance, or `-1` on decode failure. |

### Description

Wraps GD's `gdImageColorClosestHWB`, which converts to the hue/whiteness/blackness color model before measuring distance — this can pick a visually closer match than plain RGB distance for some colors, at the cost of ignoring alpha entirely (there is no alpha variant of this one in GD).

### Example

From the plugin's own test method (`TEST_color.4dm`):
```4d
$color:=imagecolorclosesthwb($GD.image; 0; 0; 0)
```

---

## imagecolorexact

### Syntax

```4d
imagecolorexact ( image ; red ; green ; blue ) → Longint
```

| Parameter | Type | Description |
|---|---|---|
| `image` | Picture | GD-backed image. Mandatory. |
| `red` | Longint | 0–255. |
| `green` | Longint | 0–255. |
| `blue` | Longint | 0–255. |
| Result | Longint | Index/value of an exact RGB match, `-1` if no exact match exists **or** if `image` failed to decode. |

### Description

Unlike [`imagecolorclosest`](#imagecolorclosest), this only succeeds on an exact match — there is no fallback to nearest color. `-1` is therefore ambiguous between "the image decoded fine but doesn't contain this exact color" and "the image itself failed to decode." If you need to tell those apart, check `image`'s own decode success (e.g. via [`imagefrompicture`](#imagefrompicture)'s `success`) before calling this.

### Example

From the plugin's own test method (`TEST_color.4dm`):
```4d
$color:=imagecolorexact($GD.image; 0; 0; 0)
```

---

## imagecolorexactalpha

### Syntax

```4d
imagecolorexactalpha ( image ; red ; green ; blue ; alpha ) → Longint
```

| Parameter | Type | Description |
|---|---|---|
| `image` | Picture | GD-backed image. Mandatory. |
| `red` | Longint | 0–255. |
| `green` | Longint | 0–255. |
| `blue` | Longint | 0–255. |
| `alpha` | Longint | GD alpha scale, 0 (opaque) – 127 (fully transparent). |
| Result | Longint | Exact RGBA match, or `-1` (no match, or decode failure — see [`imagecolorexact`](#imagecolorexact)'s ambiguity note). |

### Description

Alpha-aware counterpart to [`imagecolorexact`](#imagecolorexact). Same exact-match-only behavior and the same `-1` ambiguity applies.

### Example

From the plugin's own test method (`TEST_color.4dm`):
```4d
$color:=imagecolorexactalpha($GD.image; 0; 0; 0; 0)
```

---

## imagecolorresolve

### Syntax

```4d
imagecolorresolve ( image ; red ; green ; blue ) → Longint
```

| Parameter | Type | Description |
|---|---|---|
| `image` | Picture | GD-backed image. Mandatory. |
| `red` | Longint | 0–255. |
| `green` | Longint | 0–255. |
| `blue` | Longint | 0–255. |
| Result | Longint | Exact match if one exists; otherwise a newly allocated or closest-approximated color. `-1` only on decode failure. |

### Description

The "just get me a usable color" command: tries an exact match first, then allocates a new palette entry if there's room, and only falls back to a closest-match approximation if the palette is full. Prefer this over [`imagecolorexact`](#imagecolorexact)/[`imagecolorclosest`](#imagecolorclosest) when you don't specifically need to distinguish those cases — it never fails for a well-decoded image, only for a decode failure.

### Example

From the plugin's own test method (`TEST_color.4dm`):
```4d
$color:=imagecolorresolve($GD.image; 0; 0; 0)
```

---

## imagecolorresolvealpha

### Syntax

```4d
imagecolorresolvealpha ( image ; red ; green ; blue ; alpha ) → Longint
```

| Parameter | Type | Description |
|---|---|---|
| `image` | Picture | GD-backed image. Mandatory. |
| `red` | Longint | 0–255. |
| `green` | Longint | 0–255. |
| `blue` | Longint | 0–255. |
| `alpha` | Longint | GD alpha scale, 0 (opaque) – 127 (fully transparent). |
| Result | Longint | Alpha-aware version of [`imagecolorresolve`](#imagecolorresolve)'s result. |

### Description

Alpha-aware counterpart to [`imagecolorresolve`](#imagecolorresolve): exact match, else allocate, else closest-approximate — now also weighing alpha.

### Example

From the plugin's own test method (`TEST_color.4dm`):
```4d
$color:=imagecolorresolvealpha($GD.image; 0; 0; 0; 0)
```

---

## imagegetpixel

### Syntax

```4d
imagegetpixel ( image ; x ; y ) → Longint
```

| Parameter | Type | Description |
|---|---|---|
| `image` | Picture | GD-backed image. Mandatory. |
| `x` | Longint | Horizontal pixel coordinate, 0-based. |
| `y` | Longint | Vertical pixel coordinate, 0-based. |
| Result | Longint | The pixel's color value. `-1` if `image` failed to decode **or** if `(x, y)` is out of bounds. |

### Description

Reads a single pixel. The plugin checks `(x, y)` against the image's actual bounds before reading (via GD's own bounds-safety check), so an out-of-range coordinate returns `-1` rather than reading adjacent/garbage memory — this is one of the few commands in the plugin with an explicit bounds guard beyond "did the image decode." As with the color-lookup commands, `-1` is ambiguous between "decode failed" and "coordinate out of bounds"; check bounds yourself first if you need to distinguish them.

### Example

```4d
$c:=imagegetpixel($GD.image; 10; 10)
If ($c#-1)
	$red:=($c>>16) & 0xFF
	$green:=($c>>8) & 0xFF
	$blue:=$c & 0xFF
End if
```

---

## imagesetpixel

### Syntax

```4d
imagesetpixel ( image ; x ; y ; color ; options ) → Object
```

| Parameter | Type | Description |
|---|---|---|
| `image` | Picture | GD-backed image. Mandatory. |
| `x` | Longint | Horizontal pixel coordinate, 0-based. Mandatory. |
| `y` | Longint | Vertical pixel coordinate, 0-based. Mandatory. |
| `color` | Longint | Color value to write (e.g. from one of the `imagecolor*` commands, or a raw truecolor value you compute yourself). Mandatory. |
| `options` | Object | See table below. Optional. |
| Result | Object | `{success : Boolean; image : Picture}`. |

**`options` keys:**

| Key | Type | Description |
|---|---|---|
| `alphaBlending` | Boolean | When present, sets GD's alpha-blending mode on the image before writing the pixel — controls whether the written color blends with what's underneath or replaces it outright. |

### Description

Unlike [`imagegetpixel`](#imagegetpixel), this does **not** bounds-check `(x, y)` before writing — an out-of-range coordinate is passed straight to GD's `gdImageSetPixel` with no guard. GD itself is generally tolerant of out-of-bounds writes on its own image buffer, but this plugin does not add its own safety check here the way `imagegetpixel` does, so don't rely on out-of-bounds coordinates failing gracefully — validate `x`/`y` against the image's own dimensions yourself if they come from anything other than a trusted, pre-validated source.

### Example

```4d
$col:=imagecolorresolve($GD.image; 255; 0; 0)
$options:=New object:C1471("alphaBlending"; True)
$GD:=imagesetpixel($GD.image; 10; 10; $col; $options)

If ($GD.success)
	// $GD.image now has the updated pixel
End if
```

---

## imagegammacorrect

### Syntax

```4d
imagegammacorrect ( image ; options ) → Object
```

| Parameter | Type | Description |
|---|---|---|
| `image` | Picture | GD-backed, true-color image. Mandatory. |
| `options` | Object | See table below. Read unconditionally — see Description for what happens if it's omitted. |
| Result | Object | `{success : Boolean; image : Picture}`. |

**`options` keys:**

| Key | Type | Description |
|---|---|---|
| `inputGamma` | Real | Source gamma. Must be greater than `0` for the correction to run at all. |
| `outputGamma` | Real | Target gamma. Must also be greater than `0`. |

### Description

Applies `pow(channel / 255, inputGamma / outputGamma) * 255` to every red/green/blue channel of every pixel, preserving alpha. **`options` itself is not optional in practice** — the whole correction is gated on `(inputGamma > 0.0) && (outputGamma > 0.0)`, and both default to `0.0` if their keys aren't present, so calling this without an `options` object (or with one missing either key) silently does nothing and returns `success : false`, with no other indication of why. Also gated on the image being true-color — a non-true-color decode returns `success : false` before even checking `options`.

### Example

From the plugin's own test method (`TEST_gamma.4dm`):
```4d
$options:=New object:C1471("inputGamma"; 1; "outputGamma"; 1.537)
$GD:=imagegammacorrect($GD.image; $options)

$PNG:=imagetopicture($GD.image; IMG_PNG)
If ($PNG.success)
	WRITE PICTURE FILE:C680(System folder:C487(Desktop:K41:16)+"gamma.png"; $PNG.image)
End if
```

---

## imageflip

### Syntax

```4d
imageflip ( image ; options ) → Object
```

| Parameter | Type | Description |
|---|---|---|
| `image` | Picture | GD-backed, true-color image. Mandatory. |
| `options` | Object | See table below. |
| Result | Object | `{success : Boolean; image : Picture}`. |

**`options` keys:**

| Key | Type | Description |
|---|---|---|
| `flipMode` | Longint | One of `IMG_FLIP_HORIZONTAL` (0, default), `IMG_FLIP_VERTICAL` (1), `IMG_FLIP_BOTH` (2). |

### Description

Like [`imagegammacorrect`](#imagegammacorrect), the flip only actually runs if `options` is present **and** the `flipMode` key is present in it — omit either and you get `success : false` with no image, even though `flipMode` conceptually has a documented default of horizontal. In other words, the default value only applies to the variable's initial state internally; it is never reached in practice because the flip logic itself is nested inside the `flipMode`-defined check. Always pass `options` with `flipMode` explicitly set.

### Example

```4d
$options:=New object:C1471("flipMode"; IMG_FLIP_VERTICAL)
$GD:=imageflip($GD.image; $options)

If ($GD.success)
	// $GD.image is now vertically flipped
End if
```

---

## imagecrop

### Syntax

```4d
imagecrop ( image ; options ) → Object
```

| Parameter | Type | Description |
|---|---|---|
| `image` | Picture | GD-backed, true-color image. Mandatory. |
| `options` | Object | See table below. Mandatory in practice (see Description). |
| Result | Object | `{success : Boolean; image : Picture}`. |

**`options` keys:**

| Key | Type | Description |
|---|---|---|
| `x` | Longint | Crop rectangle's left edge. Defaults to `0` if omitted. |
| `y` | Longint | Crop rectangle's top edge. Defaults to `0` if omitted. |
| `width` | Longint | Crop rectangle's width. Defaults to the source image's full width if omitted. |
| `height` | Longint | Crop rectangle's height. Defaults to the source image's full height if omitted. |

### Description

Crops to the rectangle `(x, y, width, height)`. If `options` itself is omitted (or is `Null`), nothing happens at all — `success` stays `false`, because the crop call only happens inside the `If (options)` branch. Once `options` is present, though, each of the four fields independently falls back to a safe full-image default if its own key is missing — so, for example, passing only `{"width" : 100}` crops to a `100`-pixel-wide strip starting at `(0, 0)` running the image's full height, rather than an undefined region.

### Example

```4d
$options:=New object:C1471("x"; 10; "y"; 10; "width"; 200; "height"; 150)
$GD:=imagecrop($GD.image; $options)

If ($GD.success)
	// $GD.image is now the 200x150 region starting at (10, 10)
End if
```

Cropping just a horizontal strip, relying on the height default:
```4d
$options:=New object:C1471("width"; 400)
$GD:=imagecrop($GD.image; $options)
// crops to a 400-pixel-wide strip, full height, starting at (0, 0)
```

---

## imagecropauto

### Syntax

```4d
imagecropauto ( image ; options ) → Object
```

| Parameter | Type | Description |
|---|---|---|
| `image` | Picture | GD-backed, true-color image. Mandatory. |
| `options` | Object | See table below. Mandatory in practice — see Description. |
| Result | Object | `{success : Boolean; image : Picture}`. |

**`options` keys:**

| Key | Type | Description |
|---|---|---|
| `mode` | Longint | One of the `GD_CROP_*` constants below. Defaults to `GD_CROP_DEFAULT` if omitted. |
| `threshold` | Real | Only used with `GD_CROP_THRESHOLD`. Defaults to `0.5`. |
| `color` | Longint | Only used with `GD_CROP_THRESHOLD` — the reference color to crop against. |

**`mode` accepts:** `GD_CROP_DEFAULT`, `GD_CROP_TRANSPARENT`, `GD_CROP_BLACK`, `GD_CROP_WHITE`, `GD_CROP_SIDES` (all detect the crop boundary automatically from image content) and `GD_CROP_THRESHOLD` (crops based on distance from `color` within `threshold`).

### Description

Like [`imagecrop`](#imagecrop), the whole operation is nested inside `If (options)` — omitting `options` entirely means nothing runs and you get `success : false`. Once present, `GD_CROP_THRESHOLD` mode additionally validates `color`: if `color` is negative, or the image isn't true-color and `color` is out of range for its actual palette size, the crop is skipped entirely (silently — `success` stays `false`) rather than being passed through to GD with an invalid value. The other five modes have no equivalent validation because they don't take a caller-supplied color at all.

### Example

```4d
$options:=New object:C1471("mode"; GD_CROP_TRANSPARENT)
$GD:=imagecropauto($GD.image; $options)

If ($GD.success)
	// $GD.image is now auto-cropped to its non-transparent bounds
End if
```

Threshold-based crop against a specific color:
```4d
$col:=imagecolorresolve($GD.image; 255; 255; 255)
$options:=New object:C1471("mode"; GD_CROP_THRESHOLD; "color"; $col; "threshold"; 0.25)
$GD:=imagecropauto($GD.image; $options)
```

---

## imagerotate

### Syntax

```4d
imagerotate ( image ; options ) → Object
```

| Parameter | Type | Description |
|---|---|---|
| `image` | Picture | GD-backed, true-color image. Mandatory. |
| `options` | Object | See table below. Mandatory in practice — see Description. |
| Result | Object | `{success : Boolean; image : Picture}`. |

**`options` keys:**

| Key | Type | Description |
|---|---|---|
| `degrees` | Real | Rotation angle. Defaults to `0.0` if omitted. |
| `color` | Longint | Background color revealed by the rotation (the corners left uncovered by a non-90°-multiple rotation). Defaults to `0` if omitted. |

### Description

Uses GD's interpolated rotation (`gdImageRotateInterpolated`), which anti-aliases the result rather than producing hard-edged rotated pixels. As with `imagecrop`/`imagecropauto`, the entire operation is gated on `options` being present at all — omit it and you get `success : false` with no rotation attempted, even though `degrees` conceptually defaults to `0.0`. The resulting image's dimensions grow to fit the rotated content (GD computes the new bounding box internally) — don't assume the output is the same size as the input.

### Example

From the plugin's own test method (`TEST_rotate.4dm`):
```4d
$options:=New object:C1471("degrees"; 45; "color"; 0)
$GD:=imagerotate($GD.image; $options)

$options:=New object:C1471("transparent"; 0)
$PNG:=imagetopicture($GD.image; IMG_PNG; $options)

If ($PNG.success)
	WRITE PICTURE FILE:C680(System folder:C487(Desktop:K41:16)+"rotate.png"; $PNG.image)
End if
```

---

## imagefilter

### Syntax

```4d
imagefilter ( image ; options ) → Object
```

| Parameter | Type | Description |
|---|---|---|
| `image` | Picture | GD-backed, true-color image. Mandatory. |
| `options` | Object | See table below. `filter` key is mandatory in practice — see Description. |
| Result | Object | `{success : Boolean; image : Picture}`. |

**`options` keys:**

| Key | Type | Applies to filter | Description |
|---|---|---|---|
| `filter` | Longint | all | One of the `IMG_FILTER_*` constants below. Presence of this key is what gates the entire operation. |
| `weight` | Real | `IMG_FILTER_SMOOTH` | Smoothing weight. |
| `contrast` | Real | `IMG_FILTER_CONTRAST` | Contrast adjustment amount. |
| `brightness` | Longint | `IMG_FILTER_BRIGHTNESS` | Brightness adjustment amount. |
| `sub`, `plus` | Longint | `IMG_FILTER_SCATTER` | Scatter's sub/plus range parameters. |
| `size` | Longint | `IMG_FILTER_PIXELATE` | Pixelation block size. Validated and clamped to `1`–`4096`; a missing, non-finite, or out-of-range value falls back to `1`. |
| `mode` | Longint | `IMG_FILTER_PIXELATE` | `0` = upper-left pixel, `1` = average of the block. Validated and clamped to `0`–`1`; anything else falls back to `0`. |
| `red`, `green`, `blue`, `alpha` | Longint | `IMG_FILTER_COLORIZE` | Tint color and alpha to apply. |
| `radius` | Longint | `IMG_FILTER_GAUSSIAN_BLUR` | Blur radius. Validated and clamped to `-1`–`4096`; `-1` (also the fallback for a missing/invalid value) tells GD to auto-select a radius from `sigma`. |
| `sigma` | Real | `IMG_FILTER_GAUSSIAN_BLUR` | Blur sigma. |
| `div`, `offset` | Real | `IMG_FILTER_CONVOLUTION` | Divisor and offset applied to the convolution sum. **If `div` is omitted (or is `0`), the convolution divides by zero** — see Description. |
| `matrix` | Collection | `IMG_FILTER_CONVOLUTION` | A 3-element collection of 3-element collections of numbers — the 3×3 convolution kernel. Any row/element that isn't itself a well-formed 3-number collection is left as `0` in the kernel rather than raising an error. |

**`filter` accepts:** `IMG_FILTER_NEGATE`, `IMG_FILTER_GRAYSCALE`, `IMG_FILTER_EDGEDETECT`, `IMG_FILTER_EMBOSS`, `IMG_FILTER_GAUSSIAN_BLUR`, `IMG_FILTER_SELECTIVE_BLUR`, `IMG_FILTER_MEAN_REMOVAL`, `IMG_FILTER_SMOOTH`, `IMG_FILTER_CONTRAST`, `IMG_FILTER_BRIGHTNESS`, `IMG_FILTER_SCATTER`, `IMG_FILTER_PIXELATE`, `IMG_FILTER_COLORIZE`, `IMG_FILTER_CONVOLUTION`.

### Description

`filter` is the only key that gates whether anything runs at all — omit `options` entirely, or provide one without a `filter` key, and you get `success : false` with nothing attempted. Once `filter` is present, every other key listed above is read regardless of which filter you actually chose (they're all read unconditionally in one block), so unrelated keys left over from a previous call in an object you're reusing are harmless — only the ones relevant to your chosen `filter` actually affect the output.

`size`, `mode`, and `radius` are validated and clamped before use (see the table above) — these three directly control how much work/memory GD's pixelation and Gaussian-blur routines do internally, so an out-of-range value is clamped to a safe bound rather than passed through as-is.

**`IMG_FILTER_CONVOLUTION` has one un-validated footgun:** `div` defaults to `0.0` if you don't set it, and GD's convolution divides every channel sum by `div`. Dividing a float by `0.0` is well-defined (produces `Inf`/`NaN`, not a crash), but the result is a broken/garbage-looking image rather than an error — always set `div` explicitly when using this filter (typically the sum of your kernel's weights, or `1` if the kernel is already normalized).

`IMG_FILTER_GAUSSIAN_BLUR` is the one filter whose output is a genuinely new/resized-in-place image (`gdImageCopyGaussianBlurred` returns a separate image rather than modifying `image` in place) — this is handled internally and doesn't change how you call this command, but it's why this filter alone doesn't participate in the same "modifies in place" code path as the others if you're reading the plugin's own source.

### Example

From the plugin's own test method (`TEST_filter.4dm`):
```4d
$options:=New object:C1471("filter"; IMG_FILTER_COLORIZE; "red"; 100; "green"; 1; "blue"; 100; "alpha"; 10)
$GD:=imagefilter($GD.image; $options)

$PNG:=imagetopicture($GD.image; IMG_PNG)
If ($PNG.success)
	WRITE PICTURE FILE:C680(System folder:C487(Desktop:K41:16)+"colorize.png"; $PNG.image)
End if
```

Gaussian blur with an explicit radius and sigma:
```4d
$options:=New object:C1471("filter"; IMG_FILTER_GAUSSIAN_BLUR; "radius"; 8; "sigma"; 3.0)
$GD:=imagefilter($GD.image; $options)
```

A custom 3×3 convolution kernel (a simple sharpen kernel), with `div` set explicitly to avoid the divide-by-zero footgun:
```4d
$row0:=New collection(0; -1; 0)
$row1:=New collection(-1; 5; -1)
$row2:=New collection(0; -1; 0)
$matrix:=New collection($row0; $row1; $row2)

$options:=New object:C1471("filter"; IMG_FILTER_CONVOLUTION; "matrix"; $matrix; "div"; 1; "offset"; 0)
$GD:=imagefilter($GD.image; $options)
```

---

## Error handling & troubleshooting

- **Every failure is silent — there is no 4D error raised anywhere in this plugin.** Object-returning commands leave `success : false` with no `image` key; Longint-returning commands (the seven `imagecolor*` commands and `imagegetpixel`) return `-1`. Always check the result explicitly; don't assume a call succeeded just because it didn't throw.
- **`-1` from the color/pixel commands is ambiguous.** It can mean "the image itself failed to decode," "no exact match exists" (for the `*exact*` commands), or "coordinate out of bounds" (for `imagegetpixel`). If you need to tell these apart, verify the source image's own decode `success` first.
- **Feed these commands only pictures this plugin itself produced.** Every command after `imagefrompicture` expects its picture parameter to be a GD-produced image (an `image` result from a previous call in this plugin), not an arbitrary `Picture` loaded via `BLOB TO PICTURE`, `READ PICTURE FILE`, or similar. Passing one in will decode as if nothing were passed at all.
- **`imagegammacorrect`, `imageflip`, `imagecrop`, `imagecropauto`, `imagerotate`, and `imagefilter` all silently do nothing if you omit `options` (or omit the one key that actually gates the operation — `flipMode` for `imageflip`, `filter` for `imagefilter`).** None of these commands' documented per-key defaults (e.g. `imageflip`'s `flipMode` defaulting to horizontal) are actually reachable in practice, because the operation itself only runs inside the `If (options)`/`If (ob_is_defined(...))` check. Always pass the gating key explicitly.
- **These same six commands also require a true-color image.** A non-true-color decode returns `success : false` before even looking at `options` — this only matters if you've fed in an image some other way than through this plugin's own `imagefrompicture`, since images this plugin produces are always true-color.
- **`imagetopicture` with `IMG_WBMP` or `IMG_WEBP` can report `success : true` with no `image` set**, because those two branches don't check whether the encoder actually returned data (unlike PNG/BMP/GIF/JPEG/TIFF, which do). Check for the presence of `image`, not just `success`, when encoding to either of those two formats.
- **`IMG_FILTER_CONVOLUTION` needs an explicit `div`.** Leaving it out defaults to `0`, which divides every convolution sum by zero — not a crash, but a broken-looking output image.
- **`imagesetpixel` does not bounds-check `x`/`y`.** Unlike `imagegetpixel`, which safely returns `-1` for an out-of-range coordinate, `imagesetpixel` passes the coordinates straight through — validate them yourself against the image's known dimensions if they aren't already trusted.

---

## Quick reference

```4d
// Decode → process → encode
$file:=Folder:C1567(fk resources folder:K87:11).file("sample.png")
BLOB TO PICTURE:C682($file.getContent(); $picture)
$GD:=imagefrompicture($picture; IMG_PNG)

If ($GD.success)
	$options:=New object:C1471("filter"; IMG_FILTER_GRAYSCALE)
	$GD:=imagefilter($GD.image; $options)
	
	$PNG:=imagetopicture($GD.image; IMG_PNG)
	If ($PNG.success)
		WRITE PICTURE FILE:C680(System folder:C487(Desktop:K41:16)+"out.png"; $PNG.image)
	End if
End if
```

```4d
// Color lookup + pixel read/write
$col:=imagecolorresolve($GD.image; 255; 0; 0)
$px:=imagegetpixel($GD.image; 0; 0)
$GD:=imagesetpixel($GD.image; 0; 0; $col; New object:C1471("alphaBlending"; True))
```

```4d
// Crop, rotate, flip chained together
$GD:=imagecrop($GD.image; New object:C1471("x"; 0; "y"; 0; "width"; 200; "height"; 200))
$GD:=imagerotate($GD.image; New object:C1471("degrees"; 90; "color"; 0))
$GD:=imageflip($GD.image; New object:C1471("flipMode"; IMG_FLIP_HORIZONTAL))
```
