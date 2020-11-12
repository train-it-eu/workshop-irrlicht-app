﻿/*
 * Copyright (c) 2019, Train IT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <irrlicht-engine/engine.h>
#include <cassert>
#include <iostream>

namespace {

void log_exception(const std::exception& ex, int level = 0)
{
  std::cerr << "ERROR: " << std::string(static_cast<size_t>(level * 2), ' ') << ex.what() << '\n';
  try {
    std::rethrow_if_nested(ex);
  } catch (const std::exception& e) {
    log_exception(e, level + 1);
  } catch (...) {
  }
}

[[nodiscard]] nonstd::expected<workshop::object_handle, std::error_code> add_object(workshop::engine& engine,
                                                                                    workshop::object_handle::type type,
                                                                                    const std::string& name, float x,
                                                                                    float y, float z)
{
  auto obj = workshop::object_handle::create(engine, type, name);
  if (!obj) {
    if (obj.error() == workshop::error_condition::invalid_path)
      std::cerr << "ERROR: Please check that all the files for " << name << " object are provided\n";
    else
      std::cerr << "ERROR: " << name << " object creation failed: " << obj.error().message() << "\n";
    return obj;
  }
  workshop::selector selector(engine, *obj);
  obj->selector(selector);
  obj->position(x, y, z);
  return obj;
}

void add_objects(workshop::engine& engine, std::error_code& ec)
{
  // add FAERIE at -90, -15, -140
  auto result = add_object(engine, workshop::object_handle::type::faerie, "Maja", -90, -15, -140);
  if (!result) {
    ec = result.error();
    return;
  }

  // add NINJA at -75, -66, -80 and rotate 0, 90, 0
  result = add_object(engine, workshop::object_handle::type::ninja, "Jacek", -75, -66, -80);
  if (!result) {
    ec = result.error();
    return;
  }
  result->rotation(workshop::angle{0}, workshop::angle{90}, workshop::angle{0});

  // add DWARF at -70, -66, -30 and rotate 0, -90, 0
  result = add_object(engine, workshop::object_handle::type::dwarf, "Placek", -70, -66, -30);
  if (!result) {
    ec = result.error();
    return;
  }
  result->rotation(workshop::angle{0}, workshop::angle{-90}, workshop::angle{0});

  // add YODAN at -90, -25, 20
  result = add_object(engine, workshop::object_handle::type::yodan, "Reksio", -90, -25, 20);
  if (!result) {
    ec = result.error();
    return;
  }
}

[[nodiscard]] bool run()
{
  try {
    using namespace workshop;

    // create ENGINE and all its components (font, laser, light, camera)
    // Irrlicht media files are located at IRRLICHT_MEDIA_PATH
    engine engine(IRRLICHT_MEDIA_PATH, window_params{window_width(800), window_height(600)}, stencil_buffer(true),
                  vertical_sync(true), engine::device_type::opengl);

    // position camera [pos: 50, 50, -60; target: -70, 30, -60]
    auto& camera = engine.camera();
    camera.position(50, 50, -60);
    camera.target(-70, 30, -60);

    // add all objects and their selectors
    std::error_code ec;
    add_objects(engine, ec);
    if (ec) return false;

    // run 3D engine main loop and add user code to highlight and print the name of the selected object
    std::optional<object_handle> selected_object;

    engine.run(
      [&](std::error_code&) {
        const auto& obj = engine.selected_object();
        if (obj != selected_object) {
          if (selected_object) selected_object->highlight(false);
          selected_object = obj;
          if (selected_object) selected_object->highlight(true);
        }

        if (selected_object) engine.draw_label(selected_object->name());
      },
      ec);

    if (ec) {
      std::cerr << "ERROR: " << ec.message() << "\n";
    } else
      return true;
  } catch (const workshop::invalid_path& ex) {
    std::cerr << "Invalid path provided: " << ex.what() << "\n";
  } catch (const std::exception& ex) {
    log_exception(ex);
  } catch (...) {
    std::cerr << "ERROR: Unknown exception caught\n";
  }

  return false;
}

}  // namespace

int main()
{
  // create, run and cleanup 3D application
  bool status = run();

  // validate if problems found and print statistics
  const auto& counters = workshop::counters::instance();
  counters.print(!counters.validate());

  return status ? EXIT_SUCCESS : EXIT_FAILURE;
}
