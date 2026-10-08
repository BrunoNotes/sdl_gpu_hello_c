#pragma once

#include <bn.h>
#include <SDL3/SDL.h>

bn_dArrayPrototype(SDL_GPUBufferBinding);
bn_dArrayPrototype(SDL_GPUVertexBufferDescription);
bn_dArrayPrototype(SDL_GPUVertexAttribute);
bn_dArrayPrototype(SDL_GPUColorTargetDescription);

typedef struct {
    SDL_Window* window;
    i32 window_width, window_height;
    bool window_open;
    SDL_GPUDevice* device;
} OG_Renderer;

typedef struct {
    String window_title;
    i32 window_width, window_height;
} OG_RendererInitParams;

OG_Renderer*
og_initRenderer(BN_Allocator* allocator, OG_RendererInitParams params);
void og_destroyRenderer(OG_Renderer* renderer);

typedef struct {
    Vec3f32 position;
    Vec4f32 color;
} OG_Vertex;

bn_dArrayPrototype(OG_Vertex);

typedef struct {
    String file_path;
    u32 num_samplers, num_storage_buffers, num_storage_textures,
        num_uniform_buffers;
    SDL_GPUShaderStage stage;
} OG_GpuShaderInitParams;

SDL_GPUShader*
og_initGpuShader(OG_GpuShaderInitParams params, OG_Renderer* renderer);

typedef struct {
    OG_GpuShaderInitParams vertex_shader, fragment_shader;
} OG_PipelineInitParams;

SDL_GPUGraphicsPipeline*
og_initPipeline(OG_PipelineInitParams params, OG_Renderer* renderer);

SDL_GPUBuffer* og_uploadBuffer(
    SDL_GPUBufferCreateInfo buffer_info, void* items, OG_Renderer* renderer
);
