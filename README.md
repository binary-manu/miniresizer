# MiniResizer, a simple video crop and resize tool

|       `travis`         |        `master`        |
|:----------------------:|:----------------------:|
| [![][ci_travis]][repo] | [![][ci_master]][repo] |

[ci_travis]: https://travis-ci.com/binary-manu/miniresizer.svg?branch=travis
[ci_master]: https://travis-ci.com/binary-manu/miniresizer.svg?branch=master
[repo]:      https://travis-ci.com/binary-manu/miniresizer


MiniResizer is a simple graphical utility that helps you to calculate
crop and resize parameters when doing video editing.

Its goal is to show you a preview of how a picture will look after being
resized so that you can visually check if your choices actually remove
all the black borders or if the aspect ratio is correct. It also gives
you an idea of the distorion introduced when resizing, mainly caused by
the need to keep the video width and/or height multiples of a certain
value imposed by the compressor.

It posseses no editing or encoding capabilities: it cannot be used to
transcode movies from one format to another. There are already great
tools for that, such as FFmpeg or Avidemux.  Instead, this tool
focuses on a single task, and is meant to be used together with a
video editor/encoder.

## Supported OSes

MiniResizer is written in C++11 and uses cross-platform libraries to do
its job, so it can be easily ported to different systems.

Currently, it has been tested under some Linux distributions, namely
Ubuntu (18.04 and 16.04), CentOS 7 and Arch Linux.

It is possible to compile it for Microsoft Windows, but since it is not
my system of choice, I only test cross-builds produced under Linux with
a GCC cross-toolchain targeting Windows, and the app is tested under
Wine.

It has not been tested under any BSD system or OS X.

## License

This program is distributed under the [MIT
license](https://choosealicense.com/licenses/mit/).

## User guide

The following section gives some brief info about MiniResizer. I have
used freely available footage to make screenshots.  Free Stock footage
by [Videezy!](http://videezy.com).

Full credits for libraries, icons and other material requiring
attribution can be found in [CREDITS.txt](CREDITS.txt)

### Terms and acronyms

Before explaining the user interface, some terms need to be defined.

* The _SAR (Sample Aspect Ratio)_ is simply the ratio between the width
  and the height of a frame _as produced by the decoder_.  For example,
  if an image has a size of 720x576 pixel, its SAR is `720 / 576` or
  1.25.

* The _DAR (Display Aspect Ratio)_ is the aspect ratio that should be
  used when rendering the image on screen. For most modern videos, the
  DAR will be equal to the SAR, meaning that the image is displayed with
  the same width/height ratio as the decoded frame. Some material (i.e.
  DVD's) do not follow this rule and use a different DAR, which means
  that the image must be implicitly resized before rendering. This was
  common in pre-FHD material and DVB MPEG2 material. Typical DAR's are
  4:3 or 16:9. Back with our example, if we have an image with a SAR of
  1.25 but a DAR of 16:9, which is around 1.78, the renderer must
  correct the ratio before displaying it, for example by making it
  larger.

* The _PAR (Pixel Apsect Ratio)_ is the ratio between the DAR and the
  SAR. It tells us how much we must correct each pixel (by making it
  larger or taller) for the image to actually make the SAR equal to the
  DAR. When the DAR and the SAR are equal, the PAR is 1, which means the
  image has a "square pixel": we expect each pixel to occupy the same
  space on the display in both directions, so the displayed image has
  the same aspect ratio as the decoded picture. For our example, the PAR
  is `1.78 / 1.25` or 1.42. This effectively means that the rendered image
  is larger than what the decoder produced).

### Startup

When you launch MiniResizer, there is no main window to be shown,
instead, a dialog box prompts you to select a multimedia file to open.
All container and compression formats supported by FFmpeg are supported,
the only requirements is for the file to contain a video stream.


![Open Dialog][open-dialog]

If no file is selected, the application simply closes. Otherwise, the
main UI comes up, which is composed of two windows: the _resize window_
and the _preview window_.

The resize window contains all controls and information that guide how
the crop and resize are done.

![Resize Window][resize-window]

The results of the transformation are shown in the preview window: it
shows the results of crop and resize in real time, as the preview window
is used to tweak values. The slider on the bottom side can be used to
seek through the file and change the frame used in the preview.

![Preview Window][preview-window]

### The resize window

All decisions about how to crop and resize are made using the resize
window.  Controls are grouped into sections by purpouse.

#### Input info

Describes the width and height of the video stream as read from the
file, as well as the DAR and the SAR.

The fields in this section are dynamic: they will update when the
preview dialog slider is moved. At the moment, only changes in the DAR
are handled: these are uncommon, but may happen in DVB streams if an
older 4:3 movie is inserted between more common 16:9 material.

#### Crop control

This section controls cropping: it is possible to specify how many
pixels to remove from each side independently.

It is possible to specify the increment or decrement to apply each time
a spinner control is used via the _Crop Align_ widget. The default is 2
pixels.  If a value is typed instead of using the spinner buttons, it
will be replaced with the nearest smaller integer that is a multiple of
the increment.  With an 8-pixel step, a 10 would become an 8.

Cropping is applied before resizing, so the specified amounts of pixels
are subtracted from the original image size as reported by the _Input
info_ section. The _After crop_ boxes report the size of the image after
cropping. This is what enters the resize stage.

#### Resize Control

This group contains controls which pertain to resizing. It works on the
cropped image by doing the following:

1. first, the PAR for the original (pre-crop) image is computed;
2. then, the image aspect ratio is corrected by applying the PAR to the
   output of the crop stage. Since the PAR is independent of cropping
   (we divided for the SAR, so the PAR is independent of actual width or
   height of the image it is applied to), this produces a resized and
   cropped image with the correct aspect ratio;
3. last, the image is scaled to ensure that its width matches the
   _Target width_. The height follows, since it can be computed from the
   PAR and the width.

It's possible to customize this process with some controls. First of
all, the DAR used to compute the PAR can be changed using the _DAR_
combo box. By default, the DAR from the input file is used, but this can
be overridden with a common value from the list, or it can be set
manually by choosing _Custom_ and then filling the control on the right.

From this DAR, and the SAR computed on the original image, the PAR is
computed and shown in the _PAR_ box. _Target width_ selects the desired
final width of the resized image. The final width and height are
reported in the _Resize to_ fields.

The final width and height can both be forced to be a multiple of some
value. These are set using the _W Snap_ and _H Snap_ widgets. A typical
use is to ensure that the final sizes are acceptable by compressors that
have specific requirements (for example, being a multiple of the
macroblock size).

_Relative error_ and _Pixel delta_ report the difference between the
final picture height before and after the snapping according to _H
Snap_. For example, if we want the height to be a multiple of 16 pixels,
but the exact resizing requires a height that is not an exact multiple
of 16, for example 1020, this introduces an error of 4 pixels because
the nearest multiple is 1024. 4 is the value reported in _Pixel Delta_,
while _Relative error_ is calculated as the pixel delta divided by the
exact resized height. In our case, this would be `4 / 1020`.

#### Visual aids

It can be difficult to work with a very large picture on a small
display, as well as resizing a small picture if one cannot see the
details on the edges. This section controls image zooming and borders.

If a border color is selected, a black or white thick solid border is
placed all around the image in the preview window. The idea is to
provide an area with a strong contrast with respect to the image, to
make it easier to spot black borders to be cropped and also to place
some space between the picture and the preview window's chrome.

![Preview with border][preview-with-border]

In the image above, the white border makes its easy to spot a thin black
border on the left side.

The zoom slider applies a zoom/shrink factor to the image. By default
the image is shrinked by the specified amount, but if the _Enlarge_
check is selected, the image will be zoomed instead. This resizing is
applied only for comfort reasons and is not included in any calculations
on the final image size.  Useful when working with FHD videos on a
sub-FHD screen, as the original image will not fit the display.

#### Filtergraph expansion

The filtergraph section is used to automatically replace values computed
by the resize calculations, such as the final width and heigth or the
crop values, into a template. Such customized template will later be
passed to a video tool in order to perform the actual resize. An example
are FFmpeg filtergraphs, which actually gave the name to this feature.

This is an optional feature, whose availability can be controlled at
build time. It can be disabled, so that this section is effectively
hidden.

When enabled, two different implementations are available:

* the _wordexp_ implementation is based on the standard
  [wordexp(3)](https://manpage.me/?q=wordexp) call. The template is
  processed as if it were a piece of shell code, so that it is possible
  to use all common substitution sequences: variable expansion,
  subprocess invocation and arithmetic evaluation using `$((...))`. The
  template is treated as if it were wrapped inside double quotes, so
  that blanks are preserved: this means that double quotes must be
  `\`-escaped.  This implementation depends on the underlying system
  providing `wordexp(3)` and is usually not available under Windows;
* the _builtin_ implementation work much like _workexp_, but does all
  expansion within the program. It only supports variable expansion in
  the forms `$VARNAME` or `${VARNAME}`.

The _Source_ box contains the template, which is expanded and then
placed into the _Filter_ box. If the expansion fails because the
template is malformed (for example, it contains unbalanced quotes) the
_Filter_ box turns red.

![Malformed template][malformed-template]

The complete program environment is available for expansion. On top of
that, there are additional variables that specify crop and resize
parameters:

* `MRCL`: the left crop value
* `MRCR`: the right crop value
* `MRCT`: the top crop value
* `MRCB`: the bottom crop value
* `MRCW`: the after-crop width
* `MRCH`: the after-crop height
* `MRTW`: the final width of the resized image
* `MRTH`: the final height of the resized image

The _Make default_ button will save the current filtergraph source so
that it will be automatically loaded when the program starts. All
program settings go into FLTK preferences, which, on \*NIX systems, are
typically stored under `$HOME/.fltk/`.

### The preview window

This window shows the final result of the resizing. It is updated in
real time as the resize window is used to change parameters.

The slider on the bottom side can be used to seek through the input file
to select a frame to preview. The seeking is not frame accurate and you
cannot preview every single picture of a movie: instead, the program
simply selects up to 1000 evenly spaced frames from the input file.

The resize is done using FFmpeg's _fast bilinear_ algorithm.

## Building

This project is built using [CMake](https://cmake.org/) and requires at
least CMake 3.3. In addition to the usual CMake configuration variables,
like `CMAKE_INSTALL_PREFIX` and `CMAKE_BUILD_TYPE`, there are some
project-specific tweaks:

* `OPTION_ENABLE_FILTERGRAPH` controls if filtergraph expansion should
  be enabled and, if so, which implementation is used. It can take the
  following values:

  * `wordexp` selects the wordexp implementation. This is the default if
    this variable is not specified. If the `wordexp(3)` call is not
    available on the system, it automatically falls back on `builtin`;
  * `builtin` selects the builtin implementation;
  * `none` disables filtegraph expansion. Controls related to this
    feature will be hidden from the UI.

The default build type is _Release_.

For example, this invocation prepares the build system to compile for
release using builtin filtegraph expansion and sets the install prefix:

    cmake -DCMAKE_INSTALL_PREFIX=/usr \
      -DOPTION_ENABLE_FILTERGRAPH=builtin .

You will need a C++11 compiler, as well as the following
dependencies:

* FLTK 1.3.x
* FFmpeg, either version 4.x, 3.x or 2.8

If the build system cannot find headers and libraries under the common
paths on your system, use the `CMAKE_LIBRARY_PATH` and
`CMAKE_INCLUDE_PATH` environment variables to specify additional
directories to search. For FFmpeg, the environment variables
`FFMPEG_LIBRARY_PATH` and `FFMPEG_INCLUDE_PATH` can also be used to
specify directories that will only be searched for FFmpeg.

[open-dialog]: docs/open-dialog.png
[resize-window]: docs/resize-window.png
[preview-window]: docs/preview-window.png
[preview-with-border]: docs/preview-with-border.png
[malformed-template]: docs/malformed-template.png

<!-- vi: set tw=72 et sts=-1 sw=4 fo=tcqan autoindent : -->
