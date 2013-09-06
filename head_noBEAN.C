{

  TString makeshared(gSystem->GetMakeSharedLib());
  TString dummy = makeshared.ReplaceAll("-W ", "");
  gSystem->SetMakeSharedLib(makeshared);
  TString dummy = makeshared.ReplaceAll("-Wshadow ", "-std=c++0x ");
  gSystem->SetMakeSharedLib(makeshared);
  gSystem->Load("libFWCoreFWLite");
  AutoLibraryLoader::enable();

  gSystem->Load("libDataFormatsFWLite.so");
  gSystem->Load("libDataFormatsCommon.so");

  TString path = gSystem->GetIncludePath();
  path.Append(" -I$ROOTSYS/include/root -I./include  ");
  gSystem->SetIncludePath(path);

}
