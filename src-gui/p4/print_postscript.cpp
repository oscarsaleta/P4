/*  This file is part of P4
 *
 *  Copyright (C) 1996-2018  J.C. Artés, P. De Maesschalck, F. Dumortier
 *                           C. Herssens, J. Llibre, O. Saleta, J. Torregrosa
 *
 *  P4 is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "print_postscript.h"

#include "custom.h"
#include "file_tab.h"
#include "file_vf.h"
#include "main.h"
#include "math_p4.h"
#include "plot_tools.h"
#include "print_bitmap.h"
#include "print_points.h"

#include <QFile>
#include <QTextStream>
#include <cmath>

static bool s_PSBlackWhitePrint = true;

static std::unique_ptr<QFile> s_PSFile();
static QTextStream s_PSFileStream;
static int s_PSW = 0;
static int s_PSH = 0;

static double s_lastpsx0 = 0;
static double s_lastpsy0 = 0;
static int s_lastpscolor = 0;

static void ps_print_line(double x0, double y0, double x1, double y1,
                          int color);
// ---------------------------------------------------------------------------------------

static void ps_print_comment(QString s)
{
    if (s_PSFile) {
        s_PSFileStream << "%% " << s << "\n";
    }
}

static void ps_print_saddle(double x, double y)
{
    if (s_PSFile) {
        QString s;
        if (s_PSBlackWhitePrint) {
            s_lastpscolor = bgColours::CFOREGROUND;
            s.sprintf("col%d %f %f box\n",
                      printColorTable(bgColours::CFOREGROUND), (float)x,
                      (float)y);
        } else {
            s_lastpscolor = CSADDLE;
            s.sprintf("col%d %f %f box\n", printColorTable(CSADDLE), (float)x,
                      (float)y);
        }
        s_PSFileStream << s;
    }
}

static void ps_print_virtualsaddle(double x, double y)
{
    if (!g_VFResults.plotVirtualSingularities_)
        return;
    if (s_PSFile) {
        QString s;
        if (s_PSBlackWhitePrint) {
            s_lastpscolor = bgColours::CFOREGROUND;
            s.sprintf("col%d %f %f vbox\n",
                      printColorTable(bgColours::CFOREGROUND), (float)x,
                      (float)y);
        } else {
            s_lastpscolor = CSADDLE;
            s.sprintf("col%d %f %f vbox\n", printColorTable(CSADDLE), (float)x,
                      (float)y);
        }
        s_PSFileStream << s;
    }
}

static void ps_print_stablenode(double x, double y)
{
    if (s_PSFile) {
        QString s;
        if (s_PSBlackWhitePrint) {
            s_lastpscolor = bgColours::CFOREGROUND;
            s.sprintf("col%d %f %f box\n",
                      printColorTable(bgColours::CFOREGROUND), (float)x,
                      (float)y);
        } else {
            s_lastpscolor = CNODE_S;
            s.sprintf("col%d %f %f box\n", printColorTable(CNODE_S), (float)x,
                      (float)y);
        }
        s_PSFileStream << s;
    }
}

static void ps_print_virtualstablenode(double x, double y)
{
    if (!g_VFResults.plotVirtualSingularities_)
        return;
    if (s_PSFile) {
        QString s;
        if (s_PSBlackWhitePrint) {
            s_lastpscolor = bgColours::CFOREGROUND;
            s.sprintf("col%d %f %f vbox\n",
                      printColorTable(bgColours::CFOREGROUND), (float)x,
                      (float)y);
        } else {
            s_lastpscolor = CNODE_S;
            s.sprintf("col%d %f %f vbox\n", printColorTable(CNODE_S), (float)x,
                      (float)y);
        }
        s_PSFileStream << s;
    }
}

static void ps_print_unstablenode(double x, double y)
{
    if (s_PSFile) {
        QString s;
        if (s_PSBlackWhitePrint) {
            s_lastpscolor = bgColours::CFOREGROUND;
            s.sprintf("col%d %f %f box\n",
                      printColorTable(bgColours::CFOREGROUND), (float)x,
                      (float)y);
        } else {
            s_lastpscolor = CNODE_U;
            s.sprintf("col%d %f %f box\n", printColorTable(CNODE_U), (float)x,
                      (float)y);
        }
        s_PSFileStream << s;
    }
}

static void ps_print_virtualunstablenode(double x, double y)
{
    if (!g_VFResults.plotVirtualSingularities_)
        return;
    if (s_PSFile) {
        QString s;
        if (s_PSBlackWhitePrint) {
            s_lastpscolor = bgColours::CFOREGROUND;
            s.sprintf("col%d %f %f vbox\n",
                      printColorTable(bgColours::CFOREGROUND), (float)x,
                      (float)y);
        } else {
            s_lastpscolor = CNODE_U;
            s.sprintf("col%d %f %f vbox\n", printColorTable(CNODE_U), (float)x,
                      (float)y);
        }
        s_PSFileStream << s;
    }
}

static void ps_print_stableweakfocus(double x, double y)
{
    if (s_PSFile) {
        QString s;
        if (s_PSBlackWhitePrint) {
            s_lastpscolor = bgColours::CFOREGROUND;
            s.sprintf("col%d %f %f diamond\n",
                      printColorTable(bgColours::CFOREGROUND), (float)x,
                      (float)y);
        } else {
            s_lastpscolor = CWEAK_FOCUS_S;
            s.sprintf("col%d %f %f diamond\n", printColorTable(CWEAK_FOCUS_S),
                      (float)x, (float)y);
        }
        s_PSFileStream << s;
    }
}

static void ps_print_virtualstableweakfocus(double x, double y)
{
    if (!g_VFResults.plotVirtualSingularities_)
        return;
    if (s_PSFile) {
        QString s;
        if (s_PSBlackWhitePrint) {
            s_lastpscolor = bgColours::CFOREGROUND;
            s.sprintf("col%d %f %f vdiamond\n",
                      printColorTable(bgColours::CFOREGROUND), (float)x,
                      (float)y);
        } else {
            s_lastpscolor = CWEAK_FOCUS_S;
            s.sprintf("col%d %f %f vdiamond\n", printColorTable(CWEAK_FOCUS_S),
                      (float)x, (float)y);
        }
        s_PSFileStream << s;
    }
}

static void ps_print_unstableweakfocus(double x, double y)
{
    if (s_PSFile) {
        QString s;
        if (s_PSBlackWhitePrint) {
            s_lastpscolor = bgColours::CFOREGROUND;
            s.sprintf("col%d %f %f diamond\n",
                      printColorTable(bgColours::CFOREGROUND), (float)x,
                      (float)y);
        } else {
            s_lastpscolor = CWEAK_FOCUS_U;
            s.sprintf("col%d %f %f diamond\n", printColorTable(CWEAK_FOCUS_U),
                      (float)x, (float)y);
        }
        s_PSFileStream << s;
    }
}

static void ps_print_virtualunstableweakfocus(double x, double y)
{
    if (!g_VFResults.plotVirtualSingularities_)
        return;
    if (s_PSFile) {
        QString s;
        if (s_PSBlackWhitePrint) {
            s_lastpscolor = bgColours::CFOREGROUND;
            s.sprintf("col%d %f %f vdiamond\n",
                      printColorTable(bgColours::CFOREGROUND), (float)x,
                      (float)y);
        } else {
            s_lastpscolor = CWEAK_FOCUS_U;
            s.sprintf("col%d %f %f vdiamond\n", printColorTable(CWEAK_FOCUS_U),
                      (float)x, (float)y);
        }
        s_PSFileStream << s;
    }
}

static void ps_print_weakfocus(double x, double y)
{
    if (s_PSFile) {
        QString s;
        if (s_PSBlackWhitePrint) {
            s_lastpscolor = bgColours::CFOREGROUND;
            s.sprintf("col%d %f %f diamond\n",
                      printColorTable(bgColours::CFOREGROUND), (float)x,
                      (float)y);
        } else {
            s_lastpscolor = CWEAK_FOCUS;
            s.sprintf("col%d %f %f diamond\n", printColorTable(CWEAK_FOCUS),
                      (float)x, (float)y);
        }
        s_PSFileStream << s;
    }
}

static void ps_print_virtualweakfocus(double x, double y)
{
    if (!g_VFResults.plotVirtualSingularities_)
        return;
    if (s_PSFile) {
        QString s;
        if (s_PSBlackWhitePrint) {
            s_lastpscolor = bgColours::CFOREGROUND;
            s.sprintf("col%d %f %f vdiamond\n",
                      printColorTable(bgColours::CFOREGROUND), (float)x,
                      (float)y);
        } else {
            s_lastpscolor = CWEAK_FOCUS;
            s.sprintf("col%d %f %f vdiamond\n", printColorTable(CWEAK_FOCUS),
                      (float)x, (float)y);
        }
        s_PSFileStream << s;
    }
}

static void ps_print_center(double x, double y)
{
    if (s_PSFile) {
        QString s;
        if (s_PSBlackWhitePrint) {
            s_lastpscolor = bgColours::CFOREGROUND;
            s.sprintf("col%d %f %f diamond\n",
                      printColorTable(bgColours::CFOREGROUND), (float)x,
                      (float)y);
        } else {
            s_lastpscolor = CCENTER;
            s.sprintf("col%d %f %f diamond\n", printColorTable(CCENTER),
                      (float)x, (float)y);
        }
        s_PSFileStream << s;
    }
}

static void ps_print_virtualcenter(double x, double y)
{
    if (!g_VFResults.plotVirtualSingularities_)
        return;
    if (s_PSFile) {
        QString s;
        if (s_PSBlackWhitePrint) {
            s_lastpscolor = bgColours::CFOREGROUND;
            s.sprintf("col%d %f %f vdiamond\n",
                      printColorTable(bgColours::CFOREGROUND), (float)x,
                      (float)y);
        } else {
            s_lastpscolor = CCENTER;
            s.sprintf("col%d %f %f vdiamond\n", printColorTable(CCENTER),
                      (float)x, (float)y);
        }
        s_PSFileStream << s;
    }
}

static void ps_print_stablestrongfocus(double x, double y)
{
    if (s_PSFile) {
        QString s;
        if (s_PSBlackWhitePrint) {
            s_lastpscolor = bgColours::CFOREGROUND;
            s.sprintf("col%d %f %f diamond\n",
                      printColorTable(bgColours::CFOREGROUND), (float)x,
                      (float)y);
        } else {
            s_lastpscolor = CSTRONG_FOCUS_S;
            s.sprintf("col%d %f %f diamond\n", printColorTable(CSTRONG_FOCUS_S),
                      (float)x, (float)y);
        }
        s_PSFileStream << s;
    }
}

static void ps_print_virtualstablestrongfocus(double x, double y)
{
    if (!g_VFResults.plotVirtualSingularities_)
        return;
    if (s_PSFile) {
        QString s;
        if (s_PSBlackWhitePrint) {
            s_lastpscolor = bgColours::CFOREGROUND;
            s.sprintf("col%d %f %f vdiamond\n",
                      printColorTable(bgColours::CFOREGROUND), (float)x,
                      (float)y);
        } else {
            s_lastpscolor = CSTRONG_FOCUS_S;
            s.sprintf("col%d %f %f vdiamond\n",
                      printColorTable(CSTRONG_FOCUS_S), (float)x, (float)y);
        }
        s_PSFileStream << s;
    }
}

static void ps_print_unstablestrongfocus(double x, double y)
{
    if (s_PSFile) {
        QString s;
        if (s_PSBlackWhitePrint) {
            s_lastpscolor = bgColours::CFOREGROUND;
            s.sprintf("col%d %f %f diamond\n",
                      printColorTable(bgColours::CFOREGROUND), (float)x,
                      (float)y);
        } else {
            s_lastpscolor = CSTRONG_FOCUS_U;
            s.sprintf("col%d %f %f diamond\n", printColorTable(CSTRONG_FOCUS_U),
                      (float)x, (float)y);
        }
        s_PSFileStream << s;
    }
}

static void ps_print_virtualunstablestrongfocus(double x, double y)
{
    if (!g_VFResults.plotVirtualSingularities_)
        return;
    if (s_PSFile) {
        QString s;
        if (s_PSBlackWhitePrint) {
            s_lastpscolor = bgColours::CFOREGROUND;
            s.sprintf("col%d %f %f vdiamond\n",
                      printColorTable(bgColours::CFOREGROUND), (float)x,
                      (float)y);
        } else {
            s_lastpscolor = CSTRONG_FOCUS_U;
            s.sprintf("col%d %f %f vdiamond\n",
                      printColorTable(CSTRONG_FOCUS_U), (float)x, (float)y);
        }
        s_PSFileStream << s;
    }
}

static void ps_print_sesaddle(double x, double y)
{
    if (s_PSFile) {
        QString s;
        if (s_PSBlackWhitePrint) {
            s_lastpscolor = bgColours::CFOREGROUND;
            s.sprintf("col%d %f %f triangle\n",
                      printColorTable(bgColours::CFOREGROUND), (float)x,
                      (float)y);
        } else {
            s_lastpscolor = CSADDLE;
            s.sprintf("col%d %f %f triangle\n", printColorTable(CSADDLE),
                      (float)x, (float)y);
        }
        s_PSFileStream << s;
    }
}

static void ps_print_virtualsesaddle(double x, double y)
{
    if (!g_VFResults.plotVirtualSingularities_)
        return;
    if (s_PSFile) {
        QString s;
        if (s_PSBlackWhitePrint) {
            s_lastpscolor = bgColours::CFOREGROUND;
            s.sprintf("col%d %f %f vtriangle\n",
                      printColorTable(bgColours::CFOREGROUND), (float)x,
                      (float)y);
        } else {
            s_lastpscolor = CSADDLE;
            s.sprintf("col%d %f %f vtriangle\n", printColorTable(CSADDLE),
                      (float)x, (float)y);
        }
        s_PSFileStream << s;
    }
}

static void ps_print_sesaddlenode(double x, double y)
{
    if (s_PSFile) {
        QString s;
        if (s_PSBlackWhitePrint) {
            s_lastpscolor = bgColours::CFOREGROUND;
            s.sprintf("col%d %f %f triangle\n",
                      printColorTable(bgColours::CFOREGROUND), (float)x,
                      (float)y);
        } else {
            s_lastpscolor = CSADDLE_NODE;
            s.sprintf("col%d %f %f triangle\n", printColorTable(CSADDLE_NODE),
                      (float)x, (float)y);
        }
        s_PSFileStream << s;
    }
}

static void ps_print_virtualsesaddlenode(double x, double y)
{
    if (!g_VFResults.plotVirtualSingularities_)
        return;
    if (s_PSFile) {
        QString s;
        if (s_PSBlackWhitePrint) {
            s_lastpscolor = bgColours::CFOREGROUND;
            s.sprintf("col%d %f %f vtriangle\n",
                      printColorTable(bgColours::CFOREGROUND), (float)x,
                      (float)y);
        } else {
            s_lastpscolor = CSADDLE_NODE;
            s.sprintf("col%d %f %f vtriangle\n", printColorTable(CSADDLE_NODE),
                      (float)x, (float)y);
        }
        s_PSFileStream << s;
    }
}

static void ps_print_sestablenode(double x, double y)
{
    if (s_PSFile) {
        QString s;
        if (s_PSBlackWhitePrint) {
            s_lastpscolor = bgColours::CFOREGROUND;
            s.sprintf("col%d %f %f triangle\n",
                      printColorTable(bgColours::CFOREGROUND), (float)x,
                      (float)y);
        } else {
            s_lastpscolor = CNODE_S;
            s.sprintf("col%d %f %f triangle\n", printColorTable(CNODE_S),
                      (float)x, (float)y);
        }
        s_PSFileStream << s;
    }
}

static void ps_print_virtualsestablenode(double x, double y)
{
    if (!g_VFResults.plotVirtualSingularities_)
        return;
    if (s_PSFile) {
        QString s;
        if (s_PSBlackWhitePrint) {
            s_lastpscolor = bgColours::CFOREGROUND;
            s.sprintf("col%d %f %f vtriangle\n",
                      printColorTable(bgColours::CFOREGROUND), (float)x,
                      (float)y);
        } else {
            s_lastpscolor = CNODE_S;
            s.sprintf("col%d %f %f vtriangle\n", printColorTable(CNODE_S),
                      (float)x, (float)y);
        }
        s_PSFileStream << s;
    }
}

static void ps_print_seunstablenode(double x, double y)
{
    if (s_PSFile) {
        QString s;
        if (s_PSBlackWhitePrint) {
            s_lastpscolor = bgColours::CFOREGROUND;
            s.sprintf("col%d %f %f triangle\n",
                      printColorTable(bgColours::CFOREGROUND), (float)x,
                      (float)y);
        } else {
            s_lastpscolor = CNODE_U;
            s.sprintf("col%d %f %f triangle\n", printColorTable(CNODE_U),
                      (float)x, (float)y);
        }
        s_PSFileStream << s;
    }
}

static void ps_print_virtualseunstablenode(double x, double y)
{
    if (!g_VFResults.plotVirtualSingularities_)
        return;
    if (s_PSFile) {
        QString s;
        if (s_PSBlackWhitePrint) {
            s_lastpscolor = bgColours::CFOREGROUND;
            s.sprintf("col%d %f %f vtriangle\n",
                      printColorTable(bgColours::CFOREGROUND), (float)x,
                      (float)y);
        } else {
            s_lastpscolor = CNODE_U;
            s.sprintf("col%d %f %f vtriangle\n", printColorTable(CNODE_U),
                      (float)x, (float)y);
        }
        s_PSFileStream << s;
    }
}

static void ps_print_degen(double x, double y)
{
    if (s_PSFile) {
        QString s;
        if (s_PSBlackWhitePrint) {
            s_lastpscolor = bgColours::CFOREGROUND;
            s.sprintf(
                "LW 2.6 mul setlinewidth\n"
                "col%d %f %f cross\n"
                "LW setlinewidth\n",
                printColorTable(bgColours::CFOREGROUND), (float)x, (float)y);
        } else {
            s_lastpscolor = CDEGEN;
            s.sprintf(
                "LW 2.6 mul setlinewidth\n"
                "col%d %f %f cross\n"
                "LW setlinewidth\n",
                printColorTable(CDEGEN), (float)x, (float)y);
            s_PSFileStream << s;
        }
    }
}

static void ps_print_virtualdegen(double x, double y)
{
    if (!g_VFResults.plotVirtualSingularities_)
        return;
    if (s_PSFile) {
        QString s;
        if (s_PSBlackWhitePrint) {
            s_lastpscolor = bgColours::CFOREGROUND;
            s.sprintf(
                "LW 1.3 mul setlinewidth\n"
                "col%d %f %f cross\n"
                "LW setlinewidth\n",
                printColorTable(bgColours::CFOREGROUND), (float)x, (float)y);
        } else {
            s_lastpscolor = CDEGEN;
            s.sprintf(
                "LW 1.3 mul setlinewidth\n"
                "col%d %f %f cross\n"
                "LW setlinewidth\n",
                printColorTable(CDEGEN), (float)x, (float)y);
            s_PSFileStream << s;
        }
    }
}

static void ps_print_elips(double x0, double y0, double a, double b, int color,
                           bool dotted, std::vector<P4POLYLINES> ellipse)
{
    double t, h;
    QString s;

    // we do not use the precompiled form of the ellipse printing.  Here, it
    // is not necessary, since the PS command for drawing ellipses works just
    // fine.

    if (s_PSFile) {
        if (dotted)
            s_PSFileStream << "gsave\n";

        if (s_PSBlackWhitePrint)
            s.sprintf("col%d\n", printColorTable(bgColours::CFOREGROUND));
        else
            s.sprintf("col%d\n", printColorTable(color));

        s_PSFileStream << s;
        s_PSFileStream << "newpath\n";

        if (x0 - a >= 0 && x0 + a < s_PSW && y0 - b >= 0 && y0 + b < s_PSH) {
            // full elipse visible
            s.sprintf("%f %f moveto\n", (float)(x0 + a), (float)y0);
            s_PSFileStream << s;
            h = PI / 100;
            for (t = h; t < TWOPI; t += h) {
                s.sprintf("%f %f lineto \n", (float)(x0 + a * cos(t)),
                          (float)(y0 + b * sin(t)));
                s_PSFileStream << s;
            }

            s_PSFileStream << "closepath\n";
            s_PSFileStream << "LW setlinewidth\n";
            if (dotted) {
                s_PSFileStream << "[DS ) 0 setdash\n";
            }
            s_PSFileStream << "stroke\n";
            if (dotted) {
                s_PSFileStream << "grestore\n";
            }
        } else {
            for (auto it : ellipse) {
                ps_print_line(it.x1, it.y1, it.x2, it.y2, color);
            }
        }
    }
}

static void ps_print_line(double x0, double y0, double x1, double y1, int color)
{
    if (x0 == x1 && y0 == y1)
        return;

    if (s_PSFile) {
        QString s;
        if (s_PSBlackWhitePrint) {
            if (s_lastpscolor == bgColours::CFOREGROUND) {
                s.sprintf("%d %d moveto %d %d lineto stroke\n", (int)x0,
                          (int)y0, (int)x1, (int)y1);
            } else {
                s_lastpscolor = bgColours::CFOREGROUND;
                s.sprintf("%f %f moveto\n%f %f lineto col%d stroke\n",
                          (float)x0, (float)y0, (float)x1, (float)y1,
                          printColorTable(bgColours::CFOREGROUND));
            }
        } else {
            if (s_lastpscolor == color) {
                s.sprintf("%d %d moveto %d %d lineto stroke\n", (int)x0,
                          (int)y0, (int)x1, (int)y1);
            } else {
                s_lastpscolor = color;
                s.sprintf("%f %f moveto\n%f %f lineto col%d stroke\n",
                          (float)x0, (float)y0, (float)x1, (float)y1,
                          printColorTable(color));
            }
        }
        s_PSFileStream << s;
    }
}

static void ps_print_point(double x0, double y0, int color)
{
    if (s_lastpsx0 == x0 && s_lastpsy0 == y0 && s_lastpscolor == color)
        return;  // just a small protection against big PS files: do not
                 // print series of the same points.

    s_lastpsx0 = x0;
    s_lastpsy0 = y0;
    s_lastpscolor = color;

    if (s_PSFile) {
        QString s;
        if (s_PSBlackWhitePrint) {
            if (s_lastpscolor == bgColours::CFOREGROUND) {
                s.sprintf("%d %d dot\n", (int)x0, (int)y0);
            } else {
                s_lastpscolor = bgColours::CFOREGROUND;
                s.sprintf("col%d %f %f dot\n",
                          printColorTable(bgColours::CFOREGROUND), (float)x0,
                          (float)y0);
            }
        } else {
            if (s_lastpscolor == color) {
                s.sprintf("%d %d dot\n", (int)x0, (int)y0);
            } else {
                s_lastpscolor = color;
                s.sprintf("col%d %f %f dot\n", printColorTable(color),
                          (float)x0, (float)y0);
            }
        }
        s_PSFileStream << s;
    }
}

// ---------------------------------------------------------------------------------------

void preparePostscriptPrinting(int x0, int y0, int w, int h, bool iszoom,
                               bool isblackwhite, int resolution, int linewidth,
                               int symbolwidth)
{
    s_PSW = w;
    s_PSH = h;
    QString s;
    double bbx0, bby0, bbw, bbh, bbx1, bby1;
    double scalefactor;
    scalefactor = 72;
    scalefactor /= resolution;

    bbx0 = x0;
    bbx0 /= resolution;
    bbx0 *= 72;
    bbx0 += 0.5;
    bby0 = y0;
    bby0 /= resolution;
    bby0 *= 72;
    bby0 += 0.5;
    bbw = w;
    bbw /= resolution;
    bbw *= 72;
    bbw += 0.5;
    bbh = h;
    bbh /= resolution;
    bbh *= 72;
    bbh += 0.5;
    bbx1 = bbx0 + bbw;
    bby1 = bby0 + bbh;

    s_PSBlackWhitePrint = isblackwhite;

    s = g_ThisVF->getbarefilename() + ".eps";

    s_PSFile.reset(new QFile(s));
    if (s_PSFile->open(QIODevice::WriteOnly))
        s_PSFileStream.setDevice(s_PSFile.get());
    else {
        s_PSFile.reset();
    }

    s_lastpscolor = -1;

    plot_l = spherePrintLine;
    plot_p = spherePrintPoint;

    print_saddle = ps_print_saddle;
    print_stablenode = ps_print_stablenode;
    print_unstablenode = ps_print_unstablenode;
    print_stableweakfocus = ps_print_stableweakfocus;
    print_unstableweakfocus = ps_print_unstableweakfocus;
    print_weakfocus = ps_print_weakfocus;
    print_stablestrongfocus = ps_print_stablestrongfocus;
    print_unstablestrongfocus = ps_print_unstablestrongfocus;
    print_sesaddle = ps_print_sesaddle;
    print_sesaddlenode = ps_print_sesaddlenode;
    print_sestablenode = ps_print_sestablenode;
    print_seunstablenode = ps_print_seunstablenode;
    print_degen = ps_print_degen;
    print_center = ps_print_center;
    print_elips = ps_print_elips;
    print_point = ps_print_point;
    print_line = ps_print_line;
    print_comment = ps_print_comment;

    QString title;
    QString datestring;

    title = "Phase portrait of \"" + g_ThisVF->getbarefilename() + "\"";
    if (iszoom)
        title += " (zoom window)";

    if (s_PSFile) {
        // to calculate bounding box, we need to convert to point
        // measurements
        // (1 point = 1/72 inch)

        s.sprintf(
            "%%!PS-Adobe-3.0 EPSF-3.0\n"
            "%%%%BoundingBox: %d %d %d %d\n",
            (int)bbx0, (int)bby0, (int)bbx1, (int)bby1);

        s_PSFileStream << s;
        s_PSFileStream << "%%%%Title: " << title << "\n";
        s_PSFileStream << "%%%%Creator: P4\n";
        s_PSFileStream << "%%%%CreationDate: " << datestring << "\n";
        s_PSFileStream << "%%%%EndComments\n\n";

        s.sprintf(
            "%% User specified resolution = %d DPI\n"
            "%% User specified Linewidth and symbolwidth, in pixels:\n\n"
            "/LW %d def\n"
            "/SW %d def\n\n"
            "%% Derived dash pattern:\n"
            "/DS %d def\n\n",
            resolution, linewidth, symbolwidth, linewidth * 6);
        s_PSFileStream << s;

        s.sprintf(
            "%% Transformation from default 72 DPI to chosen resolution, "
            "and shift towards center of page:\n"
            "/scaletransformation{\n"
            "   %f %f scale\n"
            "   %d %d translate}def\n\n",
            (float)scalefactor, (float)scalefactor, x0, y0);

        s_PSFileStream << s;

        s.sprintf(
            "/frame{\n"
            "   0 setlinewidth\n"
            "   newpath 0 0 moveto 0 %d lineto %d %d lineto %d 0 lineto "
            "closepath\n"
            "   clip stroke}def\n\n",
            h, w, h, w);
        s_PSFileStream << s;

        s_PSFileStream << "%% Color table:\n"
                          "/col-1 {} def\n"
                          "/col0 {0.000 0.000 0.000 setrgbcolor} bind def\n"
                          "/col1 {0.000 0.000 1.000 setrgbcolor} bind def\n"
                          "/col2 {0.000 1.000 0.000 setrgbcolor} bind def\n"
                          "/col3 {0.000 1.000 1.000 setrgbcolor} bind def\n"
                          "/col4 {1.000 0.000 0.000 setrgbcolor} bind def\n"
                          "/col5 {1.000 0.000 1.000 setrgbcolor} bind def\n"
                          "/col6 {1.000 1.000 0.000 setrgbcolor} bind def\n"
                          "/col7 {1.000 1.000 1.000 setrgbcolor} bind def\n"
                          "/col8 {0.000 0.000 0.560 setrgbcolor} bind def\n"
                          "/col9 {0.000 0.000 0.690 setrgbcolor} bind def\n"
                          "/col10 {0.000 0.000 0.820 setrgbcolor} bind def\n"
                          "/col11 {0.530 0.810 1.000 setrgbcolor} bind def\n"
                          "/col12 {0.000 0.560 0.000 setrgbcolor} bind def\n"
                          "/col13 {0.000 0.690 0.000 setrgbcolor} bind def\n"
                          "/col14 {0.000 0.820 0.000 setrgbcolor} bind def\n"
                          "/col15 {0.000 0.560 0.560 setrgbcolor} bind def\n"
                          "/col16 {0.000 0.690 0.690 setrgbcolor} bind def\n"
                          "/col17 {0.000 0.820 0.820 setrgbcolor} bind def\n"
                          "/col18 {0.560 0.000 0.000 setrgbcolor} bind def\n"
                          "/col19 {0.690 0.000 0.000 setrgbcolor} bind def\n"
                          "/col20 {0.820 0.000 0.000 setrgbcolor} bind def\n"
                          "/col21 {0.560 0.000 0.560 setrgbcolor} bind def\n"
                          "/col22 {0.690 0.000 0.690 setrgbcolor} bind def\n"
                          "/col23 {0.820 0.000 0.820 setrgbcolor} bind def\n"
                          "/col24 {0.500 0.190 0.000 setrgbcolor} bind def\n"
                          "/col25 {0.630 0.250 0.000 setrgbcolor} bind def\n"
                          "/col26 {0.750 0.380 0.000 setrgbcolor} bind def\n"
                          "/col27 {1.000 0.500 0.500 setrgbcolor} bind def\n"
                          "/col28 {1.000 0.630 0.630 setrgbcolor} bind def\n"
                          "/col29 {1.000 0.750 0.750 setrgbcolor} bind def\n"
                          "/col30 {1.000 0.880 0.880 setrgbcolor} bind def\n"
                          "/col31 {1.000 0.840 0.000 setrgbcolor} bind def\n";

        s_PSFileStream << "/box{ moveto\n"
                          "SW neg 2 div SW 2 div rmoveto\n"
                          "SW  0 rlineto\n"
                          "0  SW neg rlineto\n"
                          "SW neg 0 rlineto\n"
                          "closepath\n"
                          "fill\n"
                          "}bind def\n\n";

        PSFileStream << "/vbox{ moveto\n"
                        "SW neg 2 div SW 2 div rmoveto\n"
                        "SW  0 rlineto\n"
                        "0  SW neg rlineto\n"
                        "SW neg 0 rlineto\n"
                        "closepath\n"
                        "stroke\n"
                        "}bind def\n\n";

        s_PSFileStream << "/dot{\n"
                          "LW 2 div 0 360 arc fill\n"
                          "}bind def\n\n";

        s_PSFileStream << "/diamond{\n"
                          "moveto\n"
                          "0  SW 2 div 1.3 mul rmoveto\n"
                          "SW 2 div 1.3 mul SW neg 2 div 1.3 mul rlineto\n"
                          "SW neg 2 div 1.3 mul SW neg 2 div 1.3 mul rlineto\n"
                          "SW neg 2 div 1.3 mul SW 2 div 1.3 mul rlineto\n"
                          "closepath\n"
                          "fill\n"
                          "}bind def\n\n";

        PSFileStream << "/vdiamond{\n"
                        "moveto\n"
                        "0  SW 2 div 1.3 mul rmoveto\n"
                        "SW 2 div 1.3 mul SW neg 2 div 1.3 mul rlineto\n"
                        "SW neg 2 div 1.3 mul SW neg 2 div 1.3 mul rlineto\n"
                        "SW neg 2 div 1.3 mul SW 2 div 1.3 mul rlineto\n"
                        "closepath\n"
                        "stroke\n"
                        "}bind def\n\n";

        s_PSFileStream << "/triangle{\n"
                          "moveto\n"
                          "0  SW 2 div 1.2 mul rmoveto\n"
                          "SW 2 div 1.2 mul SW neg 1.2 mul rlineto\n"
                          "SW 1.2 mul neg 0  rlineto\n"
                          "fill\n"
                          "}bind def\n\n";

        PSFileStream << "/vtriangle{\n"
                        "moveto\n"
                        "0  SW 2 div 1.2 mul rmoveto\n"
                        "SW 2 div 1.2 mul SW neg 1.2 mul rlineto\n"
                        "SW 1.2 mul neg 0  rlineto\n"
                        "stroke\n"
                        "}bind def\n\n";

        s_PSFileStream << "/cross{\n"
                          "moveto\n"
                          "SW 2 div SW 2 div rmoveto\n"
                          "SW neg SW neg rlineto\n"
                          "0  SW rmoveto\n"
                          "SW SW neg rlineto\n"
                          "stroke\n"
                          "}bind def\n\n";

        PSFileStream << "/doublecross{\n"
                        "moveto\n"
                        "SW 2 div SW 2 div rmoveto\n"
                        "SW neg SW neg rlineto\n"
                        "0  SW rmoveto\n"
                        "SW SW neg rlineto\n"
                        "SW -2 div SW -4 div rmoveto\n"
                        "0 SW 1.5 mul rlineto\n"
                        "SW -0.75 mul SW -0.75 mul rmoveto\n"
                        "SW 11.5 mul 0 rlineto\n"
                        "stroke\n"
                        "}bind def\n\n";

        PSFileStream << "/vcross{\n"
                        "moveto\n"
                        "SW 3 div SW 3 div rmoveto\n"
                        "SW 1.5 div neg SW 1.5 div neg rlineto\n"
                        "0  SW 1.5 div rmoveto\n"
                        "SW 1.5 div SW 1.5 div neg rlineto\n"
                        "stroke\n"
                        "}bind def\n\n";

        s.sprintf(
            "%% Main Picture (local coordinates "
            "(x0,y0,x1,y1)=(0,0,%d,%d):\n"
            "gsave\n"
            "scaletransformation\n"
            "[] 0 setdash\n",
            w, h);
        s_PSFileStream << s;

        if (!bgColours::PRINT_WHITE_BG) {
            s.sprintf(
                "%% Fill background with black rectangle:\n"
                "newpath\n"
                "0 0 moveto\n"
                "%d 0 lineto\n"
                "%d %d lineto\n"
                "0 %d lineto\n"
                "closepath\n"
                "col0\n"
                "fill\n",
                w, w, h, h);
            s_PSFileStream << s;
        }

        if (iszoom || g_VFResults.typeofview_ == TYPEOFVIEW_PLANE) {
            s_PSFileStream << "frame\n";
        }

        s_PSFileStream << "\n"
                          "%% Plot the orbits\n\n"
                          "LW setlinewidth\n";
    }
}

void finishPostscriptPrinting(void)
{
    if (s_PSFile) {
        s_PSFileStream << "grestore\n"
                          "showpage\n"
                          "%%EOF\n";

        s_PSFileStream.flush();
        s_PSFileStream.setDevice(nullptr);
        s_PSFile->close();
        s_PSFile.reset();
    }

    plot_l = spherePlotLine;
    plot_p = spherePlotPoint;
}
