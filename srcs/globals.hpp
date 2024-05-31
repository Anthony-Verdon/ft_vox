#pragma once

constexpr int WINDOW_WIDTH = 720;
constexpr int WINDOW_HEIGHT = 480;

constexpr int RENDER_DISTANCE = 5;
constexpr int RENDER_DISTANCE_2X = 2 * RENDER_DISTANCE + 1;
constexpr int FOV = 80;

constexpr int WORLD_LENGTH = 256;

constexpr int CHUNK_LENGTH = 16;
constexpr int CHUNK_HEIGHT = 256;
constexpr int HALF_CHUNK_HEIGHT = CHUNK_HEIGHT / 2;
constexpr int NB_CHUNK_ON_EDGE = WORLD_LENGTH / CHUNK_LENGTH;

constexpr int TEXT_PIXEL_SIZE = 48;
constexpr float CHAT_DISPLAY_TIME = 5;
constexpr float CHAT_FADE_TIME = 5;

#include <array>
constexpr std::array<int, 3> GREEN_COLOR = {0, 255, 0};
constexpr std::array<int, 3> BlUE_COLOR = {0, 0, 255};