# uc-fontgen - A microchip font generator
# Copyright (C) 2017 Emil Renner Berthing
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# version 2 as published by the Free Software Foundation
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

MAKEFLAGS = -rR

NAME      = uc-fontgen

CC        = $(CROSS_COMPILE)gcc -std=gnu99
CFLAGS    = -O2 -Wall -Wextra
CFLAGS   += $(shell freetype-config --cflags)
LIBS     += $(shell freetype-config --libs)

RM_F      = rm -f

.PHONY: all debug clean

all: $(NAME)

$(NAME): $(NAME).c
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $^ $(LIBS)

clean:
	$(RM_F) $(NAME)
