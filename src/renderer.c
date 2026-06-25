#include "renderer.h"

OG_Renderer*
og_initRenderer(BN_Allocator* allocator, OG_RendererInitParams params) {
    OG_Renderer* renderer = bn_allocPushStruct(allocator, OG_Renderer);

    renderer->window = SDL_CreateWindow(
        params.window_title.ptr, params.window_width, params.window_height,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN
    );
    bn_assert(renderer->window != NULL);

    renderer->window_width = params.window_width;
    renderer->window_height = params.window_height;
    renderer->window_open = true;

#ifdef DEBUG
    bool debug_mode = true;
#else
    bool debug_mode = false;
#endif

    renderer->device =
        SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, debug_mode, NULL);

    bn_assert(SDL_ClaimWindowForGPUDevice(renderer->device, renderer->window));

    return renderer;
}

void og_destroyRenderer(OG_Renderer* renderer) {
    SDL_DestroyGPUDevice(renderer->device);
    SDL_DestroyWindow(renderer->window);
}

SDL_GPUShader*
og_initGpuShader(OG_GpuShaderInitParams params, OG_Renderer* renderer) {
    u64 code_size;
    void* code = SDL_LoadFile(params.file_path.ptr, &code_size);

    SDL_GPUShaderCreateInfo shader_info = {0};
    shader_info.code = (u8*)code;
    shader_info.code_size = code_size;
    shader_info.entrypoint = "main";
    shader_info.format = SDL_GPU_SHADERFORMAT_SPIRV;
    shader_info.stage = params.stage;
    shader_info.num_samplers = params.num_samplers;
    shader_info.num_storage_buffers = params.num_storage_buffers;
    shader_info.num_storage_textures = params.num_storage_textures;
    shader_info.num_uniform_buffers = params.num_uniform_buffers;

    SDL_GPUShader* shader = SDL_CreateGPUShader(renderer->device, &shader_info);

    SDL_free(code);

    return shader;
}

SDL_GPUGraphicsPipeline*
og_initPipeline(OG_PipelineInitParams params, OG_Renderer* renderer) {
    SDL_GPUShader* vertex_shader =
        og_initGpuShader(params.vertex_shader, renderer);

    SDL_GPUShader* fragment_shader =
        og_initGpuShader(params.fragment_shader, renderer);

    SDL_GPUGraphicsPipelineCreateInfo pipeline_info = {0};
    pipeline_info.vertex_shader = vertex_shader;
    pipeline_info.fragment_shader = fragment_shader;
    pipeline_info.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;

    BN_sliceSDL_GPUVertexBufferDescription vertex_buffer_descriptions = {0};
    bn_sliceAppendArray(
        vertex_buffer_descriptions,
        ((SDL_GPUVertexBufferDescription[]){
            {
                .slot = 0,
                .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
                .instance_step_rate = 0,
                .pitch = sizeof(OG_Vertex),
            },
        })
    );

    pipeline_info.vertex_input_state.num_vertex_buffers =
        vertex_buffer_descriptions.count;
    pipeline_info.vertex_input_state.vertex_buffer_descriptions =
        vertex_buffer_descriptions.items;

    BN_sliceSDL_GPUVertexAttribute vertex_attributes = {0};
    bn_sliceAppendArray(
        vertex_attributes, ((SDL_GPUVertexAttribute[]){
                               // position
                               {.buffer_slot = 0,
                                .location = 0,
                                .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
                                .offset = 0},
                               // color
                               {
                                   .buffer_slot = 0,
                                   .location = 1,
                                   .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4,
                                   .offset = sizeof(f32) * 3,
                               },
                           })
    );

    pipeline_info.vertex_input_state.num_vertex_attributes =
        vertex_attributes.count;
    pipeline_info.vertex_input_state.vertex_attributes =
        vertex_attributes.items;

    BN_sliceSDL_GPUColorTargetDescription color_target_descriptions = {0};
    bn_sliceAppendArray(
        color_target_descriptions,
        ((SDL_GPUColorTargetDescription[]){
            {
                .blend_state =
                    {
                        .enable_blend = true,
                        .color_blend_op = SDL_GPU_BLENDOP_ADD,
                        .alpha_blend_op = SDL_GPU_BLENDOP_ADD,
                        .src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
                        .dst_color_blendfactor =
                            SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
                        .src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
                        .dst_alpha_blendfactor =
                            SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
                    },
                .format = SDL_GetGPUSwapchainTextureFormat(
                    renderer->device, renderer->window
                ),
            },
        })
    );

    pipeline_info.target_info.num_color_targets =
        color_target_descriptions.count;
    pipeline_info.target_info.color_target_descriptions =
        color_target_descriptions.items;

    SDL_GPUGraphicsPipeline* graphics_pipeline =
        SDL_CreateGPUGraphicsPipeline(renderer->device, &pipeline_info);

    SDL_ReleaseGPUShader(renderer->device, vertex_shader);
    SDL_ReleaseGPUShader(renderer->device, fragment_shader);

    return graphics_pipeline;
}
