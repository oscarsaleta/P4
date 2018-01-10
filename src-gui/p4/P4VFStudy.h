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

#ifndef P4VFSTUDY_H
#define P4VFSTUDY_H

#include "file_tab.h"

class P4VFStudy : public QObject
{
  public:
    // Constructor and destructor
    P4VFStudy(P4ParentStudy *parent = nullptr); // constructor
    ~P4VFStudy();                               // destructor

    P4ParentStudy *parent_;

    // general information

    bool singinf_;
    int dir_vec_field_;

    // vector field in various charts
    // FIXME arreglar tots els llocs on s'usen
    std::vector<p4polynomials::term2> f_vec_field_0_;
    std::vector<p4polynomials::term2> f_vec_field_1_;
    std::vector<p4polynomials::term2> vec_field_U1_0_;
    std::vector<p4polynomials::term2> vec_field_U1_1_;
    std::vector<p4polynomials::term2> vec_field_U2_0_;
    std::vector<p4polynomials::term2> vec_field_U2_1_;
    std::vector<p4polynomials::term2> vec_field_V1_0_;
    std::vector<p4polynomials::term2> vec_field_V1_1_;
    std::vector<p4polynomials::term2> vec_field_V2_0_;
    std::vector<p4polynomials::term2> vec_field_V2_1_;
    std::vector<p4polynomials::term3> vec_field_C_0_;
    std::vector<p4polynomials::term3> vec_field_C_1_;

    // singular points and their properties:
    std::vector<p4singularities::saddle> first_saddle_point_;
    std::vector<p4singularities::semi_elementary> first_se_point_;
    std::vector<p4singularities::node> first_node_point_;
    std::vector<p4singularities::strong_focus> first_sf_point_;
    std::vector<p4singularities::weak_focus> first_wf_point_;
    std::vector<p4singularities::degenerate> first_de_point_;

    // Greatest common factor if present:
    std::vector<p4polynomials::term2> gcf_;
    std::vector<p4polynomials::term2> gcf_U1_;
    std::vector<p4polynomials::term2> gcf_U2_;
    std::vector<p4polynomials::term2> gcf_V1_;
    std::vector<p4polynomials::term2> gcf_V2_;
    std::vector<p4polynomials::term3> gcf_C_;
    std::vector<p4orbits::orbits_points> gcf_points_;

    // curves
    std::vector<curves> curve_vector_;
    // isoclines
    std::vector<isoclines> isocline_vector_;

    // reading of the Maple/Reduce results
    bool readTables(QString basename);
    bool readGCF(FILE *fp);
    bool readCurve(QString basename);
    bool readIsoclines(QString basename);
    bool readVectorField(FILE *fp, std::vector<p4polynom::term2> &vf0,
                         std::vector<p4polynom::term2> &vf1);
    bool readVectorFieldCylinder(FILE *fp, std::vector<p4polynom::term3> &vf0,
                                 std::vector<p4polynom::term3> &vf1);
    bool readPoints(FILE *fp);

    bool readSaddlePoint(FILE *fp);
    bool readSemiElementaryPoint(FILE *fp);
    bool readStrongFocusPoint(FILE *fp);
    bool readWeakFocusPoint(FILE *fp);
    bool readDegeneratePoint(FILE *fp);
    bool readNodePoint(FILE *fp);
    bool readBlowupPoints(FILE *fp,
                          std::vector<p4singularities::blow_up_points> b,
                          int n);
    bool readTransformations(
        FILE *fp, std::vector<p4singularities::transformations> trans, int n);

    void setupCoordinateTransformations(void); // see math_p4.cpp

    void dump(QTextEdit &m);

  private:
    void dumpSeparatrices(QTextEdit &m, std::vector<p4singularities::sep> separ,
                          int margin);
    void dumpSingularities(QTextEdit &m, std::vector<p4singularities::saddle> p,
                           bool longversion);
    void dumpSingularities(QTextEdit &m,
                           std::vector<p4singularities::degenerate> p,
                           bool longversion);
    void dumpSingularities(QTextEdit &m,
                           std::vector<p4singularities::strong_focus> p,
                           bool longversion);
    void dumpSingularities(QTextEdit &m,
                           std::vector<p4singularities::weak_focus> p,
                           bool longversion);
    void dumpSingularities(QTextEdit &m, std::vector<p4singularities::node> p,
                           bool longversion);
    void dumpSingularities(QTextEdit &m,
                           std::vector < p4singularities::semi_elementary p,
                           bool longversion);
};

#endif /* P4VFSTUDY_H */