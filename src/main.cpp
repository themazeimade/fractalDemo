#include "glapp.h"
#include "stealpool.h"
#include "threadpool.h"
#include <chrono>
#include <exception>
#include <future>
#include <iostream>

bool job() {
  std::this_thread::sleep_for(std::chrono::milliseconds(700));
  std::cout << "task completed by thread: " << std::this_thread::get_id()
            << std::endl;
  return true;
}

auto gui_ = [] {
  // Create a window called "My First Tool", with a menu bar.
  ImGui::Begin("Hello, ImGui!");           // Create a window
  ImGui::Text("This is a simple window."); // Display some text
  ImGui::Text("Frame rate: %.1f FPS", ImGui::GetIO().Framerate); // Display FPS
  ImGui::End();
};

int main() {
  try {
    /*threadpool pool;*/
    stealpool steal;
    steal.submit();
    glapp::getInst().initContext();
    glapp::getInst().getImgui()->setGUI(gui_);
    glapp::getInst().glAppStart();
  } catch (const std::exception &e) {
    // Catch the exception and print its detailed message
    std::cout << "Exception caught: " << e.what() << std::endl;
  }
  return 0;
}
