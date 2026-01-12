#include <wups.h>
#include <gx2/common.h>
#include <nsysnet/socket.h>
#include <coreinit/thread.h>
#include <string.h>

#define PORT_VIDEO 5000
#define CHUNK_SIZE 1400

static int video_sock = -1;
static struct sockaddr_in win_addr;
static int frame_counter = 0;

WUPS_PLUGIN_NAME("XtremeStreamer");
WUPS_PLUGIN_AUTHOR("User");
WUPS_PLUGIN_VERSION("1.4");

WUPS_INITIALIZE() {
    socket_lib_init();
    video_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    memset(&win_addr, 0, sizeof(win_addr));
    win_addr.sin_family = AF_INET;
    win_addr.sin_port = htons(PORT_VIDEO);
    win_addr.sin_addr.s_addr = 0xFFFFFFFF; // Broadcast para Windows

    int broadcastEnable = 1;
    setsockopt(video_sock, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));

    return WUPS_INIT_OK;
}

DECL_FUNCTION(void, GX2CopyScanBuffer, void* buffer, uint32_t scanTarget) {
    real_GX2CopyScanBuffer(buffer, scanTarget);

    // MODALIDAD ANTILAG: Solo envía 1 de cada 2 cuadros para máxima fluidez en Windows
    frame_counter++;
    if (frame_counter % 2 != 0) return; 

    if (buffer != NULL && video_sock >= 0) {
        sendto(video_sock, buffer, CHUNK_SIZE, 0, (struct sockaddr*)&win_addr, sizeof(win_addr));
    }
}

WUPS_MUST_REPLACE(GX2CopyScanBuffer, WUPS_LOADER_LIBRARY_GX2, GX2CopyScanBuffer);
