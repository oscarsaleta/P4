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

#include "print_postscript.hpp"

#include <QFile>
#include <QTextStream>
#include <cmath>

#include "P4InputVF.hpp"
#include "P4ParentStudy.hpp"
#include "custom.hpp"
#include "main.hpp"
#include "math_p4.hpp"
#include "plot_tools.hpp"
#include "print_bitmap.hpp"
#include "print_points.hpp"
#include "tables.hpp"

static bool sPSBlackWhitePrint{true};

static QFile *sPSFile;
static QTextStream sPSFileStream;
static int sPSW{0};
static int sPSH{0};

static double sLastPSX0{0};
static double sLastPSY0{0};
static int sLastPSColor{0};

static void ps_print_line(double x0, double y0, double x1, double y1,
                          int color);
// ---------------------------------------------------------------------------------------

static void ps_print_comment(QString s)
{
    if (sPSFile != nullptr) {
        sPSFileStream << "%% " << s << "\n";
    }
}

static void ps_print_saddle(double x, double y)
{
    if (sPSFile != nullptr) {
        QString s;
        if (sPSBlackWhitePrint) {
            sLastPSColor = bgColours::CFOREGROUND;
            s.sprintf("col%d %8.5g %8.5g box\n",
                      printColorTable(bgColours::CFOREGROUND), x, y);
        } else {
            sLastPSColor = CSADDLE;
            s.sprintf("col%d %8.5g %8.5g box\n", printColorTable(CSADDLE), x,
                      y);
        }
        sPSFileStream << s;
    }
}

static void ps_print_virtualsaddle(double x, double y)
{
    if (!gVFResults.plotVirtualSingularities_)
        return;
    if (sPSFile != nullptr) {
        QString s;
        if (sPSBlackWhitePrint) {
            sLastPSColor = bgColours::CFOREGROUND;
            s.sprintf("col%d %8.5g %8.5g vbox\n",
                      printColorTable(bgColours::CFOREGROUND), x, y);
        } else {
            sLastPSColor = CSADDLE;
            s.sprintf("col%d %8.5g %8.5g vbox\n", printColorTable(CSADDLE), x,
                      y);
        }
        sPSFileStream << s;
    }
}

static void ps_print_stablenode(double x, double y)
{
    if (sPSFile != nullptr) {
        QString s;
        if (sPSBlackWhitePrint) {
            sLastPSColor = bgColours::CFOREGROUND;
            s.sprintf("col%d %8.5g %8.5g box\n",
                      printColorTable(bgColours::CFOREGROUND), x, y);
        } else {
            sLastPSColor = CNODE_S;
            s.sprintf("col%d %8.5g %8.5g box\n", printColorTable(CNODE_S), x,
                      y);
        }
        sPSFileStream << s;
    }
}

static void ps_print_virtualstablenode(double x, double y)
{
    if (!gVFResults.plotVirtualSingularities_)
        return;
    if (sPSFile != nullptr) {
        QString s;
        if (sPSBlackWhitePrint) {
            sLastPSColor = bgColours::CFOREGROUND;
            s.sprintf("col%d %8.5g %8.5g vbox\n",
                      printColorTable(bgColours::CFOREGROUND), x, y);
        } else {
            sLastPSColor = CNODE_S;
            s.sprintf("col%d %8.5g %8.5g vbox\n", printColorTable(CNODE_S), x,
                      y);
        }
        sPSFileStream << s;
    }
}

static void ps_print_unstablenode(double x, double y)
{
    if (sPSFile != nullptr) {
        QString s;
        if (sPSBlackWhitePrint) {
            sLastPSColor = bgColours::CFOREGROUND;
            s.sprintf("col%d %8.5g %8.5g box\n",
                      printColorTable(bgColours::CFOREGROUND), x, y);
        } else {
            sLastPSColor = CNODE_U;
            s.sprintf("col%d %8.5g %8.5g box\n", printColorTable(CNODE_U), x,
                      y);
        }
        sPSFileStream << s;
    }
}

static void ps_print_virtualunstablenode(double x, double y)
{
    if (!gVFResults.plotVirtualSingularities_)
        return;
    if (sPSFile != nullptr) {
        QString s;
        if (sPSBlackWhitePrint) {
            sLastPSColor = bgColours::CFOREGROUND;
            s.sprintf("col%d %8.5g %8.5g vbox\n",
                      printColorTable(bgColours::CFOREGROUND), x, y);
        } else {
            sLastPSColor = CNODE_U;
            s.sprintf("col%d %8.5g %8.5g vbox\n", printColorTable(CNODE_U), x,
                      y);
        }
        sPSFileStream << s;
    }
}

static void ps_print_stableweakfocus(double x, double y)
{
    if (sPSFile != nullptr) {
        QString s;
        if (sPSBlackWhitePrint) {
            sLastPSColor = bgColours::CFOREGROUND;
            s.sprintf("col%d %8.5g %8.5g diamond\n",
                      printColorTable(bgColours::CFOREGROUND), x, y);
        } else {
            sLastPSColor = CWEAK_FOCUS_S;
            s.sprintf("col%d %8.5g %8.5g diamond\n",
                      printColorTable(CWEAK_FOCUS_S), x, y);
        }
        sPSFileStream << s;
    }
}

static void ps_print_virtualstableweakfocus(double x, double y)
{
    if (!gVFResults.plotVirtualSingularities_)
        return;
    if (sPSFile != nullptr) {
        QString s;
        if (sPSBlackWhitePrint) {
            sLastPSColor = bgColours::CFOREGROUND;
            s.sprintf("col%d %8.5g %8.5g vdiamond\n",
                      printColorTable(bgColours::CFOREGROUND), x, y);
        } else {
            sLastPSColor = CWEAK_FOCUS_S;
            s.sprintf("col%d %8.5g %8.5g vdiamond\n",
                      printColorTable(CWEAK_FOCUS_S), x, y);
        }
        sPSFileStream << s;
    }
}

static void ps_print_unstableweakfocus(double x, double y)
{
    if (sPSFile != nullptr) {
        QString s;
        if (sPSBlackWhitePrint) {
            sLastPSColor = bgColours::CFOREGROUND;
            s.sprintf("col%d %8.5g %8.5g diamond\n",
                      printColorTable(bgColours::CFOREGROUND), x, y);
        } else {
            sLastPSColor = CWEAK_FOCUS_U;
            s.sprintf("col%d %8.5g %8.5g diamond\n",
                      printColorTable(CWEAK_FOCUS_U), x, y);
        }
        sPSFileStream << s;
    }
}

static void ps_print_virtualunstableweakfocus(double x, double y)
{
    if (!gVFResults.plotVirtualSingularities_)
        return;
    if (sPSFile != nullptr) {
        QString s;
        if (sPSBlackWhitePrint) {
            sLastPSColor = bgColours::CFOREGROUND;
            s.sprintf("col%d %8.5g %8.5g vdiamond\n",
                      printColorTable(bgColours::CFOREGROUND), x, y);
        } else {
            sLastPSColor = CWEAK_FOCUS_U;
            s.sprintf("col%d %8.5g %8.5g vdiamond\n",
                      printColorTable(CWEAK_FOCUS_U), x, y);
        }
        sPSFileStream << s;
    }
}

static void ps_print_weakfocus(double x, double y)
{
    if (sPSFile != nullptr) {
        QString s;
        if (sPSBlackWhitePrint) {
            sLastPSColor = bgColours::CFOREGROUND;
            s.sprintf("col%d %8.5g %8.5g diamond\n",
                      printColorTable(bgColours::CFOREGROUND), x, y);
        } else {
            sLastPSColor = CWEAK_FOCUS;
            s.sprintf("col%d %8.5g %8.5g diamond\n",
                      printColorTable(CWEAK_FOCUS), x, y);
        }
        sPSFileStream << s;
    }
}

static void ps_print_virtualweakfocus(double x, double y)
{
    if (!gVFResults.plotVirtualSingularities_)
        return;
    if (sPSFile != nullptr) {
        QString s;
        if (sPSBlackWhitePrint) {
            sLastPSColor = bgColours::CFOREGROUND;
            s.sprintf("col%d %8.5g %8.5g vdiamond\n",
                      printColorTable(bgColours::CFOREGROUND), x, y);
        } else {
            sLastPSColor = CWEAK_FOCUS;
            s.sprintf("col%d %8.5g %8.5g vdiamond\n",
                      printColorTable(CWEAK_FOCUS), x, y);
        }
        sPSFileStream << s;
    }
}

static void ps_print_center(double x, double y)
{
    if (sPSFile != nullptr) {
        QString s;
        if (sPSBlackWhitePrint) {
            sLastPSColor = bgColours::CFOREGROUND;
            s.sprintf("col%d %8.5g %8.5g diamond\n",
                      printColorTable(bgColours::CFOREGROUND), x, y);
        } else {
            sLastPSColor = CCENTER;
            s.sprintf("col%d %8.5g %8.5g diamond\n", printColorTable(CCENTER),
                      x, y);
        }
        sPSFileStream << s;
    }
}

static void ps_print_virtualcenter(double x, double y)
{
    if (!gVFResults.plotVirtualSingularities_)
        return;
    if (sPSFile != nullptr) {
        QString s;
        if (sPSBlackWhitePrint) {
            sLastPSColor = bgColours::CFOREGROUND;
            s.sprintf("col%d %8.5g %8.5g vdiamond\n",
                      printColorTable(bgColours::CFOREGROUND), x, y);
        } else {
            sLastPSColor = CCENTER;
            s.sprintf("col%d %8.5g %8.5g vdiamond\n", printColorTable(CCENTER),
                      x, y);
        }
        sPSFileStream << s;
    }
}

static void ps_print_stablestrongfocus(double x, double y)
{
    if (sPSFile != nullptr) {
        QString s;
        if (sPSBlackWhitePrint) {
            sLastPSColor = bgColours::CFOREGROUND;
            s.sprintf("col%d %8.5g %8.5g diamond\n",
                      printColorTable(bgColours::CFOREGROUND), x, y);
        } else {
            sLastPSColor = CSTRONG_FOCUS_S;
            s.sprintf("col%d %8.5g %8.5g diamond\n",
                      printColorTable(CSTRONG_FOCUS_S), x, y);
        }
        sPSFileStream << s;
    }
}

static void ps_print_virtualstablestrongfocus(double x, double y)
{
    if (!gVFResults.plotVirtualSingularities_)
        return;
    if (sPSFile != nullptr) {
        QString s;
        if (sPSBlackWhitePrint) {
            sLastPSColor = bgColours::CFOREGROUND;
            s.sprintf("col%d %8.5g %8.5g vdiamond\n",
                      printColorTable(bgColours::CFOREGROUND), x, y);
        } else {
            sLastPSColor = CSTRONG_FOCUS_S;
            s.sprintf("col%d %8.5g %8.5g vdiamond\n",
                      printColorTable(CSTRONG_FOCUS_S), x, y);
        }
        sPSFileStream << s;
    }
}

static void ps_print_unstablestrongfocus(double x, double y)
{
    if (sPSFile != nullptr) {
        QString s;
        if (sPSBlackWhitePrint) {
            sLastPSColor = bgColours::CFOREGROUND;
            s.sprintf("col%d %8.5g %8.5g diamond\n",
                      printColorTable(bgColours::CFOREGROUND), x, y);
        } else {
            sLastPSColor = CSTRONG_FOCUS_U;
            s.sprintf("col%d %8.5g %8.5g diamond\n",
                      printColorTable(CSTRONG_FOCUS_U), x, y);
        }
        sPSFileStream << s;
    }
}

static void ps_print_virtualunstablestrongfocus(double x, double y)
{
    if (!gVFResults.plotVirtualSingularities_)
        return;
    if (sPSFile != nullptr) {
        QString s;
        if (sPSBlackWhitePrint) {
            sLastPSColor = bgColours::CFOREGROUND;
            s.sprintf("col%d %8.5g %8.5g vdiamond\n",
                      printColorTable(bgColours::CFOREGROUND), x, y);
        } else {
            sLastPSColor = CSTRONG_FOCUS_U;
            s.sprintf("col%d %8.5g %8.5g vdiamond\n",
                      printColorTable(CSTRONG_FOCUS_U), x, y);
        }
        sPSFileStream << s;
    }
}

static void ps_print_sesaddle(double x, double y)
{
    if (sPSFile != nullptr) {
        QString s;
        if (sPSBlackWhitePrint) {
            sLastPSColor = bgColours::CFOREGROUND;
            s.sprintf("col%d %8.5g %8.5g triangle\n",
                      printColorTable(bgColours::CFOREGROUND), x, y);
        } else {
            sLastPSColor = CSADDLE;
            s.sprintf("col%d %8.5g %8.5g triangle\n", printColorTable(CSADDLE),
                      x, y);
        }
        sPSFileStream << s;
    }
}

static void ps_print_virtualsesaddle(double x, double y)
{
    if (!gVFResults.plotVirtualSingularities_)
        return;
    if (sPSFile != nullptr) {
        QString s;
        if (sPSBlackWhitePrint) {
            sLastPSColor = bgColours::CFOREGROUND;
            s.sprintf("col%d %8.5g %8.5g vtriangle\n",
                      printColorTable(bgColours::CFOREGROUND), x, y);
        } else {
            sLastPSColor = CSADDLE;
            s.sprintf("col%d %8.5g %8.5g vtriangle\n", printColorTable(CSADDLE),
                      x, y);
        }
        sPSFileStream << s;
    }
}

static void ps_print_sesaddlenode(double x, double y)
{
    if (sPSFile != nullptr) {
        QString s;
        if (sPSBlackWhitePrint) {
            sLastPSColor = bgColours::CFOREGROUND;
            s.sprintf("col%d %8.5g %8.5g triangle\n",
                      printColorTable(bgColours::CFOREGROUND), x, y);
        } else {
            sLastPSColor = CSADDLE_NODE;
            s.sprintf("col%d %8.5g %8.5g triangle\n",
                      printColorTable(CSADDLE_NODE), x, y);
        }
        sPSFileStream << s;
    }
}

static void ps_print_virtualsesaddlenode(double x, double y)
{
    if (!gVFResults.plotVirtualSingularities_)
        return;
    if (sPSFile != nullptr) {
        QString s;
        if (sPSBlackWhitePrint) {
            sLastPSColor = bgColours::CFOREGROUND;
            s.sprintf("col%d %8.5g %8.5g vtriangle\n",
                      printColorTable(bgColours::CFOREGROUND), x, y);
        } else {
            sLastPSColor = CSADDLE_NODE;
            s.sprintf("col%d %8.5g %8.5g vtriangle\n",
                      printColorTable(CSADDLE_NODE), x, y);
        }
        sPSFileStream << s;
    }
}

static void ps_print_sestablenode(double x, double y)
{
    if (sPSFile != nullptr) {
        QString s;
        if (sPSBlackWhitePrint) {
            sLastPSColor = bgColours::CFOREGROUND;
            s.sprintf("col%d %8.5g %8.5g triangle\n",
                      printColorTable(bgColours::CFOREGROUND), x, y);
        } else {
            sLastPSColor = CNODE_S;
            s.sprintf("col%d %8.5g %8.5g triangle\n", printColorTable(CNODE_S),
                      x, y);
        }
        sPSFileStream << s;
    }
}

static void ps_print_virtualsestablenode(double x, double y)
{
    if (!gVFResults.plotVirtualSingularities_)
        return;
    if (sPSFile != nullptr) {
        QString s;
        if (sPSBlackWhitePrint) {
            sLastPSColor = bgColours::CFOREGROUND;
            s.sprintf("col%d %8.5g %8.5g vtriangle\n",
                      printColorTable(bgColours::CFOREGROUND), x, y);
        } else {
            sLastPSColor = CNODE_S;
            s.sprintf("col%d %8.5g %8.5g vtriangle\n", printColorTable(CNODE_S),
                      x, y);
        }
        sPSFileStream << s;
    }
}

static void ps_print_seunstablenode(double x, double y)
{
    if (sPSFile != nullptr) {
        QString s;
        if (sPSBlackWhitePrint) {
            sLastPSColor = bgColours::CFOREGROUND;
            s.sprintf("col%d %8.5g %8.5g triangle\n",
                      printColorTable(bgColours::CFOREGROUND), x, y);
        } else {
            sLastPSColor = CNODE_U;
            s.sprintf("col%d %8.5g %8.5g triangle\n", printColorTable(CNODE_U),
                      x, y);
        }
        sPSFileStream << s;
    }
}

static void ps_print_virtualseunstablenode(double x, double y)
{
    if (!gVFResults.plotVirtualSingularities_)
        return;
    if (sPSFile != nullptr) {
        QString s;
        if (sPSBlackWhitePrint) {
            sLastPSColor = bgColours::CFOREGROUND;
            s.sprintf("col%d %8.5g %8.5g vtriangle\n",
                      printColorTable(bgColours::CFOREGROUND), x, y);
        } else {
            sLastPSColor = CNODE_U;
            s.sprintf("col%d %8.5g %8.5g vtriangle\n", printColorTable(CNODE_U),
                      x, y);
        }
        sPSFileStream << s;
    }
}

static void ps_print_degen(double x, double y)
{
    if (sPSFile != nullptr) {
        QString s;
        if (sPSBlackWhitePrint) {
            sLastPSColor = bgColours::CFOREGROUND;
            s.sprintf("LW 2.6 mul setlinewidth\n"
                      "col%d %8.5g %8.5g cross\n"
                      "LW setlinewidth\n",
                      printColorTable(bgColours::CFOREGROUND), x, y);
        } else {
            sLastPSColor = CDEGEN;
            s.sprintf("LW 2.6 mul setlinewidth\n"
                      "col%d %8.5g %8.5g cross\n"
                      "LW setlinewidth\n",
                      printColorTable(CDEGEN), x, y);
            sPSFileStream << s;
        }
    }
}

static void ps_print_virtualdegen(double x, double y)
{
    if (!gVFResults.plotVirtualSingularities_)
        return;
    if (sPSFile != nullptr) {
        QString s;
        if (sPSBlackWhitePrint) {
            sLastPSColor = bgColours::CFOREGROUND;
            s.sprintf("LW 1.3 mul setlinewidth\n"
                      "col%d %8.5g %8.5g cross\n"
                      "LW setlinewidth\n",
                      printColorTable(bgColours::CFOREGROUND), x, y);
        } else {
            sLastPSColor = CDEGEN;
            s.sprintf("LW 1.3 mul setlinewidth\n"
                      "col%d %8.5g %8.5g cross\n"
                      "LW setlinewidth\n",
                      printColorTable(CDEGEN), x, y);
            sPSFileStream << s;
        }
    }
}

static void ps_print_coinciding(double x, double y)
{
    if (sPSFile != nullptr) {
        QString s;
        if (sPSBlackWhitePrint) {
            sLastPSColor = bgColours::CFOREGROUND;
            s.sprintf("LW 1.3 mul setlinewidth\n"
                      "col%d %8.5g %8.5g doublecross\n"
                      "LW setlinewidth\n",
                      printColorTable(bgColours::CFOREGROUND), x, y);
        } else {
            sLastPSColor = CDEGEN;
            s.sprintf("LW 1.3 mul setlinewidth\n"
                      "col%d %8.5g %8.5g doublecross\n"
                      "LW setlinewidth\n",
                      printColorTable(CDEGEN), x, y);
        }
        sPSFileStream << s;
    }
}

static void ps_print_elips(double x0, double y0, double a, double b, int color,
                           bool dotted, const std::vector<P4POLYLINES> &ellipse)
{
    double t, h;
    QString s;

    // we do not use the precompiled form of the ellipse printing.  Here, it
    // is not necessary, since the PS command for drawing ellipses works just
    // fine.

    if (sPSFile != nullptr) {
        if (dotted)
            sPSFileStream << "gsave\n";

        if (sPSBlackWhitePrint)
            s.sprintf("col%d\n", printColorTable(bgColours::CFOREGROUND));
        else
            s.sprintf("col%d\n", printColorTable(color));

        sPSFileStream << s;
        sPSFileStream << "newpath\n";

        if (x0 - a >= 0 && x0 + a < sPSW && y0 - b >= 0 && y0 + b < sPSH) {
            // full elipse visible
            s.sprintf("%8.5g %8.5g moveto\n", (x0 + a), y0);
            sPSFileStream << s;
            h = PI / 100;
            for (t = h; t < TWOPI; t += h) {
                s.sprintf("%8.5g %8.5g lineto \n", (x0 + a * cos(t)),
                          (y0 + b * sin(t)));
                sPSFileStream << s;
            }

            sPSFileStream << "closepath\n";
            sPSFileStream << "LW setlinewidth\n";
            if (dotted) {
                sPSFileStream << "[DS ) 0 setdash\n";
            }
            sPSFileStream << "stroke\n";
            if (dotted) {
                sPSFileStream << "grestore\n";
            }
        } else {
            for (auto const &it : ellipse) {
                ps_print_line(it.x1, it.y1, it.x2, it.y2, color);
            }
        }
    }
}

static void ps_print_line(double x0, double y0, double x1, double y1, int color)
{
    if (x0 == x1 && y0 == y1)
        return;

    if (sPSFile != nullptr) {
        QString s;
        if (sPSBlackWhitePrint) {
            if (sLastPSColor == bgColours::CFOREGROUND) {
                s.sprintf("%d %d moveto %d %d lineto stroke\n",
                          static_cast<int>(std::trunc(x0)),
                          static_cast<int>(std::trunc(y0)),
                          static_cast<int>(std::trunc(x1)),
                          static_cast<int>(std::trunc(y1)));
            } else {
                sLastPSColor = bgColours::CFOREGROUND;
                s.sprintf(
                    "%8.5g %8.5g moveto\n%8.5g %8.5g lineto col%d stroke\n", x0,
                    y0, x1, y1, printColorTable(bgColours::CFOREGROUND));
            }
        } else {
            if (sLastPSColor == color) {
                s.sprintf("%d %d moveto %d %d lineto stroke\n",
                          static_cast<int>(std::trunc(x0)),
                          static_cast<int>(std::trunc(y0)),
                          static_cast<int>(std::trunc(x1)),
                          static_cast<int>(std::trunc(y1)));
            } else {
                sLastPSColor = color;
                s.sprintf(
                    "%8.5g %8.5g moveto\n%8.5g %8.5g lineto col%d stroke\n", x0,
                    y0, x1, y1, printColorTable(color));
            }
        }
        sPSFileStream << s;
    }
}

static void ps_print_point(double x0, double y0, int color)
{
    if (sLastPSX0 == x0 && sLastPSY0 == y0 && sLastPSColor == color)
        return; // just a small protection against big PS files: do not
                // print series of the same points.

    sLastPSX0 = x0;
    sLastPSY0 = y0;
    sLastPSColor = color;

    if (sPSFile != nullptr) {
        QString s;
        if (sPSBlackWhitePrint) {
            if (sLastPSColor == bgColours::CFOREGROUND) {
                s.sprintf("%d %d dot\n", static_cast<int>(std::trunc(x0)),
                          static_cast<int>(std::trunc(y0)));
            } else {
                sLastPSColor = bgColours::CFOREGROUND;
                s.sprintf("col%d %8.5g %8.5g dot\n",
                          printColorTable(bgColours::CFOREGROUND), x0, y0);
            }
        } else {
            if (sLastPSColor == color) {
                s.sprintf("%d %d dot\n", static_cast<int>(std::trunc(x0)),
                          static_cast<int>(std::trunc(y0)));
            } else {
                sLastPSColor = color;
                s.sprintf("col%d %8.5g %8.5g dot\n", printColorTable(color), x0,
                          y0);
            }
        }
        sPSFileStream << s;
    }
}

// ---------------------------------------------------------------------------------------

void preparePostscriptPrinting(int x0, int y0, int w, int h, bool iszoom,
                               bool isblackwhite, int resolution, int linewidth,
                               int symbolwidth)
{
    sPSW = w;
    sPSH = h;
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

    sPSBlackWhitePrint = isblackwhite;

    s = gThisVF->getbarefilename() + ".eps";

    if (sPSFile != nullptr)
        delete sPSFile;
    sPSFile = new QFile{s};
    if (sPSFile->open(QIODevice::WriteOnly))
        sPSFileStream.setDevice(sPSFile);
    else {
        delete sPSFile;
        sPSFile = nullptr;
    }

    sLastPSColor = -1;

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
    print_virtualsaddle = ps_print_virtualsaddle;
    print_virtualstablenode = ps_print_virtualstablenode;
    print_virtualunstablenode = ps_print_virtualunstablenode;
    print_virtualstableweakfocus = ps_print_virtualstableweakfocus;
    print_virtualunstableweakfocus = ps_print_virtualunstableweakfocus;
    print_virtualweakfocus = ps_print_virtualweakfocus;
    print_virtualstablestrongfocus = ps_print_virtualstablestrongfocus;
    print_virtualunstablestrongfocus = ps_print_virtualunstablestrongfocus;
    print_virtualsesaddle = ps_print_virtualsesaddle;
    print_virtualsesaddlenode = ps_print_virtualsesaddlenode;
    print_virtualsestablenode = ps_print_virtualsestablenode;
    print_virtualseunstablenode = ps_print_virtualseunstablenode;
    print_virtualdegen = ps_print_virtualdegen;
    print_virtualcenter = ps_print_virtualcenter;
    print_coinciding = ps_print_coinciding;

    QString title;
    QString datestring;

    title = "Phase portrait of \"" + gThisVF->getbarefilename() + "\"";
    if (iszoom)
        title += " (zoom window)";

    if (sPSFile != nullptr) {
        // to calculate bounding box, we need to convert to point
        // measurements
        // (1 point = 1/72 inch)

        s.sprintf("%%!PS-Adobe-3.0 EPSF-3.0\n"
                  "%%%%BoundingBox: %d %d %d %d\n",
                  static_cast<int>(std::trunc(bbx0)),
                  static_cast<int>(std::trunc(bby0)),
                  static_cast<int>(std::trunc(bbx1)),
                  static_cast<int>(std::trunc(bby1)));

        sPSFileStream << s;
        sPSFileStream << "%%%%Title: " << title << "\n";
        sPSFileStream << "%%%%Creator: P4\n";
        sPSFileStream << "%%%%CreationDate: " << datestring << "\n";
        sPSFileStream << "%%%%EndComments\n\n";

        s.sprintf("%% User specified resolution = %d DPI\n"
                  "%% User specified Linewidth and symbolwidth, in pixels:\n\n"
                  "/LW %d def\n"
                  "/SW %d def\n\n"
                  "%% Derived dash pattern:\n"
                  "/DS %d def\n\n",
                  resolution, linewidth, symbolwidth, linewidth * 6);
        sPSFileStream << s;

        s.sprintf("%% Transformation from default 72 DPI to chosen resolution, "
                  "and shift towards center of page:\n"
                  "/scaletransformation{\n"
                  "   %8.5g %8.5g scale\n"
                  "   %d %d translate}def\n\n",
                  scalefactor, scalefactor, x0, y0);

        sPSFileStream << s;

        s.sprintf("/frame{\n"
                  "   0 setlinewidth\n"
                  "   newpath 0 0 moveto 0 %d lineto %d %d lineto %d 0 lineto "
                  "closepath\n"
                  "   clip stroke}def\n\n",
                  h, w, h, w);
        sPSFileStream << s;

        sPSFileStream << "%% Color table:\n"
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

        sPSFileStream << "/box{ moveto\n"
                         "SW neg 2 div SW 2 div rmoveto\n"
                         "SW  0 rlineto\n"
                         "0  SW neg rlineto\n"
                         "SW neg 0 rlineto\n"
                         "closepath\n"
                         "fill\n"
                         "}bind def\n\n";

        sPSFileStream << "/vbox{ moveto\n"
                         "SW neg 2 div SW 2 div rmoveto\n"
                         "SW  0 rlineto\n"
                         "0  SW neg rlineto\n"
                         "SW neg 0 rlineto\n"
                         "closepath\n"
                         "stroke\n"
                         "}bind def\n\n";

        sPSFileStream << "/dot{\n"
                         "LW 2 div 0 360 arc fill\n"
                         "}bind def\n\n";

        sPSFileStream << "/diamond{\n"
                         "moveto\n"
                         "0  SW 2 div 1.3 mul rmoveto\n"
                         "SW 2 div 1.3 mul SW neg 2 div 1.3 mul rlineto\n"
                         "SW neg 2 div 1.3 mul SW neg 2 div 1.3 mul rlineto\n"
                         "SW neg 2 div 1.3 mul SW 2 div 1.3 mul rlineto\n"
                         "closepath\n"
                         "fill\n"
                         "}bind def\n\n";

        sPSFileStream << "/vdiamond{\n"
                         "moveto\n"
                         "0  SW 2 div 1.3 mul rmoveto\n"
                         "SW 2 div 1.3 mul SW neg 2 div 1.3 mul rlineto\n"
                         "SW neg 2 div 1.3 mul SW neg 2 div 1.3 mul rlineto\n"
                         "SW neg 2 div 1.3 mul SW 2 div 1.3 mul rlineto\n"
                         "closepath\n"
                         "stroke\n"
                         "}bind def\n\n";

        sPSFileStream << "/triangle{\n"
                         "moveto\n"
                         "0  SW 2 div 1.2 mul rmoveto\n"
                         "SW 2 div 1.2 mul SW neg 1.2 mul rlineto\n"
                         "SW 1.2 mul neg 0  rlineto\n"
                         "fill\n"
                         "}bind def\n\n";

        sPSFileStream << "/vtriangle{\n"
                         "moveto\n"
                         "0  SW 2 div 1.2 mul rmoveto\n"
                         "SW 2 div 1.2 mul SW neg 1.2 mul rlineto\n"
                         "SW 1.2 mul neg 0  rlineto\n"
                         "stroke\n"
                         "}bind def\n\n";

        sPSFileStream << "/cross{\n"
                         "moveto\n"
                         "SW 2 div SW 2 div rmoveto\n"
                         "SW neg SW neg rlineto\n"
                         "0  SW rmoveto\n"
                         "SW SW neg rlineto\n"
                         "stroke\n"
                         "}bind def\n\n";

        sPSFileStream << "/doublecross{\n"
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

        sPSFileStream << "/vcross{\n"
                         "moveto\n"
                         "SW 3 div SW 3 div rmoveto\n"
                         "SW 1.5 div neg SW 1.5 div neg rlineto\n"
                         "0  SW 1.5 div rmoveto\n"
                         "SW 1.5 div SW 1.5 div neg rlineto\n"
                         "stroke\n"
                         "}bind def\n\n";

        s.sprintf("%% Main Picture (local coordinates "
                  "(x0,y0,x1,y1)=(0,0,%d,%d):\n"
                  "gsave\n"
                  "scaletransformation\n"
                  "[] 0 setdash\n",
                  w, h);
        sPSFileStream << s;

        if (!bgColours::PRINT_WHITE_BG) {
            s.sprintf("%% Fill background with black rectangle:\n"
                      "newpath\n"
                      "0 0 moveto\n"
                      "%d 0 lineto\n"
                      "%d %d lineto\n"
                      "0 %d lineto\n"
                      "closepath\n"
                      "col0\n"
                      "fill\n",
                      w, w, h, h);
            sPSFileStream << s;
        }

        if (iszoom || gVFResults.typeofview_ == TYPEOFVIEW_PLANE) {
            sPSFileStream << "frame\n";
        }

        sPSFileStream << "\n"
                         "%% Plot the orbits\n\n"
                         "LW setlinewidth\n";
    }
}

void finishPostscriptPrinting(void)
{
    if (sPSFile != nullptr) {
        sPSFileStream << "grestore\n"
                         "showpage\n"
                         "%%EOF\n";

        sPSFileStream.flush();
        sPSFileStream.setDevice(nullptr);
        sPSFile->close();
        delete sPSFile;
        sPSFile = nullptr;
    }

    plot_l = spherePlotLine;
    plot_p = spherePlotPoint;
}
