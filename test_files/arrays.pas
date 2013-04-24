program arrays;
var
   tab : array [20..30] of integer;
   i : integer;
begin
   i := 0;
   while i < 10 do
   begin
      tab[i + 20] := i;
      i := i + 2;
   end;
   i := 0;
   while i < 10 do
   begin
      write(tab[i + 20]);
      i := i + 1;
   end;
end.
