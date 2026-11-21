// 부하 측정용 가짜 Spout Sender (VRChat 대신 사용)
// 사용: test_sender.exe [너비] [높이] [fps] [초]   기본 1920 1080 60 90
#include <windows.h>
#include <mmsystem.h>
#include <vector>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include "SpoutDX.h"

int main(int argc, char** argv) {
    unsigned w = argc > 1 ? atoi(argv[1]) : 1920, h = argc > 2 ? atoi(argv[2]) : 1080;
    double fps = argc > 3 ? atof(argv[3]) : 60, secs = argc > 4 ? atof(argv[4]) : 90;
    spoutDX s;
    if (!s.OpenDirectX11()) return 1;
    s.SetSenderName("AO_TestSender");
    s.SetSenderFormat(DXGI_FORMAT_B8G8R8A8_UNORM);
    std::vector<uint32_t> px((size_t)w * h);
    timeBeginPeriod(1);
    LARGE_INTEGER f, t0, t; QueryPerformanceFrequency(&f); QueryPerformanceCounter(&t0);
    double interval = 1.0 / fps; double next = 0; long frame = 0;
    for (;;) {
        QueryPerformanceCounter(&t);
        double now = (double)(t.QuadPart - t0.QuadPart) / f.QuadPart;
        if (now > secs) break;
        if (now < next) { Sleep(1); continue; }
        next += interval;
        // 투명 배경 위를 움직이는 원 (premultiplied 아님, straight alpha)
        float cx = w * (0.5f + 0.3f * (float)std::sin(now)), cy = h * 0.5f, r = h * 0.3f;
        for (unsigned y = 0; y < h; y++) {
            uint32_t* row = &px[(size_t)y * w];
            float dy = y - cy;
            for (unsigned x = 0; x < w; x++) {
                float dx = x - cx;
                row[x] = (dx * dx + dy * dy < r * r) ? 0xFF3C78E6u : 0x00000000u;
            }
        }
        s.SendImage((const unsigned char*)px.data(), w, h);
        frame++;
    }
    if (FILE* fp = fopen("test_sender_fps.txt", "w")) { fprintf(fp, "%.1f fps\n", frame / secs); fclose(fp); }
    s.ReleaseSender();
    s.CloseDirectX11();
    timeEndPeriod(1);
    return 0;
}
