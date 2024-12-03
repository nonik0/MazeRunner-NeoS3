#include <Arduino.h>

#include "maze_runner_7x7.h"

MazeRunner7x7TaskHandler mazeRunner;

void setup()
{
  Serial.begin(115200);
  log_i("Starting setup...");
  mazeRunner.createTask();
  log_i("Setup complete");
}

void loop()
{
}