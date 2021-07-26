{
  Export list of TREEs
}
unit UserScript;

var
  sl: TStringList;

function Initialize: integer;
begin
  sl := TStringList.Create;
end;

function Process(e: IInterface): integer;
var
  s: string;
  model: string;
  texture: string; 
begin
  if Signature(e) <> 'TREE' then
    Exit;
  s := GetElementEditValues(e, 'Model\MODL');
  if s = '' then
    Exit;
  
  model := LowerCase(ExtractFileName(s));
  if model = '' then
    Exit;

  s := GetElementEditValues(e, 'ICON');
  if s = '' then
    Exit;
  
  texture := LowerCase(ExtractFileName(s));
  if texture = '' then
    Exit;

  sl.Add(model + ';' + texture);
end;

function Finalize: integer;
var
  fname: string;
begin
  fname := ProgramPath + 'Edit Scripts\trees.txt';
  AddMessage('Saving list to ' + fname);
  sl.SaveToFile(fname);
  sl.Free;
end;

end.
