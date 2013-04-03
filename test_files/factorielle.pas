program factorielle;

procedure p;
var
   a : integer;
begin
   a := read();
end;

function fact(var a, n : integer) : integer;
var c : integer;
begin
   if n = 0 then fact := 1
   else
      if n mod 2 = 0 then
      begin
         c := fact(a, n div 2);
         fact := c*c;
      end
      else fact := a* fact(a, n-1);
end;

begin
   write(fact(2,3));
end.
