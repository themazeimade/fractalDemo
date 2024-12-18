#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "glapp.h"
#include <chrono>
#include <cmath>
#include <cstdio>
#include <iostream>

struct scrollingBuff {
  int MAXSIZE;
  int offset;
  ImVector<ImVec2> Data;
  scrollingBuff(int max_size = 2000) {
    MAXSIZE = max_size;
    offset = 0;
    Data.reserve(MAXSIZE);
  }
  void addData(float x, float y) {
    if (Data.size() < MAXSIZE) {
      Data.push_back(ImVec2(x, y));
    } else {
      Data[offset] = ImVec2(x, y);
      offset = (offset + 1) % MAXSIZE;
    }
  }
  void erase() {
    if (Data.size() > 0) {
      Data.shrink(0);
      offset = 0;
    }
  }
};

bool job() { // simulate job
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  std::cout << "task completed by thread: " << std::this_thread::get_id()
            << std::endl;
  return true;
}

void jobs(int n) {
  for (int i = 0; i < n; ++i) {
    Tpool.submit(job);
  }
}

auto Stats_ = [] {
  // Create a window called "My First Tool", with a menu bar.
  ImGui::Begin("ThreadPool Stats"); // Create a window
  static scrollingBuff taskStat;
  static float t = 0;
  t += ImGui::GetIO().DeltaTime;
  taskStat.addData(t, (float)(Tpool.getRamaining_task()) * 0.05f);
  /*ImGui::Text("This is a simple window."); // Display some text*/
  /*ImGui::Text("Frame rate: %.1f FPS", ImGui::GetIO().Framerate); // Display
   * FPS*/

  static ImPlotAxisFlags flags = ImPlotAxisFlags_NoTickLabels;
  static float history = 10.0f;

  if (ImPlot::BeginPlot("##Scrolling", ImVec2(-1, 800))) {
    ImPlot::SetupAxes(nullptr, nullptr, flags, flags);
    ImPlot::SetupAxisLimits(ImAxis_X1, t - history, t, ImGuiCond_Always);
    ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 1);
    ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5f);
    ImPlot::PlotShaded("Task in Queue", &taskStat.Data[0].x,
                       &taskStat.Data[0].y, taskStat.Data.size(), -INFINITY, 0,
                       taskStat.offset, 2 * sizeof(float));
    ImPlot::EndPlot();
  }
  ImGui::End();
};

static bool showStats = false;

auto Control_ = [] {
  ImGui::Begin("Simulate things");
  ImGui::Checkbox("Show Stats", &showStats);
  if (ImGui::Button("Submit Job")) {
    Tpool.submit(job);
  }
  if (ImGui::Button("Submit 5 Jobs")) {
    jobs(5);
  }
  if (ImGui::Button("Submit 10 Jobs")) {
    jobs(10);
  }
  if (ImGui::Button("Submit 100 Jobs")) {
    jobs(100);
  }
  ImGui::End();
};

auto gui_ = [] {
  Control_();
  if (showStats) {
    Stats_();
  }
};

// entry point
int main() {
  /*if (Tpool.isDone()==0) {*/
  /*  printf("ThreadPool is Done");*/
  /*} else {*/
  /*  printf("ThreadPool is not Done");*/
  /*}*/
  try {
    // Code that may throw an exception
    glapp::getInst().initContext();
    glapp::getInst().getImgui()->setGUI(gui_);
    glapp::getInst().glAppStart();
  } catch (const std::exception &e) {
    // Code to handle the exception
    std::cerr << "Exception caught: " << e.what() << std::endl;
  } catch (...) {
    // Catch any exception (optional)
    std::cerr << "An unknown exception occurred!" << std::endl;
  }
  return 0;
}
