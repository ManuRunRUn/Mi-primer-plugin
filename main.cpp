#include <wups.h>
#include <gx2/common.h>
#include <nsysnet/socket.h>
#include <string.h>

#define PORT_APP 5001 

static int video_sock = -1;
static struct sockaddr_in app_addr;

WUPS_PLUGIN_NAME("XtremeStreamer");
WUPS_PLUGIN_AUTHOR("User");
WUPS_PLUGIN_VERSION("4.0");

WUPS_INITIALIZE() {
    socket_lib_init();
    video_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    return WUPS_INIT_OK;
}

DECL_FUNCTION(void, GX2CopyScanBuffer, void* buffer, uint32_t target) {
    real_GX2CopyScanBuffer(buffer, target);

    if (video_sock >= 0 && buffer != NULL) {
        // Enviamos el ID que tu app de Python espera recibir
        sendto(video_sock, "WIIU_ID", 7, 0, (struct sockaddr*)&app_addr, sizeof(app_addr));
        
        // Enviamos los datos para que tu app los procese
        sendto(video_sock, buffer, 1400, 0, (struct sockaddr*)&app_addr, sizeof(app_addr));
    }
}

WUPS_MUST_REPLACE(GX2CopyScanBuffer, WUPS_LOADER_LIBRARY_GX2, GX2CopyScanBuffer);
