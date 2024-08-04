#pragma once

// code
constexpr bool LOG_ASSERT = true;

// window
constexpr int WINDOW_WIDTH = 1080;
constexpr int WINDOW_HEIGHT = 720;

// text
constexpr int TEXT_PIXEL_SIZE = 48;
constexpr float CHAT_DISPLAY_TIME = 5;
constexpr float CHAT_FADE_TIME = 5;

// player
constexpr float RANGE_ACTION = 5;

// camera
constexpr int RENDER_DISTANCE = 5;
constexpr int RENDER_DISTANCE_2X = 2 * RENDER_DISTANCE + 1;
constexpr int FOV = 80;

// world
constexpr int CHUNK_LENGTH = 16;
constexpr int CHUNK_LENGTH_PLUS_2 = CHUNK_LENGTH + 2;
constexpr int CHUNK_HEIGHT = 256;
constexpr int CHUNK_ARRAY_SIZE = CHUNK_LENGTH_PLUS_2 * CHUNK_LENGTH_PLUS_2 * CHUNK_HEIGHT;
constexpr int WORLD_LENGTH = 16384;
constexpr int WATER_LEVEL = 62;
constexpr int MINICHUNK_HEIGHT = 16; // for info mode

// world generation
constexpr int NOISE_SIZE = 1024;
constexpr int CONTINENTALNESS_OCTAVES = 8;
constexpr float CONTINENTALNESS_FREQUENCY = 2;
constexpr float CONTINENTALNESS_PERSISTENCE = 0.3;
constexpr int PV_OCTAVES = 8;
constexpr float PV_FREQUENCY = 3;
constexpr float PV_PERSISTENCE = 0.1;

// textures
constexpr int TILESET_WIDTH = 3;
constexpr int TILESET_HEIGHT = 3;

// gameplay
constexpr int DAY_DURATION_IN_MIN = 20;
constexpr int DAY_DURATION_IN_SEC = DAY_DURATION_IN_MIN * 60;