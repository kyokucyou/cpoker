#    Console poker game
#    Copyright (C) 2021  kyokucyou
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Affero General Public License as published
#    by the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Affero General Public License for more details.
#
#    You should have received a copy of the GNU Affero General Public License
#    along with this program.  If not, see <https://www.gnu.org/licenses/>.

# => gcc version 10.2
CC       = gcc-12
CCP      = g++-12
CFLAGS   = -Wall -Wextra -g
CCPFLAGS = -std=c++2a
LDFLAGS  = -lncurses
PROGNAME = cpoker

OBJS = prog.o

all : $(PROGNAME)

$(PROGNAME) : $(OBJS)
	$(CCP) $(CFLAGS) $(CCPFLAGS) $(LDFLAGS) -o $@ $^

%.o : %.cc
	$(CCP) -c $(CFLAGS) $(CCPFLAGS) -o $@ $<

.PHONY : clean
clean :
	rm -rfv $(PROGNAME) $(OBJS)
