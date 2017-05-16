#
# This file was automatically generated by MPLSTRIP.
# Source file: main.tex
# Time stamp on source file: Tue May 16 13:35:54 2017
#
restart;
read( "tools.m" );
read( "gcd.m" );
read( "num_coeff.m" );
read( "realroot.m" );
read( "writelog.m" );
read( "type.m" );
read( "infinity.m" );
read( "blowup.m" );
read( "weakfocus.m" );

user_bindir := "/usr/local/p4/bin":
user_tmpdir := "/tmp":
user_lypexe := "lyapunov":
user_lypexe_mpf := "lyapunov_mpf":
user_removecmd := "rm":
user_sepexe := "separatrice":
user_exeprefix := "cmd /c ":

user_numeric := false:
user_precision := 8:
user_precision0 := 0:
epsilon := 0.01:
test_sep := 0:
taylor_level := 10:
numeric_level := 10:
max_level := 10:
weakness_level := 5:
normalexit := 0;

x0 := 0:
y0 := 0:
x_min := -1:
x_max := +1:
y_min := -1:
y_max := +1:

user_p := 1:
user_q := 1:

user_f := [ 0,0 ]:
user_gcf := 0:

all_crit_points := 0:
save_all := false:
vec_table := "TEST_VEC.TAB":
finite_table := "TEST_FIN.TAB":
finite_res := "TEST_FIN.RES":
infinite_table := "TEST_INF.TAB":
infinite_res := "TEST_INF.RES":
curve_table := "TEST_CURVE.TAB":

hamiltonian := false:

ishamiltonian := proc( vf )
    local expr;
    expr := norpoly2(  diff(vf[1], x) + diff(vf[2], y), x, y );
    if rounded then
        if evalf( expr ) = 0. then true else false fi
    else
        if expr = 0 then true else false fi
    fi
end:

CheckIfGoodPolynomial := proc(f)
    return type( evalf(f), polynom( numeric, [x,y] ) );
end:

p4main := proc()
    global rounded, user_f, user_gcf, user_p, user_q, hamiltonian, log_file, result_file, normalexit;
    local gc, gcf, gcdresult, orig_f, g, ge, gn, j;

    normalexit := 0;
    rounded := user_numeric;
    if user_precision0 <> 0 then
        Digits := user_precision0;
    else
        Digits := floor(evalhf(Digits));
    fi;
    if user_precision >= Digits then
        writef( "Please increase precision or decrease accuracy. Precision = %d <= Accuracy = %d.\n", Digits, user_precision );
        `quit`(1);
    end if;

    if CheckIfGoodPolynomial(user_f[1]) = false or
        CheckIfGoodPolynomial(user_f[2]) = false or
        CheckIfGoodPolynomial(user_gcf) = false then
        writef( "The system is not well-defined (not polynomial, or coefficients do not evaluate numerically.)\n" );
        `quit`(1);
    end if;

    writef( "LET US STUDY THE DIFFERENTIAL SYSTEM:\n" );
    writef( "    x' = %a\n    y' = %a\n\n", user_f[1], user_f[2] );

    openfile( vec_table );
    writef( "%d\n%d\n%d\n", all_crit_points, user_p, user_q );
    if all_crit_points = 3 then
        writef( "    %g %g %g %g\n", x_min, x_max, y_min, y_max )
    fi;
    openfile( terminal );

    if user_gcf = 0 then
        orig_f := user_f;
        gcdresult := my_gcd( user_f[1], user_f[2] );
        gc := gcdresult[2];
        user_gcf := gcdresult[3];
        user_f := simplify(gcdresult[1]);
    else
        gc := 2;
        orig_f := user_f;
        user_f := [ simplify( orig_f[1] / user_gcf ), simplify( orig_f[2]/user_gcf ) ];
        if CheckIfGoodPolynomial(user_f[1]) = false or
            CheckIfGoodPolynomial(user_f[2]) = false then
        writef( "Error: The given gcf is not a common factor.\n" );
        `quit`(1);
        end if;
    fi;
    write_gcf( user_gcf );

    openfile( vec_table );
    write_vec_field( user_f );
    openfile( terminal );

    if ishamiltonian( user_f ) then
        writef( "The system is Hamiltonian.\n" );
        hamiltonian := true
    else
        hamiltonian := false
    fi;

    if not member( all_crit_points, {2,3} ) then
        log_file := finite_table;
        result_file := finite_res;
        writef( "LET US STUDY THE SYSTEM AT THE FINITE REGION\n\n" );
        openfile( result_file );
        writef( "LET US STUDY THE DIFFERENTIAL SYSTEM:\n" );
        writef( "    x' = %a\n    y' = %a\n", orig_f[1], orig_f[2] );
        writef( "AT THE FINITE REGION\n" );
        if gc=1 or ( gc=2 and user_gcf <> 1 ) then
            writef( "The system has a common factor: %a\n", user_gcf );
            writef( "Hence we shall study the reduced system:\n", user_gcf );
            writef( "    x' = %a\n    y' = %a\n", user_f[1], user_f[2] )
        fi;
        openfile( terminal );
        if not(user_numeric) then
            g := find_real_roots( user_f, x, y );
            ge := g[1];
            gn := g[2];
            openfile( log_file );
            writef( "%d\n", nops(ge) + nops(gn) );
            openfile( terminal );
            writef( "--------------------------------------------------------\n" );
            for j from 1 to nops(ge) do
                sing_type( user_f, op(1, ge[j]), op(2, ge[j]), 0 )
            od;
            rounded := true;
            for j from 1 to nops(gn) do
                sing_type( user_f, op(1, gn[j]), op(2, gn[j]), 0 )
            od;
            rounded := user_numeric;
        else
            gn := find_real_roots_numeric( user_f, x, y );
            openfile( log_file );
            writef( "%d\n", nops(gn) );
            openfile( terminal );
            writef( "--------------------------------------------------------\n" );
            for j from 1 to nops(gn) do
                sing_type( user_f, op(1, gn[j]), op(2, gn[j]), 0 )
            od
        fi;
        closefile( log_file );
        closefile( result_file );
    fi;

    if not member( all_crit_points, {1,3} ) then
        log_file := infinite_table;
        result_file := infinite_res;
        writef( "LET US STUDY THE SYSTEM AT INFINITY\n\n" );
        writef( "LET US STUDY THE DIFFERENTIAL SYSTEM:\n" );
        if gc=2 then
            writef( "    x' = %a\n    y' = %a\n", user_gcf*user_f[1], user_gcf*user_f[2] )
        else
            writef( "    x' = %a\n    y' = %a\n", orig_f[1], orig_f[2] )
        fi;
        writef( "AT INFINITY\n" );
        if gc=1 or ( gc=2 and user_gcf <> 1 ) then
            writef( "The system has a common factor: %a\n", user_gcf );
            writef( "Hence we shall study the reduced system:\n", user_gcf );
            writef( "    x' = %a\n    y' = %a\n", user_f[1], user_f[2] )
        fi;
        openfile( terminal )
    fi;

    if user_p = 1 and user_q = 1 then
        vec_field_diff_chart( user_f, x, y );
    else
        pl_chart( user_f );
    fi;

    if not member( all_crit_points, {1,3} ) then
        closefile( log_file );
        closefile( result_file );
    fi;

    closefile( vec_table );

    if all_crit_points = 3 then
        log_file := finite_table;
        result_file := finite_res;
        openfile( log_file );
        writef( "%d\n", 1 );
        openfile( terminal );
        writef( "We shall study the differential system:\n" );
        if gc = 2 then
            writef( "    x' = %a\n    y' = %a\n", user_gcf*user_f[1], user_gcf*user_f[2] )
        else
            writef( "    x' = %a\n    y' = %a\n", orig_f[1], orig_f[2] )
        fi;
        writef( "in the point (%a,%a).\n", x0, y0 );
        if gc=1 or ( gc=2 and user_gcf <> 1 ) then
            writef( "The system has a common factor: %a\n", user_gcf );
            writef( "Hence we shall study the reduced system:\n", user_gcf );
            writef( "    x' = %a\n    y' = %a\n", user_f[1], user_f[2] )
        fi;
        openfile( terminal );
        sing_type( user_f, x0, y0, 0 );
        closefile( log_file );
        closefile( result_file );
    fi;
    openfile( terminal );
    writef( "\nWe have finished the study of the vector field.\n" );
    closeallfiles();
    normalexit := 1;
end:

prepareCurve := proc()
    global user_curve, normalexit;
    openfile( curve_table );
    write_curve(user_curve);
    writef( "\nWe have finished writing the curve as a C++ readable list.\n" );    
    normalexit := 1;
end:

prepareIsoclines := proc()
    global user_isoclines, normalexit;
    openfile( isoclines_table );
    write_isoclines(user_isoclines);
    writef("\nWe have finished writing the isoclines as C++ readable lists.\n");
    normalexit := 1;
end:

save( user_numeric, user_precision, user_precision0, epsilon, test_sep, taylor_level, 
        numeric_level, max_level, weakness_level, x0, y0, x_min, y_min, x_max, y_max,
        user_p, user_q, user_f, user_gcf, user_curve, all_crit_points, save_all, vec_table,
        finite_table, finite_res, infinite_table, infinite_res, hamiltonian, ishamiltonian,
        p4main, prepareCurve, prepareIsoclines, CheckIfGoodPolynomial, normalexit,
        user_exeprefix, "main.m" );

save( epsilon, test_sep, taylor_level, numeric_level, max_level, weakness_level, x0, y0,
        x_min, y_min, x_max, y_max, all_crit_points, save_all, vec_table, finite_table,
        finite_res, infinite_table, infinite_res, hamiltonian, ishamiltonian, p4main,
        prepareCurve, prepareIsoclines, CheckIfGoodPolynomial, normalexit,
#tools
        rounded, Hardware_precision, reduce_coeff, write_number, reduce_llt,
        reduce_evalf, reduce_gt, reduce_gteq, reduce_eeq, reduce_nneq, optimizepolynomial1, 
        optimizepolynomial2, first_term, lleadterm, nlterm, ddeg, low_ddeg, nterm, low_deg1, 
        optimizevf, quasihom_degree1, quasihom_degree, change_poly, sort_list, reduce_imp, 
        reduce_conj, reduce_re, reduce_im, openfile, closefile, writef, currentopenfile, 
        openfilelist, closeallfiles, norpoly, norpoly2, norpoly3,
#gcd
        my_gcd,

#num_coeff
        num_coeff,

#realroot
        save_all, real_roots_numeric, find_real_roots1, find_real_roots_numeric, find_real_roots, find_real_roots11,
#write_log
        coeff_degree1, coeff_degree2, coeff_degree3,
        write_vec_field, write_vec_field_cylinder,
        write_saddle, write_node, write_weak_focus,
        write_strong_focus, write_semi_elementary,
        write_separatrice, write_degenerate, write_gcf, write_curve,
#type
        jacobian, eigenvector, translation, transformation, normalization, manifold,
        center_manifold, saddle, strong_focus, node, degenerate, semi_elementary,
        sing_type, separatrice_test,
#weak_focus
        weak_focus, lyapunov, readlyapunovresult, preparelyapunovfile,
        lyapunovsym,preparelyapunovsym, sumtable, part_lyapunov_coeff,
        lyp_multc_poly, lyp_findpoly, lyp_Regz, lyp_Imgz, lyp_LL, lyp_G, lyp_conj_poly,
#infinity
        vec_field_diff_chart, show_charts, behaviour_inf, pl_chart, find_inf_roots, 
        find_inf_roots_sing, saddle_inf, se_inf, change_stab, de_inf, type_inf,
#blowup
        assoc_points, newton_diagram, GAMMA_1, quasi_degree, center_type1,
        center_type2, power, sort_set_pos, sort_set_neg, put_separatrice,
        append_trajectors, split_trajectors, make_transformation, write_trajectors,
        manifold_blow_up, manifold2_blow_up, center_manifold_blow_up, blowing_up,
        blow_upx, blow_upy, sing_eigenv, blowing_upy2, blowing_up2,
        ManXPos, ManXNeg, NumXPos, NumXNeg, NumberY, ManYPos, ManYNeg,
#environment variables
        user_bindir, user_tmpdir, user_lypexe, user_lypexe_mpf, user_removecmd, user_sepexe,

"p4.m" );
