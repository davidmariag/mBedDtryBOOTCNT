void WriteStr(String command, String txt) 
{
  String _component = command;
  String _strVal = txt;

    Serial2.print(_component);
    Serial2.print(F("=\""));
    Serial2.print(_strVal);
    Serial2.print(F("\""));
    Serial2.print(F("\xFF\xFF\xFF"));
    _component = "";
    _strVal = "";
  
}

void WriteNum(String command, int val) 
{
  String _component = command;
  int _numVal = val;

  Serial2.print(_component);
  Serial2.print(F("="));
  Serial2.print(_numVal);
  Serial2.print(F("\xFF\xFF\xFF"));
  _component = "";
  command = "";
}
