#
# This file was automatically generated by MPLSTRIP.
# Source file: type.tex
# Time stamp on source file: Tue Oct 24 14:36:22 2017
#
restart;
read( "tools.m" );
read( "writelog.m" );

jacobian := proc( f, xeq, yeq )
    local expr;
    eval( map( expr -> subs( { xeq, yeq }, expr ),
    [ diff( f[1], lhs(xeq) ), diff( f[1], lhs(yeq) ), diff( f[2], lhs(xeq) ), diff( f[2], lhs(yeq) ) ] ) );
end:

eigenvector := proc( a,b,c,d )
    local _c, n;
    if reduce_nneq( b, 0 ) then _c := [ 1, -a/b ]
    else if reduce_nneq( a, 0 ) then _c := [0,1]
    else if reduce_nneq( d, 0 ) then _c := [1,-c/d]
    else  _c := [0,1]
    fi fi fi;
    if rounded then
        n := sqrt(_c[1]^2 + _c[2]^2);
        _c := evalf( [ _c[1]/n, _c[2]/n ] );
    fi;
    _c
end:

translation := proc( v, x, y, x0, y0 )
    local f1, f2;
    f1 := subs( { x = x + x0, y = y + y0 }, v[1] );
    f2 := subs( { x = x + x0, y = y + y0 }, v[2] );
    if rounded then
        f1 := change_poly( f1, x, y );
        f2 := change_poly( f2, x, y );
    fi;
    [f1,f2]
end:

transformation := proc( v, a, b, c, d )
    local f1, f2, _x, _y, _a, _b, _c, _d, M, g1, g2;

    f1 := subs( { x = a*_x+b*_y, y = c*_x + d*_y}, v[1] );
    f2 := subs( { x = a*_x+b*_y, y = c*_x + d*_y}, v[2] );

    M := a*d - b*c;
    _a := d/M; _b := -b/M; _c := -c/M; _d := a/M;
    g1 := subs( _x = x, _y = y, _a * f1 + _b * f2 );
    g2 := subs( _x = x, _y = y, _c * f1 + _d * f2 );
    optimizevf( [g1,g2], x, y );
end:
normalization := proc( v )
    local L1, L2, m;
    L1 := coeff_degree2( optimizepolynomial2( v[1], x, y ), x, y );
    L2 := coeff_degree2( optimizepolynomial2( v[2], x, y ), x, y );
    L1 := map( expr -> evalf(abs(expr[3])), L1 );
    L2 := map( expr -> evalf(abs(expr[3])), L2 );
    m := max( op(L1),op(L2));
    if not rounded then m := convert(m,rational) fi;
    optimizevf( [ 1000*v[1]/m, 1000*v[2]/m ], x, y );
end:
saddle := proc( f, x0, y0, a, b, c, d, rho, sigma, chart )
    local lambda1, lambda2, lambda_ratio, lambda_ratio_num, lambda_ratio_den, ff, g, g1, v1, v2, stable, sep1, sep2;
    writef( "(%a,%a) is a saddle point.\n", evalf(x0), evalf(y0) );
    openfile( result_file );
    writef( "(%a,%a) is a saddle point.\n", evalf(x0), evalf(y0) );
    openfile( terminal );
    lambda1 := (rho - sqrt(sigma))/2;
    lambda2 := (rho + sqrt(sigma))/2;
    if abs(evalf(lambda1)) > abs(evalf(lambda2)) then
        lambda_ratio := lambda1/lambda2;
    else
        lambda_ratio := lambda2/lambda1;
    end if;
    if (not rounded) and (type(lambda1, integer)) and (type(lambda2, integer)) then 
        lambda_ratio_num := numer(lambda_ratio);
        lambda_ratio_den := denom(lambda_ratio);
        writef( "The ratio between eigenvalues is %a:%a\n", lambda_ratio_den, lambda_ratio_num);
        openfile( result_file );
        writef( "The ratio between eigenvalues is %a:%a\n", lambda_ratio_den, lambda_ratio_num);
        openfile( terminal );
    else
        lambda_ratio := convert(evalf(lambda_ratio), rational);
        lambda_ratio := convert(evalf(lambda_ratio), rational);        
        lambda_ratio_num := numer(lambda_ratio);
        lambda_ratio_den := denom(lambda_ratio);
        writef( "The ratio between eigenvalues is %a:%a (approximation)\n", lambda_ratio_den, lambda_ratio_num );
        openfile( result_file );
        writef( "The ratio between eigenvalues is %a:%a (approximation)\n", lambda_ratio_den, lambda_ratio_num );
        openfile( terminal );
    end if;
    v1 := eigenvector( a-lambda1, b, c, d-lambda1 );
    v2 := eigenvector( a-lambda2, b, c, d-lambda2 );
    if save_all then
        openfile( result_file );
        writef( " The eigenvalues are: { %a, %a }\n", lambda1, lambda2 );
        writef( " The eigenvectors are: { %a, %a }\n", v1, v2 );
        openfile( terminal );
    fi;
    if save_all then openfile( terminal, result_file ) end if;
    if x0 <> 0 or y0 <> 0 then
        ff := translation( f, x, y, x0, y0 );
        writef( "Moving this saddle point to the origin.\n" );
        writef( "The new vector field is %a\n", ff );
    else
        ff := f;
    end if;
    g1 := transformation( ff, v1[1], v2[1], v1[2], v2[2] );
    writef( "Transform this vector field so that the linear part is diagonal.\n" );
    writef( "The new vector field is %a\n", g1 );
    g := normalization( g1 );
    writef( "Make a 1000-normalization\n" );
    writef( "so that the new vector field is %a\n", g );
    writef( "We calculate the invariant separatrices\n" );
    openfile( terminal );
    if evalf( lambda1 ) < 0 then stable :=1 else stable := 0 fi;
    sep1 := manifold( g, taylor_level, stable );
    if evalf( lambda2 ) < 0 then stable := 1 else stable := 0 fi;
    sep2 := manifold( transformation(g,0,1,1,0),taylor_level, stable );
    if reduce_gt(lambda1, 0) then
        writef( "The stable separatrice is %a.\n", [ sep2,t ] );
        writef( "The unstable separatrice is %a.\n", [t, sep1] );
        if save_all then
            openfile( result_file );
            writef( "The stable separatrice is %a.\n", [ sep2,t ] );
            writef( "The unstable separatrice is %a.\n", [t, sep1] );
            openfile( terminal );
        fi
    else
        writef( "The stable separatrice is %a.\n", [ t,sep1 ] );
        writef( "The unstable separatrice is %a.\n", [sep2,t] );
        if save_all then
            openfile( result_file );
            writef( "The stable separatrice is %a.\n", [ t,sep1] );
            writef( "The unstable separatrice is %a.\n", [sep2,t] );
            openfile( terminal );
        fi
    fi;
    writef( "########################################################\n" );
    openfile( result_file );
    writef( "########################################################\n" );
    openfile( terminal );
    write_saddle( x0, y0, v1[1], v2[1], v1[2], v2[2], g, [sep1,sep2],
             [sign(evalf(lambda1)), sign(evalf(lambda2))], chart );
end:
strong_focus := proc( sigma, rho, x0, y0, chart )
    local lambda1, lambda2, L;
    lambda1 := (rho-sqrt(sigma))/2;
    lambda2 := (rho+sqrt(sigma))/2;
    if reduce_gt(rho,0) then
        writef( "(%a,%a) is a strong unstable focus.\n", evalf(x0), evalf(y0) );
        openfile( result_file );
        writef( "(%a,%a) is a strong unstable focus.\n", evalf(x0), evalf(y0) );
        openfile( terminal );
        L := 1;
    else
        writef( "(%a,%a) is a strong stable focus.\n", evalf(x0), evalf(y0) );
        openfile( result_file );
        writef( "(%a,%a) is a strong stable focus.\n", evalf(x0), evalf(y0) );
        openfile( terminal );
        L := -1;
    fi;
    writef( "########################################################\n" );
    openfile( result_file );
    writef( "########################################################\n" );
    openfile( terminal );
    write_strong_focus(x0,y0,L,chart);
end:
node := proc( f, sigma, rho, x0, y0, chart )
    local L;
    if reduce_llt( rho, 0 ) then
        writef( "(%a,%a) is a stable node.\n", evalf(x0), evalf(y0) );
        openfile( result_file );
        writef( "(%a,%a) is a stable node.\n", evalf(x0), evalf(y0) );
        openfile( terminal );
        L := -1;
    else
        writef( "(%a,%a) is an unstable node.\n", evalf(x0), evalf(y0) );
        openfile( result_file );
        writef( "(%a,%a) is an unstable node.\n", evalf(x0), evalf(y0) );
        openfile( terminal );
        L := 1;
    fi;
    lambda1 := (rho - sqrt(sigma))/2;
    lambda2 := (rho + sqrt(sigma))/2;
    if abs(evalf(lambda1)) > abs(evalf(lambda2)) then
        lambda_ratio := lambda1/lambda2;
    else
        lambda_ratio := lambda2/lambda1;
    end if;
    if (not rounded) and (type(lambda1, integer)) and (type(lambda2, integer)) then 
        lambda_ratio_num := numer(lambda_ratio);
        lambda_ratio_den := denom(lambda_ratio);
        writef( "The ratio between eigenvalues is %a:%a\n", lambda_ratio_den, lambda_ratio_num);
        openfile( result_file );
        writef( "The ratio between eigenvalues is %a:%a\n", lambda_ratio_den, lambda_ratio_num);
        openfile( terminal );
    else
        lambda_ratio := convert(evalf(lambda_ratio), rational);
        lambda_ratio := convert(evalf(lambda_ratio), rational);
        lambda_ratio_num := numer(lambda_ratio);
        lambda_ratio_den := denom(lambda_ratio);
        writef( "The ratio between eigenvalues is %a:%a (approximation)\n", lambda_ratio_den, lambda_ratio_num );
        openfile( result_file );
        writef( "The ratio between eigenvalues is %a:%a (approximation)\n", lambda_ratio_den, lambda_ratio_num );
        openfile( terminal );
    end if;
    writef( "########################################################\n" );
    openfile( result_file );
    writef( "########################################################\n" );
    openfile( terminal );
    write_node( x0, y0, L, chart );
end:
degenerate := proc( f, x0, y0, chart )
    local trajectors;
    writef( "(%a,%a) is a non-elementary point.\n", evalf(x0), evalf(y0) );
    writef( "Blowing up the vector field...\n" );
    openfile( result_file );
    writef( "(%a,%a) is a non-elementary point.\n", evalf(x0), evalf(y0) );
    openfile( terminal );
    trajectors := blowing_up( f, x0, y0, true ); # 1 finite point
    writef( "########################################################\n" );
    openfile( result_file );
    writef( "########################################################\n" );
    openfile( terminal );
    write_degenerate( x0, y0, trajectors, chart );
end:
semi_elementary := proc( f, x0, y0, a, b, c, d, lambda2, chart )
    local v1, v2, ff, g, g1, center_sep, sep1, sep2, stype, stable;
    v1 := eigenvector( a,b,c,d );
    v2 := eigenvector( a-lambda2,b,c,d-lambda2 );
    ff := translation( f, x, y, x0, y0 );
    g1 := transformation( ff, v1[1], v2[1], v1[2], v2[2] );
    g := normalization( g1 );
    writef( "(%a,%a) is semi-hyperbolic.\nMoving this point to the origin.\n", evalf(x0), evalf(y0) );
    writef( "The new vector field is %a\n", ff );
    writef( "Transform this vector field so that the linear part is diagonal.\n" );
    writef( "The new vector field is %a\n", g1 );
    writef( "Make a 1000-normalization\n" );
    writef( "so that the new vector field is %a\n", g );
    writef( "We calculate the invariant separatrices\n" );
    openfile( result_file );
    writef( "(%a,%a) is semi-hyperbolic.\n", evalf(x0), evalf(y0) );
    if save_all then
        writef( "Moving this point to the origin.\n" );
        writef( "The new vector field is %a\n", ff );
        writef( "Transform this vector field so that the linear part is diagonal.\n" );
        writef( "The new vector field is %a\n", g1 );
        writef( "Make a 1000-normalization\n" );
        writef( "so that the new vector field is %a\n", g );
        writef( "We calculate the invariant separatrices\n" );
    fi;
    openfile( terminal );
    if reduce_llt(lambda2, 0) then stable := 1 else stable := 0 fi;
    center_sep := center_manifold(g,taylor_level,stable);
    sep1 := subs( x=t,center_sep[1] );
    sep2 := subs( x=t, manifold(transformation(g,0,1,1,0),taylor_level,stable));
    stype := center_sep[3];
    if member( stype, {1,3} ) then
        writef( "We have a saddle-node.\n" );
        writef( "center separatrice %a\n", [t,sep1] );
        writef( "unstable separatrice %a\n", [sep2,t] );
        openfile( result_file );
        writef( "We have a saddle-node.\n" );
        if save_all then writef( "center separatrice %a\n", [t,sep1] );
            writef( "unstable separatrice %a\n", [sep2,t] ); fi;
        openfile( terminal );
    else if member( stype, {2,4} ) then
        writef( "We have a saddle-node.\n" );
        writef( "center separatrice %a\n", [t,sep1] );
        writef( "stable separatrice %a\n", [sep2,t] );
        openfile( result_file );
        writef( "We have a saddle-node.\n" );
        if save_all then writef( "center separatrice %a\n", [t,sep1] );
            writef( "stable separatrice %a\n", [sep2,t] ); fi;
        openfile( terminal );
    else if stype=6 then
        writef( "We have a saddle.\n" );
        writef( "unstable center separatrice %a\n", [t,sep1] );
        writef( "stable separatrice %a\n", [sep2,t] );
        openfile( result_file );
        writef( "We have a saddle.\n" );
        if save_all then writef( "unstable center separatrice %a\n", [t,sep1] );
        writef( "stable separatrice %a\n", [sep2,t] ); fi;
        openfile( terminal );
    else if stype=7 then
        writef( "We have a saddle.\n" );
        writef( "stable center separatrice %a\n", [t,sep1] );
        writef( "unstable separatrice %a\n", [sep2,t] );
        openfile( result_file );
        writef( "We have a saddle.\n" );
        if save_all then writef( "stable center separatrice %a\n", [t,sep1] );
        writef( "unstable separatrice %a\n", [sep2,t] ); fi;
        openfile( terminal );
    else if stype=5 then
        writef( "We have an unstable node.\n" );
        writef( "center separatrice %a\n", [t,sep1] );
        writef( "unstable separatrice %a\n", [sep2,t] );
        openfile( result_file );
        writef( "We have an unstable node.\n" );
        if save_all then writef( "center separatrice %a\n", [t,sep1] );
        writef( "stable separatrice %a\n", [sep2,t] ); fi;
        openfile( terminal );
    else
        writef( "We have a stable node.\n" );
        writef( "center separatrice %a\n", [t,sep1] );
        writef( "stable separatrice %a\n", [sep2,t] );
        openfile( result_file );
        writef( "We have a stable node.\n" );
        if save_all then writef( "center separatrice %a\n", [t,sep1] );
        writef( "stable separatrice %a\n", [sep2,t] ); fi;
        openfile( terminal );
    fi fi fi fi fi;
    writef( "########################################################\n" );
    openfile( result_file );
    writef( "########################################################\n" );
    openfile( terminal );
    write_semi_elementary(x0,y0, v1[1],v2[1],v1[2],v2[2], g,[stype,1],[sep1,sep2],chart);
end:
sing_type := proc(f, x0,y0, chart )
    local jac,delta,rho,sigma;
    jac := jacobian(f,x=x0,y=y0); if not(rounded) and user_simplify then jac := map(user_simplifycmd,jac) fi;
    delta := jac[1]*jac[4]-jac[2]*jac[3];
    if not(rounded) and user_simplify then delta := user_simplifycmd(delta); fi;
    rho := jac[1]+jac[4]; if not(rounded) and user_simplify then rho := user_simplifycmd(rho); fi;
    sigma := rho^2-4*delta; if not(rounded) and user_simplify then sigma := user_simplifycmd(sigma); fi;
    if save_all then
        openfile( result_file );
        writef( "(%a,%a) is a singular point.\n", x0, y0 );
        writef( "Jacobian = [ [ %a, %a ], [ %a, %a ] ]\n", jac[1], jac[2], jac[3], jac[4] );
        writef( "Delta = %a\n", delta );
        writef( "Rho = %a\n", rho );
        writef( "Sigma = %a\n", sigma );
        openfile( terminal );
    fi;
    if reduce_llt( delta, 0 ) then
        saddle( f, x0, y0, jac[1], jac[2], jac[3], jac[4], rho, sigma, chart );
    else
        if reduce_gt( delta, 0 ) then
            if reduce_gteq( sigma, 0 ) then
                node( f, sigma, rho, x0, y0, chart )
            else
                if reduce_eeq( rho, 0 ) then
                    weak_focus( f, x0, y0, chart )
                else
                    strong_focus( sigma, rho, x0, y0, chart )
                fi
            fi
        else
            if reduce_eeq( rho, 0 ) then
                degenerate( f, x0, y0, chart );
            else
                semi_elementary( f, x0, y0,jac[1],jac[2],jac[3],jac[4],rho,chart );
            fi
        fi;
    fi;
end:
manifold := proc( g, n, stable )
    local vf1, vf2, h1, h2, k, L1, L2, sep, _y, lambda, ok, n1; global test_sep;

    if rounded then vf1 := evalf(g[1]); vf2 := evalf(g[2]); else vf1 := g[1]; vf2 := g[2]; fi;
    L1 := coeff(collect(subs(y=0,g[1]),x),x,1);
    L2 := coeff(collect(subs(x=0,g[2]),y),y,1);
    sep := 0;
    for k from 2 to n do
        lambda := coeff(collect( subs(y=0,vf2),x),x,k) / (L1*k-L2);
        if not rounded and user_simplify then lambda := user_simplifycmd(lambda) fi;
        h1 := expand( subs( y=_y+lambda*x^k,vf1) );
        h2 := expand( subs( y=_y+lambda*x^k,vf2) - lambda*k*x^(k-1)*h1);
        vf1 := subs( _y=y, h1 );
        vf2 := subs( _y=y, h2 );

        sep := sep + lambda*x^k;
    od;
    # Test separatrice for x = 0...2*epsilon

    if test_sep then
        writef( "Testing separatrix for x > 0\n" );
        ok := separatrice_test( sep, g, n, stable, 1 );
        n1 := n;
        while not(ok) and n1 < max_level do
            n1 := n1 + 1;
            if n1 >= numeric_level then if n1 = numeric_level then vf1 := evalf(vf1); vf2 := evalf(vf2) fi;
                lambda := evalf( coeff(collect( subs(y=0,vf2),x),x,n1) / (L1*n1-L2) );
                h1 := expand( subs( y=_y+lambda*x^n1,vf1) );
                h2 := expand( subs( y=_y+lambda*x^n1,vf2) - lambda*n1*x^(n1-1)*h1);
                vf1 := subs( _y=y, h1 );
                vf2 := subs( _y=y, h2 );
                sep := sep + lambda*x^n1;
            else
                lambda := coeff(collect( subs(y=0,vf2),x),x,n1) / (L1*n1-L2);
                if not rounded and user_simplify then lambda := user_simplifycmd(lambda) fi;
                h1 := expand( subs( y=_y+lambda*x^n1,vf1) );
                h2 := expand( subs( y=_y+lambda*x^n1,vf2) - lambda*n1*x^(n1-1)*h1);
                vf1 := subs( _y=y, h1 );
                vf2 := subs( _y=y, h2 );
                sep := sep + lambda*x^n1;
            fi;
            ok := separatrice_test( sep, g, n1, stable, 1 );
        od;
    fi;

    # Test separatrice for x=-2epsilon..0

    if test_sep then
        writef( "Testing separatrix for x < 0\n" );
        ok := separatrice_test( sep, g, n1, stable, -1 );
        while not(ok) and n1 < max_level do
            n1 := n1 + 1;
            if n1 >= numeric_level then if n1 = numeric_level then vf1 := evalf(vf1); vf2 := evalf(vf2) fi;
                lambda := evalf( coeff(collect( subs(y=0,vf2),x),x,n1) / (L1*n1-L2) );
                h1 := expand( subs( y=_y+lambda*x^n1,vf1) );
                h2 := expand( subs( y=_y+lambda*x^n1,vf2) - lambda*n1*x^(n1-1)*h1);
                vf1 := subs( _y=y, h1 );
                vf2 := subs( _y=y, h2 );
                sep := sep + lambda*x^n1;
            else
                lambda := coeff(collect( subs(y=0,vf2),x),x,n1) / (L1*n1-L2);
                if not rounded and user_simplify then lambda := user_simplifycmd(lambda) fi;
                h1 := expand( subs( y=_y+lambda*x^n1,vf1) );
                h2 := expand( subs( y=_y+lambda*x^n1,vf2) - lambda*n1*x^(n1-1)*h1);
                vf1 := subs( _y=y, h1 );
                vf2 := subs( _y=y, h2 );
                sep := sep + lambda*x^n1;
            fi;
            ok := separatrice_test( sep, g, n1, stable, 1 );
        od;
    fi;
    subs( x=t, sep );
end:
center_manifold := proc( g, n, stable )
    local vf1, vf2, h1, h2, k, L2, sep, _y, lambda, ok, n1, dx, b, m, am, stype, stable2;
    global test_sep;

    if rounded then vf1 := evalf(g[1]); vf2 := evalf(g[2]); else vf1 := g[1]; vf2 := g[2]; fi;
    L2 := coeff(collect(subs(x=0,g[2]),y),y,1);
    sep := 0; n1 := n;
    for k from 2 to n1 do
        lambda := - coeff(collect( subs(y=0,vf2),x),x,k) / L2;
        if not rounded and user_simplify then lambda := user_simplifycmd(lambda) fi;
        h1 := expand( subs( y=_y+lambda*x^k,vf1) );
        h2 := expand( subs( y=_y+lambda*x^k,vf2) - lambda*k*x^(k-1)*h1);
        vf1 := subs( _y=y, h1 );
        vf2 := subs( _y=y, h2 );
        sep := sep + lambda*x^k;
        dx := optimizepolynomial1( subs( y=0, vf1 ), x ); if dx = 0 then n1 := n1 + 1 fi
    od;
    # calculate type
    b := first_term(dx,x);
    m := degree(b,x);
    am := evalf(subs(x=1,b));
    if type(m,even) then
        if am >= 0 then if stable=0 then stype := 1 else stype := 2 fi;  # saddle-node
        else if stable=0 then stype := 3 else stype := 4; fi fi # saddle-node
    else
        if am >= 0 then if stable=0 then stype := 5 else stype := 6 fi
        else if stable=0 then stype := 7 else stype := 8 fi fi
    fi;

    # Test separatrice for x = 0...2*epsilon
    if test_sep then
        if member(stype,{2,6}) then stable2:=0 else stable2:=1; fi;
        if member(stype,{2,6,3,7}) then
            writef( "Testing center separatrix for x > 0\n" );
            ok := separatrice_test( sep, g, n1, stable2, 1 );
            while not(ok) and n1 < max_level do
                n1 := n1 + 1;
                if n1 >= numeric_level then
                    if n1 = numeric_level then vf1 := evalf(vf1); vf2 := evalf(vf2) fi;
                    lambda := evalf( -coeff(collect( subs(y=sep,vf2),x),x,n1) / L2 );
                    h1 := expand( subs( y=_y+lambda*x^n1,vf1) );
                    h2 := expand( subs( y=_y+lambda*x^n1,vf2) - lambda*n1*x^(n1-1)*h1);
                    vf1 := subs( _y=y, h1 );
                    vf2 := subs( _y=y, h2 );
                    sep := sep + lambda*x^n1;
                else
                    lambda := - coeff(collect( subs(y=sep,vf2),x),x,n1) / L2;
                    if not rounded and user_simplify then lambda := user_simplifycmd(lambda) fi;
                    h1 := expand( subs( y=_y+lambda*x^n1,vf1) );
                    h2 := expand( subs( y=_y+lambda*x^n1,vf2) - lambda*n1*x^(n1-1)*h1);
                    vf1 := subs( _y=y, h1 );
                    vf2 := subs( _y=y, h2 );
                    sep := sep + lambda*x^n1;
                fi;
                ok := separatrice_test( sep, g, n1, stable2, 1 );
            od;
        end if;
    fi;

    # Test separatrice for x=-2epsilon..0

    if test_sep then
        if member( stype,{4,6}) then stable2:=0 else stable2:=1; fi;
        if member( stype,{4,6,1,7}) then
            writef( "Testing center separatrix for x < 0\n" );
            ok := separatrice_test( sep, g, n1, stable2, -1 );
            while not(ok) and n1 < max_level do
                n1 := n1 + 1;
                if n1 >= numeric_level then if n1 = numeric_level then vf1 := evalf(vf1); vf2 := evalf(vf2) fi;
                    lambda := evalf( -coeff(collect( subs(y=sep,vf2),x),x,n1) / L2 );
                    h1 := expand( subs( y=_y+lambda*x^n1,vf1) );
                    h2 := expand( subs( y=_y+lambda*x^n1,vf2) - lambda*n1*x^(n1-1)*h1);
                    vf1 := subs( _y=y, h1 );
                    vf2 := subs( _y=y, h2 );
                    sep := sep + lambda*x^n1;
                else
                    lambda := -coeff(collect( subs(y=sep,vf2),x),x,n1) / L2;
                    if not rounded and user_simplify then lambda := user_simplifycmd(lambda) fi;
                    h1 := expand( subs( y=_y+lambda*x^n1,vf1) );
                    h2 := expand( subs( y=_y+lambda*x^n1,vf2) - lambda*n1*x^(n1-1)*h1);
                    vf1 := subs( _y=y, h1 );
                    vf2 := subs( _y=y, h2 );
                    sep := sep + lambda*x^n1;
                fi;
                ok := separatrice_test( sep, g, n1, stable2, 1 );
            od;
        end if;
     end if;

    dx := optimizepolynomial1( subs( y=sep, vf1 ), x );
    [subs( x=t, sep ), dx, stype ];
end:
separatrice_test := proc( sep, vf, n, stable, dir )
    local ok, rndnum, exefile, sepfile, k;
    global user_exeprefix, user_tmpdir, user_lypexe, user_bindir, user_removecmd, epsilon;

    exefile := cat( user_bindir, user_sepexe );
    rndnum := irem(rand(),1000);
    sepfile := cat( user_tmpdir, "P4SEP_", rndnum, ".TMP" );
    openfile( sepfile );
    writef( "%g\n", epsilon );
    writef( "%d\n", dir );
    writef( "%d\n", stable );
    write_vec_field( vf );
    writef( "%d\n", n-1 );
    for k from 2 to n do
        writef( "%d %g ", k, coeff(sep,x,k) );
    end do;
    writef( "\n" );
    closefile( sepfile );
    writef( "Separatrix test (n=%d)... ", n );
    ok := ssystem( cat( user_exeprefix, exefile, " \"", sepfile, "\"" ) );
    ssystem( cat( user_removecmd, " \"", sepfile, "\"" ) );
    if ok[1] = 0 then
        writef( "not ok\n" );
        false;
    else
        writef( "ok\n" );
        true;
    end if;
end:
save( jacobian, eigenvector, translation, transformation, normalization, manifold,
      center_manifold, saddle, strong_focus, node, degenerate, semi_elementary,
      sing_type, separatrice_test,
      "type.m" );
