#
# This file was automatically generated by MPLSTRIP.
# Source file: writelog.tex
# Time stamp on source file: Wed Jun 13 11:03:01 2018
#
restart;
read( "tools.m" );

coeff_degree1 := proc( f, x )
    local g, d, j, L;
    g := optimizepolynomial1( f, x );
    d := max(0,degree(g,x)); L := [];
    for j from 0 to d do
        if coeff(g,x,j) <> 0 then L := [op(L), [j,coeff(g,x,j)]] end if;
    end do;
    if nops(L) = 0 then L := [[0,0]] end if;
    L
end proc:

coeff_degree2 := proc( f, x, y )
    local g, d, j, L, dx, h, k;
    global rounded;
    g := optimizepolynomial2( f, x, y );
    dx := max(0,degree(g,x)); L := [];
    for j from 0 to dx do
        h := coeff( g, x, j );
        d := max(0,degree(g,y));
        for k from 0 to d do
            if coeff(h,y,k) <> 0 then L := [op(L), [j,k,coeff(h,y,k)]] end if
        end do
    end do;
    if nops(L) = 0 then L := [[0,0,0]] end if;
    L
end proc:

coeff_degree3 := proc( f, x, y, z )
    local g, d1, d2, d3, j1, g1, g2, j2, j3, L;
    global rounded;
    g := norpoly3( f, x,y,z );
    d1 := max(0,degree(g,x)); L := [];
    for j1 from 0 to d1 do
        g1 := optimizepolynomial2(coeff(g,x,j1),y,z);
        d2 := max(0,degree(g1,y));
        for j2 from 0 to d2 do
            g2 := coeff(g1,y,j2);
            d3 := max(0,degree(g2,z));
            for j3 from 0 to d3 do
                if coeff(g2,z,j3) <> 0 then L := [op(L), [j1,j2,j3,coeff(g2,z,j3)]] end if
            end do
        end do
    end do;
    if nops(L)=0 then L := [[0,0,0,0]] end if;
    L;
end proc:
write_vec_field := proc( vf )
    local L, j;
    L := coeff_degree2( vf[1], x, y );
    writef( "%d\n", nops(L) );
    for j from 1 to nops(L) do
        writef( "%d %d %g\n", op(1,L[j]), op(2,L[j]), evalf(op(3,L[j])) );
    end do;
    L := coeff_degree2( vf[2], x, y );
    writef( "%d\n", nops(L) );
    for j from 1 to nops(L) do
        writef( "%d %d %g\n", op(1,L[j]), op(2,L[j]), evalf(op(3,L[j])) );
    end do;
end proc:
write_vec_field_cylinder := proc( vf )
    local L, j;
    L := coeff_degree3( vf[1], r, _co, _si );
    writef( "%d\n", nops(L) );
    for j from 1 to nops(L) do
        writef( "%d %d %d %g\n", op(1,L[j]), op(2,L[j]), op(3,L[j]), evalf(op(4,L[j])) );
    end do;
    L := coeff_degree3( vf[2], r, _co, _si );
    writef( "%d\n", nops(L) );
    for j from 1 to nops(L) do
        writef( "%d %d %d %g\n", op(1,L[j]), op(2,L[j]), op(3,L[j]), evalf(op(4,L[j])) );
    end do;
end proc:

write_saddle := proc( x0, y0, a, b, c, d, vf, sep, stype, chart )
    local L, j, k; openfile( log_file );
    writef( "%d %g %g\n", 1, evalf(x0), evalf(y0) );
    writef( "  %g %g %g %g\n", evalf(a), evalf(b), evalf(c), evalf(d) );
    write_vec_field( vf );
    writef( "  %d\n", chart );
    for j from 1 to nops(sep) do
        writef( "  %d ", stype[j] );
        L := coeff_degree1( sep[j], t );
        writef( "%d", nops( L ) );
        for k from 1 to nops(L) do
             writef( " %d %g", op(1, L[k]), op(2, L[k]) )
        end do;
        writef( "\n" );
    end do;
    writef( "  %g\n", epsilon ); openfile( terminal );
end proc:

write_node := proc( x0, y0, stab, chart ) openfile( log_file );
    writef( "%d %g %g\n", 2, evalf(x0), evalf(y0) );
    writef( "  %d %d\n", stab, chart ); openfile( terminal );
end proc:

write_weak_focus := proc( x0, y0, chart, stype ) openfile( log_file );
    writef( "%d %g %g\n", 3, evalf(x0), evalf(y0) );
    writef( "  %d %d\n", stype, chart ); openfile( terminal );
end proc:

write_strong_focus := proc( x0, y0, stab, chart ) openfile( log_file );
    writef( "%d %d %g %g\n", 4, stab, evalf(x0), evalf(y0) );
    writef( "  %d\n", chart ); openfile( terminal );
end proc:


write_semi_elementary := proc( x0, y0, a, b, c, d, vf, stype, sep, chart )
    local L, j, k; openfile( log_file );
    writef( "%d %g %g\n", 5, evalf(x0), evalf(y0) );
    writef( "  %g %g %g %g\n", evalf(a), evalf(b), evalf(c), evalf(d) );
    write_vec_field( vf );
    writef( "  %d %d %d\n", stype[1], stype[2], chart );
    if ((chart=0) and member(stype[1],{1,2,3,4,6,7})) or
       ((chart <> 0) and (stype[2]=0) and member(stype[1],{1,2,3,4,6,7})) or
       ((chart <> 0) and (stype[2]=1) and member(stype[1],{2,3,6,7})) then
        for j from 1 to nops(sep) do
            L := coeff_degree1( sep[j], t );
            writef( "  %d", nops(L) );
            for k from 1 to nops(L) do
                writef( " %d %g", op(1,L[k]), op(2,L[k]) );
            end do;
            writef( "\n" );
        end do;
    end if;
    writef( "  %g\n", epsilon ); openfile( terminal );
end proc:

write_degenerate := proc( x0, y0, trajectors, chart )
    local j;
    openfile( log_file );
    writef( "%d %g %g\n", 6, evalf(x0), evalf(y0) );
    writef( "%g\n", epsilon );
    writef( "%d\n", nops(trajectors) );
    for j from 1 to nops(trajectors) do
        write_separatrice( trajectors[j] );
    end do;
    writef( "%d\n", chart );
    openfile( terminal );
end proc:

write_separatrice := proc( sep )
    local trans, j, k, p2, L, a;
    trans := sep[1];
    writef( "%d", nops(trans) );
    for j from nops(trans) to 1 by -1 do
        a := trans[j];
        for k from 1 to 2 do writef( " %g", evalf(a[k]) ) end do;
        for k from 3 to 9 do writef( " %d", a[k] ) end do;
        writef( "\n" );
    end do;
    p2 := sep[2];
    writef( "%g %g\n", evalf(p2[1]), evalf(p2[2]) );
    trans := sep[3];
    writef( "%g %g %g %g\n", evalf( trans[1] ), evalf( trans[2] ), evalf( trans[3] ), evalf( trans[4] ) );
    write_vec_field( sep[4] );
    L := coeff_degree1( sep[5], t );
    writef( "%d", nops(L) );
    for k from 1 to nops(L) do
        writef( " %d %g", op(1,L[k]), op(2,L[k]) );
    end do;
    writef( "\n" );
    writef( "%d\n", sep[6] );
end proc:

write_gcf := proc( gcff )
    local j, p, q, z1, z2, s, _co, _si, d, L, g;
    global user_p, user_q;
    openfile(vec_table);
    d := ddeg( gcff, x, y );
    if d = 0 then
        writef( "%d\n", 0 )
    else
        L := coeff_degree2( gcff, x, y );
        writef( "%d %d", d, nops(L) );
        for j from 1 to nops(L) do
            writef( " %d %d %g", op(1, L[j]), op(2, L[j]), evalf( op(3,L[j]) ) );
        end do;
        writef( "\n" );
        p := user_p; q := user_q;
        g := subs( x = 1/z2^p, y = z1/z2^q, gcff );
        g := norpoly2( g, z1, z2 );
        g := numer( g );
        L := coeff_degree2( g, z1, z2 );
        writef( "%d", nops(L) );
        for j from 1 to nops(L) do
            writef( " %d %d %g", op(1, L[j]), op(2, L[j]), evalf( op(3,L[j]) ) );
        end do;
        writef( "\n" );

        g := subs( x = z1/z2^p, y = 1/z2^q, gcff );
        g := norpoly2( g, z1,z2 );
        g := numer( g );
        L := coeff_degree2( g, z1, z2 );
        writef( "%d", nops(L) );
        for j from 1 to nops(L) do
            writef( " %d %d %g", op(1, L[j]), op(2, L[j]), evalf( op(3,L[j]) ) );
        end do;
        writef( "\n" );

        g := subs( x = -1/z2^p, y = z1/z2^q, gcff );
        g := norpoly2( g, z1,z2 );
        g := numer( g );
        L := coeff_degree2( g, z1, z2 );
        writef( "%d", nops(L) );
        for j from 1 to nops(L) do
            writef( " %d %d %g", op(1, L[j]), op(2, L[j]), evalf( op(3,L[j]) ) );
        end do;
        writef( "\n" );

        g := subs( x = z1/z2^p, y = -1/z2^q, gcff );
        g := norpoly2( g, z1,z2 );
        g := numer( g );
        L := coeff_degree2( g, z1, z2 );
        writef( "%d", nops(L) );
        for j from 1 to nops(L) do
            writef( " %d %d %g", op(1, L[j]), op(2, L[j]), evalf( op(3,L[j]) ) );
        end do;
        writef( "\n" );

        if p <> 1 or q <> 1 then
            g := subs( x=_co/s^p, y = _si/s^q, gcff );
            g := norpoly3( g, s, _co, _si );
            g := numer( g );
            L := coeff_degree3( g, s, _co, _si );
            writef( "%d", nops(L) );
            for j from 1 to nops(L) do
                writef( " %d %d %d %g", op(1, L[j]), op(2, L[j]), op(3, L[j]), evalf( op(4,L[j]) ) );
            end do;
            writef( "\n" );
        end if;
    end if;
    openfile(terminal);
end proc:

write_curve := proc( curvef )
    local j, p, q, z1, z2, s, _co, _si, d, L, g;
    global user_p, user_q, rounded;#, curve_table;
    #openfile(curve_table);        
    d := ddeg( curvef, x, y );
    if d = 0 then
        writef( "%d\n", 0 )
    else
        L := coeff_degree2( curvef, x, y );
        writef( "%d %d", d, nops(L) );
        for j from 1 to nops(L) do
            writef( " %d %d %g", op(1, L[j]), op(2, L[j]), evalf( op(3,L[j]) ) );
        end do;
        writef( "\n" );
        p := user_p; q := user_q;
        g := subs( x = 1/z2^p, y = z1/z2^q, curvef );
        g := norpoly2( g, z1, z2 );
        g := numer( g );
        L := coeff_degree2( g, z1, z2 );
        writef( "%d", nops(L) );
        for j from 1 to nops(L) do
            writef( " %d %d %g", op(1, L[j]), op(2, L[j]), evalf( op(3,L[j]) ) );
        end do;
        writef( "\n" );

        g := subs( x = z1/z2^p, y = 1/z2^q, curvef );
        g := norpoly2( g, z1,z2 );
        g := numer( g );
        L := coeff_degree2( g, z1, z2 );
        writef( "%d", nops(L) );
        for j from 1 to nops(L) do
            writef( " %d %d %g", op(1, L[j]), op(2, L[j]), evalf( op(3,L[j]) ) );
        end do;
        writef( "\n" );

        g := subs( x = -1/z2^p, y = z1/z2^q, curvef );
        g := norpoly2( g, z1,z2 );
        g := numer( g );
        L := coeff_degree2( g, z1, z2 );
        writef( "%d", nops(L) );
        for j from 1 to nops(L) do
            writef( " %d %d %g", op(1, L[j]), op(2, L[j]), evalf( op(3,L[j]) ) );
        end do;
        writef( "\n" );

        g := subs( x = z1/z2^p, y = -1/z2^q, curvef );
        g := norpoly2( g, z1,z2 );
        g := numer( g );
        L := coeff_degree2( g, z1, z2 );
        writef( "%d", nops(L) );
        for j from 1 to nops(L) do
            writef( " %d %d %g", op(1, L[j]), op(2, L[j]), evalf( op(3,L[j]) ) );
        end do;
        writef( "\n" );

        if p <> 1 or q <> 1 then
            g := subs( x=_co/s^p, y = _si/s^q, curvef );
            g := norpoly3( g, s, _co, _si );
            g := numer( g );
            L := coeff_degree3( g, s, _co, _si );
            writef( "%d", nops(L) );
            for j from 1 to nops(L) do
                writef( " %d %d %d %g", op(1, L[j]), op(2, L[j]), op(3, L[j]), evalf( op(4,L[j]) ) );
            end do;
            writef( "\n" );
        end if;
    end if;
    #closefile(curve_table);
    #openfile(terminal);
end proc:

write_isoclines := proc( isoclinesf )
    local j, p, q, z1, z2, s, _co, _si, d, L, g;
    global user_p, user_q, isoclines_table;
    openfile(isoclines_table);
    d := ddeg( isoclinesf, x, y );
    if d = 0 then
        writef( "%d\n", 0 )
    else
        L := coeff_degree2( isoclinesf, x, y );
        writef( "%d %d", d, nops(L) );
        for j from 1 to nops(L) do
            writef( " %d %d %g", op(1, L[j]), op(2, L[j]), evalf( op(3,L[j]) ) );
        end do;
        writef( "\n" );
        p := user_p; q := user_q;
        g := subs( x = 1/z2^p, y = z1/z2^q, isoclinesf );
        g := norpoly2( g, z1, z2 );
        g := numer( g );
        L := coeff_degree2( g, z1, z2 );
        writef( "%d", nops(L) );
        for j from 1 to nops(L) do
            writef( " %d %d %g", op(1, L[j]), op(2, L[j]), evalf( op(3,L[j]) ) );
        end do;
        writef( "\n" );

        g := subs( x = z1/z2^p, y = 1/z2^q, isoclinesf );
        g := norpoly2( g, z1,z2 );
        g := numer( g );
        L := coeff_degree2( g, z1, z2 );
        writef( "%d", nops(L) );
        for j from 1 to nops(L) do
            writef( " %d %d %g", op(1, L[j]), op(2, L[j]), evalf( op(3,L[j]) ) );
        end do;
        writef( "\n" );

        g := subs( x = -1/z2^p, y = z1/z2^q, isoclinesf );
        g := norpoly2( g, z1,z2 );
        g := numer( g );
        L := coeff_degree2( g, z1, z2 );
        writef( "%d", nops(L) );
        for j from 1 to nops(L) do
            writef( " %d %d %g", op(1, L[j]), op(2, L[j]), evalf( op(3,L[j]) ) );
        end do;
        writef( "\n" );

        g := subs( x = z1/z2^p, y = -1/z2^q, isoclinesf );
        g := norpoly2( g, z1,z2 );
        g := numer( g );
        L := coeff_degree2( g, z1, z2 );
        writef( "%d", nops(L) );
        for j from 1 to nops(L) do
            writef( " %d %d %g", op(1, L[j]), op(2, L[j]), evalf( op(3,L[j]) ) );
        end do;
        writef( "\n" );

        if p <> 1 or q <> 1 then
            g := subs( x=_co/s^p, y = _si/s^q, isoclinesf );
            g := norpoly3( g, s, _co, _si );
            g := numer( g );
            L := coeff_degree3( g, s, _co, _si );
            writef( "%d", nops(L) );
            for j from 1 to nops(L) do
                writef( " %d %d %d %g", op(1, L[j]), op(2, L[j]), op(3, L[j]), evalf( op(4,L[j]) ) );
            end do;
            writef( "\n" );
        end if;
    end if;
    closefile(isoclines_table);
    openfile(terminal);
end proc:
save( coeff_degree1, coeff_degree2, coeff_degree3,
      write_vec_field, write_vec_field_cylinder,
      write_saddle, write_node, write_weak_focus,
      write_strong_focus, write_semi_elementary,
      write_separatrice, write_degenerate, write_gcf,
      write_curve, write_isoclines, "writelog.m" );
