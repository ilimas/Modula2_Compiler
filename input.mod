MODULE modula2;
VAR a:INTEGER;
VAR b:INTEGER;
PROCEDURE Sum(VAR k:INTEGER, VAR d:INTEGER);
BEGIN
IF d>0 THEN
k:=k+d;
d:=d-1;
Sum(k, d);
ELSE
a:=k;
END
END Sum
BEGIN
printstring("Vvedite_N:");
scanint(b);
a:=0;
Sum(a, b);
printstring("Summa_ryada=");
printint(a);
END modula2.

