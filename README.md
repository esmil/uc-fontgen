# uc-fontgen

Renders a freetype font as an 8x8 bitmap encoded as a C array.

## Dependencies

    freetype

## Building

Just run ``make``

## Usage

    uc-fontgen [-d|-h] /path/to/font > bitmap.c

If -d is provided, ASCII images of each character will be printed to stderr.
