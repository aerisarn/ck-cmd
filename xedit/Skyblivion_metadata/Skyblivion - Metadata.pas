// Apply metadata meshes adjsutments

unit SkyblivionMetadata;

const
  sMetadataFile = 'skyblivion.metadata';

var
  slMeta, slv: TStringList;

//============================================================================
function Initialize: integer;
var
  slFile, sl: TStringList;
  i: integer;
  s: string;
begin
  slMeta := TStringList.Create;
  slv := TStringList.Create;
  slv.Delimiter := ' ';
  //slv.StrictDelimiter := True;

  slFile := TStringList.Create;
  // reading metadata file
  slFile.LoadFromFile(ScriptsPath + sMetadataFile);
  for i := 0 to Pred(slFile.Count) do begin
    s := Trim(slFile[i]);
    if s = '' then Continue;
    // new mesh
    if (s[1] = '[') and (s[length(s)] = ']') then begin
      sl := TStringList.Create;
      slMeta.AddObject(LowerCase(Copy(s, 2, length(s) - 2)), sl);
    end
    // new command
    else
      if Assigned(sl) then
        sl.Add(s);
  end;
  slFile.Free;
end;

//============================================================================
function Process(e: IInterface): integer;
var
  s: string;
  i, idx: integer;
  x, y, z, rx, ry, rz, scale: double;
  sl: TStringList;
begin
  if Signature(e) <> 'REFR' then
    Exit;

  s := GetElementEditValues(BaseRecord(e), 'Model\MODL');
  if s = '' then
    Exit;
  
  s := LowerCase(ExtractFileName(s));
  if s = '' then
    Exit;

  idx := slMeta.IndexOf(s);
  if idx = -1 then
    Exit;
  
  sl := slMeta.Objects[idx];
  for i := 0 to Pred(sl.Count) do begin
    slv.DelimitedText := sl[i];
    if slv[0] = 'TRANSLATE_VECTOR' then begin
      x := FloatToStr(slv[1]);
      y := FloatToStr(slv[2]);
      z := FloatToStr(slv[3]);
      rx := GetElementNativeValues(e, 'DATA\Position\X');
      ry := GetElementNativeValues(e, 'DATA\Position\Y');
      rz := GetElementNativeValues(e, 'DATA\Position\Z');
      if ElementExists(e, 'XSCL') then
        scale := GetElementNativeValues(e, 'XSCL')
      else
        scale := 1;

      rx := rx + x*scale;
      ry := ry + y*scale;
      //rz := rz - z; // revert previous displacement
      rz := rz + z*scale;
      SetElementNativeValues(e, 'DATA\Position\X', rx);
      SetElementNativeValues(e, 'DATA\Position\Y', ry);
      SetElementNativeValues(e, 'DATA\Position\Z', rz);
    end;
  end;
end;

//============================================================================
function Finalize: integer;
var
  i: integer;
begin
  for i := 0 to Pred(slMeta.Count) do
    TStringList(slMeta.Objects[i]).Free;
  slMeta.Free;
  slv.Free;
end;

end.
