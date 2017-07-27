/*
 * uc-fontgen - A microchip font generator
 * Copyright (C) 2017 Emil Renner Berthing
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include <freetype/ftbitmap.h>

#ifdef NDEBUG
#define debug(...) do {} while(0)
#else
#define debug(...) printf(__VA_ARGS__)
#endif

#define CHAR_FIRST 32
#define CHAR_LAST  126

static FT_Error
render_character(FT_Library library, FT_Face face, FT_Bitmap *bm, unsigned int ch)
{
	FT_GlyphSlot slot = face->glyph;
	FT_Error err;
	unsigned int x;
	unsigned int y;
	uint8_t line[8];
	uint8_t mask;

	err = FT_Load_Char(face, ch, FT_LOAD_DEFAULT);
	if (err) {
		fprintf(stderr, "warning: failed FT_Load_Glyph 0x%02x %d\n", ch, err);
		return err;
	}

	err = FT_Render_Glyph(slot, FT_RENDER_MODE_MONO);
	if (err) {
		fprintf( stderr, "warning: failed FT_Render_Glyph 0x%x %d\n", ch, err);
		return err;
	}

	err = FT_Bitmap_Convert(library, &slot->bitmap, bm, 1);
	if (err) {
		fprintf(stderr, "warning: failed FT_Bitmap_Convert %d\n", err);
		return err;
	}

	memset(line, 0, sizeof(line));

	debug("+--------+\n");
	for (y = 0, mask = 1; y < 8; y++, mask <<= 1) {
		debug("|");
		for (x = 0; x < 8; x++) {
			unsigned int i = x - slot->bitmap_left;
			unsigned int j = y - (7 - slot->bitmap_top);
			char v;

			if (i < bm->width && j < bm->rows)
				v = bm->buffer[j*bm->pitch + i];
			else
				v = 0;

			if (v) {
				line[x] |= mask;
				debug("#");
			} else
				debug(" ");
		}
		debug("|\n");
	}
	debug("+--------+\n");

	printf("\t{ 0x%02hhx, 0x%02hhx, 0x%02hhx, 0x%02hhx,"
	          " 0x%02hhx, 0x%02hhx, 0x%02hhx, 0x%02hhx }, /* U+%04x",
		line[0], line[1], line[2], line[3],
		line[4], line[5], line[6], line[7], ch);

		if (ch == 32)
			printf(" (space)");
		else if (ch > 32 && ch < 127)
			printf(" (%c)", ch);
		else if (ch == 127)
			printf(" (del)");

		printf(" */\n");

	return 0;
}

int
main(int argc, char *argv[])
{
	FT_Library library;
	FT_Face face;
	FT_Bitmap bm;
	FT_Error err;
	char *filename;
	unsigned int ch;

	if (argc < 2) {
		fprintf(stderr, "usage: %s <font file>\n", argv[0]);
		return EXIT_FAILURE;
	}
	filename = argv[1];

	err = FT_Init_FreeType(&library);
	if (err) {
		fprintf(stderr, "error: Init_Freetype failed %d\n", err);
		return EXIT_FAILURE;
	}

	err = FT_New_Face(library, filename, 0, &face);
	if (err) {
		fprintf(stderr, "error: FT_New_Face failed %d\n", err);
		return EXIT_FAILURE;
	}

	err = FT_Set_Pixel_Sizes(face, 8, 8);
	if (err) {
		fprintf(stderr, "error: FT_Set_Pixel_Sizes failed %d\n", err);
		return EXIT_FAILURE;
	}

	FT_Bitmap_New(&bm);

	printf("#include <stdint.h>\n\n"
	       "static const uint8_t font8x8[%u][8] = {\n",
			CHAR_LAST - CHAR_FIRST + 2);

	for (ch = CHAR_FIRST; ch <= CHAR_LAST ; ch++) {
		err = render_character(library, face, &bm, ch);
		if (err)
			return EXIT_FAILURE;
	}

	printf("\t{ 0x00, 0x7e, 0x42, 0x42, 0x42, 0x42, 0x7e, 0x00 }, /* U+007f (del) */\n");
	printf("};\n");

	FT_Bitmap_Done(library, &bm);
	FT_Done_Face(face);
	FT_Done_FreeType(library);

	return EXIT_SUCCESS;
}
