// ---------------------------------------------------------------------------
//      start_plot_saddle_sep
// ---------------------------------------------------------------------------
void start_plot_saddle_sep(std::shared_ptr<P4WinSphere> spherewnd)
{
    double p[3];
    p4orbits::orbits_points &last_sep_point{
        g_VFResults.selected_sep->sep_points.back()};

    draw_sep(spherewnd, g_VFResults.selected_sep_->first_sep_point);

    if (!last_sep_point.empty()) {
        copy_x_into_y(last_sep_point.pcoord, p);
        g_VFResults.selected_sep_->sep_points.push_back(integrate_sep(
            spherewnd, p, g_VFResults.config_currentstep_, last_sep_point.dir,
            last_sep_point.type, g_VFResults.config_intpoints_));
    } else {
        g_VFResults.selected_sep_->sep.points =
            plot_separatrice(spherewnd, g_VFResults.selected_saddle_point_->x0,
                             g_VFResults.selected_saddle_point_->y0,
                             g_VFResults.selected_saddle_point_->a11,
                             g_VFResults.selected_saddle_point_->a12,
                             g_VFResults.selected_saddle_point_->a21,
                             g_VFResults.selected_saddle_point_->a22,
                             g_VFResults.selected_saddle_point_->epsilon,
                             g_VFResults.selected_sep_, &points,
                             g_VFResults.selected_saddle_point_->chart);
        g_VFResults.selected_sep_->last_sep_point = points;
    }
}

void cont_plot_saddle_sep(std::shared_ptr<P4WinSphere> spherewnd)
{
    double p[3];
    orbits_points *points;

    copy_x_into_y(g_VFResults.selected_sep_->last_sep_point->pcoord, p);
    g_VFResults.selected_sep_->last_sep_point->next_point =
        integrate_sep(spherewnd, p, g_VFResults.config_currentstep_,
                      g_VFResults.selected_sep_->last_sep_point->dir,
                      g_VFResults.selected_sep_->last_sep_point->type,
                      g_VFResults.config_intpoints_, &points);
    g_VFResults.selected_sep_->last_sep_point = points;
}

void plot_next_saddle_sep(std::shared_ptr<P4WinSphere> spherewnd)
{
    draw_sep(spherewnd, g_VFResults.selected_sep_->first_sep_point);
    if (!(g_VFResults.selected_sep_ = g_VFResults.selected_sep_->next_sep))
        g_VFResults.selected_sep_ =
            g_VFResults.selected_saddle_point_->separatrices;
    start_plot_saddle_sep(spherewnd);
}

void select_next_saddle_sep(std::shared_ptr<P4WinSphere> spherewnd)
{
    draw_sep(spherewnd, g_VFResults.selected_sep_->first_sep_point);
    if (!(g_VFResults.selected_sep_ = g_VFResults.selected_sep_->next_sep))
        g_VFResults.selected_sep_ =
            g_VFResults.selected_saddle_point_->separatrices;
    draw_selected_sep(spherewnd, g_VFResults.selected_sep_->first_sep_point,
                      CW_SEP);
}

static void plot_all_saddle_sep(std::shared_ptr<P4WinSphere> spherewnd,
                                saddle *point)
{
    sep *sep1;
    orbits_points *points;
    double p[3];

    while (point != nullptr) {
        if (point->notadummy) {
            sep1 = point->separatrices;
            while (sep1) {
                if (sep1->last_sep_point) {
                    copy_x_into_y(sep1->last_sep_point->pcoord, p);
                    sep1->last_sep_point->next_point = integrate_sep(
                        spherewnd, p, g_VFResults.config_currentstep_,
                        sep1->last_sep_point->dir, sep1->last_sep_point->type,
                        g_VFResults.config_intpoints_, &points);
                    sep1->last_sep_point = points;
                } else {
                    sep1->first_sep_point = plot_separatrice(
                        spherewnd, point->x0, point->y0, point->a11, point->a12,
                        point->a21, point->a22, point->epsilon, sep1, &points,
                        point->chart);
                    sep1->last_sep_point = points;
                }
                sep1 = sep1->next_sep;
            }
        }
        point = point->next_saddle;
    }
}