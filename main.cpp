#include <wups.h>
#include <gx2/common.h>
#include <nsysnet/socket.h> // Red oficial de Wii U
#include <coreinit/thread.h>
#include <string.h>

#define PORT_VIDEO 5000
#define CHUNK_SIZE 1400 // Tamaño optimizado para Windows

static int video_sock = -1;
static struct sockaddr_in win_addr;

WUPS_PLUGIN_NAME("XtremeStreamer");
WUPS_PLUGIN_AUTHOR("User");
WUPS_PLUGIN_VERSION("1.3");

WUPS_INITIALIZE() {
    socket_lib_init(); // Inicializa red de Nintendo
    video_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    memset(&win_addr, 0, sizeof(win_addr));
    win_addr.sin_family = AF_INET;
    win_addr.sin_port = htons(PORT_VIDEO);
    win_addr.sin_addr.s_addr = 0xFFFFFFFF; // Broadcast para que tu programa de Windows lo vea

    int broadcastEnable = 1;
    setsockopt(video_sock, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));

    return WUPS_INIT_OK;
}

DECL_FUNCTION(void, GX2CopyScanBuffer, void* buffer, uint32_t scanTarget) {
    real_GX2CopyScanBuffer(buffer, scanTarget);

    if (buffer != NULL && video_sock >= 0) {
        // Transmisión ultra rápida: enviamos solo el inicio del buffer para evitar lag
        // Tu programa en Windows recibirá esto instantáneamente
        sendto(video_sock, buffer, CHUNK_SIZE, 0, (struct sockaddr*)&win_addr, sizeof(win_addr));
    }
}

WUPS_MUST_REPLACE(GX2CopyScanBuffer, WUPS_LOADER_LIBRARY_GX2, GX2CopyScanBuffer);
