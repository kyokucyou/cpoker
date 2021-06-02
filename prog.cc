//    Console poker game
//    Copyright (C) 2021  kyokucyou
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU Affero General Public License as published
//    by the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Affero General Public License for more details.
//
//    You should have received a copy of the GNU Affero General Public License
//    along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <map>
#include <algorithm>
#include <functional>
#include <thread>
#include <stdexcept>

#include <cstdlib>
#include <cstring>
#include <cctype>
#include <curses.h>

namespace {
  enum class StartMenuAction {
    NONE,
    SINGLEPLAYER,
    MULTIPLAYER,
    OPTIONS,
    ABOUT,
    FAQ,
    EXIT
  };

  class Program {
  public:
    Program() : act_(StartMenuAction::NONE) {
      initscr();
      noecho();
      cbreak();
      curs_set(0);
    }

    ~Program() { endwin(); }

    StartMenuAction action() const { return act_; }
    void action(StartMenuAction a) { act_ = a; }

  private:
    StartMenuAction act_;
  };

  class TextMenu {
  public:
    struct Item {
      std::string text;
      bool selectable;
      StartMenuAction action;
    };

    TextMenu(const std::vector<Item>&& items) : items_(std::move(items)) { }
    const std::vector<Item>& items() const { return items_; }

  private:
    const std::vector<Item> items_;
  };

  class Window {
  public:
    Window(int rows, int cols, int y, int x)
        : w(newwin(rows, cols, y, x)), rows(rows), cols(cols), y(y), x(x) {
      box(w, 0, 0);
      wrefresh(w);
      keypad(w, 1);
    }

    void RunMenu(const TextMenu& menu, Program& program, const std::string& banner = "") {
      const auto& items = menu.items();
      auto it = std::find_if(items.begin(), items.end(),
                             [] (const auto& i) { return i.selectable; });
      if (it == items.end())
        throw std::logic_error("No selectable item");
      int highlighted = std::distance(items.begin(), it);

      auto redraw = [&]() {
        int i = 0;
        for (auto&& item : items) {
          int len = item.text.length();
          if (i == highlighted)
            wattron(w, A_REVERSE);
          mvwaddstr(w, ++i, (cols - len) / 2, item.text.c_str());
          wattroff(w, A_REVERSE);
        }
        if (int len = banner.length(); len != 0)
          mvwaddstr(w, rows - 1, (cols - len - 2) / 2, (" " + banner + " ").c_str());
        wrefresh(w);
      };

      auto advance = [&]<bool forward>() {
        auto i = it;
        if constexpr (forward)
          while (i != std::prev(items.end())) {
            ++i;
            if (i->selectable)
              break;
          }
        else
          while (i != items.begin()) {
            --i;
            if (i->selectable)
              break;
          }
        it = i;
      };

      bool running = true;
      int ch;
      do {
        redraw();
        switch(ch = wgetch(w)) {
        case KEY_UP:    advance.operator()<false>(); break;
        case KEY_DOWN:  advance.operator()<true>();  break;
        case 10:        running = false;
        }
        highlighted = std::distance(items.begin(), it);
      } while(running);

      program.action(items[highlighted].action);
    }

    ~Window() { delwin(w); }

  private:
    WINDOW *w;
    int rows, cols, y, x;
  };

  const std::array<TextMenu::Item, 8> startMenu = {{
    { "(1) Singleplayer",    true,  StartMenuAction::SINGLEPLAYER },
    { "(2) Multiplayer",     true,  StartMenuAction::MULTIPLAYER  },
    { "(3) Options",         true,  StartMenuAction::OPTIONS      },
    { "",                    false, StartMenuAction::NONE         },
    { "(4) About this game", true,  StartMenuAction::ABOUT        },
    { "(5) Poker FAQ",       true,  StartMenuAction::FAQ          },
    { "",                    false, StartMenuAction::NONE         },
    { "(6) Exit",            true,  StartMenuAction::EXIT         }
  }};
}

int main() {
  try {
    Program p;
    {
      TextMenu menu{{startMenu.begin(), startMenu.end()}};
      Window wStart(15, 50, (LINES - 15) / 2, (COLS - 50) / 2);
      wStart.RunMenu(menu, p, "Created by kyokucyou 2021");
    }
    switch (p.action()) {
    case StartMenuAction::NONE:
      return 1;
    case StartMenuAction::SINGLEPLAYER:
    case StartMenuAction::MULTIPLAYER:
    case StartMenuAction::OPTIONS:
    case StartMenuAction::ABOUT:
    case StartMenuAction::FAQ:
      break;
    case StartMenuAction::EXIT:
      break;
    }
  } catch (const std::exception& e) {
    std::cerr << "The game has crashed due to an error." << std::endl
              << "Error message: " << e.what() << std::endl;
  }

  return 0;
}
