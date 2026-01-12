#include <wups.h>
#include <gx2/common.h>
#include <nsysnet/socket.h>
#include <coreinit/thread.h>
#include <coreinit/time.h>
#include <string.h>
#include <malloc.h>

#define PORT_VIDEO 5000
#define MAX_PACKET_SIZE 1400

static int video_sock = -1;
static struct sockaddr_in windows_addr;
static uint8_t *video_buffer = NULL;
static uint32_t last_size = 0;
static OSThread stream_thread;
static uint8_t thread_stack[0x4000] __attribute__((aligned(32)));

WUPS_PLUGIN_NAME("XtremeStreamer");
WUPS_PLUGIN_AUTHOR("User");
WUPS_PLUGIN_VERSION("1.2");

// Hilo separado para que la red no trabe el juego
static int StreamThreadMain(int argc, const char **argv) {
    while (true) {
        if (video_sock >= 0 && video_buffer != NULL && last_size > 0) {
            // Enviamos la imagen en trozos para no saturar el buffer de Windows
            for (uint32_t i = 0; i < last_size; i += MAX_PACKET_SIZE) {
                uint32_t to_send = (last_size - i > MAX_PACKET_SIZE) ? MAX_PACKET_SIZE : (last_size - i);
                sendto(video_sock, video_buffer + i, to_send, 0, (struct sockaddr*)&windows_addr, sizeof(windows_addr));
            }
            last_size = 0; // Esperar al siguiente frame
        }
        OSSleepTicks(OSMillisecondsToTicks(16)); // Limitar a 60fps para estabilidad
    }
    return 0;
}

WUPS_INITIALIZE() {
    socket_lib_init();
    video_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    memset(&windows_addr, 0, sizeof(windows_addr));
    windows_addr.sin_family = AF_INET;
    windows_addr.sin_port = htons(PORT_VIDEO);
    windows_addr.sin_addr.s_addr = 0xFFFFFFFF; // Broadcast para descubrimiento

    int broadcastEnable = 1;
    setsockopt(video_sock, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));

    // Crear el hilo de transmisión
    OSCreateThread(&stream_thread, StreamThreadMain, 0, NULL, thread_stack + sizeof(thread_stack), sizeof(thread_stack), 10, OS_THREAD_ATTRIB_AFFINITY_CPU1);
    OSResumeThread(&stream_thread);

    return WUPS_INIT_OK;
}

DECL_FUNCTION(void, GX2CopyScanBuffer, void* buffer, uint32_t scanTarget) {
    real_GX2CopyScanBuffer(buffer, scanTarget);

    // Solo copiamos los datos, el hilo de arriba se encarga de enviarlos
    if (buffer != NULL && last_size == 0) { 
        video_buffer = (uint8_t*)buffer;
        last_size = 0x10000; // Tamaño de prueba ajustable
    }
}

WUPS_MUST_REPLACE(GX2CopyScanBuffer, WUPS_LOADER_LIBRARY_GX2, GX2CopyScanBuffer);
} 

// Registro del "gancho" para capturar el video
WUPS_MUST_REPLACE(GX2CopyScanBuffer, WUPS_LOADER_LIBRARY_GX2, GX2CopyScanBuffer);

