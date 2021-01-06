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
//    along with this program.  If not, see <https://www.gnu.org/licenses/>.#include <array>

#include <vector>
#include <algorithm>
#include <functional>
#include <thread>
#include <stdexcept>

#include <cstdlib>
#include <cstring>
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
      const char *text;
      bool selectable;
      std::function<void(Program&)> action;
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

    int RunMenu(const TextMenu& menu) {
      const auto& items = menu.items();
      auto it = std::find_if(items.begin(), items.end(),
                             [] (const auto& i) { return i.selectable; });
      if (it == items.end())
        throw std::logic_error("No selectable item");
      int highlighted = std::distance(items.begin(), it);

      auto redraw = [&]() {
        int i = 0;
        for (auto&& item : items) {
          int len = std::strlen(item.text);
          if (i == highlighted)
            wattron(w, A_REVERSE);
          mvwaddstr(w, ++i, (cols - len) / 2, item.text);
          wattroff(w, A_REVERSE);
        }
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
    }

    ~Window() { delwin(w); }

  private:
    WINDOW *w;
    int rows, cols, y, x;
  };

  const std::array<TextMenu::Item, 8> startMenu = {{
    { "(1) Singleplayer",    true,  [](Program& p) { p.action(StartMenuAction::SINGLEPLAYER); } },
    { "(2) Multiplayer",     true,  [](Program& p) { p.action(StartMenuAction::MULTIPLAYER); } },
    { "(3) Options",         true,  [](Program& p) { p.action(StartMenuAction::OPTIONS); } },
    { "",                    false, [](Program& p) { p.action(StartMenuAction::NONE); } },
    { "(4) About this game", true,  [](Program& p) { p.action(StartMenuAction::ABOUT); } },
    { "(5) Poker FAQ",       true,  [](Program& p) { p.action(StartMenuAction::FAQ); } },
    { "",                    false, [](Program& p) { p.action(StartMenuAction::NONE); } },
    { "(6) Exit",            true,  [](Program& p) { p.action(StartMenuAction::EXIT); } }
  }};
}

int main() {
  Program p;
  {
    TextMenu menu{{startMenu.begin(), startMenu.end()}};
    Window wStart(15, 50, (LINES - 15) / 2, (COLS - 50) / 2);
    wStart.RunMenu(menu);
  }
  return 0;
}
