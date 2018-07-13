#
# This file was automatically generated by MPLSTRIP.
# Source file: blowup.tex
# Time stamp on source file: Fri Jul 13 15:48:42 2018
#
restart;
read("tools.m");
read("writelog.m");
read("realroot.m");
assoc_points := proc(vf)
    local Lf, Lg, expr, w;

    Lf := coeff_degree2(evalf(vf[1]), x, y);
    Lf := map(expr -> [ expr[1]-1, expr[2] ], Lf);

    Lg := coeff_degree2(evalf(vf[2]), x, y);
    Lg := map(expr -> [ expr[1], expr[2]-1 ], Lg);

    convert({op(Lf),op(Lg)},list);
end proc:
newton_diagram := proc(vf)
    local points, pk, pj, pL, k, j, L, hset, N, w, p;

    hset := assoc_points(vf);
    points := hset;

    N := nops(points);

    for k from 1 to N-1 do
        pk := points[k];
        if member(pk, hset) then
            for j from k+1 to N do
                pj := points[j];
                if member(pj, hset) then
                    if pk[1] <= pj[1] and pk[2] <= pj[2] then
                       hset := remove(p -> p=pj, hset);
                    else
                        for L from 1 to N do
                            pL := points[L];
                            if member(pL, hset) and L <> k and L <> j then
                                if (pk[1] <= pL[1] and pk[2] <= pL[2]) or
                                    (pj[1] <= pL[1] and pj[2] <= pL[2]) then
                                    hset := remove(p -> p=pL, hset);
                                else
                                    if (pk[1] < pL[1] and pL[1] < pj[1]) or
                                        (pj[1] < pL[1] and pL[1] < pk[1]) then
                                        w := (pj[2]-pk[2])/(pj[1]-pk[1]);
                                        if w * (pL[1]-pj[1]) + pj[2] <= pL[2] then
                                            hset := remove(p -> p=pL, hset);
                                        end if
                                    end if
                                end if
                            end if
                        end do
                    end if
                end if
            end do
        end if
    end do;
    hset
end proc:
GAMMA_1 := proc(s)
    local L, p, q, r, k, sk, G;

    L := nops(s);
    p := s[1];
    q := s[2];

    if p[1] >= q[1] then
        p := s[2];
        q := s[1];
    end if;

    if L = 2 then
        G := [p,q]
    else
        r := s[3];
        if r[1] < p[1] then
            r := q;
            q := p;
            p := s[3]
        else
            if r[1] < q[1] then
                r := q;
                q := s[3]
            end if
       end if;

       for k from 4 to L do
            sk := s[k];
            if sk[1] < p[1] then
                r := q;
                q := p;
                p := sk
            else
                if sk[1] < q[1] then
                    r := q;
                    q := sk
                else
                    if sk[1] < r[1] then
                        r := sk
                    end if
                end if
            end if
        end do;
        if q[1] > 0 then
            G := [p,q]
        else
            G := [q,r]
        end if;
    end if;
    G
end proc:
quasi_degree := proc(s)
    local h,alpha,beta,d;

    h := - (op(2,s[2]) - op(2,s[1])) / (op(1,s[2]) - op(1,s[1]));
    alpha := numer(h);
    beta := denom(h);
    d := alpha*op(1,s[1]) + beta*op(2,s[1]);
    [ alpha, beta, d ];
end proc:
center_type1 := proc(f, x, L, dir)
    local s, g, m, b;

    g := first_term(f, x);
    m := max(0,degree(g, x));
    b := sign(evalf(subs(x=1,g)));
    if type(m, even) then
        s := [b,1,sign(evalf(L))]
    else
        s := [b,-1,sign(evalf(L))]
    end if;

    if dir=1 then
        if s = [-1,1,1] then return(10) end if;
        if s = [1,1,1] then return(9) end if;
        if s = [1,-1,1] then return(3) end if;
        if s = [-1,-1,1] then return(1) end if;
        if s = [-1,1,-1] then return(12) end if;
        if s = [1,1,-1] then return(11) end if;
        if s = [1,-1,-1] then return(2) end if;
        return(4)
    else
        if s = [-1,1,1] then return(9) end if;
        if s = [1,1,1] then return(10) end if;
        if s = [1,-1,1] then return(3) end if;
        if s = [-1,-1,1] then return(1) end if;
        if s = [-1,1,-1] then return(11) end if;
        if s = [1,1,-1] then return(12) end if;
        if s = [1,-1,-1] then return(2) end if;
        return(4)
    end if;
end proc:
center_type2 := proc(f, x, L, dir)
    local s, g, m, b;

    g := first_term(f, x);
    m := max(0,degree(g, x));
    b := sign(evalf(subs(x=1,g)));
    if type(m, even) then
        s := [b,1,sign(evalf(L))]
    else
        s := [b,-1,sign(evalf(L))]
    end if;

    if dir=1 then
        if s = [-1,1,1] then return(9) end if;
        if s = [1,1,1] then return(10) end if;
        if s = [1,-1,1] then return(3) end if;
        if s = [-1,-1,1] then return(1) end if;
        if s = [-1,1,-1] then return(11) end if;
        if s = [1,1,-1] then return(12) end if;
        if s = [1,-1,-1] then return(2) end if;
        return(4)
    else
        if s = [-1,1,1] then return(10) end if;
        if s = [1,1,1] then return(9) end if;
        if s = [1,-1,1] then return(3) end if;
        if s = [-1,-1,1] then return(1) end if;
        if s = [-1,1,-1] then return(12) end if;
        if s = [1,1,-1] then return(11) end if;
        if s = [1,-1,-1] then return(2) end if;
        return(4)
    end if;
end proc:
power := proc(a,b)
    if b = 0 then 1 else a^b end if;
end proc:
sort_set_pos := proc(s, y0, r)
    local a, k, L;

    a := [$1..nops(s)+1];
    k := 1;
    for L from 1 to nops(s) do
        if reduce_llt(y0, op(1, s[L])) and k=L then
            a[k] := [y0,r];
            k := k+1
        end if;

        a[k] := s[L];
        k := k+1;
    end do;
    if k=L then
        a[k] := [y0,r];
    end if;
    a;
end proc:
sort_set_neg := proc(s, y0, r)
    local a, k, L;

    a := [$1..nops(s)+1];
    k := 1;
    for L from 1 to nops(s) do
        if reduce_gt(y0, op(1, s[L])) and k=L then
            a[k] := [y0,r];
            k := k+1
        end if;

        a[k] := s[L];
        k := k+1;
    end do;
    if k=L then
        a[k] := [y0,r];
    end if;
    a;
end proc:
put_separatrice := proc(xpos, stype, transformations, pt, trans, vec_field, sep)
    local newentry;
    global NumXPos, NumXNeg, ManXPos, ManXNeg;

    newentry := [transformations,pt,trans,vec_field,sep,stype,rounded];

    if xpos=1 then
        NumXPos := NumXPos + 1;
        ManXPos := [ op(ManXPos), newentry ];
    else
        NumXNeg := NumXNeg + 1;
        ManXNeg := [ op(ManXNeg), newentry ];
    end if;
end proc:
make_transformation := proc(sep)
    local L, hist, pt, trans, s, x0, y0, x1 ,y1, a;

    hist := sep[1];
    pt := sep[2];
    trans := sep[3];
    s := sep[5];

    x0 := pt[1] + trans[1]*t + trans[2]*s;
    y0 := pt[2] + trans[3]*t + trans[4]*s;

    L := nops(hist);
    while L > 0 do
        a := hist[L];
        x1 := a[1] + a[3]*power(x0,a[5])*power(y0,a[6]);
        y1 := a[2] + a[4]*power(x0,a[7])*power(y0,a[8]);
        x0 := optimizepolynomial1(x1,t);
        y0 := optimizepolynomial1(y1,t);
        if (x0=0) and (y0=0) then
            L := 0
        else
            L := L-1
        end if
    end do;

    [x0,y0];
end proc:
append_trajectors := proc()
    local man1, man2, j;

    print("ManYNeg = ", ManYNeg);
    print("ManYPos = ", ManYPos);
    print("ManXNeg = ", ManXNeg);
    print("ManXPos = ", ManXPos);

    man1:= [];
    if NumberY <> 0 then
        man2 := make_transformation(ManYNeg);
        man1 := [op(man1), [op(ManYNeg),man2]];
    end if;

    for j from 1 to NumXPos do
        man2 := make_transformation(ManXPos[j]);
        man1 := [op(man1), [op(ManXPos[j]),man2]];
    end do;

    if NumberY <> 0 then
        man2 := make_transformation(ManYPos);
        man1 := [op(man1), [op(ManYPos),man2]];
    end if;

    for j from 1 to NumXNeg do
        man2 := make_transformation(ManXNeg[j]);
        man1 := [op(man1), [op(ManXNeg[j]),man2]];
    end do;

    man1
end proc:
split_trajectors := proc(sep)
    local sep_pos, sep_neg, j, s, g;
    sep_pos := [];
    sep_neg := [];
    for j from 1 to nops(sep) do
        s := op(8, sep[j]);
        if s[2] <> 0 then
            g := sign(evalf(subs(t=1, first_term(diff(s[2],t), t))));
            if g > 0 then
                sep_pos := [ op(sep_pos), sep[j] ];
            else
                sep_neg := [ op(sep_neg), sep[j] ];
            end if;
        end if;
    end do;
    [sep_pos, sep_neg];
end proc:
write_trajectors := proc(sep, x0, y0)
    local L, w1, s1, s2, j, el, hyp, w2, dl1, dl2, dl3, dl4, dl5;

    L := nops(sep);
    el := 0;
    hyp := 0;
    dl1 := "";
    dl2 := " ";
    dl3 := "";
    dl4 := "";
    dl5 := "";

    if L <> 0 then
        openfile(result_file, terminal);
        writef("\n\nThe invariant separatrices are: (t>0)\n");
        openfile(terminal);

        w1 := {1,3,5,7,9,10,13,14};  # unstable ones
        w2 := {2,4,6,8,11,12,15,16}; # stable ones
        s1 := op(6,sep[1]);
        writef(sprintf(" [%d]" ,s1));
        if member(s1, w1) then
            openfile(result_file, terminal);
            writef("[01]: %a : unstable\n", op(8,sep[1]));
            openfile(terminal);
            dl1 := cat(dl1, "   |    ");
            dl2 := cat(dl2,  "  ^  ");
            dl3 := cat(dl3, "   |    ");
        else
            openfile(result_file, terminal);
            writef("[01]: %a : stable\n", op(8,sep[1]));
            openfile(terminal);
            dl1 := cat(dl1, "   |    ");
            dl2 := cat(dl2,  "  v  ");
            dl3 := cat(dl3, "   |    ");
        end if;
        if member(s1, {2,3,5,6,10,12,13}) then
            dl4 := cat(dl4 ,  "-<-*");
        else
            if member(s1, {1,4,7,8,9,11,15}) then
            dl4 := cat(dl4,  "->-*");
            else
                dl4 := cat(dl4 , "****");
            end if;
        end if;
        if member(s1, {1,4,7,8,10,12,16}) then
            dl4 := cat(dl4, "-<- ");
        else
            if member(s1, {2,3,5,6,9,11,14}) then
            dl4 := cat(dl4, "->- ");
            else
                dl4 := cat(dl4, "*** ");
            end if;
        end if;
        dl5 := cat(dl5, sprintf("  %02d    ", 1));

        for j from 2 to L do
            s2 := op(6, sep[j]);
            if (member(s1,{1,7,10}) and member(s2, {2,6,12})) or
                (member(s1,{2,6,11}) and member(s2, {1,7,9})) then
                openfile(result_file, terminal);
                writef("hyperbolic sector\n");
                openfile(terminal);
                hyp := hyp + 1;
                dl2 := cat(dl2, "HYP");
            else
                if (member(s1,{3,5,9,14}) and member(s2,{4,8,11,15})) or
                    (member(s1,{4,8,12,16}) and member(s2,{3,5,10,13})) then
                    openfile(result_file, terminal);
                    writef("elliptic sector\n");
                    openfile(terminal);
                    el := el + 1;
                    dl2 := cat(dl2, "ELL");
                else
                    openfile(result_file, terminal);
                    writef("parabolic sector\n");
                    openfile(terminal);
                    dl2 := cat(dl2, "PAR");
                end if
            end if;

            s1 := s2;
            writef(sprintf(" [%d]" ,s1));

            if member(s1, w1) then
                openfile(result_file, terminal);
                writef("[%02d]: %a : unstable\n", j, op(8,sep[j]));
                openfile(terminal);
                dl1 := cat(dl1, "   |    ");
                dl2 := cat(dl2,  "  ^  ");
                dl3 := cat(dl3, "   |    ");
            else
                openfile(result_file, terminal);
                writef("[%02d]: %a : stable\n", j, op(8,sep[j]));
                openfile(terminal);
                dl1 := cat(dl1, "   |    ");
                dl2 := cat(dl2,  "  v  ");
                dl3 := cat(dl3, "   |    ");
            end if;
            if member(s1, {2,3,5,6,10,12,13}) then
                dl4 := cat(dl4 ,  "-<-*");
            else
                if member(s1, {1,4,7,8,9,11,15}) then
                dl4 := cat(dl4,  "->-*");
                else
                    dl4 := cat(dl4 , "****");
                end if;
            end if;
            if member(s1, {1,4,7,8,10,12,16}) then
                dl4 := cat(dl4, "-<- ");
            else
                if member(s1, {2,3,5,6,9,11,14}) then
                dl4 := cat(dl4, "->- ");
                else
                    dl4 := cat(dl4, "*** ");
                end if;
            end if;
            dl5 := cat(dl5, sprintf("  %02d    ", j));
        end do;

        s1 := op(6, sep[L]);
        s2 := op(6, sep[1]);

        if (member(s1,{1,7,10}) and member(s2, {2,6,12})) or
            (member(s1,{2,6,11}) and member(s2, {1,7,9})) then
                openfile(result_file, terminal);
                writef("hyperbolic sector\n");
                openfile(terminal);
                hyp := hyp + 1;
                dl2 := cat(dl2, "HYP");
        else
            if (member(s1,{3,5,9}) and member(s2,{4,8,11})) or
                (member(s1,{4,8,12}) and member(s2,{3,5,10})) then
                openfile(result_file, terminal);
                writef("elliptic sector\n");
                openfile(terminal);
                el := el + 1;
                dl2 := cat(dl2 , "ELL");
            else
                openfile(result_file, terminal);
                writef("parabolic sector\n");
                openfile(terminal);
                dl2 := cat(dl2, "PAR");
            end if
        end if
    end if;

    openfile(result_file, terminal);
    writef("The singularity (%a,%a) has index %a\n", x0, y0, 1+(el-hyp)/2);
    writef("Sector representation (counterclockwise):\n");
    writef(dl1); writef("\n");
    writef(dl2); writef("\n");
    writef(dl3); writef("\n");
    writef(dl4); writef("\n");
    writef(dl5); writef("\n\n");
    openfile(terminal);
end proc:
manifold_blow_up := proc(_g,n)
    local lambda1, lambda2, g, f, a, dfx, k, h;

    g := optimizevf(_g,x,y);
    lambda1 := coeff(coeff(g[1], x, 1), y, 0);
    lambda2 := coeff(coeff(g[2], y, 1), x, 0);

    f := add(a[k]*x^k, k=2..n);
    dfx := add(k*a[k]*x^(k-1), k=2..n);

    h := expand(subs(y=f, g[2] - lambda2*y) - dfx * subs(y=f, g[1]-lambda1*x));

    for k from 2 to n do
        h := eval(h, a[k] = coeff(h,x,k)/(lambda1*k - lambda2));
        f := eval(f,a[k] = coeff(h,x,k)/(lambda1*k - lambda2));
    end do;

    return subs(x=t,f);
end proc:

manifold2_blow_up := proc(g,n)
    local L1,L2,f,dfx,k,i,h,a,n1;

    L1:=coeff(coeff(g[1],x,1),y,0);
    L2:=coeff(coeff(g[2],y,1),x,0);
    f := add(a[k]*x^k, k=2..n);
    dfx := add(k*a[k]*x^(k-1), k=2..n);
    h := subs(y=f, g[2] - L2*y) - dfx*subs(y=f, g[1] - L1*x);
    k := 2:
    n1 := n;
    while k < n+1 do
        if reduce_nneq(k*L1-L2,0) then
            h :=  eval(h, a[k] = coeff(h,x,k)/(k*L1-L2));
            f :=  eval(f, a[k] = coeff(h,x,k)/(k*L1-L2));
            k := k+1;
        else
            n1 := k-1;
            k := n+1;
        end if;
    end do;
    add(coeff(f,x,i)*t^i, i=2..n1);
end proc:

center_manifold_blow_up := proc(g, n)
    local vf1, vf2, h1, h2, k, L2, sep, _y, lambda, ok, n1, dx, b, m, am;

    if rounded then vf1 := evalf(g[1]); vf2 := evalf(g[2]); else vf1 := g[1]; vf2 := g[2]; fi;
    L2 := coeff(collect(subs(x=0,g[2]),y),y,1);
    sep := 0; n1 := n;
    for k from 2 to n1 do
        lambda := - coeff(collect(subs(y=0,vf2),x),x,k) / L2;
        if not rounded and user_simplify then lambda := user_simplifycmd(lambda) fi;
        h1 := expand(subs(y=_y+lambda*x^k,vf1));
        h2 := expand(subs(y=_y+lambda*x^k,vf2) - lambda*k*x^(k-1)*h1);
        vf1 := subs(_y=y, h1);
        vf2 := subs(_y=y, h2);
        sep := sep + lambda*x^k;
        dx := optimizepolynomial1(subs(y=0, vf1), x); if dx = 0 then n1 := n1 + 1 fi
    od;
    dx := optimizepolynomial1(subs(y=0, vf1), x);
    [ sep, dx, first_term(dx,x) ];
end:
ManXPos := [];
ManYPos := [];
ManXNeg := [];
ManYNeg := [];
NumXPos := 0;
NumXNeg := 0;
NumberY := 0;

blowing_up := proc(vf, x0, y0, finite_point)
    local r, vf2, n_d, g1, q_degree, z, trajectors;
    global ManXPos, ManXNeg, ManYPos, ManYNeg,
           NumXPos, NumXNeg, NumberY;

    if save_all then openfile(result_file, terminal) end if;
    writef("blowing up %a in point (%a,%a).\n", vf, x0, y0);
    openfile(terminal);

    if x0 <> 0 or y0 <> 0 then
        vf2 := optimizevf(translation(vf, x, y, x0, y0), x, y);
        if save_all then openfile(result_file, terminal) end if;
        writef("moving (%a,%a) to the origin.\n", x0, y0);
        writef("new vector field: %a\n", vf2);
        openfile(terminal);
    else
        vf2 := optimizevf(vf, x, y);
    end if;

    #calculate newton_diagram (in symbolic mode)

    n_d := newton_diagram(vf2);
    g1 := GAMMA_1(n_d);
    q_degree := quasi_degree(g1);

    if save_all then openfile(result_file, terminal) end if;
    writef("Newton diagram: %a\n", n_d);
    writef("g1 = %a\n", g1);
    writef("[alpha,beta,degree] = %a\n", q_degree);
    writef("\n");
    writef("1. Blowing up in the positive x-direction\n");
    writef("-----------------------------------------\n");
    writef("\n");
    writef("blowing up level 1\n");
    writef("------------------\n");
    writef("blowing up %a in point (0,0)\n", vf2);
    openfile(terminal);

    z:=blow_upx(vf2,q_degree,1,x,y);
    if save_all then openfile(result_file, terminal) end if;
    writef("blowing up vectorfield %a\n", z[1]);
    openfile(terminal);

    sing_eigenv(z, 1, 1, [[ x0, y0, 1, 1, q_degree[1], 0,
                    q_degree[2], 1, q_degree[3] ]], 1);

    if save_all then openfile(result_file, terminal) end if;
    writef("---------------------------------------------------------\n");
    writef("\n");
    writef("2. Blowing up in the negative x-direction\n");
    writef("-----------------------------------------\n");
    writef("\n");
    writef("\n");
    writef("blowing up level 1\n");
    writef("------------------\n");
    writef("blowing up %a in point (0,0)\n", vf2);
    openfile(terminal);

    z:=blow_upx(vf2,q_degree,-1,x,y);

    if save_all then openfile(result_file, terminal) end if;
    writef("blowing up vectorfield %a\n", z[1]);
    openfile(terminal);

    sing_eigenv(z, -1, 1, [[ x0, y0, -1, 1, q_degree[1], 0,
                q_degree[2], 1, q_degree[3] ]], -1);

    # Is (0,pi/2) a singular point? LINE OF SING --> SEMI-EL POINT IS ALWAYS NORMALLY HYPERBOLIC, SO NO CONTACT POINT!

    if op(1, g1[1]) > -1 and z[3] <> 0 then
        if save_all then openfile(result_file, terminal) end if;
        writef("3. Blowing up in the positive y-direction\n");
        writef("-----------------------------------------\n");
        openfile(terminal);

        ManYPos := blow_upy(vf2, q_degree, 1, [ [ x0,y0,1,1,1,
                    q_degree[1],0,q_degree[2],q_degree[3] ]]);

        NumberY := 1;

        if save_all then openfile(result_file, terminal) end if;
        writef("\n");
        writef("4. Blowing up in the negative y-direction\n");
        writef("-----------------------------------------\n");
        openfile(terminal);

        ManYNeg := blow_upy(vf2, q_degree, -1, [ [ x0,y0,1,-1,1,
                    q_degree[1],0,q_degree[2],q_degree[3]]]);
    end if;

    trajectors:=append_trajectors();
    write_trajectors(trajectors, x0, y0);
    if not(finite_point) then
        trajectors:=split_trajectors(trajectors)
    end if;

    NumXPos := 0;
    NumXNeg := 0;
    NumberY := 0;
    ManXPos := [];
    ManXNeg := [];
    ManYPos := [];
    ManYNeg := [];

    trajectors;
end proc:
blow_upx := proc(vf, q, dir, x, y)
    local alpha, beta, d, vx, vy, pg, qg, tg, f, g, r, s, delta;

    alpha := q[1];
    beta := q[2];
    d := q[3];
    vx := 0;
    vy := 0;
    pg := 0;
    qg := 0;
    tg := 0;

    f := optimizepolynomial2(vf[1], x, y);
    while f <> 0 do
        g := nlterm(f, x, y);
        r := max(0,degree(g,x))-1;
        s := max(0,degree(g,y));
        delta := alpha*r + beta*s;
        vx := vx + dir * subs(x=dir,g)*x^(delta+1-d);
        vy := vy - dir * x^(delta-d) * beta*y*subs(x=dir,g);
        if delta = d then
            pg := pg + dir*subs(x=dir,g);
            tg := tg - dir*beta*y*subs(x=dir,g);
        end if;
        f := optimizepolynomial2(f-g, x, y);
    end do;

    f := optimizepolynomial2(vf[2], x, y);
    while f <> 0 do
        g := nlterm(f, x, y);
        r := max(0,degree(g,x));
        s := max(0,degree(g,y))-1;
        delta := alpha*r + beta*s;
        vy := vy + alpha*x^(delta-d)*subs(x=dir,g);
        if delta=d then
            qg := qg + subs(x=dir,g);
            tg := tg + alpha*subs(x=dir,g);
        end if;
        f := optimizepolynomial2(f-g, x, y);
    end do;

    [ [vx, vy], [pg,qg], tg ];
end proc:

blow_upy := proc(vec_field, q_degree, direction, transformations)
    local z, vf2, lambda1, lambda2, vec, g, man, sep;

    vf2 := transformation(vec_field, 0, 1, 1, 0);
    z := blow_upx(vf2, [ q_degree[2], q_degree[1], q_degree[3] ], direction, x, y);
    vf2 := transformation(z[1], 0, 1, 1, 0);

    if save_all then openfile(result_file, terminal) end if;
    writef("blowing up vector field %a\n", vf2);

    lambda1 := subs(y=0, op(1,z[2]));
    lambda2 := subs(y=0, diff(z[3], y));

    writef("(0,0) is a singular point with eigenvalues %a, %a.\n", lambda2, lambda1);
    openfile(terminal);

    # which type of singularity ?

    if reduce_eeq(lambda1,0) or reduce_eeq(lambda2,0) or reduce_gteq(0,lambda1*lambda2) then
        vec := subs(x=0, y=0, diff(op(2, z[1]),x)) / (lambda1-lambda2);
        g := transformation(z[1], 1, 0, vec, 1);

        if save_all then openfile(result_file, terminal) end if;
        writef("Transform this vector field so that the linear part is diagonal\n");
        writef("The new vector field is %a.\n", transformation(g, 0,1,1,0));
        openfile(terminal);

        if reduce_gt(lambda1,0) then
            sep := manifold_blow_up(g, taylor_level);
            if save_all then openfile(result_file, terminal) end if;
            writef("unstable separatrice x=%a", subs(t=y,sep));
            openfile(terminal);
            if reduce_nneq(lambda2,0) then
                man := [ transformations, [0,0], [vec,1,1,0], g, sep, 1, rounded ]
            else
                man := [ transformations, [0,0], [vec,1,1,0], g, sep,
                        center_type2(subs(y=0,vf2[1]),x,lambda1,direction), rounded ];
            end if
        else
            if reduce_gt(0, lambda1) then
                sep := manifold_blow_up(g, taylor_level);
                if save_all then openfile(result_file, terminal) end if;
                writef("stable separatrice x=%a\n",subs(t=y,sep));
                openfile(terminal);
                if reduce_nneq(lambda2,0) then
                    man := [ transformations, [0,0], [vec,1,1,0], g, sep, 2, rounded ]
                else
                    man := [ transformations, [0,0], [vec,1,1,0], g, sep,
                        center_type2(subs(y=0,vf2[1]),x,lambda1,direction), rounded ];
                end if
            else
                sep:=center_manifold_blow_up(g, taylor_level);
                if save_all then openfile(result_file, terminal) end if;
                writef("center manifold x=%a\n",subs(x=y,sep[1]));
                writef("on center manifold y'=%a\n",subs(x=y,sep[2]));
                openfile(terminal);
                if reduce_gt(lambda2,0) then
                    if subs(x=1,sep[-1]) > 0  then
                        man := [ transformations, [0,0], [vec,1,1,0], g, subs(x=t,sep[1]), 5, rounded ]
                    else
                        man := [ transformations, [0,0], [vec,1,1,0], g, subs(x=t,sep[1]), 6, rounded ]
                    end if
                else
                    if subs(x=1,sep[-1]) > 0 then
                        man := [ transformations, [0,0], [vec,1,1,0], g, subs(x=t,sep[1]), 7, rounded ]
                    else
                        man := [ transformations, [0,0], [vec,1,1,0], g, subs(x=t,sep[1]), 8, rounded ]
                    end if
                end if
            end if
        end if
    else
        if reduce_gt(lambda1,0) then
            if reduce_nneq(lambda1,lambda2) then
                vec := subs(x=0,y=0,diff(op(2,z[1]),x))/(lambda1-lambda2);
                g := transformation(z[1], 1,0,vec,1);
                sep := manifold2_blow_up(g, taylor_level);
                man := [ transformations, [0,0], [vec,1,1,0], g, sep, 3, rounded ];
            else
                man := [ transformations, [0,0], [0,1,1,0], z[1], 0, 3, rounded ]
            end if
        else
            if reduce_nneq(lambda1,lambda2) then
                vec := subs(x=0,y=0,diff(op(2,z[1]),x))/(lambda1-lambda2);
                g := transformation(z[1], 1,0,vec,1);
                sep := manifold2_blow_up(g, taylor_level);
                man := [ transformations, [0,0], [vec,1,1,0], g, sep, 4, rounded ]
            else
                man := [ transformations, [0,0], [0,1,1,0], z[1], 0, 4, rounded ];
            end if
        end if;
    end if;
    return man;
end proc;


sing_eigenv := proc(z, dir, lvl, transformations, xpos)
    local r,s,L,L1,L2,h,pt,vec,ff,trans,g,sep;

    if z[3] <> 0 then
        h := find_real_roots1(z[3], y);
        s := [];
        for L from 1 to nops(h[1]) do
            if xpos=1 then
                s := sort_set_pos(s, op(L, h[1]), false);
            else
                s := sort_set_neg(s, op(L, h[1]), false);
            end if;
        end do;
        for L from 1 to nops(h[2]) do
            if xpos=1 then
                s := sort_set_pos(s, op(L, h[2]), true);
            else
                s := sort_set_neg(s, op(L, h[2]), true);
            end if;
        end do;
        for L from 1 to nops(s) do
            pt := op(1, s[L]);
            L1 := eval(op(1,z[2]), y=pt);
            L2 := eval(diff(z[3], y), y=pt);
            if op(2,s[L]) then
                L1 := evalf(L1);
                L2 := evalf(L2);
            end if;

            if save_all then openfile(result_file, terminal) end if;

            if reduce_eeq(L1, 0) and reduce_eeq(L2, 0) then
                writef("(0,%a) is a non-elementary point\n", pt);
                openfile(terminal);
                blowing_up2(z[1], pt, lvl+1, transformations, xpos, false);
            else
                if reduce_eeq(L1, 0) or reduce_eeq(L2, 0) or reduce_gteq(0, L1*L2) then
                    vec := diff(op(2,z[1]),x);
                    vec := subs(x=0,y=pt,vec / (L1-L2));
                    if op(2,s[L]) then
                        vec := evalf(vec);
                    end if;
                    ff := translation(z[1], x, y, 0, pt);
                    if op(2,s[L]) then
                        ff := evalf(ff);
                    end if;
                    trans := [1,0,vec,1];
                    g := transformation(ff, 1,0,vec,1);
                    if op(2,s[L]) then
                        g := evalf(g);
                    end if;

                    writef("(0,%a) is a singular point with eigenvalues %a,%a\n", pt, L1, L2);
                    writef("moving this point to the origin\n");
                    writef("the new vector field is %a\n", ff);
                    writef("transform this vector field so that the linear part is diagonal\n");
                    writef("the new vector field is %a\n", g);

                    if reduce_gt(L1, 0) then
                        sep := manifold_blow_up(g, taylor_level);
                        writef("unstable separatrice y=%a\n", subs(t=x,sep));

                        if reduce_nneq(L2, 0) then
                            put_separatrice(xpos, 1, transformations, [0,pt], trans, g, sep);
                        else
                            put_separatrice(xpos, center_type1(subs(x=0, g[2]), y, L1, dir),
                                            transformations, [0,pt], trans, g, sep);
                        end if
                    else
                        if reduce_gt(0, L1) then
                            sep := manifold_blow_up(g, taylor_level);
                            writef("stable separatrice y=%a\n", subs(t=x,sep));
                            if reduce_nneq(L2, 0) then
                                put_separatrice(xpos, 2, transformations, [0,pt], trans, g, sep);
                            else
                                put_separatrice(xpos, center_type1(subs(x=0,g[2]),y,L1,dir),
                                                transformations, [0,pt], trans, g, sep);
                            end if
                        else
                            sep := center_manifold_blow_up(g, taylor_level);
                            writef("center manifold y=%a\n",sep[1]);
                            writef("on center manifold is x'=%a\n", sep[2]);
                            if reduce_gt(L2, 0) then
                                if sign(evalf(subs(x=1,sep[3]))) > 0 then
                                    put_separatrice(xpos, 5, transformations, [0,pt], trans, g, subs(x=t,sep[1]))
                                else
                                    put_separatrice(xpos, 6, transformations, [0,pt], trans, g, subs(x=t,sep[1]))
                                end if
                            else
                                if sign(evalf(subs(x=1,sep[3]))) > 0 then
                                    put_separatrice(xpos, 7, transformations, [0,pt], trans, g, subs(x=t,sep[1]))
                                else
                                    put_separatrice(xpos, 8, transformations, [0,pt], trans, g, subs(x=t,sep[1]))
                                end if
                            end if
                        end if
                    end if
                else
                    if reduce_gt(L1, 0) then
                        writef("(0,%a) is an unstable node with eigenvalues %a,%a\n", pt, L1, L2);

                        if reduce_nneq(L1, L2) then
                            vec := diff(op(2,z[1]), x) / (L1-L2);
                            vec := subs(x=0,y=pt,vec);
                            if op(2, s[L]) then
                                vec := evalf(vec)
                            end if;
                            ff := translation(z[1], x, y, 0, pt);
                            if op(2, s[L]) then
                                ff := evalf(ff);
                            end if;
                            trans := [1,0,vec,1];
                            g := transformation(ff, 1,0,vec,1);
                            sep := manifold2_blow_up(g, taylor_level);
                            put_separatrice(xpos, 3, transformations, [0,pt], trans, g, sep);
                        else
                            put_separatrice(xpos, 3, transformations, [0,pt], [1,0,0,1],
                                            translation(z[1],x,y,0,pt), 0);
                        end if
                    else
                        writef("(0,%a) is a stable node with eigenvalues %a,%a\n", pt, L1, L2);

                        if reduce_nneq(L1, L2) then
                            vec := diff(op(2,z[1]), x) / (L1-L2);
                            vec := subs(x=0,y=pt,vec);
                            if op(2, s[L]) then
                                vec := evalf(vec)
                            end if;
                            ff := translation(z[1], x, y, 0, pt);
                            if op(2, s[L]) then
                                ff := evalf(ff);
                            end if;
                            trans := [1,0,vec,1];
                            g := transformation(ff, 1,0,vec,1);
                            sep := manifold2_blow_up(g, taylor_level);
                            put_separatrice(xpos, 4, transformations, [0,pt], trans, g, sep);
                        else
                            put_separatrice(xpos, 4, transformations, [0,pt], [1,0,0,1],
                                            translation(z[1],x,y,0,pt), 0);
                        end if
                    end if
                end if
            end if
        end do
    else
        #x=0 is a line of singularities !!!
        if save_all then openfile(result_file, terminal) end if;

        writef("x=0 is a line of singularities - point is star-like\n");
        h := find_real_roots1(op(1,z[2]), y);
        s := [];
        for L from 1 to nops(h[1]) do
            if xpos=1 then
                s := sort_set_pos(s, op(L, h[1]), false);
            else
                s := sort_set_neg(s, op(L, h[1]), false);
            end if;
        end do;
        for L from 1 to nops(h[2]) do
            if xpos=1 then
                s := sort_set_pos(s, op(L, h[2]), true);
            else
                s := sort_set_neg(s, op(L, h[2]), true);
            end if;
        end do;
        writef("the non-elementary singular points (contact points) are:\n");
        for L from 1 to nops(h[1]) do
            writef("(0,%a)\n", op(L, h[1]));
        end do;
        for L from 1 to nops(h[2]) do
            writef("(0,%a)\n", op(L, h[2]));
        end do;
        openfile(terminal);
        for L from 1 to nops(s) do
            blowing_up2(z[1], op(1,s[L]), lvl+1, transformations, xpos, true);
        end do;
    end if;
    openfile(terminal);
end proc:
blowing_upy2 := proc(vf, q_degree, dir, transformations, xpos, sing)
    local vf2, z, sx, sy;

    vf2 := transformation(vf, 0,1,1,0);
    z := blow_upx(vf2, [ q_degree[2], q_degree[1], q_degree[3] ], dir, x, y);
    vf2 := transformation(z[1], 0,1,1,0);

    if save_all then openfile(result_file,terminal) end if;
    if dir=1 then
        writef("blowing up in the positive y-direction\n");
    else
        writef("blowing up in the negative y-direction\n");
    end if;
    writef("blowing up the vector field %a.\n", vf2);
    openfile(terminal);

    if sing = false then
        sx := sign(evalf(subs(x=1, first_term(subs(y=0, vf2[1]), x))));
        sy := sign(evalf(subs(y=1, first_term(subs(x=0, vf2[2]), y))));

        if sx=sy then
            if sx>0 then
                put_separatrice(xpos, 3, transformations, [0,0], [1,0,0,1], vf2, t);
            else
                put_separatrice(xpos, 4, transformations, [0,0], [1,0,0,1], vf2, t);
            end if
        end if
    else
        if first_term(subs(y=0, vf2[1]), x) = 0 then
            error("case not treated: star-like points should become non star-like after second blowup.");
        end if;
        sx := sign(evalf(subs(x=1, first_term(subs(y=0, vf2[1]), x))));

        if dir*xpos>0 then
            put_separatrice(xpos, 14-sx, transformations, [0,0], [1,0,0,1], vf2, t);
        else
            put_separatrice(xpos, 15-sx, transformations, [0,0], [1,0,0,1], vf2, t);
        end if
    end if
end proc:

blowing_up2 := proc(vf, pt, lvl, transformations, xpos, sing)
    local vf2, n_d, g1, q_degree, trf, z;

    if save_all then openfile(terminal, result_file) end if;
    writef("Blowing up level %d\n", lvl);
    writef("------------------\n");
    writef("blowing up %a in point (0,%a)\n", vf, pt);

    if reduce_nneq(pt, 0) then
        vf2 := translation(vf, x, y, 0, pt);
        writef("moving (0,%a) to the origin\n", pt);
        writef("new vector field %a\n", vf2);
    else
        vf2 := vf
    end if;

    n_d := newton_diagram(vf2);
    g1 := GAMMA_1(n_d);
    q_degree := quasi_degree(g1);

    writef("Newton diagram %a\n", n_d);
    writef("[alpha,beta,degree]=%a\n", q_degree);

    openfile(terminal);

    #first we blow up in the negative y-direction if xpos=1
    #else we blow up in the positive y-direction
    #we have to know what happens in the corner
    #only in the case that {X=0} is not a line of singularities

    if sing=false then
        if xpos=1 then
            trf := [0,pt,1,-1,1,q_degree[1],0,q_degree[2],q_degree[3]];
            blowing_upy2(vf2, q_degree, -1, [ op(transformations), trf ], xpos, false)
        else
            trf := [0,pt,1,1,1,q_degree[1],0,q_degree[2],q_degree[3]];
            blowing_upy2(vf2, q_degree, 1, [ op(transformations), trf ], xpos, false)
        end if
    else
        if xpos=1 then
            trf := [0,pt,1,-1,1,q_degree[1],0,q_degree[2],q_degree[3]];
            blowing_upy2(vf2, q_degree, -1, [ op(transformations), trf ], xpos, true)
        else
            trf := [0,pt,1,1,1,q_degree[1],0,q_degree[2],q_degree[3]];
            blowing_upy2(vf2, q_degree, 1, [ op(transformations), trf ], xpos, true)
        end if
    end if;
    if save_all then openfile(terminal, result_file) end if;
    writef("blowing up in the positive x direction\n");
    z := blow_upx(vf2, q_degree, 1, x, y);
    writef("blowing up vector field %a\n", z[1]);
    openfile(terminal);

    trf := [0,pt,1,1,q_degree[1],0,q_degree[2],1,q_degree[3]];
    sing_eigenv(z, 1, lvl, [ op(transformations), trf ], xpos);

    #now we blow up in the positive y direction if xpos=1
    #else we blow up in the negative y direction

    if sing=false then
        if xpos=1 then
            trf := [0,pt,1,1,1,q_degree[1],0,q_degree[2],q_degree[3]];
            blowing_upy2(vf2, q_degree, 1, [ op(transformations), trf ], xpos, false)
        else
            [0,pt,1,-1,1,q_degree[1],0,q_degree[2],q_degree[3]];
            blowing_upy2(vf2, q_degree, -1, [ op(transformations), trf ], xpos, false)
        end if;
    else
        if xpos=1 then
            trf := [0,pt,1,1,1,q_degree[1],0,q_degree[2],q_degree[3]];
            blowing_upy2(vf2, q_degree, 1, [ op(transformations), trf ], xpos, true)
        else
            [0,pt,1,-1,1,q_degree[1],0,q_degree[2],q_degree[3]];
            blowing_upy2(vf2, q_degree, -1, [ op(transformations), trf ], xpos, true)
        end if;
    end if;
    if save_all then openfile(terminal, result_file) end if;
    writef("---------------------------------------\n");
    writef("Back to level %d\n",lvl-1);
    openfile(terminal);
end proc:
save(assoc_points, newton_diagram, GAMMA_1, quasi_degree, center_type1,
    center_type2, power, sort_set_pos, sort_set_neg, put_separatrice,
    append_trajectors, split_trajectors, make_transformation, write_trajectors,
    manifold_blow_up, manifold2_blow_up, center_manifold_blow_up, blowing_up,
    blow_upx, blow_upy, sing_eigenv, blowing_upy2, blowing_up2,
    ManXPos, ManXNeg, NumXPos, NumXNeg, NumberY, ManYPos, ManYNeg,

    "blowup.m");
