program types;
var
   i, j : integer;
   b : boolean;
   tab : array [0..10] of integer;
begin
   i := 3 * 4;
   b := true;
   tab[42 + i] := 2;
   tab[1] := 3;
   while b do
   begin
      if i < j then
         b := false
      else
         i := i + tab[1];
   end;
   write(i);
end.
