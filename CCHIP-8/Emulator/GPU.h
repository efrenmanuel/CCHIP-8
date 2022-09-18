#pragma once

unsigned long long screen_buffer[32];

void c8_gpu_flip_pixel(short x, short y);
void c8_gpu_cls();
void c8_gpu_refresh_display();
void c8_gpu_clear();
void c8_gpu_initialize();
void c8_gpu_display_sprite(short start_x, short start_y, Byte* sprite, int rows);

void c8_gpu_dump_buffer();