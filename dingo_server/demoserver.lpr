program demoserver;

uses
  classes, sysutils, blcksock, synsock;

const
  Timeout = 30000;
  Port = 7890;

var
  u, sign: TUDPBlockSocket;
  buf: array[0..65535] of byte;
  ActiveSin: TVarSin;
  activetime: QWord;
  s: Integer;
begin
  u:=TUDPBlockSocket.Create;
  u.Bind('0.0.0.0', inttostr(port));

  sign:=TUDPBlockSocket.Create;
  sign.connect('gw.sommerhack.dk', '10000');

  activetime:=0;

  while true do
  begin
    s:=u.RecvBufferFrom(@buf[0], length(buf));
    if s>0 then
    begin
      if not CompareMem(@u.remotesin, @activesin, sizeof(ActiveSin)) then
      begin
        if (gettickcount64-activetime)>Timeout then
        begin
          move(u.remotesin, activesin, sizeof(activesin));
          activetime:=GetTickCount64;
        end;
      end;

      if CompareMem(@u.remotesin, @activesin, sizeof(ActiveSin)) then
      begin
        sign.SendBuffer(@buf[0], s);
      end;
    end;
  end;

  sign.free;
  u.Free;
end.

