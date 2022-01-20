Var InstallerCustomJavaPluginDisabled

!macro CustomCodePreInstall
	${If} ${FileExists} "$INSTDIR\bin\plugin2-disabled\*.*"
		StrCpy $InstallerCustomJavaPluginDisabled true
	${EndIf}
!macroend

!macro CustomCodePostInstall
	Rename "$INSTDIR\jre1.8.0_201\bin" "$INSTDIR\bin"
	Rename "$INSTDIR\jre1.8.0_201\lib" "$INSTDIR\lib"
	Rename "$INSTDIR\jre1.8.0_201\COPYRIGHT" "$INSTDIR\COPYRIGHT"
	Rename "$INSTDIR\jre1.8.0_201\LICENSE" "$INSTDIR\LICENSE"
	Rename "$INSTDIR\jre1.8.0_201\README.txt" "$INSTDIR\README.txt"
	Rename "$INSTDIR\jre1.8.0_201\release" "$INSTDIR\release"
	Rename "$INSTDIR\jre1.8.0_201\THIRDPARTYLICENSEREADME.txt" "$INSTDIR\THIRDPARTYLICENSEREADME.txt"
	Rename "$INSTDIR\jre1.8.0_201\THIRDPARTYLICENSEREADME-JAVAFX.txt" "$INSTDIR\THIRDPARTYLICENSEREADME-JAVAFX.txt"
	Rename "$INSTDIR\jre1.8.0_201\Welcome.html" "$INSTDIR\Welcome.html"
	RMDir "$INSTDIR\jre1.8.0_201"
	nsExec::ExecToLog `"$INSTDIR\Other\Source\UnpackPostInstall.bat"`
	;ExecDOS::exec '"$INSTDIR\bin\unpack200.exe" -r -q "$INSTDIR\lib\charsets.pack" "$INSTDIR\lib\charsets.jar"' "" ""
	;Pop $R0
	;ExecDOS::exec '"$INSTDIR\bin\unpack200.exe" -r -q "$INSTDIR\lib\deploy.pack" "$INSTDIR\lib\deploy.jar"' "" ""
	;Pop $R0
	;ExecDOS::exec '"$INSTDIR\bin\unpack200.exe" -r -q "$INSTDIR\lib\javaws.pack" "$INSTDIR\lib\javaws.jar"' "" ""
	;Pop $R0
	;ExecDOS::exec '"$INSTDIR\bin\unpack200.exe" -r -q "$INSTDIR\lib\jfxrt.pack" "$INSTDIR\lib\jfxrt.jar"' "" ""
	;Pop $R0
	;ExecDOS::exec '"$INSTDIR\bin\unpack200.exe" -r -q "$INSTDIR\lib\jsse.pack" "$INSTDIR\lib\jsse.jar"' "" ""
	;Pop $R0
	;ExecDOS::exec '"$INSTDIR\bin\unpack200.exe" -r -q "$INSTDIR\lib\plugin.pack" "$INSTDIR\lib\plugin.jar"' "" ""
	;Pop $R0
	;ExecDOS::exec '"$INSTDIR\bin\unpack200.exe" -r -q "$INSTDIR\lib\rt.pack" "$INSTDIR\lib\rt.jar"' "" ""
	;Pop $R0
	;ExecDOS::exec '"$INSTDIR\bin\unpack200.exe" -r -q "$INSTDIR\lib\ext\localedata.pack" "$INSTDIR\lib\ext\localedata.jar"' "" ""
	;Pop $R0
	;CopyFiles /silent "$INSTDIR\bin\npdeployJava1.dll" "$INSTDIR\bin\plugin2"
	;CopyFiles /silent "$INSTDIR\bin\msvcr71.dll" "$INSTDIR\bin\plugin2"
	${If} $InstallerCustomJavaPluginDisabled == true
		Rename "$INSTDIR\bin\plugin2" "$INSTDIR\bin\plugin2-disabled"
	${EndIf}
!macroend
