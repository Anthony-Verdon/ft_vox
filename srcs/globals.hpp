#pragma once

// window
constexpr int WINDOW_WIDTH = 1920;
constexpr int WINDOW_HEIGHT = 1080;

// text
constexpr int TEXT_PIXEL_SIZE = 48;
constexpr float CHAT_DISPLAY_TIME = 5;
constexpr float CHAT_FADE_TIME = 5;

// camera
constexpr int RENDER_DISTANCE = 7;
constexpr int RENDER_DISTANCE_2X = 2 * RENDER_DISTANCE + 1;
constexpr int FOV = 80;

// world
constexpr int CHUNK_LENGTH = 16;
constexpr int CHUNK_LENGTH_PLUS_2 = CHUNK_LENGTH + 2;
constexpr int CHUNK_HEIGHT = 256;
constexpr int CHUNK_ARRAY_SIZE = CHUNK_LENGTH_PLUS_2 * CHUNK_LENGTH_PLUS_2 * CHUNK_HEIGHT;
constexpr int NOISE_SIZE = 128;
constexpr int WATER_LEVEL = 80;

// textures
constexpr int TILESET_WIDTH = 2;
constexpr int TILESET_HEIGHT = 2;

// gameplay
constexpr int DAY_DURATION_IN_MIN = 20;
constexpr int DAY_DURATION_IN_SEC = DAY_DURATION_IN_MIN * 60;

constexpr int CONTINENTALNESS_OCTAVES = 8;
constexpr float CONTINENTALNESS_FREQUENCY = 4;
constexpr float CONTINENTALNESS_PERSISTENCE = 0.3;
constexpr int CONTINENTALNESS_ROUND_FACTOR = 10;
constexpr int EROSION_OCTAVES = 8;
constexpr float EROSION_FREQUENCY = 2;
constexpr float EROSION_PERSISTENCE = 0.2;
constexpr int EROSION_ROUND_FACTOR = 5;
constexpr int PV_OCTAVES = 8;
constexpr float PV_FREQUENCY = 8;
constexpr float PV_PERSISTENCE = 0.1;
constexpr int PV_ROUND_FACTOR = 5;