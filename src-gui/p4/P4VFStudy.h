/*  This file is part of P4
 *
 *  Copyright (C) 1996-2017  J.C. Artés, P. De Maesschalck, F. Dumortier
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
    P4VFStudy();   // constructor
    ~P4VFStudy();  // destructor

    P4ParentStudy *parent_;

    // general information

    bool singinf_;
    int dir_vec_field_;

    // vector field in various charts

    p4polynomials::term2 *f_vec_field_[2];
    p4polynomials::term2 *vec_field_U1_[2];
    p4polynomials::term2 *vec_field_U2_[2];
    p4polynomials::term2 *vec_field_V1_[2];
    p4polynomials::term2 *vec_field_V2_[2];
    p4polynomials::term3 *vec_field_C_[2];

    // singular points and their properties:

    saddle *first_saddle_point_;
    semi_elementary *first_se_point_;
    node *first_node_point_;
    strong_focus *first_sf_point_;
    weak_focus *first_wf_point_;
    degenerate *first_de_point_;

    // Greatest common factor if present:

    p4polynomials::term2 *gcf_;
    p4polynomials::term2 *gcf_U1_;
    p4polynomials::term2 *gcf_U2_;
    p4polynomials::term2 *gcf_V1_;
    p4polynomials::term2 *gcf_V2_;
    p4polynomials::term3 *gcf_C_;
    P4ORBIT gcf_points_;

    // curves
    std::vector<curves> curve_vector_;
    // isoclines
    std::vector<isoclines> isocline_vector_;

    // initialization and destruction of structures

    void deleteVF(void);

    void deleteSaddle(saddle *);
    void deleteSemiElementary(semi_elementary *);
    void deleteNode(node *p);
    void deleteStrongFocus(strong_focus *p);
    void deleteWeakFocus(weak_focus *p);
    void deleteDegenerate(degenerate *);
    void deleteSeparatrices(sep *);
    void deleteTransformations(transformations *t);
    void deleteBlowup(blow_up_points *b);

    // reading of the Maple/Reduce results

    bool readTables(QString basename);
    bool readGCF(FILE *fp);
    bool readCurve(QString basename);
    bool readIsoclines(QString basename);
    bool readVectorField(FILE *fp, p4polynomials::term2 **vf);
    bool readVectorFieldCylinder(FILE *fp, p4polynomials::term3 **vf);
    bool readPoints(FILE *fp);

    bool readSaddlePoint(FILE *fp);
    bool readSemiElementaryPoint(FILE *fp);
    bool readStrongFocusPoint(FILE *fp);
    bool readWeakFocusPoint(FILE *fp);
    bool readDegeneratePoint(FILE *fp);
    bool readNodePoint(FILE *fp);
    bool readBlowupPoints(FILE *fp, blow_up_points *b, int n);
    bool readTransformations(FILE *fp, transformations *trans, int n);

    void setupCoordinateTransformations(void);  // see math_p4.cpp

    void dump(QString basename, QString info = "");

   private:
    void dumpSeparatrices(QTextEdit *m, p4singularities::sep *separ,
                          int margin);
    void dumpSingularities(QTextEdit *m, p4singularities::genericsingularity *p,
                           const char *type, bool longversion);
};

#endif /* P4VFSTUDY_H */