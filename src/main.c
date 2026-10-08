#define BN_IMPLEMENTATION
#include <bn.h>

#include <SDL3/SDL.h>

#include "renderer.h"
#include "renderer.c"

void OG_sdlLogHandler(
    void* userdata, int category, SDL_LogPriority priority, const char* message
) {
    (void)userdata;

    switch (priority) {
    case SDL_LOG_PRIORITY_DEBUG:
        bn_logDebugf("SDL: [%d] %s", category, message);
        break;
    case SDL_LOG_PRIORITY_INFO:
        bn_logInfof("SDL: [%d] %s", category, message);
        break;
    case SDL_LOG_PRIORITY_WARN:
        bn_logWarnf("SDL: [%d] %s", category, message);
        break;
    case SDL_LOG_PRIORITY_ERROR:
        bn_logErrorf("SDL: [%d] %s", category, message);
        break;
    case SDL_LOG_PRIORITY_COUNT:
    default:
        bn_logInfof("SDL: [%d] %s", category, message);
        break;
    }
}

typedef struct {
    f32 time;
} OG_UniformBuffer;

int main(void) {
    bn_initContext(BN_DEFAULT_CONTEXT_PARAMS);

    bn_logInfo("Hello from origami!");

#if defined(DEBUG)
    SDL_SetLogPriorities(SDL_LOG_PRIORITY_VERBOSE);
#else
    SDL_SetLogPriorities(SDL_LOG_PRIORITY_INFO);
#endif

    SDL_SetLogOutputFunction(OG_sdlLogHandler, NULL);

    OG_Renderer* renderer = og_initRenderer(
        &bn_context->allocator, (OG_RendererInitParams){
                                    .window_title = stringLit("Origami"),
                                    .window_width = 800,
                                    .window_height = 600,
                                }
    );

    SDL_GPUGraphicsPipeline* pipeline = og_initPipeline(
        (OG_PipelineInitParams){
            .vertex_shader =
                (OG_GpuShaderInitParams){
                    .file_path = stringLit("bin/triangle.vert.spv"),
                    .stage = SDL_GPU_SHADERSTAGE_VERTEX,
                },
            .fragment_shader =
                (OG_GpuShaderInitParams){
                    .file_path = stringLit("bin/triangle.frag.spv"),
                    .stage = SDL_GPU_SHADERSTAGE_FRAGMENT,
                    .num_uniform_buffers = 1,
                },
        },
        renderer
    );

    OG_Vertex vertices_array[] = {
        {.position = {{0.0f, 0.5f, 0.0}}, .color = {{1.0, 0.0, 0.0, 1.0}}},
        {.position = {{-0.5f, -0.5f, 0.0}}, .color = {{1.0, 1.0, 0.0, 1.0}}},
        {.position = {{0.5f, -0.5f, 0.0}}, .color = {{1.0, 0.0, 1.0, 1.0}}},
    };

    BN_arrayOG_Vertex vertices = bn_daInit();
    for (u32 i = 0; i < bn_arrayLen(vertices_array); i++) {
        bn_daAppend(&vertices, vertices_array[i], &bn_context->allocator);
    }

    SDL_GPUBufferCreateInfo buffer_info = {0};
    buffer_info.size = sizeof(OG_Vertex) * vertices.length;
    buffer_info.usage = SDL_GPU_BUFFERUSAGE_VERTEX;

    SDL_GPUBuffer* vertex_buffer =
        og_uploadBuffer(buffer_info, vertices.items, renderer);

    bn_allocatorFreeAll(&bn_context->temp_allocator);

    OG_UniformBuffer time_uniform = {0};

    while (renderer->window_open) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_EVENT_QUIT:
                renderer->window_open = false;
                break;
            case SDL_EVENT_WINDOW_RESIZED:;
                SDL_GetWindowSize(
                    renderer->window, &renderer->window_width,
                    &renderer->window_height
                );
                break;
            default:
                break;
            }
        }

        SDL_GPUCommandBuffer* cmd_buffer =
            SDL_AcquireGPUCommandBuffer(renderer->device);

        SDL_GPUTexture* swapchain_texture = {0};
        u32 width, height = 0;
        SDL_WaitAndAcquireGPUSwapchainTexture(
            cmd_buffer, renderer->window, &swapchain_texture, &width, &height
        );

        if (swapchain_texture == NULL) {
            bn_assert(SDL_SubmitGPUCommandBuffer(cmd_buffer));
            break;
        }

        SDL_GPUColorTargetInfo color_target_info = {0};
        color_target_info.clear_color = (SDL_FColor){
            100 / 255.0f, 149 / 255.0f, 237 / 255.0f, 255 / 255.0f
        };
        color_target_info.load_op = SDL_GPU_LOADOP_CLEAR;
        color_target_info.store_op = SDL_GPU_STOREOP_STORE;
        color_target_info.texture = swapchain_texture;

        SDL_GPURenderPass* render_pass =
            SDL_BeginGPURenderPass(cmd_buffer, &color_target_info, 1, NULL);

        SDL_BindGPUGraphicsPipeline(render_pass, pipeline);

        BN_arraySDL_GPUBufferBinding buffer_bindings =
            bn_daInit();
        SDL_GPUBufferBinding vertex_binding = (SDL_GPUBufferBinding){
            .buffer = vertex_buffer,
            .offset = 0,
        };
        bn_daAppend(&buffer_bindings, vertex_binding, &bn_context->allocator);

        SDL_BindGPUVertexBuffers(
            render_pass, 0, buffer_bindings.items, buffer_bindings.length
        );

        time_uniform.time = SDL_GetTicksNS() / 1e9;

        SDL_PushGPUComputeUniformData(
            cmd_buffer, 0, &time_uniform, sizeof(OG_UniformBuffer)
        );

        SDL_DrawGPUPrimitives(render_pass, vertices.length, 1, 0, 0);

        SDL_EndGPURenderPass(render_pass);

        bn_assert(SDL_SubmitGPUCommandBuffer(cmd_buffer));

        bn_allocatorFreeAll(&bn_context->temp_allocator);
    }

    SDL_ReleaseGPUBuffer(renderer->device, vertex_buffer);

    SDL_ReleaseGPUGraphicsPipeline(renderer->device, pipeline);

    og_destroyRenderer(renderer);

    bn_destroyContext();
    return 0;
}
