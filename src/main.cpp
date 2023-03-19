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
#include <iostream>

namespace {

bool run()
{
  bool status = true;

  std::cout << "Irrlicht media path: " << IRRLICHT_MEDIA_PATH << '\n';

  // create ENGINE and all its components (font, laser, light, camera)
  // Irrlicht media files are located at IRRLICHT_MEDIA_PATH

  // position camera [pos: 50, 50, -60; target: -70, 30, -60]

  // add all objects and their selectors
  // add FAERIE at -90, -15, -140
  // add NINJA at -75, -66, -80 and rotate 0, 90, 0
  // add DWARF at -70, -66, -30 and rotate 0, -90, 0
  // add YODAN at -90, -25, 20

  // run 3D engine main loop and add user code to highlight and print the name of the selected object

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
