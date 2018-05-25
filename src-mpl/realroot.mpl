#
# This file was automatically generated by MPLSTRIP.
# Source file: realroot.tex
# Time stamp on source file: Fri May 25 12:07:19 2018
#
# P4 Maple routines
# RealRoot.mw

# Routines to solve for roots of a polynomial, either numerically or symbolically.

# Saved as the library "realroot.m"
# Original file: "find_real_roots.red"

# Exported routines:
# - real_roots_numeric      Numerical solve for real roots of a univariate polynomial.  Does not count multiplicity.  Returns a list of real solutions (ordered).
# - find_real_roots1        First tries to solve exactly for roots of a univariate polynomial.  For those singularities not found exactly, the routine continues numerically.
#               Returns a list containing two lists, the first one contains all exact solutions, the second one contains all numeric solutions.
# - find_real_roots11       Solves a system of univariate polynomials.  Used to find singularities at infinity, in the case there is a line of singularities at infinity.
# - find_real_roots_numeric Numerical solve for singularities of a vector field.  Solves only for isolated singularities.  Returns a list of points.
# - find_real_roots     Same as find_real_roots1, but solves for singularities of vector fields.  Also prints information to standard output.


# Exported variables:
# - save_all            boolean.  Relevant when printing information.

restart;
read( "tools.m" );

real_roots_numeric := proc( f, x )
    local cplx, j, k, g;
    cplx := [fsolve( f, x, complex, fulldigits )];
    if cplx = [x=x] then cplx := []; end if;
    g := [];
    for j from 1 to nops(cplx) do
        if abs(Im(cplx[j])) <= 10^(-user_precision) then
            cplx[j] := Re(cplx[j]);
            for k from 1 to nops(g) do
                if reduce_eeq( cplx[j], g[k] ) then break fi
            od;
            if k > nops(g) then g := [op(g),cplx[j]] fi
        fi
    od;
    sort(g)
end:

find_real_roots1 := proc( f, x )
    local h, ge, gn, k, b;

    if rounded then
        ge := [];
        gn := real_roots_numeric(f,x);
    else
        _EnvExplicit := true; _EnvAllSolutions := true;
        _EnvTryHard := true;
        h := [solve( f, x )];
        if h = [x] then h := []; end if;
        ge := [];
        gn := [];
        for k from 1 to nops(h) do
            b := h[k];
            if has(b, RootOf) then
                b := evalf(b);
                if abs(Im(b)) <= 10^(-user_precision) then
                    b := Re(b);
                    gn := [op(gn),b];
                fi
            else
                if abs(evalf(Im(b))) <= 10^(-user_precision) then
                    b := Re(b);
                    ge := [op(ge),b];
                fi
            fi
        od
    fi;
    [convert(convert(ge,set),list),convert(convert(gn,set),list)]
end:

find_real_roots11 := proc( L, x )
    local f1, f2, f, g, ge, gn, ge2, gn2, j;

    f1 := optimizepolynomial1(L[1],x);
    f2 := optimizepolynomial1(L[2],x);
    if max(degree(f1,x),0) <> 0 then
        g := find_real_roots1(f1,x);
        f := f2;
    else if max(degree(f2,x),0) <> 0 then
        g := find_real_roots1(f2,x);
        f := f1;
    else
        g := [[],[]];
        f := 0;
    fi fi;
    ge := g[1]; gn := g[2]; ge2 := []; gn2 := [];
    for j from 1 to nops(ge) do
        if abs(evalf(subs(x=ge[j],f))) < 10^(-user_precision) then;
            ge2 := [op(ge2),ge[j]];
        fi;
    od;
    for j from 1 to nops(gn) do
        if abs(evalf(subs(x=gn[j],f))) < 10^(-user_precision) then;
            gn2 := [op(gn2),gn[j]];
        fi;
    od;
    [ge2,gn2];
end:





find_real_roots_numeric := proc( f, x, y )
    local g, h, j, k, a;
    _EnvExplicit := true;
    _EnvAllSolutions := true;
    _EnvTryHard := true;
    h := evalf( map( allvalues, [ solve( convert(f, set), {x,y} ) ] ) );
    g := [];
    for j from 1 to nops(h) do
        a := subs(h[j],[x,y]);
        if abs(Im(a[1])) < 10^(-user_precision) and abs(Im(a[2])) < 10^(-user_precision) then
            a := map(Re,a);
            for k from 1 to nops(g) do
                if abs( op(1,g[k])-a[1] ) < 10^(-user_precision) and
                    abs( op(2,g[k])-a[2] ) < 10^(-user_precision) then break fi
            od;
            if k > nops(g) then
                g := [op(g),a];   if nops(g)=1 then writef( "Numeric real singularities:\n" ) fi;
                writef( "  (%g,%g) -> { x' = %g, y' = %g }\n", a[1], a[2],
                    evalf(subs( { x=a[1], y=a[2] }, f[1] )),
                    evalf( subs( { x=a[1], y=a[2] }, f[2] )) );
        fi fi
    od;
    if nops(g)=0 then writef( "No numeric singularities are found.\n" ) fi; g
end:

find_real_roots := proc( f, x, y )
    local g, h, j, k, a, an, gn, s, sn, bn;
    _EnvExplicit := true;
    _EnvAllSolutions := true;
    _EnvTryHard := true;
    h := map( allvalues, [ solve( convert(f, set), {x,y} ) ] );
    g := []; gn := [];
    for j from 1 to nops(h) do
        a := subs(h[j],[x,y]); an := map(Re,evalf(a)); bn := map(Im,evalf(a));
        if abs(bn[1]) < 10^(-user_precision) and abs(bn[2]) < 10^(-user_precision) then
            for k from 1 to nops(gn) do
                if abs( op(1,gn[k])-an[1] ) < 10^(-user_precision) and
                    abs( op(2,gn[k])-an[2] ) < 10^(-user_precision) then break fi
            od;
            if k > nops(gn) then
                g := [op(g),a];
                gn := [op(gn),an];
        fi fi
    od;
    s := []; sn := [];
    for k from 1 to nops(gn) do
        if has( op(1,g[k]), RootOf ) or has( op(2,g[k]), RootOf ) then
            sn := [op(sn),gn[k]]
        else
            s := [op(s),g[k]]
        fi;
    od;

    for j from 1 to nops(s) do
        s[j][1]:=Re(s[j][1]):
        s[j][2]:=Re(s[j][2]):
    end do;


    if nops(s) = 0 then writef( "No symbolic real singularities are found.\n" )
        else writef( "Symbolic real singularities:\n" );
    fi;
    for k from 1 to nops( s ) do
        writef( "  (%g,%g) -> { x' = %g, y' = %g }\n", op(1,s[k]), op(2,s[k]),
               evalf( subs( {x=op(1,s[k]),y=op(2,s[k])}, f[1] )),
               evalf( subs( {x=op(1,s[k]),y=op(2,s[k])}, f[1] )) );
    od;
    if nops( sn ) = 0 then
        if nops(s) = 0 then writef( "No numeric real singularities are found.\n" );
        else writef( "No other numeric real singularities are found.\n" );
        fi
    else
        writef( "Numeric real singularities:\n" );
    fi;
    for k from 1 to nops( sn ) do
        writef( "  (%g,%g) -> { x' = %g, y' = %g }\n", op(1,sn[k]), op(2,sn[k]),
               evalf( subs( {x=op(1,sn[k]),y=op(2,sn[k])}, f[1] )),
               evalf( subs( {x=op(1,sn[k]),y=op(2,sn[k])}, f[1] )) );
    od;
    [ s, sn ];
end:

save_all := false:

save( save_all, real_roots_numeric, find_real_roots1, find_real_roots_numeric, find_real_roots, find_real_roots11,
      "realroot.m" );
