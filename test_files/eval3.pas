program eval3;

function plus_un (x : integer) : integer;
begin
   plus_un := x + 1;
end;

function plus_deux (x : integer) : integer;
begin
   plus_deux := x + 2;
end;

begin
   write(plus_deux(plus_un(read())));
end.
