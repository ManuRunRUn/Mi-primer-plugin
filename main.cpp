#include <wups.h>
#include <gx2/common.h>
#include <nsysnet/socket.h>
#include <string.h>
#include <coreinit/thread.h>

#define PORT_APP 5001

static int video_sock = -1;
static struct sockaddr_in app_addr;
static bool app_connected = false;

WUPS_PLUGIN_NAME("XtremeStreamer");
WUPS_PLUGIN_AUTHOR("User");
WUPS_PLUGIN_VERSION("2.1");

WUPS_INITIALIZE() {
    socket_lib_init();
    video_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    // Modo no bloqueante para no congelar el juego
    int enable = 1;
    setsockopt(video_sock, SOL_SOCKET, SO_NONBLOCK, &enable, sizeof(enable));

    return WUPS_INIT_OK;
}

DECL_FUNCTION(void, GX2CopyScanBuffer, void* buffer, uint32_t scanTarget) {
    real_GX2CopyScanBuffer(buffer, scanTarget);

    if (buffer == NULL || video_sock < 0) return;

    // Escuchar el "Ping" de tu app de Python
    char recv_buf[16];
    struct sockaddr_in sender_addr;
    int addr_len = sizeof(sender_addr);
    
    // Si tu app manda algo, respondemos para que nos detecte
    int bytes = recvfrom(video_sock, recv_buf, sizeof(recv_buf), 0, (struct sockaddr*)&sender_addr, &addr_len);
    if (bytes > 0) {
        // Tu app espera "WIIU_ID", así que se lo mandamos de vuelta
        sendto(video_sock, "WIIU_ID", 7, 0, (struct sockaddr*)&sender_addr, sizeof(sender_addr));
        app_addr = sender_addr;
        app_addr.sin_port = htons(PORT_APP);
        app_connected = true;
    }

    // Si ya nos detectó, empezamos a mandar la imagen
    if (app_connected) {
        sendto(video_sock, buffer, 1400, 0, (struct sockaddr*)&app_addr, sizeof(app_addr));
    }
}

WUPS_MUST_REPLACE(GX2CopyScanBuffer, WUPS_LOADER_LIBRARY_GX2, GX2CopyScanBuffer);
