MODULE modula2;
VAR n:INTEGER;
VAR i:INTEGER;
VAR res:INTEGER;
BEGIN
printstring("Vvedite_chislo:");
scanint(n);
res:=1;
i:=1;
WHILE i<=n DO
res:=res*i;
i:=i+1;
END
printstring("Factorial=");
printint(res);
END modula2.
