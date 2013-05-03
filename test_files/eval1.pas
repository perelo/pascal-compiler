program eval1;
var
   a,b,tmp : integer;
begin
   a := read();
   b := read();
   tmp := a;
   a := b;
   b := tmp;
   write(a);
   write(b);
end.
