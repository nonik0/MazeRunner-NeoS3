#pragma once

#include <Adafruit_NeoPixel.h>

#include "display_task_handler.h"
#include "maze_runner_lib.h"

class MazeRunner7x7TaskHandler : public DisplayTaskHandler
{
private:
    const int MAZE_DELAY_MS = 60;
    const uint32_t BLACK = Adafruit_NeoPixel::Color(0x00, 0x00, 0x00);
    const uint32_t RED = Adafruit_NeoPixel::Color(0xFF, 0x00, 0x00);
    const uint32_t ORANGE = Adafruit_NeoPixel::Color(0xCC, 0x44, 0x00);
    const uint32_t YELLOW = Adafruit_NeoPixel::Color(0xFF, 0xFF, 0x00);
    const uint32_t YELLOWGREEN = Adafruit_NeoPixel::Color(0xCC, 0xFF, 0x00);
    const uint32_t GREEN = Adafruit_NeoPixel::Color(0x00, 0xFF, 0x00);
    const uint32_t BLUE = Adafruit_NeoPixel::Color(0x00, 0x00, 0x77);
    const uint32_t PURPLE = Adafruit_NeoPixel::Color(0x77, 0x00, 0x77);
    static const uint8_t EN_PIN = 39;
    static const uint8_t RGB_LED_MATRIX_PIN = 16;
    static const uint8_t RGB_LED_PIN = 40;
    static const uint8_t BLUE_LED_PIN = 13;
    static const int WIDTH = 7;
    static const int HEIGHT = 7;

    Adafruit_NeoPixel _matrix;
    Adafruit_NeoPixel _rgbLed;
    MazeRunner *_mazeRunner;

public:
    MazeRunner7x7TaskHandler() : _rgbLed(1, RGB_LED_PIN), _matrix(WIDTH * HEIGHT, RGB_LED_MATRIX_PIN) {}

    bool createTask() override;
    void setDisplay(bool display) override;

private:
    void task(void *parameters) override;
};

bool MazeRunner7x7TaskHandler::createTask()
{
    log_i("Starting MazeRunner7x7 setup");

    if (_taskHandle != NULL)
    {
        log_w("Task already started");
        return false;
    }

    pinMode(EN_PIN, OUTPUT);
    digitalWrite(EN_PIN, true);

    pinMode(BLUE_LED_PIN, OUTPUT);
    digitalWrite(BLUE_LED_PIN, false);

    _matrix.begin();
    _matrix.setBrightness(10);

    _rgbLed.begin();
    _rgbLed.setBrightness(20);
    _rgbLed.setPixelColor(0, GREEN);
    _rgbLed.show();

    _mazeRunner = new MazeRunner(
        WIDTH,
        HEIGHT,
        BLACK,  // off
        ORANGE, // wall
        YELLOW, // runner
        RED,    // sentry
        PURPLE, // exit
        [this](int x, int y, uint32_t c)
        { _matrix.setPixelColor(y * WIDTH + x, c); },
        [this](uint32_t c)
        { _rgbLed.setPixelColor(0, c); if (c == PURPLE) { digitalWrite(BLUE_LED_PIN, true); } });

    _mazeRunner->init();

    log_i("Starting MazeRunner7x7Task");
    xTaskCreatePinnedToCore(taskWrapper, "MazeRunner7x7Task", 4096 * 4, this, 2, &_taskHandle, 0); // other Arduino tasks are on Core 1

    log_i("MazeRunner7x7 setup complete");
    return true;
}

void MazeRunner7x7TaskHandler::setDisplay(bool displayState)
{
    DisplayTaskHandler::setDisplay(displayState);

    log_i("Setting display to %s", displayState ? "on" : "off");
    digitalWrite(EN_PIN, displayState); // turns off LDO for 7x7 matrix
}

void MazeRunner7x7TaskHandler::task(void *parameters)
{
    while (1)
    {
        if (_display && _mazeRunner->update())
        {
            _matrix.show();
            _rgbLed.show();
        }
        delay(MAZE_DELAY_MS);
    }
}
