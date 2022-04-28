#include <gtest/gtest-message.h>  // for Message
#include <gtest/gtest-test-part.h>  // for TestPartResult, SuiteApiResolver, TestFactoryImpl
#include <memory>  // for allocator, __shared_ptr_access, shared_ptr
#include <string>  // for string, basic_string
#include <vector>  // for vector

#include "ftxui/component/captured_mouse.hpp"     // for ftxui
#include "ftxui/component/component.hpp"          // for container
#include "ftxui/component/component_base.hpp"     // for ComponentBase
#include "ftxui/component/component_options.hpp"  // for MenuOption
#include "ftxui/component/event.hpp"  // for Event, Event::ArrowDown, Event::Return
#include "ftxui/util/ref.hpp"         // for Ref
#include "gtest/gtest_pred_impl.h"  // for Test, EXPECT_EQ, TEST

namespace ftxui {

namespace {

Event MousePressed(int x, int y) {
  Mouse mouse;
  mouse.button = Mouse::Left;
  mouse.motion = Mouse::Pressed;
  mouse.shift = false;
  mouse.meta = false;
  mouse.control = false;
  mouse.x = x;
  mouse.y = y;
  return Event::Mouse("jjj", mouse);
}

}  // namespace

using namespace std::chrono_literals;

TEST(ButtonTest, Basic) {
  int press_count = 0;
  std::string last_press = "";
  auto btn1 = Button("btn1", [&] {
    press_count++;
    last_press = "btn1";
  });
  auto btn2 = Button("btn2", [&] {
    press_count++;
    last_press = "btn2";
  });

  int selected = 0;
  auto container = Container::Horizontal({
      btn1,
      btn2,
  }, &selected);

  (void)container->Render();

  EXPECT_EQ(selected, 0);
  EXPECT_TRUE(btn1->Focused());
  EXPECT_FALSE(btn2->Focused());

  container->OnEvent(Event::ArrowLeft);
  EXPECT_EQ(selected, 0);
  EXPECT_TRUE(btn1->Focused());
  EXPECT_FALSE(btn2->Focused());

  container->OnEvent(Event::ArrowRight);
  EXPECT_EQ(selected, 1);
  EXPECT_FALSE(btn1->Focused());
  EXPECT_TRUE(btn2->Focused());

  container->OnEvent(Event::ArrowRight);
  EXPECT_EQ(selected, 1);
  EXPECT_FALSE(btn1->Focused());
  EXPECT_TRUE(btn2->Focused());

  EXPECT_EQ(press_count, 0);

  container->OnEvent(Event::Return);
  EXPECT_EQ(press_count, 1);
  EXPECT_EQ(last_press, "btn2");

  container->OnEvent(Event::Return);
  EXPECT_EQ(press_count, 2);
  EXPECT_EQ(last_press, "btn2");

  container->OnEvent(Event::ArrowLeft);
  container->OnEvent(Event::Return);
  EXPECT_EQ(press_count, 3);
  EXPECT_EQ(last_press, "btn1");

  (void)container->Render();
}

TEST(ButtonTest, Animation) {
  int press_count = 0;
  std::string last_press = "";
  auto option = ButtonOption::Animated();
  auto btn1 = Button("btn1", [&] {
    press_count++;
    last_press = "btn1";
  }, option);
  auto btn2 = Button("btn2", [&] {
    press_count++;
    last_press = "btn2";
  }, option);

  int selected = 0;
  auto container = Container::Horizontal({
      btn1,
      btn2,
  }, &selected);

  {
    Screen screen(12, 3);
    Render(screen, container->Render());
    EXPECT_EQ(
        screen.ToString(),
        "\x1B[1m\x1B[38;5;250m\x1B[48;5;16m      \x1B[22m      "
        "\x1B[39m\x1B[49m\r\n\x1B[1m\x1B[38;5;250m\x1B[48;5;16m btn1 \x1B[22m "
        "btn2 \x1B[39m\x1B[49m\r\n\x1B[1m\x1B[38;5;250m\x1B[48;5;16m      "
        "\x1B[22m      \x1B[39m\x1B[49m");
  }
  selected = 1;
  {
    Screen screen(12, 3);
    Render(screen, container->Render());
    EXPECT_EQ(
        screen.ToString(),
        "\x1B[38;5;250m\x1B[48;5;16m      \x1B[1m      "
        "\x1B[22m\x1B[39m\x1B[49m\r\n\x1B[38;5;250m\x1B[48;5;16m btn1 \x1B[1m "
        "btn2 \x1B[22m\x1B[39m\x1B[49m\r\n\x1B[38;5;250m\x1B[48;5;16m      "
        "\x1B[1m      \x1B[22m\x1B[39m\x1B[49m");
  }
  animation::Params params(2s);
  container->OnAnimation(params);
  {
    Screen screen(12, 3);
    Render(screen, container->Render());
    EXPECT_EQ(
        screen.ToString(),
        "\x1B[38;5;250m\x1B[48;5;16m      \x1B[1m\x1B[38;5;231m\x1B[48;5;244m  "
        "    \x1B[22m\x1B[39m\x1B[49m\r\n\x1B[38;5;250m\x1B[48;5;16m btn1 "
        "\x1B[1m\x1B[38;5;231m\x1B[48;5;244m btn2 "
        "\x1B[22m\x1B[39m\x1B[49m\r\n\x1B[38;5;250m\x1B[48;5;16m      "
        "\x1B[1m\x1B[38;5;231m\x1B[48;5;244m      \x1B[22m\x1B[39m\x1B[49m");
  }
  EXPECT_EQ(selected, 1);
  container->OnEvent(MousePressed(3, 1));
  EXPECT_EQ(selected, 0);
  {
    Screen screen(12, 3);
    Render(screen, container->Render());
    EXPECT_EQ(screen.ToString(),
              "\x1B[1m\x1B[38;5;253m\x1B[48;5;238m      "
              "\x1B[22m\x1B[38;5;231m\x1B[48;5;244m      "
              "\x1B[39m\x1B[49m\r\n\x1B[1m\x1B[38;5;253m\x1B[48;5;238m btn1 "
              "\x1B[22m\x1B[38;5;231m\x1B[48;5;244m btn2 "
              "\x1B[39m\x1B[49m\r\n\x1B[1m\x1B[38;5;253m\x1B[48;5;238m      "
              "\x1B[22m\x1B[38;5;231m\x1B[48;5;244m      \x1B[39m\x1B[49m");
  }
  container->OnAnimation(params);
  {
    Screen screen(12, 3);
    Render(screen, container->Render());
    EXPECT_EQ(screen.ToString(),
              "\x1B[1m\x1B[38;5;231m\x1B[48;5;244m      "
              "\x1B[22m\x1B[38;5;250m\x1B[48;5;16m      "
              "\x1B[39m\x1B[49m\r\n\x1B[1m\x1B[38;5;231m\x1B[48;5;244m btn1 "
              "\x1B[22m\x1B[38;5;250m\x1B[48;5;16m btn2 "
              "\x1B[39m\x1B[49m\r\n\x1B[1m\x1B[38;5;231m\x1B[48;5;244m      "
              "\x1B[22m\x1B[38;5;250m\x1B[48;5;16m      \x1B[39m\x1B[49m");
  }
}

}  // namespace ftxui

// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.