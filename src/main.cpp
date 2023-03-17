/*
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

workshop::object_handle add_object(workshop::engine* engine, workshop::object_handle::type type,
                                   const std::string& name, float x, float y, float z)
{
  assert(engine);

  try {
    workshop::object_handle obj(engine, type, &name);
    workshop::selector selector(engine, &obj);
    obj.selector(&selector);
    obj.position(x, y, z);
    return obj;
  } catch (...) {
    std::throw_with_nested(std::runtime_error("'" + name + "' object creation failed"));
  }
}

void add_objects(workshop::engine* engine)
{
  assert(engine);

  // add FAERIE at -90, -15, -140
  add_object(engine, workshop::object_handle::type_faerie, "Maja", -90, -15, -140);

  // add NINJA at -75, -66, -80 and rotate 0, 90, 0
  auto ninja = add_object(engine, workshop::object_handle::type_ninja, "Jacek", -75, -66, -80);
  ninja.rotation(0, 90, 0);

  // add DWARF at -70, -66, -30 and rotate 0, -90, 0
  auto dwarf = add_object(engine, workshop::object_handle::type_dwarf, "Placek", -70, -66, -30);
  dwarf.rotation(0, -90, 0);

  // add YODAN at -90, -25, 20
  add_object(engine, workshop::object_handle::type_yodan, "Reksio", -90, -25, 20);
}

bool run()
{
  workshop::object_handle* selected_object = nullptr;

  try {
    // create ENGINE and all its components (font, laser, light, camera)
    // Irrlicht media files are located at IRRLICHT_MEDIA_PATH
    workshop::engine::device_type devType = workshop::engine::device_opengl;
    workshop::engine engine(IRRLICHT_MEDIA_PATH, 800, 600, 32, false, true, true, &devType);

    // position camera [pos: 50, 50, -60; target: -70, 30, -60]
    auto* camera = engine.camera();
    camera->position(50, 50, -60);
    camera->target(-70, 30, -60);

    // add all objects and their selectors
    add_objects(&engine);

    // run 3D engine main loop and add user code to highlight and print the name of the selected object
    while (engine.run()) {
      if (engine.window_active()) {
        engine.begin_scene();

        workshop::object_handle* obj = engine.selected_object();
        if (obj != selected_object) {
          if (selected_object) {
            selected_object->highlight(false);
            delete selected_object;
          }

          selected_object = obj;
          if (selected_object) selected_object->highlight(true);
        }
        if (selected_object) {
          std::string string;
          selected_object->name(&string);
          engine.draw_label(string);
        }

        engine.end_scene();
      } else
        engine.yield();
    }

    if (selected_object) delete selected_object;

    return true;
  } catch (const std::exception& ex) {
    log_exception(ex);
  } catch (...) {
    std::cerr << "ERROR: Unknown exception caught\n";
  }

  if (selected_object) delete selected_object;

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
