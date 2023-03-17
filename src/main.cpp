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

bool add_object(workshop::engine* engine, workshop::object_handle::type type, const std::string& name, float x, float y,
                float z, workshop::selector* selector, workshop::object_handle** obj)
{
  assert(engine);
  assert(selector);
  assert(obj);
  *obj = new (std::nothrow) workshop::object_handle(type, &name);
  if (!(*obj)) return false;

  if (!(*obj)->resource_set(engine)) {
    delete obj;
    return false;
  }

  if (!selector->init(engine, *obj)) {
    delete obj;
    return false;
  }

  (*obj)->selector(selector);
  (*obj)->position(x, y, z);

  selector->destroy();

  return true;
}

bool add_objects(workshop::engine* engine)
{
  assert(engine);

  workshop::object_handle* obj = nullptr;
  workshop::selector selector;

  // add FAERIE at -90, -15, -140
  if (!add_object(engine, workshop::object_handle::type_faerie, "Maja", -90, -15, -140, &selector, &obj)) {
    std::cerr << "Error: Maja object creation failed\n";
    return false;
  }
  delete obj;
  obj = nullptr;

  // add NINJA at -75, -66, -80 and rotate 0, 90, 0
  if (!add_object(engine, workshop::object_handle::type_ninja, "Jacek", -75, -66, -80, &selector, &obj)) {
    std::cerr << "Error: Jacek object creation failed\n";
    return false;
  }
  obj->rotation(0, 90, 0);
  delete obj;
  obj = nullptr;

  // add DWARF at -70, -66, -30 and rotate 0, -90, 0
  if (!add_object(engine, workshop::object_handle::type_dwarf, "Placek", -70, -66, -30, &selector, &obj)) {
    std::cerr << "Error: Placek object creation failed\n";
    return false;
  }
  obj->rotation(0, -90, 0);
  delete obj;
  obj = nullptr;

  // add YODAN at -90, -25, 20
  if (!add_object(engine, workshop::object_handle::type_yodan, "Reksio", -90, -25, 20, &selector, &obj)) {
    std::cerr << "Error: Reksio object creation failed\n";
    return false;
  }
  delete obj;
  obj = nullptr;

  return true;
}

bool run()
{
  workshop::engine* engine = nullptr;
  workshop::camera* camera = nullptr;
  workshop::object_handle* selected_object = nullptr;
  bool status = true;

  // create ENGINE and all its components (font, laser, light, camera)
  // Irrlicht media files are located at IRRLICHT_MEDIA_PATH
  workshop::engine::device_type devType = workshop::engine::device_opengl;
  engine = new (std::nothrow) workshop::engine(IRRLICHT_MEDIA_PATH, &devType);
  assert(engine);  // if it fails we are in deep shit already

  if (!engine->internal_event_receiver_create()) goto fail;
  if (engine->init_device(800, 600, 32, false, true, true)) goto fail;
  if (!engine->font()) goto fail;
  if (!engine->add_laser()) goto fail;
  if (engine->add_light()) goto fail;
  if (engine->create_camera(&camera)) goto fail;

  // position camera [pos: 50, 50, -60; target: -70, 30, -60]
  camera->position(50, 50, -60);
  camera->target(-70, 30, -60);

  // add all objects and their selectors
  if (!add_objects(engine)) goto fail;

  // run 3D engine main loop and add user code to highlight and print the name of the selected object
  while (engine->run()) {
    if (engine->window_active()) {
      if (!engine->begin_scene()) goto fail;

      workshop::object_handle* obj = engine->selected_object();
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
        engine->draw_label(string);
      }

      if (!engine->end_scene()) goto fail;
    } else {
      engine->yield();
    }
  }

  goto finalize;

fail:
  status = false;

finalize:
  if (camera) engine->destroy_camera();
  delete selected_object;
  delete engine;

  return status;
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
