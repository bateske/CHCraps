#pragma GCC optimize("Os")   // cold code: size over speed (hot pixel loops live in Draw/Mask and CHGfx)
#include <CHGfx.h>
#include "Felt.h"
#include "Layout.h"
#include "Zones.h"
#include "Chips.h"
#include "../game/Craps.h"
#include "../gfx/Draw.h"
#include "../gfx/Palette.h"

namespace felt {

using namespace lay;

static void centred35(int cx, int y, const char *s, uint8_t c) { text35(cx - text35Width(s) / 2, y, s, c); }
static void centred57(int cx, int y, const char *s, uint8_t c) { gfx_text(cx - gfx_textWidth(s) / 2, y, s, c); }

// Two dice side by side, the way the centre of a real layout pictures its bets.
static void pair(int x, int y, uint8_t a, uint8_t b) {
    art::dieFace(x, y, 5, a);
    art::dieFace(x + 6, y, 5, b);
}

static void spot(const Craps &g, const Zone &z, bool beginner) {
    int x = z.x, y = z.y, w = z.w, h = z.h, cx = x + w / 2;
    // Printed outline (neighbours share their edges).
    if (z.bet < CODDS4) gfx_rect(x, y, w, h, FELT_LT);
    bool closed = !g.onTable(z.bet);
    switch (z.bet) {
        case PLACE4: case PLACE5: case PLACE6: case PLACE8: case PLACE9: case PLACE10: {
            static const char *const NUM[6] = {"4", "5", "SIX", "8", "NINE", "10"};
            uint8_t i = (uint8_t)(z.bet - PLACE4);
            const char *s = NUM[i];
            // The 6 and 9 are spelled out on real tables (so nobody reads them
            // upside down); in a narrow box the words don't fit, so digits.
            if (!beginner) s = i == 2 ? "6" : (i == 4 ? "9" : s);
            centred57(cx, y + 2, s, closed ? FELT_DK : GOLD);
            if (closed) gfx_dither(x + 1, y + 1, w - 2, h - 2, FELT_DK, 0);
            break;
        }
        case COME:
            centred57(x + 36, y + 1, "COME", WHITE);
            break;
        case FIELD: {
            // 2 3 4 9 10 11 12 across the top, the 2 and 12 circled (they pay more).
            int ny = beginner ? y + 2 : y + 2, nx = beginner ? x + 40 : x + 4;
            static const char *const F[7] = {"2", "3", "4", "9", "10", "11", "12"};
            int gap = beginner ? 5 : 4;
            for (uint8_t k = 0; k < 7; k++) {
                int tw = text35Width(F[k]);
                bool pays = k == 0 || k == 6;
                if (pays) roundRect(nx - 2, ny - 2, tw + 4, 9, 3, GOLD);
                text35(nx, ny, F[k], pays ? GOLD : WHITE);
                nx += tw + gap + (k == 0 || k == 5 ? 2 : 0);
            }
            if (beginner) {
                gfx_text(x + 4, y + 4, "FIELD", GOLD);
                text35(x + 40, y + 8, "2 PAYS 2X 12 3X", FELT_LT);
            } else {
                text35(x + 4, y + 8, "FIELD", GOLD);
            }
            break;
        }
        case DONT:
            text35(x + 3, y + (beginner ? 3 : 2), "DONT PASS BAR", WHITE);
            if (beginner) pair(x + 57, y + 3, 6, 6);
            break;
        case DONT_ODDS: text35(x + 2, y + 2, "LAY", FELT_LT); break;
        case PASS: gfx_text(x + 3, y + (beginner ? 3 : 2), "PASS LINE", WHITE); break;
        case PASS_ODDS: text35(x + 2, y + 2, "ODDS", FELT_LT); break;
        case ANY7:
            fillRound(x + 2, y + 2, w - 4, h - 4, 2, RED);
            text35(x + 5, y + 3, "ANY 7", WHITE);
            break;
        case HARD4: case HARD6: case HARD8: case HARD10: {
            uint8_t v = (uint8_t)(2 + (z.bet - HARD4));
            pair(x + 3, y + 2, v, v);
            break;
        }
        case YO:
            pair(x + 3, y + 3, 5, 6);
            text35(x + 17, y + 3, "YO", GOLD);
            break;
        case ANYCRAPS:
            text35(x + 2, y + 2, "ANY CRAPS", GOLD);
            break;
        default: break;
    }
}

void draw(const Craps &g) {
    bool beginner = g.opt.table == TABLE_BEGINNER;
    gfx_fillRect(0, FELT_Y, 128, TRIM_Y - FELT_Y, FELT);
    uint8_t n = zones::count(g.opt.table);
    for (uint8_t i = 0; i < n; i++) {
        const Zone &z = zones::at(g.opt.table, i);
        if (z.bet < Z_CHIP0) spot(g, z, beginner);
    }
    if (!beginner) gfx_vline(90, FELT_Y, TRIM_Y - FELT_Y, GOLD);   // the centre's frame
    gfx_hline(0, TRIM_Y, 128, GOLD);
}

void hover(uint8_t table, uint8_t i, uint8_t colour) {
    const Zone &z = zones::at(table, i);
    if (z.bet >= Z_CHIP0) return;                        // the bar draws its own
    roundRect(z.x + 1, z.y + 1, z.w - 2, z.h - 2, 2, colour);
}

}  // namespace felt
