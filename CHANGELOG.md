# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

## [0.1.1] - 2024-09-21

### Fixed

* Fix seeking. It did not work with H.265 files.

## [0.1.0] - 2020-01-14

### Added

* It is now possible to select the value to which output width and height must
  _snap_, that is, they must be multiple of a given value. Before, this value
  was fixed to 16 pixels, which was a common macroblock size for many
  compressors, but more recent compressors can work with sizes that are multiple
  of smaller values.
* Add proper licensing information.
* Add an About dialog.

### Changed

* Build system has been switched to CMake.
* Monolithic sources have been split into smaller, more coherent units.
* Code was linted with `clang-tidy` and refactored accordingly.
* Code has been modernized to use C++11 features and now requires a C++11
  toolchain.
* New icon. _Icon made by Pixelmeetup from www.flaticon.com_. Attribution is also
  available in `CREDITS.txt`.

### Fixed

* Should compile with any version of FFmpeg from 2.8 to 4.x. Some API's have
  been deprecated or removed during FFmpeg's lifecycle, but old versions are
  still found in the wild (i.e. CentOS 7). As far as feasible, the code should
  compile against all of them. Some deprecation warnings may appear while
  building due to the way feature checks are written.
* Fix crashes caused by some numeric controls being allowed to take
  non-positive values.
* Fix crashes caused by undefined variables in filtergraphs when using the
  builtin implementation.

## [0.0.8] - 2017-09-24

### Fixed

* Fix seeking to make it work with MPEG Transport Stream files.

## [0.0.7] - 2016-02-07

### Added

* Implement built-in filtergraph expansion, which does not rely on `wordexp(3)`
  and can be used under Windows. The current limitation is that only variable
  expansion is performed, so one can use `$VAR` or `${VAR}` in templates, but not
  things like `$(cmd)`. The expansion mechanism can be chosen at build-time; the
  default is to use `wordexp(3)` if available, and to fallback on the internal
  implementation if it is not.

## [0.0.6] - 2015-12-06

### Fixed

* Handle unbalanced quoting in filtergraphs.


## [0.0.5] - 2015-08-10

### Added

* Add an icon for the Windows build.
* Add icons and a desktop file for XDG compliant DE's.
* The file to open can be passed on the command line as the only argument.
* Add _filtergraph expansion_, a.k.a. replacing computed values for the image
  width, height and other values into user provided contents. This is mainly
  used to produce FFmpeg filtergraphs with actual values from a template, hence
  the name of this feature. This feature uses `wordexp(3)` to perform expansion
  and thus is not available in Windows builds.

### Changed

* Project is now built with the GNU Autotools.

## [0.0.4] - 2014-08-26

### Changed

* Project is now built with the GNU Autotools.

## [0.0.3] - 2014-08-13

### Added

* Add preview resize controls.

### Fixed

* Fix widget redraw issues under Windows.

## [0.0.2] - 2014-07-04

### Added

* First versioned release.
* Add drawing a solid border around the preview to ease cropping.
