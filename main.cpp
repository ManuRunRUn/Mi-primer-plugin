#include <wups.h>
#include <gx2/common.h>
#include <nsysnet/socket.h>
#include <string.h>

// Puerto definido en tu receptor_pro.py
#define PORT 5001 

WUPS_PLUGIN_NAME("XtremeStreamer");
WUPS_PLUGIN_AUTHOR("User");
WUPS_PLUGIN_VERSION("1.0");

static int video_sock = -1;
static struct sockaddr_in addr;

WUPS_INITIALIZE() {
    socket_lib_init();
    video_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = 0xFFFFFFFF; // Broadcast para localizacion

    return WUPS_INIT_OK;
}

DECL_FUNCTION(void, GX2CopyScanBuffer, void* buffer, uint32_t target) {
    real_GX2CopyScanBuffer(buffer, target);

    if (video_sock >= 0 && buffer != NULL) {
        // Envia el ID que espera tu receptor_pro.py
        sendto(video_sock, "WIIU_ID", 7, 0, (struct sockaddr*)&addr, sizeof(addr));
    }
}

WUPS_MUST_REPLACE(GX2CopyScanBuffer, WUPS_LOADER_LIBRARY_GX2, GX2CopyScanBuffer);
